#pragma once

#include <string_view>

namespace alar::json_lexer
{

    enum class JsonTokenType
    {
        ObjectBegin, // {
        ObjectEnd,   // }
        ArrayBegin,  // [
        ArrayEnd,    // ]
        Colon,       // :
        Comma,       // ,

        String,
        Number,
        True,
        False,
        Null,

        EndOfFile,
        Invalid
    };

    struct JsonToken
    {
        JsonTokenType type;
        std::string_view value;
        std::size_t column;
        std::size_t line;
    };

    class Lexer
    {
      public:
        explicit Lexer(std::string_view input) noexcept;
        JsonToken next_token() noexcept;

      private:
        void skip_whitespace() noexcept;
        bool validate_continuations(std::size_t count) const noexcept;
        void read_string(JsonToken& token) noexcept;
        void validate_literal(JsonToken& token, std::string_view word, const JsonTokenType type) noexcept;
        void read_literal(JsonToken& token) noexcept;
        void read_number(JsonToken& token) noexcept;

        std::string_view m_input;
        std::size_t m_position{};
        std::size_t m_line{1};
        std::size_t m_column{1};
    };
} // namespace alar::json_lexer
