#pragma once

#include "alar_json/json_iterator.hpp"
#include "alar_json/json_types.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>

namespace alar
{
    /**
     * @brief Represents all types of values supported by JSON.
     *
     * JsonValueType can contain a JSON null, boolean, integer, floating-point
     * number, string, array, or object.
     */
    using JsonValueType =
        std::variant<std::nullptr_t, bool, std::int64_t, double, std::string, JsonArray, std::unique_ptr<JsonObject>>;

    /**
     * @brief Represents a JSON value.
     *
     * JsonValue is the main value type of the alar_json library. It can
     * represent any JSON value: null, boolean, integer, floating-point
     * number, string, array, or object.
     */
    class JsonValue
    {
    public:
        /**
         * @brief Constructs a null JSON value.
         */
        JsonValue();

        /**
         * @brief Constructs a boolean JSON value.
         *
         * @param val Boolean value.
         */
        JsonValue(bool val);

        /**
         * @brief Constructs an integer JSON value.
         *
         * Integral types other than bool are converted to std::int64_t.
         *
         * @tparam T Integral type.
         * @param val Integer value.
         */
        template <typename T>
            requires(std::is_integral_v<T> && !std::is_same_v<T, bool>)
        JsonValue(T val)
            : m_data{static_cast<std::int64_t>(val)}
        {
        }

        /**
         * @brief Constructs a null JSON value.
         */
        JsonValue(std::nullptr_t) noexcept;
        
        /**
         * @brief Constructs a floating-point JSON value.
         *
         * @param val Floating-point value.
         */
        JsonValue(double val);

        /**
         * @brief Constructs a string JSON value.
         *
         * @param val String value.
         */
        JsonValue(std::string val);

        /**
         * @brief Constructs a string JSON value from a null-terminated string.
         *
         * @param val Null-terminated string.
         */
        JsonValue(const char *val);

        /**
         * @brief Constructs a JSON array value.
         *
         * @param val Array value.
         */
        JsonValue(JsonArray val);

        /**
         * @brief Constructs a JSON object value.
         *
         * @param val Object value.
         */
        JsonValue(JsonObject val);

        /**
         * @brief Constructs a copy of another JSON value.
         *
         * @param other JSON value to copy.
         */
        JsonValue(const JsonValue& other);

        /**
         * @brief Constructs a JSON value by moving another value.
         *
         * @param other JSON value to move from.
         */
        JsonValue(JsonValue&& other) noexcept = default;

        /**
         * @brief Copy-assigns another JSON value.
         *
         * @param other JSON value to copy.
         *
         * @return Reference to this JSON value.
         */
        JsonValue& operator=(const JsonValue& other);

        /**
         * @brief Move-assigns another JSON value.
         *
         * @param other JSON value to move from.
         *
         * @return Reference to this JSON value.
         */
        JsonValue& operator=(JsonValue&& other) noexcept = default;

        /**
         * @brief Destroys the JSON value.
         */
        ~JsonValue() = default;

        /**
         * @brief Accesses an object member by key.
         *
         * If the current value is null, it is converted to an empty
         * JSON object before accessing the key.
         *
         * @param key Object member key.
         *
         * @return Reference to the associated JSON value.
         *
         * @throws JsonTypeError If the current value is neither null nor
         *         a JSON object.
         */
        JsonValue& operator[](std::string_view key);

        /**
         * @brief Accesses an object member by key without modifying the value.
         *
         * @param key Object member key.
         *
         * @return Pointer to the associated JSON value, or nullptr if the
         *         current value is not an object or the key does not exist.
         */
        const JsonValue *operator[](std::string_view key) const;

        /**
         * @brief Accesses an array element by index without bounds checking.
         *
         * @param index Array element index.
         *
         * @return Reference to the array element.
         *
         * @throws JsonTypeError If the current value is not a JSON array.
         */
        JsonValue& operator[](std::size_t index);

        /**
         * @brief Accesses an array element by index without bounds checking.
         *
         * @param index Array element index.
         *
         * @return Const reference to the array element.
         *
         * @throws JsonTypeError If the current value is not a JSON array.
         */
        const JsonValue& operator[](std::size_t index) const;

        /**
         * @brief Accesses an object member by key with bounds checking.
         *
         * @param key Object member key.
         *
         * @return Reference to the associated JSON value.
         *
         * @throws JsonTypeError If the current value is not a JSON object.
         * @throws std::out_of_range If the key does not exist.
         */
        JsonValue& at(std::string_view key);

