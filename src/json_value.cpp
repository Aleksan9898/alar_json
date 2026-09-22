#include "alar_json/json_value.hpp"
#include "alar_json/json_error.hpp"

#include <stdexcept>
#include <utility>
#include <cstdio>

namespace
{
    template <typename Object>
    decltype(auto) find_key(Object& object, std::string_view key)
    {
        auto it = object.find(std::string{key});

        if (it == object.end())
        {
            throw std::out_of_range("Json object does not contain key: " + std::string{key});
        }

        return (it->second);
    }

} // namespace

namespace alar
{

    JsonValue::JsonValue()
        : m_data{nullptr}
    {
    }

    JsonValue::JsonValue(bool val)
        : m_data{val}
    {
    }

    JsonValue::JsonValue(std::nullptr_t) noexcept
        : m_data{nullptr}
    {
    }

    JsonValue::JsonValue(double val)
        : m_data{val}
    {
    }

    JsonValue::JsonValue(std::string val)
        : m_data{std::move(val)}
    {
    }

    JsonValue::JsonValue(const char *val)
        : m_data{std::string{val}}
    {
    }

    JsonValue::JsonValue(JsonArray val)
        : m_data{std::move(val)}
    {
    }

    JsonValue::JsonValue(JsonObject val)
        : m_data{std::make_unique<JsonObject>(std::move(val))}
    {
    }

    JsonValue::JsonValue(const JsonValue& other)
        : m_data{std::visit(
              [](const auto& value) -> JsonValueType {
                  using T = std::decay_t<decltype(value)>;

                  if constexpr (std::is_same_v<T, std::unique_ptr<JsonObject>>)
                  {
                      if (value)
                      {
                          return std::make_unique<JsonObject>(*value);
                      }

                      return std::unique_ptr<JsonObject>{};
                  }
                  else
                  {
                      return value;
                  }
              },
              other.m_data)}
    {
         std::fprintf(stderr, "JsonValue COPY\n");
    }

    JsonValue& JsonValue::operator=(const JsonValue& other)
    {
        if (this != &other)
        {
            JsonValue temp{other};
            *this = std::move(temp);
        }

        return *this;
    }

    JsonValue& JsonValue::operator[](std::string_view key)
    {
        if (is_null())
        {
            m_data = std::make_unique<JsonObject>();
        }

        return get_object()[std::string{key}];
    }

    const JsonValue *JsonValue::operator[](std::string_view key) const
    {
        if (!is_object())
        {
            return nullptr;
        }

        const auto& object = get_object();

        auto it = object.find(std::string{key});

        if (it == object.end())
        {
            return nullptr;
        }

        return &it->second;
    }

    JsonValue& JsonValue::operator[](std::size_t index)
    {
        return get_array()[index];
    }

    const JsonValue& JsonValue::operator[](std::size_t index) const
    {
        return get_array()[index];
    }

    JsonValue& JsonValue::at(std::string_view key)
    {
        return find_key(get_object(), key);
    }

    const JsonValue& JsonValue::at(std::string_view key) const
    {
        return find_key(get_object(), key);
    }

    JsonValue& JsonValue::at(std::size_t index)
    {
        return get_array().at(index);
    }

    const JsonValue& JsonValue::at(std::size_t index) const
    {
        return get_array().at(index);
    }

    JsonIterator JsonValue::begin()
    {
        if (is_object())
        {
            return JsonIterator{get_object().begin()};
        }

        if (is_array())
        {
            return JsonIterator{get_array().begin()};
        }

        throw JsonTypeError("JsonValue is not iterable");
    }

    JsonIterator JsonValue::end()
    {
        if (is_object())
        {
            return JsonIterator{get_object().end()};
        }

        if (is_array())
        {
            return JsonIterator{get_array().end()};
        }

        throw JsonTypeError("JsonValue is not iterable");
    }

