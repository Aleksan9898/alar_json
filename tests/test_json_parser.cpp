#include "alar_json/json_value.hpp"
#include "parser/json_parser.hpp"
#include "parser/json_parser_error.hpp"
#include <gtest/gtest.h>

namespace alar::json_parser::testing
{
    using namespace alar;

    TEST(JsonParserTest, EmptyObject)
    {
        Parser parser{"{}"};

        auto result = parser.parse();

        ASSERT_TRUE(result.has_value());

        const auto& value = result.value();

        EXPECT_TRUE(value.is_object());
        EXPECT_TRUE(value.get_object().empty());
    }

    TEST(JsonParserTest, EmptyArray)
    {
        Parser parser{"[]"};

        auto result = parser.parse();

        ASSERT_TRUE(result.has_value());

        const auto& value = result.value();

        EXPECT_TRUE(value.is_array());
        EXPECT_TRUE(value.get_array().empty());
    }

    TEST(JsonParserTest, SimpleObject)
    {
        Parser parser{R"({"name":"Alex","age":25,"active":true})"};

        auto result = parser.parse();

        ASSERT_TRUE(result.has_value());

        const auto& object = result.value().get_object();

        ASSERT_EQ(object.size(), 3);

        EXPECT_EQ(object.at("name").to_string(), "Alex");
        EXPECT_EQ(object.at("age").to_integer(), 25);
        EXPECT_TRUE(object.at("active").to_boolean());
    }

    TEST(JsonParserTest, SimpleArray)
    {
        Parser parser{R"([1,2,3,4])"};

        auto result = parser.parse();

        ASSERT_TRUE(result.has_value());

        const auto& array = result.value().get_array();

        ASSERT_EQ(array.size(), 4);

        EXPECT_EQ(array[0].to_integer(), 1);
        EXPECT_EQ(array[1].to_integer(), 2);
        EXPECT_EQ(array[2].to_integer(), 3);
        EXPECT_EQ(array[3].to_integer(), 4);
    }

    TEST(JsonParserTest, AllLiteralValues)
    {
        Parser parser{R"([null,true,false])"};

        auto result = parser.parse();

        ASSERT_TRUE(result.has_value());

        const auto& array = result.value().get_array();

        ASSERT_EQ(array.size(), 3);

        EXPECT_TRUE(array[0].is_null());
        EXPECT_TRUE(array[1].to_boolean());
        EXPECT_FALSE(array[2].to_boolean());
    }

    TEST(JsonParserTest, DifferentNumberTypes)
    {
        Parser parser{R"([0,-42,123456,3.14,-0.5,1e10,1.5e-3])"};

        auto result = parser.parse();

        ASSERT_TRUE(result.has_value());

        const auto& array = result.value().get_array();

        ASSERT_EQ(array.size(), 7);

        EXPECT_TRUE(array[0].is_integer());
        EXPECT_EQ(array[0].to_integer(), 0);

        EXPECT_TRUE(array[1].is_integer());
        EXPECT_EQ(array[1].to_integer(), -42);

        EXPECT_TRUE(array[2].is_integer());
        EXPECT_EQ(array[2].to_integer(), 123456);

        EXPECT_TRUE(array[3].is_double());
        EXPECT_DOUBLE_EQ(array[3].to_double(), 3.14);

        EXPECT_TRUE(array[4].is_double());
        EXPECT_DOUBLE_EQ(array[4].to_double(), -0.5);

        EXPECT_TRUE(array[5].is_double());
        EXPECT_DOUBLE_EQ(array[5].to_double(), 1e10);

        EXPECT_TRUE(array[6].is_double());
        EXPECT_DOUBLE_EQ(array[6].to_double(), 1.5e-3);
    }

    TEST(JsonParserTest, EmptyStringKey)
    {
        Parser parser{R"({"":1})"};

        auto result = parser.parse();

        ASSERT_TRUE(result.has_value());

        const auto& object = result.value().get_object();

        ASSERT_EQ(object.size(), 1);
        EXPECT_EQ(object.at("").to_integer(), 1);
    }

    TEST(JsonParserTest, NestedObjectAndArray)
    {
        Parser parser{
            R"({
                "name":"test",
                "config":{
                    "enabled":true,
                    "values":[1,2,3]
                },
                "data":[
                    {"id":1},
                    {"id":2}
                ]
            })"};

        auto result = parser.parse();

        ASSERT_TRUE(result.has_value());

        const auto& root = result.value().get_object();

        ASSERT_EQ(root.size(), 3);

        EXPECT_EQ(root.at("name").to_string(), "test");

        const auto& config = root.at("config").get_object();

        EXPECT_TRUE(config.at("enabled").to_boolean());

        const auto& values = config.at("values").get_array();

        ASSERT_EQ(values.size(), 3);
        EXPECT_EQ(values[0].to_integer(), 1);
        EXPECT_EQ(values[1].to_integer(), 2);
        EXPECT_EQ(values[2].to_integer(), 3);

        const auto& data = root.at("data").get_array();

