#include "json_parser.hpp"
#include <charconv>
#include <cstdint>

namespace
{
    std::optional<alar::JsonValue> parse_number_as(std::string_view value) noexcept
    {
        if (value.find_first_of(".eE") != std::string_view::npos)
        {
            double result{};

            const auto [ptr, ec] = std::from_chars(value.begin(), value.end(), result);

            if (ec == std::errc{} && ptr == value.end())
                return alar::JsonValue{result};
        }
        else
        {
            std::int64_t result{};

            const auto [ptr, ec] = std::from_chars(value.begin(), value.end(), result);

            if (ec == std::errc{} && ptr == value.end())
                return alar::JsonValue{result};
        }

        return std::nullopt;
    }
} // namespace

namespace alar::json_parser
{
    using json_lexer::JsonTokenType;

    Parser::Parser(std::string_view input) noexcept
        : m_lexer{input}
    {
    }

    Result_t Parser::parse() noexcept
    {
        m_current_token = m_lexer.next_token();

        if (m_current_token.type != JsonTokenType::ArrayBegin && m_current_token.type != JsonTokenType::ObjectBegin)
        {
            return JsonParseError{
                JsonParseErrorType::ExpectedObjectOrArray, m_current_token.line, m_current_token.column};
        }

        do
        {
            parse_value();

            if (has_error())
            {
                return std::move(*m_error);
            }

            m_current_token = m_lexer.next_token();

        } while (!m_context_st.empty());

        if (m_current_token.type != JsonTokenType::EndOfFile)
        {
            return JsonParseError{JsonParseErrorType::UnexpectedToken, m_current_token.line, m_current_token.column};
        }

        return std::move(*m_result);
    }

    void Parser::parse_value() noexcept
    {
        switch (m_current_token.type)
        {
        case JsonTokenType::ObjectBegin:
            m_context_st.push({ContextType::Object, ContextState::ObjectExpectKeyOrEnd, JsonValue{JsonObject{}}, ""});
            break;

        case JsonTokenType::ArrayBegin:
            m_context_st.push({ContextType::Array, ContextState::ArrayExpectValueOrEnd, JsonValue{JsonArray{}}, ""});
            break;

        case JsonTokenType::Colon: {
            auto& top = m_context_st.top();

            if (top.state == ContextState::ObjectExpectColon)
            {
                top.state = ContextState::ObjectExpectValue;
            }
            else
            {
                m_error.emplace(JsonParseErrorType::UnexpectedToken, m_current_token.line, m_current_token.column);
            }

            break;
        }

        case JsonTokenType::Comma: {
            auto& top = m_context_st.top();

            if (top.state == ContextState::ObjectExpectCommaOrEnd)
            {
                top.state = ContextState::ObjectExpectKey;
            }
            else if (top.state == ContextState::ArrayExpectCommaOrEnd)
            {
                top.state = ContextState::ArrayExpectValue;
            }
            else
            {
                m_error.emplace(JsonParseErrorType::UnexpectedToken, m_current_token.line, m_current_token.column);
            }

            break;
        }

        case JsonTokenType::ObjectEnd:
            close_context();
            break;

        case JsonTokenType::ArrayEnd:
            close_context();
            break;

        case JsonTokenType::EndOfFile: {
            if (!m_context_st.empty())
            {
                m_error.emplace(JsonParseErrorType::UnexpectedToken, m_current_token.line, m_current_token.column);
            }
            break;
        }
        case JsonTokenType::Number:
            parse_number();
            break;

        case JsonTokenType::String:
            parse_string();
            break;

        case JsonTokenType::Null:
        case JsonTokenType::True:
        case JsonTokenType::False:
            parse_literal();
            break;
        default:
            m_error.emplace(JsonParseErrorType::UnexpectedToken, m_current_token.line, m_current_token.column);
            break;
        }
    }

    void Parser::parse_number() noexcept
    {
        auto value = parse_number_as(m_current_token.value);

        if (!value)
        {
            m_error.emplace(JsonParseErrorType::InvalidNumber, m_current_token.line, m_current_token.column);

            return;
        }
        store_value(std::move(*value));
    }