    JsonConstIterator JsonValue::begin() const
    {
        if (is_object())
        {
            return JsonConstIterator{get_object().cbegin()};
        }

        if (is_array())
        {
            return JsonConstIterator{get_array().cbegin()};
        }

        throw JsonTypeError("JsonValue is not iterable");
    }

    JsonConstIterator JsonValue::end() const
    {
        if (is_object())
        {
            return JsonConstIterator{get_object().cend()};
        }

        if (is_array())
        {
            return JsonConstIterator{get_array().cend()};
        }

        throw JsonTypeError("JsonValue is not iterable");
    }

    JsonConstIterator JsonValue::cbegin() const
    {
        return begin();
    }

    JsonConstIterator JsonValue::cend() const
    {
        return end();
    }

    bool JsonValue::is_null() const noexcept
    {
        if (std::holds_alternative<std::nullptr_t>(m_data))
        {
            return true;
        }

        const auto *object = std::get_if<std::unique_ptr<JsonObject>>(&m_data);

        return object != nullptr && !*object;
    }

    bool JsonValue::is_bool() const noexcept
    {
        return std::holds_alternative<bool>(m_data);
    }

    bool JsonValue::is_integer() const noexcept
    {
        return std::holds_alternative<std::int64_t>(m_data);
    }

    bool JsonValue::is_double() const noexcept
    {
        return std::holds_alternative<double>(m_data);
    }

    bool JsonValue::is_string() const noexcept
    {
        return std::holds_alternative<std::string>(m_data);
    }

    bool JsonValue::is_array() const noexcept
    {
        return std::holds_alternative<JsonArray>(m_data);
    }

    bool JsonValue::is_object() const noexcept
    {
        const auto *object = std::get_if<std::unique_ptr<JsonObject>>(&m_data);

        return object != nullptr && *object != nullptr;
    }

    std::int64_t JsonValue::to_integer() const
    {
        if (!is_integer())
        {
            throw JsonTypeError("JsonValue is not an integer");
        }
        return std::get<std::int64_t>(m_data);
    }

    double JsonValue::to_double() const
    {
        if (!is_double())
        {
            throw JsonTypeError("JsonValue is not an double");
        }
        return std::get<double>(m_data);
    }

    bool JsonValue::to_boolean() const
    {
        if (!is_bool())
        {
            throw JsonTypeError("JsonValue is not an bool");
        }
        return std::get<bool>(m_data);
    }

    std::string JsonValue::to_string() const
    {
        if (is_null())
        {
            return "null";
        }

        if (is_bool())
        {
            return std::get<bool>(m_data) ? "true" : "false";
        }

        if (is_integer())
        {
            return std::to_string(std::get<std::int64_t>(m_data));
        }

        if (is_double())
        {
            return std::to_string(std::get<double>(m_data));
        }

        if (is_string())
        {
            return std::get<std::string>(m_data);
        }

        if (is_array())
        {
            throw JsonTypeError("Cannot convert JSON array to string");
        }

        throw JsonTypeError("Cannot convert JSON object to string");
    }

    JsonArray& JsonValue::get_array()
    {
        if (!is_array())
        {
            throw JsonTypeError("JsonValue is not an array");
        }

        return std::get<JsonArray>(m_data);
    }

    const JsonArray& JsonValue::get_array() const
    {
        if (!is_array())
        {
            throw JsonTypeError("JsonValue is not an array");
        }

        return std::get<JsonArray>(m_data);
    }

    JsonObject& JsonValue::get_object()
    {
        if (!is_object())
        {
            throw JsonTypeError("JsonValue is not an object");
        }

        return *std::get<std::unique_ptr<JsonObject>>(m_data);
    }

    const JsonObject& JsonValue::get_object() const
    {
        if (!is_object())
        {
            throw JsonTypeError("JsonValue is not an object");
        }

        return *std::get<std::unique_ptr<JsonObject>>(m_data);
    }

} // namespace alar
