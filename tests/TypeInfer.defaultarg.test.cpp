// This file is part of the Luau programming language and is licensed under MIT License; see LICENSE.txt for details
#include "Fixture.h"
#include "doctest.h"

using namespace Luau;

TEST_SUITE_BEGIN("Default Arguments");

TEST_CASE_FIXTURE(Fixture, "default_argument_inference")
{
    // Type of x should be inferred as a number from the default value
    CheckResult result = check(R"(
        function foo(x = 1)
            return x
        end
    )");

    LUAU_REQUIRE_NO_ERRORS(result);
    dumpErrors(result);

    TypeId foo = follow(requireType("foo"));
    const FunctionType* ftv = get<FunctionType>(foo);
    REQUIRE(ftv != nullptr);

    // Check return type, not argument type, because argument type is nil-able
    std::optional<TypeId> ret_ = first(ftv->retTypes);
    REQUIRE(bool(ret_));
    TypeId ret = follow(*ret_);

    CHECK_EQ(PrimitiveType::Number, getPrimitiveType(ret));
}

TEST_CASE_FIXTURE(Fixture, "default_argument_optional")
{
    // x should not need passed due to default argument
    CheckResult result = check(R"(
        function foo(x = 1) end
        foo()
    )");
    LUAU_REQUIRE_NO_ERRORS(result);
}

TEST_CASE_FIXTURE(Fixture, "default_argument_checked")
{
    // The inferred type should take precedence over a passed type
    CheckResult result = check(R"(
        function foo(x = 1) end
        foo("bar")
    )");
    LUAU_REQUIRE_ERROR_COUNT(1, result);
    REQUIRE(get<TypeMismatch>(result.errors[0]));
}

TEST_CASE_FIXTURE(Fixture, "default_argument_annotation_checked")
{
    // Type is inferred as string, so "= 5" is invalid
    CheckResult result = check(R"(
        function foo(x: string = 1)
            return x
        end
    )");
    LUAU_REQUIRE_ERROR_COUNT(1, result);
    REQUIRE(get<TypeMismatch>(result.errors[0]));

    // Ensure type was inferred as string not number
    TypeId foo = follow(requireType("foo"));
    const FunctionType* ftv = get<FunctionType>(foo);
    REQUIRE(ftv != nullptr);

    std::optional<TypeId> ret_ = first(ftv->retTypes);
    REQUIRE(bool(ret_));
    TypeId ret = follow(*ret_);

    CHECK_EQ(PrimitiveType::String, getPrimitiveType(ret));
}

TEST_CASE_FIXTURE(Fixture, "default_argument_complex_type")
{
    // Missing "b" from table type
    CheckResult result = check(R"(
        function foo(x = {a = 0, b = 0}) end
        foo({a = 0})
    )");
    LUAU_REQUIRE_ERROR_COUNT(1, result);
    REQUIRE(get<TypeMismatch>(result.errors[0]));

    // Table type is not sealed, so adding "c" is okay
    result = check(R"(
        function foo(x = {a = 0, b = 0}) end
        foo({a = 0, b = 0, c = 0})
    )");
    LUAU_REQUIRE_NO_ERRORS(result);
}
