
#include "alar_json/json_error.hpp"
#include "alar_json/json_value.hpp"
#include <gtest/gtest.h>

namespace alar::json_value::tests
{
    using namespace alar;

    TEST(JsonValueTest, DefaultValueIsNull)
    {
        JsonValue value;
        EXPECT_TRUE(value.is_null());
    }

    TEST(JsonValueTest, BooleanValue)
    {
        JsonValue value{true};
        EXPECT_TRUE(value.is_bool());
    }

    TEST(JsonValueTest, IntegerTypes)
    {
        JsonValue int_value{42};
        JsonValue unsigned_value{42u};
        JsonValue long_value{42L};
        JsonValue long_long_value{42LL};

        EXPECT_TRUE(int_value.is_integer());
        EXPECT_TRUE(unsigned_value.is_integer());
        EXPECT_TRUE(long_value.is_integer());
        EXPECT_TRUE(long_long_value.is_integer());
    }

    TEST(JsonValueTest, DoubleValue)
    {
        JsonValue value{3.14};
        EXPECT_TRUE(value.is_double());
    }

    TEST(JsonValueTest, StringValue)
    {
        JsonValue value{"Asdasdas"};
        EXPECT_TRUE(value.is_string());
    }

    TEST(JsonValueTest, EmptyStringIsNotNull)
    {
        JsonValue value{""};

        EXPECT_TRUE(value.is_string());
        EXPECT_FALSE(value.is_null());
    }

    TEST(JsonValueTest, ArrayValue)
    {
        JsonArray array{JsonValue{1}, JsonValue{"hello"}, JsonValue{true}};

        JsonValue value{std::move(array)};

        EXPECT_TRUE(value.is_array());
        EXPECT_FALSE(value.is_object());
    }

    TEST(JsonValueTest, ObjectValue)
    {
        JsonObject object;

        object.emplace("name", JsonValue{"Test"});
        object.emplace("age", JsonValue{25});

        JsonValue value{std::move(object)};

        EXPECT_TRUE(value.is_object());
        EXPECT_FALSE(value.is_array());
    }

    TEST(JsonValueTest, ValueHasOnlyOneActiveType)
    {
        JsonValue value{42};

        EXPECT_TRUE(value.is_integer());
        EXPECT_FALSE(value.is_null());
        EXPECT_FALSE(value.is_bool());
        EXPECT_FALSE(value.is_double());
        EXPECT_FALSE(value.is_string());
        EXPECT_FALSE(value.is_array());
        EXPECT_FALSE(value.is_object());
    }

    TEST(JsonValueTest, CopyObject)
    {
        JsonObject object;
        object.emplace("name", JsonValue{"Test"});

        JsonValue original{std::move(object)};
        JsonValue copy{original};

        EXPECT_TRUE(original.is_object());
        EXPECT_TRUE(copy.is_object());
    }

    TEST(JsonValueTest, ToInteger)
    {
        JsonValue value{42};

        EXPECT_EQ(value.to_integer(), 42);
    }

    TEST(JsonValueTest, ToDouble)
    {
        JsonValue value{3.14};

        EXPECT_DOUBLE_EQ(value.to_double(), 3.14);
    }

    TEST(JsonValueTest, ToBoolean)
    {
        JsonValue value{true};

        EXPECT_TRUE(value.to_boolean());
    }

    TEST(JsonValueTest, ToString)
    {
        JsonValue value{"hello"};

        EXPECT_EQ(value.to_string(), "hello");
    }

    TEST(JsonValueTest, ToIntegerThrowsForWrongType)
    {
        JsonValue value{"hello"};

        EXPECT_THROW(value.to_integer(), JsonTypeError);
    }

    TEST(JsonValueTest, IntegerToString)
    {
        JsonValue value{45};

        EXPECT_EQ(value.to_string(), "45");
    }

    TEST(JsonValueTest, ArrayToStringThrows)
    {
        JsonValue value{JsonArray{JsonValue{1}, JsonValue{2}}};

        EXPECT_THROW(value.to_string(), JsonTypeError);
    }

    TEST(JsonValueTest, SubscriptInitializesNullAsObject)
    {
        JsonValue value;

        EXPECT_TRUE(value.is_null());

        value["name"] = JsonValue{"Test"};

        EXPECT_TRUE(value.is_object());
        EXPECT_EQ(value["name"].to_string(), "Test");
    }

