// This file is part of the Luau programming language and is licensed under MIT License; see LICENSE.txt for details
// Tests for safe navigation operators: ?. ?[ ?( ?:

#include "Fixture.h"
#include "ScopedFlags.h"

#include "Luau/Common.h"
#include "Luau/Error.h"

#include "doctest.h"

using namespace Luau;

TEST_SUITE_BEGIN("SafeNavTest");

// ─── ?. operator ─────────────────────────────────────────────────────────────

TEST_CASE_FIXTURE(Fixture, "safe_index_name_no_error_on_optional_receiver")
{
    // ?. must NOT produce OptionalValueAccess even when receiver is T|nil
    DOES_NOT_PASS_OLD_SOLVER_GUARD();

    CheckResult result = check(R"(
        type T = { x: number }
        local a: T? = nil
        local v = a?.x
    )");

    LUAU_REQUIRE_NO_ERRORS(result);
}

TEST_CASE_FIXTURE(Fixture, "safe_index_name_result_is_nullable")
{
    // a?.x where a: T? should produce T.x | nil
    DOES_NOT_PASS_OLD_SOLVER_GUARD();

    CheckResult result = check(R"(
        type T = { x: number }
        local a: T? = nil
        local v = a?.x
    )");

    LUAU_REQUIRE_NO_ERRORS(result);
    CHECK("number?" == toString(requireType("v")));
}

TEST_CASE_FIXTURE(Fixture, "safe_index_name_purely_nil_receiver_no_error")
{
    // nil?.x should be a no-op with nil result, not an error
    DOES_NOT_PASS_OLD_SOLVER_GUARD();

    CheckResult result = check(R"(
        local v = (nil :: nil)?.x
    )");

    LUAU_REQUIRE_NO_ERRORS(result);
}

TEST_CASE_FIXTURE(Fixture, "safe_index_name_unknown_property_still_errors")
{
    // ?. suppresses OptionalValueAccess but NOT UnknownProperty
    DOES_NOT_PASS_OLD_SOLVER_GUARD();

    CheckResult result = check(R"(
        type T = { x: number }
        local a: T? = nil
        local v = a?.y
    )");

    LUAU_REQUIRE_ERROR_COUNT(1, result);
    CHECK(get<UnknownProperty>(result.errors[0]));
}

TEST_CASE_FIXTURE(Fixture, "regular_dot_still_errors_on_optional_receiver")
{
    // Sanity check: regular . still produces OptionalValueAccess
    DOES_NOT_PASS_OLD_SOLVER_GUARD();

    CheckResult result = check(R"(
        type T = { x: number }
        local a: T? = nil
        local v = a.x
    )");

    LUAU_REQUIRE_ERROR_COUNT(1, result);
    CHECK(get<OptionalValueAccess>(result.errors[0]));
}

// ─── ?[ operator ─────────────────────────────────────────────────────────────

TEST_CASE_FIXTURE(Fixture, "safe_index_expr_no_error_on_optional_receiver")
{
    DOES_NOT_PASS_OLD_SOLVER_GUARD();

    CheckResult result = check(R"(
        local a: {[number]: string}? = nil
        local v = a?[1]
    )");

    LUAU_REQUIRE_NO_ERRORS(result);
}

TEST_CASE_FIXTURE(Fixture, "safe_index_expr_result_is_nullable")
{
    DOES_NOT_PASS_OLD_SOLVER_GUARD();

    CheckResult result = check(R"(
        local a: {[number]: string}? = nil
        local v = a?[1]
    )");

    LUAU_REQUIRE_NO_ERRORS(result);
    CHECK("string?" == toString(requireType("v")));
}

TEST_CASE_FIXTURE(Fixture, "safe_index_expr_string_key_no_error_on_optional")
{
    DOES_NOT_PASS_OLD_SOLVER_GUARD();

    CheckResult result = check(R"(
        type T = { x: number }
        local a: T? = nil
        local v = a?["x"]
    )");

    LUAU_REQUIRE_NO_ERRORS(result);
}

TEST_CASE_FIXTURE(Fixture, "safe_index_expr_string_key_unknown_property_still_errors")
{
    DOES_NOT_PASS_OLD_SOLVER_GUARD();

    CheckResult result = check(R"(
        type T = { x: number }
        local a: T? = nil
        local v = a?["y"]
    )");

    LUAU_REQUIRE_ERROR_COUNT(1, result);
    CHECK(get<UnknownProperty>(result.errors[0]));
}

// ─── ?( operator ─────────────────────────────────────────────────────────────