    void Parser::parse_string() noexcept
    {
        if (auto& top = m_context_st.top();
            top.type == ContextType::Object &&
            (top.state == ContextState::ObjectExpectKey || top.state == ContextState::ObjectExpectKeyOrEnd))
        {
            top.key = m_current_token.value;
            top.state = ContextState::ObjectExpectColon;
            return;
        }

        store_value(JsonValue{static_cast<std::string>(m_current_token.value)});
    }

    void Parser::parse_literal() noexcept
    {
        JsonValue value;
        switch (m_current_token.type)
        {
        case JsonTokenType::Null:
            value = nullptr;
            break;
        case JsonTokenType::True:
            value = true;
            break;
        case JsonTokenType::False:
            value = false;
            break;
        default:
            m_error.emplace(JsonParseErrorType::UnexpectedToken, m_current_token.line, m_current_token.column);
            return;
        }

        store_value(std::move(value));
    }

    void Parser::close_context() noexcept
    {
        if (m_context_st.empty())
        {
            m_error.emplace(JsonParseErrorType::UnexpectedToken, m_current_token.line, m_current_token.column);

            return;
        }

        auto& top = m_context_st.top();

        const bool valid_close =
            (top.type == ContextType::Object && m_current_token.type == JsonTokenType::ObjectEnd &&
             (top.state == ContextState::ObjectExpectKeyOrEnd || top.state == ContextState::ObjectExpectCommaOrEnd)) ||
            (top.type == ContextType::Array && m_current_token.type == JsonTokenType::ArrayEnd &&
             (top.state == ContextState::ArrayExpectValueOrEnd || top.state == ContextState::ArrayExpectCommaOrEnd));

        if (!valid_close)
        {
            m_error.emplace(JsonParseErrorType::UnexpectedToken, m_current_token.line, m_current_token.column);

            return;
        }

        Context top_context = std::move(top);
        m_context_st.pop();

        if (m_context_st.empty())
        {
            m_result.emplace(std::move(top_context.value));
            return;
        }

        Context& parent = m_context_st.top();

        try
        {
            if (parent.type == ContextType::Object)
            {
                parent.value.get_object()[parent.key] = std::move(top_context.value);

                parent.state = ContextState::ObjectExpectCommaOrEnd;
            }
            else
            {
                parent.value.get_array().emplace_back(std::move(top_context.value));

                parent.state = ContextState::ArrayExpectCommaOrEnd;
            }
        }
        catch (const JsonTypeError&)
        {
            m_error.emplace(JsonParseErrorType::InternalError,
                            m_current_token.line,
                            m_current_token.column,
                            "Failed to close JSON context");
        }
        catch (const std::exception& ex)
        {
            m_error.emplace(JsonParseErrorType::InternalError, m_current_token.line, m_current_token.column, ex.what());
        }
    }

    bool Parser::has_error() const noexcept
    {
        return m_error.has_value();
    }

    void Parser::store_value(JsonValue value) noexcept
    {
        auto& top = m_context_st.top();

        try
        {
            if (top.type == ContextType::Object && top.state == ContextState::ObjectExpectValue)
            {
                top.value.get_object()[top.key] = std::move(value);
                top.state = ContextState::ObjectExpectCommaOrEnd;
            }
            else if (top.type == ContextType::Array &&
                     (top.state == ContextState::ArrayExpectValue || top.state == ContextState::ArrayExpectValueOrEnd))
            {
                top.value.get_array().push_back(std::move(value));
                top.state = ContextState::ArrayExpectCommaOrEnd;
            }
            else
            {
                m_error.emplace(JsonParseErrorType::UnexpectedToken, m_current_token.line, m_current_token.column);
            }
        }
        catch (const JsonTypeError&)
        {
            m_error.emplace(JsonParseErrorType::InternalError,
                            m_current_token.line,
                            m_current_token.column,
                            "JSON type error while storing parsed value");
        }
        catch (const std::exception& ex)
        {
            m_error.emplace(JsonParseErrorType::InternalError, m_current_token.line, m_current_token.column, ex.what());
        }
    }
} // namespace alar::json_parser