        ASSERT_EQ(data.size(), 2);
        EXPECT_EQ(data[0].get_object().at("id").to_integer(), 1);
        EXPECT_EQ(data[1].get_object().at("id").to_integer(), 2);
    }

    TEST(JsonParserTest, WhitespaceIsIgnored)
    {
        Parser parser{" \n\t { \n"
                      "   \"value\" \t : \n"
                      "   42 \n"
                      " } \n\t"};

        auto result = parser.parse();

        ASSERT_TRUE(result.has_value());

        EXPECT_EQ(result.value().get_object().at("value").to_integer(), 42);
    }

    TEST(JsonParserTest, ObjectWithAllValueTypes)
    {
        Parser parser{
            R"({
                "null":null,
                "bool":true,
                "integer":42,
                "double":3.14,
                "string":"hello",
                "array":[1,2],
                "object":{"value":10}
            })"};

        auto result = parser.parse();

        ASSERT_TRUE(result.has_value());

        const auto& object = result.value().get_object();

        EXPECT_TRUE(object.at("null").is_null());
        EXPECT_TRUE(object.at("bool").is_bool());
        EXPECT_TRUE(object.at("integer").is_integer());
        EXPECT_TRUE(object.at("double").is_double());
        EXPECT_TRUE(object.at("string").is_string());
        EXPECT_TRUE(object.at("array").is_array());
        EXPECT_TRUE(object.at("object").is_object());
    }

    // -------------------------------------------------------------------------
    // Syntax errors
    // -------------------------------------------------------------------------

    TEST(JsonParserTest, RootMustBeObjectOrArray)
    {
        Parser parser{"42"};

        auto result = parser.parse();

        EXPECT_FALSE(result.has_value());
    }

    TEST(JsonParserTest, StringCannotBeRoot)
    {
        Parser parser{R"("hello")"};

        auto result = parser.parse();

        EXPECT_FALSE(result.has_value());
    }

    TEST(JsonParserTest, BooleanCannotBeRoot)
    {
        Parser parser{"true"};

        auto result = parser.parse();

        EXPECT_FALSE(result.has_value());
    }

    TEST(JsonParserTest, NullCannotBeRoot)
    {
        Parser parser{"null"};

        auto result = parser.parse();

        EXPECT_FALSE(result.has_value());
    }

    TEST(JsonParserTest, MissingColon)
    {
        Parser parser{R"({"value" 42})"};

        auto result = parser.parse();

        EXPECT_FALSE(result.has_value());
    }

    TEST(JsonParserTest, MissingCommaInObject)
    {
        Parser parser{R"({"a":1 "b":2})"};

        auto result = parser.parse();

        EXPECT_FALSE(result.has_value());
    }

    TEST(JsonParserTest, MissingCommaInArray)
    {
        Parser parser{"[1 2]"};

        auto result = parser.parse();

        EXPECT_FALSE(result.has_value());
    }

    TEST(JsonParserTest, MissingObjectValue)
    {
        Parser parser{R"({"value":})"};

        auto result = parser.parse();

        EXPECT_FALSE(result.has_value());
    }

    TEST(JsonParserTest, TrailingCommaInObject)
    {
        Parser parser{R"({"a":1,})"};

        auto result = parser.parse();

        EXPECT_FALSE(result.has_value());
    }

    TEST(JsonParserTest, TrailingCommaInArray)
    {
        Parser parser{"[1,]"};

        auto result = parser.parse();

        EXPECT_FALSE(result.has_value());
    }

    TEST(JsonParserTest, MissingClosingObject)
    {
        Parser parser{R"({"a":1)"};

        auto result = parser.parse();

        EXPECT_FALSE(result.has_value());
    }

    TEST(JsonParserTest, MissingClosingArray)
    {
        Parser parser{"[1,2"};

        auto result = parser.parse();

        EXPECT_FALSE(result.has_value());
    }

    TEST(JsonParserTest, MismatchedClosingBracket)
    {
        Parser parser{R"({"a":[1})"};

        auto result = parser.parse();

        EXPECT_FALSE(result.has_value());
    }

    TEST(JsonParserTest, MismatchedClosingBrace)
    {
        Parser parser{"[1}"};

        auto result = parser.parse();

        EXPECT_FALSE(result.has_value());
    }

    TEST(JsonParserTest, UnexpectedCommaAfterOpeningObject)
    {
        Parser parser{"{,"};

        auto result = parser.parse();

        EXPECT_FALSE(result.has_value());
    }

    TEST(JsonParserTest, UnexpectedCommaAfterOpeningArray)
    {
        Parser parser{"[,"};

        auto result = parser.parse();

        EXPECT_FALSE(result.has_value());
    }

    TEST(JsonParserTest, MultipleRootValues)
    {
        Parser parser{"[]{}"};

        auto result = parser.parse();

        EXPECT_FALSE(result.has_value());
    }

    TEST(JsonParserTest, UnexpectedClosingBracket)
    {
        Parser parser{"]"};

        auto result = parser.parse();

        EXPECT_FALSE(result.has_value());
    }

    TEST(JsonParserTest, UnexpectedClosingBrace)
    {
        Parser parser{"}"};

        auto result = parser.parse();

        EXPECT_FALSE(result.has_value());
    }

    TEST(JsonParserTest, IntegerOverflow)
    {
        Parser parser{"[9223372036854775808]"};

        auto result = parser.parse();

        EXPECT_FALSE(result.has_value());
    }
} // namespace alar::json_parser::testing