TEST_CASE_FIXTURE(Fixture, "safe_call_no_error_on_optional_function")
{
    DOES_NOT_PASS_OLD_SOLVER_GUARD();

    CheckResult result = check(R"(
        local f: (() -> number)? = nil
        local v = f?()
    )");

    LUAU_REQUIRE_NO_ERRORS(result);
}

TEST_CASE_FIXTURE(Fixture, "safe_call_result_is_nullable")
{
    DOES_NOT_PASS_OLD_SOLVER_GUARD();

    CheckResult result = check(R"(
        local f: (() -> number)? = nil
        local v = f?()
    )");

    LUAU_REQUIRE_NO_ERRORS(result);
    CHECK("number?" == toString(requireType("v")));
}

TEST_CASE_FIXTURE(Fixture, "safe_call_purely_nil_function_no_error")
{
    DOES_NOT_PASS_OLD_SOLVER_GUARD();

    CheckResult result = check(R"(
        local f: nil = nil
        local v = f?()
    )");

    LUAU_REQUIRE_NO_ERRORS(result);
}

TEST_CASE_FIXTURE(Fixture, "safe_call_nil_branch_supplies_nil_for_extra_results")
{
    DOES_NOT_PASS_OLD_SOLVER_GUARD();

    CheckResult result = check(R"(
        local f: (() -> (number, number))? = nil
        local a, b = f?()
    )");

    LUAU_REQUIRE_NO_ERRORS(result);
    CHECK("number?" == toString(requireType("a")));
    CHECK("number?" == toString(requireType("b")));
}

TEST_CASE_FIXTURE(Fixture, "regular_call_errors_on_optional_function")
{
    // Sanity check: regular () still produces OptionalValueAccess
    DOES_NOT_PASS_OLD_SOLVER_GUARD();

    CheckResult result = check(R"(
        local f: (() -> number)? = nil
        local v = f()
    )");

    LUAU_REQUIRE_ERROR_COUNT(1, result);
    CHECK(get<OptionalValueAccess>(result.errors[0]));
}

// ─── ?: operator ─────────────────────────────────────────────────────────────

TEST_CASE_FIXTURE(Fixture, "safe_method_call_no_error_on_optional_receiver")
{
    DOES_NOT_PASS_OLD_SOLVER_GUARD();

    CheckResult result = check(R"(
        type T = { greet: (T) -> string }
        local a: T? = nil
        local v = a?:greet()
    )");

    LUAU_REQUIRE_NO_ERRORS(result);
}

TEST_CASE_FIXTURE(Fixture, "safe_method_call_result_can_be_chained")
{
    DOES_NOT_PASS_OLD_SOLVER_GUARD();

    CheckResult result = check(R"(
        type Child = { name: string }
        type T = { getChild: (T, string) -> Child? }

        local t: T? = nil
        local v = t?:getChild("alice")?.name
    )");

    LUAU_REQUIRE_NO_ERRORS(result);
    CHECK("string?" == toString(requireType("v")));
}

TEST_CASE_FIXTURE(Fixture, "safe_method_call_unknown_method_still_errors")
{
    DOES_NOT_PASS_OLD_SOLVER_GUARD();

    CheckResult result = check(R"(
        type T = { greet: (T) -> string }
        local a: T? = nil
        local v = a?:farewell()
    )");

    LUAU_REQUIRE_ERROR_COUNT(1, result);
    CHECK(get<UnknownProperty>(result.errors[0]));
}

TEST_CASE_FIXTURE(Fixture, "regular_method_call_errors_on_optional_receiver")
{
    // Sanity check: regular : still produces OptionalValueAccess (at minimum)
    DOES_NOT_PASS_OLD_SOLVER_GUARD();

    CheckResult result = check(R"(
        type T = { greet: (T) -> string }
        local a: T? = nil
        local v = a:greet()
    )");

    LUAU_REQUIRE_ERRORS(result);
    bool foundOptionalValueAccess = false;
    for (const auto& err : result.errors)
        if (get<OptionalValueAccess>(err))
            foundOptionalValueAccess = true;
    CHECK(foundOptionalValueAccess);
}

// ─── Chaining ────────────────────────────────────────────────────────────────

TEST_CASE_FIXTURE(Fixture, "chained_safe_navigation_no_error")
{
    DOES_NOT_PASS_OLD_SOLVER_GUARD();

    CheckResult result = check(R"(
        type Inner = { z: number }
        type Outer = { inner: Inner? }
        local a: Outer? = nil
        local v = a?.inner?.z
    )");

    LUAU_REQUIRE_NO_ERRORS(result);
}

TEST_SUITE_END();