    TEST(JsonValueTest, ObjectSubscript)
    {
        JsonValue value{
            JsonObject{{"name", JsonValue{"Test"}}, {"age", JsonValue{25}}}};

        EXPECT_EQ(value["name"].to_string(), "Test");
        EXPECT_EQ(value["age"].to_integer(), 25);
    }

    TEST(JsonValueTest, ArrayAtOutOfRangeThrows)
    {
        JsonValue value{JsonArray{JsonValue{10}}};

        EXPECT_THROW(value.at(1), std::out_of_range);
    }

    TEST(JsonValueTest, ArrayAt)
    {
        JsonValue value{JsonArray{JsonValue{10}, JsonValue{20}}};

        EXPECT_EQ(value.at(0).to_integer(), 10);
        EXPECT_EQ(value.at(1).to_integer(), 20);
    }

    TEST(JsonValueTest, ObjectIterator)
    {
        JsonObject object;
        object.emplace("name", JsonValue{"Test"});
        object.emplace("age", JsonValue{25});

        JsonValue value{std::move(object)};

        std::size_t count = 0;

        for (auto it = value.begin(); it != value.end(); ++it) {
            EXPECT_FALSE(it.key().empty());
            EXPECT_TRUE(it.value().is_string() || it.value().is_integer());

            ++count;
        }

        EXPECT_EQ(count, 2);
    }

    TEST(JsonValueTest, ArrayIterator)
    {
        JsonArray array{JsonValue{10}, JsonValue{20}, JsonValue{30}};

        JsonValue value{std::move(array)};

        std::size_t count = 0;

        for (auto it = value.begin(); it != value.end(); ++it) {
            EXPECT_TRUE(it.value().is_integer());

            ++count;
        }

        EXPECT_EQ(count, 3);
    }

    TEST(JsonValueTest, IteratorDereference)
    {
        JsonObject object;
        object.emplace("name", JsonValue{"Test"});

        JsonValue value{std::move(object)};

        auto it = value.begin();

        EXPECT_TRUE((*it).is_string());
        EXPECT_EQ((*it).to_string(), "Test");
    }

    TEST(JsonValueTest, IteratorArrowOperator)
    {
        JsonObject object;
        object.emplace("name", JsonValue{"Test"});

        JsonValue value{std::move(object)};

        auto it = value.begin();

        EXPECT_TRUE(it->is_string());
        EXPECT_EQ(it->to_string(), "Test");
    }

    TEST(JsonValueTest, ArrayIteratorKeyThrows)
    {
        JsonArray array{JsonValue{10}, JsonValue{20}};

        JsonValue value{std::move(array)};

        auto it = value.begin();

        EXPECT_THROW(it.key(), JsonTypeError);
    }

    TEST(JsonValueTest, ArrayIteratorValue)
    {
        JsonArray array{JsonValue{10}, JsonValue{20}};

        JsonValue value{std::move(array)};

        auto it = value.begin();

        EXPECT_EQ(it.value().to_integer(), 10);

        ++it;

        EXPECT_EQ(it.value().to_integer(), 20);
    }

    TEST(JsonValueTest, IteratorPostIncrement)
    {
        JsonArray array{JsonValue{10}, JsonValue{20}};

        JsonValue value{std::move(array)};

        auto it = value.begin();

        auto old = it++;

        EXPECT_EQ(old.value().to_integer(), 10);
        EXPECT_EQ(it.value().to_integer(), 20);
    }

    TEST(JsonValueTest, ConstIterator)
    {
        JsonObject object;
        object.emplace("name", JsonValue{"Test"});

        const JsonValue value{std::move(object)};

        auto it = value.begin();

        EXPECT_TRUE(it->is_string());
        EXPECT_EQ(it.value().to_string(), "Test");
    }

    TEST(JsonValueTest, ConstBeginAndConstEnd)
    {
        JsonArray array{JsonValue{10}, JsonValue{20}, JsonValue{30}};

        const JsonValue value{std::move(array)};

        auto it = value.cbegin();
        auto end = value.cend();

        std::size_t count = 0;

        for (; it != end; ++it) {
            EXPECT_TRUE(it.value().is_integer());
            ++count;
        }

        EXPECT_EQ(count, 3);
    }

    TEST(JsonValueTest, EmptyObjectIterators)
    {
        JsonValue value{JsonObject{}};

        EXPECT_EQ(value.begin(), value.end());
    }
} // namespace json_value::tests