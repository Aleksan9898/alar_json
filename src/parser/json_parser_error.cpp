#include "json_parser_error.hpp"

namespace alar::json_parser_error
{
    JsonParseError::JsonParseError(JsonParseErrorType type, std::size_t line, std::size_t column, std::string message)
        : m_type{type},
          m_line{line},
          m_column{column},
          m_message{message}
    {
    }

    JsonParseErrorType JsonParseError::type() const noexcept
    {
        return m_type;
    }

    std::size_t JsonParseError::line() const noexcept
    {
        return m_line;
    }

    std::size_t JsonParseError::column() const noexcept
    {
        return m_column;
    }

    const char *JsonParseError::get_error() const noexcept
    {
        switch (m_type)
        {
        case JsonParseErrorType::UnexpectedToken:
            return "Unexpected token";

        case JsonParseErrorType::UnexpectedEnd:
            return "Unexpected end of input";

        case JsonParseErrorType::ExpectedObjectOrArray:
            return "Expected object or array";

        case JsonParseErrorType::ExpectedColon:
            return "Expected ':'";

        case JsonParseErrorType::ExpectedComma:
            return "Expected ','";

        case JsonParseErrorType::ExpectedValue:
            return "Expected value";

        case JsonParseErrorType::ExpectedObjectKey:
            return "Expected object key";

        case JsonParseErrorType::ExpectedObjectEnd:
            return "Expected '}'";

        case JsonParseErrorType::ExpectedArrayEnd:
            return "Expected ']'";

        case JsonParseErrorType::InvalidNumber:
            return "Invalid number";

        case JsonParseErrorType::InvalidString:
            return "Invalid string";

        case JsonParseErrorType::InvalidEscape:
            return "Invalid escape sequence";
        case JsonParseErrorType::InternalError:
            return m_message.c_str();
        }

        return "Unknown parse error";
    }

} // namespace alar::json_parser_error