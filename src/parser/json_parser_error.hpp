#pragma once

#include <cstddef>
#include <string>

namespace alar::json_parser_error
{
    enum class JsonParseErrorType
    {
        UnexpectedToken,
        UnexpectedEnd,

        ExpectedObjectOrArray,

        ExpectedColon,
        ExpectedComma,
        ExpectedValue,

        ExpectedObjectKey,
        ExpectedObjectEnd,
        ExpectedArrayEnd,

        InvalidNumber,
        InvalidString,
        InvalidEscape,

        InternalError
    };

    class JsonParseError
    {
    public:
        JsonParseError(JsonParseErrorType type, std::size_t line, std::size_t column, std::string message = {});

        [[nodiscard]]
        JsonParseErrorType type() const noexcept;

        [[nodiscard]]
        std::size_t line() const noexcept;

        [[nodiscard]]
        std::size_t column() const noexcept;

        [[nodiscard]]
        const char *get_error() const noexcept;

    private:
        JsonParseErrorType m_type;
        std::size_t m_line;
        std::size_t m_column;
        std::string m_message;
    };
} // namespace alar::json_parser_error