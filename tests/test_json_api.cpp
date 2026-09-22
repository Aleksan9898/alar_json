#include <gtest/gtest.h>

#include "alar_json/json.hpp"
#include "alar_json/json_error.hpp"

#include <sstream>
#include <string>

namespace alar::json_api
{
    using alar::JsonParseException;
    using alar::JsonValue;

    TEST(JsonApiTest, ParseString)
    {
        const JsonValue value = alar::parse(R"({"name":"Alex","age":30})");

        EXPECT_TRUE(value.is_object());
        EXPECT_EQ(value.at("name").to_string(), "Alex");
        EXPECT_EQ(value.at("age").to_integer(), 30);
    }

    TEST(JsonApiTest, ParseArray)
    {
        const JsonValue value = alar::parse(R"([1,true,"text",null])");

        ASSERT_TRUE(value.is_array());
        ASSERT_EQ(value.get_array().size(), 4);

        EXPECT_EQ(value[0].to_integer(), 1);
        EXPECT_TRUE(value[1].to_boolean());
        EXPECT_EQ(value[2].to_string(), "text");
        EXPECT_TRUE(value[3].is_null());
    }

    TEST(JsonApiTest, ParseNestedJson)
    {
        const JsonValue value = alar::parse(
            R"({
            "user": {
                "name": "Alex",
                "roles": ["admin", "developer"]
            }
        })");

        const auto& user = value.at("user");

        EXPECT_EQ(user.at("name").to_string(), "Alex");
        EXPECT_EQ(user.at("roles")[0].to_string(), "admin");
        EXPECT_EQ(user.at("roles")[1].to_string(), "developer");
    }

    TEST(JsonApiTest, InvalidJsonThrowsJsonParseError)
    {
        try
        {
            alar::parse(R"({"name":})");
            FAIL() << "Expected JsonParseError";
        }
        catch (const JsonParseException&)
        {
            SUCCEED();
        }
        catch (const std::exception& error)
        {
            FAIL() << "Unexpected exception: " << error.what();
        }
    }

    TEST(JsonApiTest, InvalidJsonThrowsJsonParseErrorForMissingColon)
    {
        EXPECT_THROW(alar::parse(R"({"name","Alex"})"), JsonParseException);
    }

    TEST(JsonApiTest, InvalidJsonThrowsJsonParseErrorForTrailingComma)
    {
        EXPECT_THROW(alar::parse(R"([1,2,])"), JsonParseException);
    }

    TEST(JsonApiTest, ParseStream)
    {
        std::istringstream input{R"({"name":"Alex","value":42})"};

        const JsonValue value = alar::parse(input);

        EXPECT_TRUE(value.is_object());
        EXPECT_EQ(value.at("name").to_string(), "Alex");
        EXPECT_EQ(value.at("value").to_integer(), 42);
    }

    TEST(JsonApiTest, ParseArrayFromStream)
    {
        std::istringstream input{R"([10,20,30])"};

        const JsonValue value = alar::parse(input);

        ASSERT_TRUE(value.is_array());
        ASSERT_EQ(value.get_array().size(), 3);

        EXPECT_EQ(value[0].to_integer(), 10);
        EXPECT_EQ(value[1].to_integer(), 20);
        EXPECT_EQ(value[2].to_integer(), 30);
    }

    TEST(JsonApiTest, InvalidStreamJsonThrowsJsonParseError)
    {
        std::istringstream input{R"({"name":})"};

        EXPECT_THROW(alar::parse(input), JsonParseException);
    }

    TEST(JsonApiTest, EmptyStringThrowsJsonParseError)
    {
        EXPECT_THROW(alar::parse(""), JsonParseException);
    }

    TEST(JsonApiTest, ParseComplexNestedJson)
    {
        const JsonValue value = alar::parse(R"({
        "application": {
            "name": "alar_json",
            "version": 1.2,
            "enabled": true,
            "database": {
                "host": "localhost",
                "port": 5432,
                "options": {
                    "pool_size": 10,
                    "ssl": false
                }
            },
            "servers": [
                {
                    "name": "server-1",
                    "address": "192.168.1.10",
                    "ports": [80, 443],
                    "active": true
                },
                {
                    "name": "server-2",
                    "address": "192.168.1.11",
                    "ports": [8080, 8443],
                    "active": false
                }
            ],
            "features": [
                "parser",
                "lexer",
                "iterator",
                null
            ]
        }
    })");

        ASSERT_TRUE(value.is_object());

        const auto& application = value.at("application");

        EXPECT_EQ(application.at("name").to_string(), "alar_json");
        EXPECT_DOUBLE_EQ(application.at("version").to_double(), 1.2);
        EXPECT_TRUE(application.at("enabled").to_boolean());

        const auto& database = application.at("database");

        EXPECT_EQ(database.at("host").to_string(), "localhost");
        EXPECT_EQ(database.at("port").to_integer(), 5432);

        const auto& options = database.at("options");

        EXPECT_EQ(options.at("pool_size").to_integer(), 10);
        EXPECT_FALSE(options.at("ssl").to_boolean());

        const auto& servers = application.at("servers");

        ASSERT_EQ(servers.get_array().size(), 2);

        EXPECT_EQ(servers[0].at("name").to_string(), "server-1");
        EXPECT_EQ(servers[0].at("address").to_string(), "192.168.1.10");
        EXPECT_TRUE(servers[0].at("active").to_boolean());

        EXPECT_EQ(servers[0].at("ports")[0].to_integer(), 80);
        EXPECT_EQ(servers[0].at("ports")[1].to_integer(), 443);

        EXPECT_EQ(servers[1].at("name").to_string(), "server-2");
        EXPECT_EQ(servers[1].at("address").to_string(), "192.168.1.11");
        EXPECT_FALSE(servers[1].at("active").to_boolean());

        EXPECT_EQ(servers[1].at("ports")[0].to_integer(), 8080);
        EXPECT_EQ(servers[1].at("ports")[1].to_integer(), 8443);

        const auto& features = application.at("features");

        ASSERT_EQ(features.get_array().size(), 4);

        EXPECT_EQ(features[0].to_string(), "parser");
        EXPECT_EQ(features[1].to_string(), "lexer");
        EXPECT_EQ(features[2].to_string(), "iterator");
        EXPECT_TRUE(features[3].is_null());
    }

} // namespace alar::json_api