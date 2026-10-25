#include "core/Overload.hpp"

#include <gtest/gtest.h>

#include <string>
#include <variant>

TEST(Overload, visitCallsTheMatchingOverload)
{
    const auto describe{ [](const std::variant<int, std::string>& variant) {
        return std::visit(
            fcad::core::Overload{
                [](int value) { return "int " + std::to_string(value); },
                [](const std::string& value) { return "string " + value; },
            },
            variant
        );
    } };

    EXPECT_EQ(describe(42), "int 42");
    EXPECT_EQ(describe(std::string{ "abc" }), "string abc");
}