        /**
         * @brief Accesses an object member by key with bounds checking.
         *
         * @param key Object member key.
         *
         * @return Const reference to the associated JSON value.
         *
         * @throws JsonTypeError If the current value is not a JSON object.
         * @throws std::out_of_range If the key does not exist.
         */
        const JsonValue& at(std::string_view key) const;

        /**
         * @brief Accesses an array element by index with bounds checking.
         *
         * @param index Array element index.
         *
         * @return Reference to the array element.
         *
         * @throws JsonTypeError If the current value is not a JSON array.
         * @throws std::out_of_range If the index is out of range.
         */
        JsonValue& at(std::size_t index);

        /**
         * @brief Accesses an array element by index with bounds checking.
         *
         * @param index Array element index.
         *
         * @return Const reference to the array element.
         *
         * @throws JsonTypeError If the current value is not a JSON array.
         * @throws std::out_of_range If the index is out of range.
         */
        const JsonValue& at(std::size_t index) const;

        /**
         * @brief Returns an iterator to the first element.
         *
         * @return Iterator to the first element of the JSON object or array.
         *
         * @throws JsonTypeError If the current value is neither an object
         *         nor an array.
         */
        JsonIterator begin();

        /**
         * @brief Returns an iterator past the last element.
         *
         * @return Iterator past the last element of the JSON object or array.
         *
         * @throws JsonTypeError If the current value is neither an object
         *         nor an array.
         */
        JsonIterator end();

        /**
         * @brief Returns a const iterator to the first element.
         *
         * @return Const iterator to the first element of the JSON object or array.
         *
         * @throws JsonTypeError If the current value is neither an object
         *         nor an array.
         */
        JsonConstIterator begin() const;

        /**
         * @brief Returns a const iterator past the last element.
         *
         * @return Const iterator past the last element of the JSON object or array.
         *
         * @throws JsonTypeError If the current value is neither an object
         *         nor an array.
         */
        JsonConstIterator end() const;

        /**
         * @brief Returns a const iterator to the first element.
         *
         * @return Const iterator to the first element of the JSON object or array.
         *
         * @throws JsonTypeError If the current value is neither an object
         *         nor an array.
         */
        JsonConstIterator cbegin() const;

        /**
         * @brief Returns a const iterator past the last element.
         *
         * @return Const iterator past the last element of the JSON object or array.
         *
         * @throws JsonTypeError If the current value is neither an object
         *         nor an array.
         */
        JsonConstIterator cend() const;

        /**
         * @brief Checks whether the value is JSON null.
         *
         * @return true if the value is null, otherwise false.
         */
        bool is_null() const noexcept;

        /**
         * @brief Checks whether the value is a JSON boolean.
         *
         * @return true if the value is a boolean, otherwise false.
         */
        bool is_bool() const noexcept;

        /**
         * @brief Checks whether the value is a JSON integer.
         *
         * @return true if the value is an integer, otherwise false.
         */
        bool is_integer() const noexcept;

        /**
         * @brief Checks whether the value is a JSON floating-point number.
         *
         * @return true if the value is a floating-point number, otherwise false.
         */
        bool is_double() const noexcept;

        /**
         * @brief Checks whether the value is a JSON string.
         *
         * @return true if the value is a string, otherwise false.
         */
        bool is_string() const noexcept;

        /**
         * @brief Checks whether the value is a JSON array.
         *
         * @return true if the value is an array, otherwise false.
         */
        bool is_array() const noexcept;

        /**
         * @brief Checks whether the value is a JSON object.
         *
         * @return true if the value is an object, otherwise false.
         */
        bool is_object() const noexcept;

        /**
         * @brief Returns the stored integer value.
         *
         * @return Stored integer value.
         *
         * @throws JsonTypeError If the value is not an integer.
         */
        std::int64_t to_integer() const;

        /**
         * @brief Returns the stored floating-point value.
         *
         * @return Stored floating-point value.
         *
         * @throws JsonTypeError If the value is not a floating-point number.
         */
        double to_double() const;

        /**
         * @brief Returns the stored boolean value.
         *
         * @return Stored boolean value.
         *
         * @throws JsonTypeError If the value is not a boolean.
         */
        bool to_boolean() const;

        /**
         * @brief Converts the JSON scalar value to a string representation.
         *
         * Null, boolean, integer, floating-point, and string values can be
         * converted to strings. Arrays and objects cannot be converted using
         * this function.
         *
         * @return String representation of the value.
         *
         * @throws JsonTypeError If the value is an array or an object.
         */
        std::string to_string() const;

        JsonArray& get_array();
        const JsonArray& get_array() const;

        JsonObject& get_object();
        const JsonObject& get_object() const;

    private:
        JsonValueType m_data;
    };

} // namespace alar
