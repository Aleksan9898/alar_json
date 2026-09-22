#include "json_lexer.hpp"
#include <cctype>
#include <cstdint>

namespace
{
    constexpr bool is_continuation(std::uint8_t byte) noexcept
    {
        return (byte & 0xC0) == 0x80;
    }
} // namespace

namespace alar::json_lexer
{
    Lexer::Lexer(std::string_view input) noexcept
        : m_input{input}
    {
    }

    JsonToken Lexer::next_token() noexcept
    {
        JsonToken result;

        skip_whitespace();

        if (m_position >= m_input.length())
        {
            result = {JsonTokenType::EndOfFile, "", m_column, m_line};
            return result;
        }

        const char c = m_input[m_position];
        result.column = m_column;
        result.line = m_line;
        switch (c)
        {
        case '{':
            result.type = JsonTokenType::ObjectBegin;
            ++m_position;
            ++m_column;
            break;
        case '}':
            result.type = JsonTokenType::ObjectEnd;
            ++m_position;
            ++m_column;
            break;
        case '[':
            result.type = JsonTokenType::ArrayBegin;
            ++m_position;
            ++m_column;
            break;
        case ']':
            result.type = JsonTokenType::ArrayEnd;
            ++m_position;
            ++m_column;
            break;
        case ':':
            result.type = JsonTokenType::Colon;
            ++m_position;
            ++m_column;
            break;
        case ',':
            result.type = JsonTokenType::Comma;
            ++m_position;
            ++m_column;
            break;
        case '"':
            read_string(result);
            break;
        case 't':
        case 'f':
        case 'n':
            read_literal(result);
            break;
        default:
            if (std::isdigit(static_cast<unsigned char>(c)) || c == '-')
            {
                read_number(result);
            }
            else
            {
                result.type = JsonTokenType::Invalid;
            }
            break;
        }

        return result;
    }

    void Lexer::skip_whitespace() noexcept
    {
        while (m_position < m_input.size())
        {
            const char c = m_input[m_position];
            switch (c)
            {
            case ' ':
            case '\t':
                ++m_position;
                ++m_column;
                break;
            case '\n':
                ++m_position;
                ++m_line;
                m_column = 1;
                break;
            case '\r':
                ++m_position;
                if (m_position < m_input.size() && m_input[m_position] == '\n')
                {
                    ++m_position;
                }
                ++m_line;
                m_column = 1;
                break;
            default:
                return;
            }
        }
    }

    bool Lexer::validate_continuations(std::size_t count) const noexcept
    {
        if (m_position + count >= m_input.size())
        {
            return false;
        }
        for (std::size_t i = 1; i <= count; ++i)
        {
            const auto b = static_cast<std::uint8_t>(m_input[m_position + i]);
            if (!is_continuation(b))
            {
                return false;
            }
        }
        return true;
    }

    void Lexer::read_string(JsonToken& token) noexcept
    {
        ++m_position;
        ++m_column;

        const auto start_pos = m_position;

        while (m_position < m_input.size())
        {
            const std::uint8_t b = static_cast<std::uint8_t>(m_input[m_position]);

            if (b < 0x20)
            {
                token.type = JsonTokenType::Invalid;
                return;
            }

            if (b == '"')
            {
                token.type = JsonTokenType::String;
                token.value = m_input.substr(start_pos, m_position - start_pos);
                ++m_position;
                ++m_column;
                return;
            }

            if (b == '\\')
            {
                if (m_position + 1 >= m_input.size())
                {
                    token.type = JsonTokenType::Invalid;
                    return;
                }

                const char next = m_input[m_position + 1];
                switch (next)
                {
                case '"':
                case '\\':
                case '/':
                case 'b':
                case 'f':
                case 'n':
                case 'r':
                case 't':
                    m_position += 2;
                    m_column += 2;
                    break;
                case 'u':
                    if (m_position + 5 >= m_input.size())
                    {
                        token.type = JsonTokenType::Invalid;
                        return;
                    }
                    for (std::size_t i = 2; i <= 5; ++i)
                    {
                        const auto hex = static_cast<unsigned char>(m_input[m_position + i]);
                        if (!std::isxdigit(hex))
                        {
                            token.type = JsonTokenType::Invalid;
                            return;
                        }
                    }
                    m_position += 6;
                    m_column += 6;
                    break;
                default:
                    token.type = JsonTokenType::Invalid;
                    return;
                }
                continue;
            }

            if ((b & 0x80) == 0x00)
            {
                ++m_position;
                ++m_column;
            }
            else if ((b & 0xE0) == 0xC0)
            {
                if (!validate_continuations(1))
                {
                    token.type = JsonTokenType::Invalid;
                    return;
                }
                m_position += 2;
                ++m_column;
            }
            else if ((b & 0xF0) == 0xE0)
            {
                if (!validate_continuations(2))
                {
                    token.type = JsonTokenType::Invalid;
                    return;
                }
                m_position += 3;
                ++m_column;
            }
            else if ((b & 0xF8) == 0xF0)
            {
                if (!validate_continuations(3))
                {
                    token.type = JsonTokenType::Invalid;
                    return;
                }
                m_position += 4;
                ++m_column;
            }
            else
            {
                token.type = JsonTokenType::Invalid;
                return;
            }
        }

        token.type = JsonTokenType::Invalid;
    }

    void Lexer::validate_literal(JsonToken& token, std::string_view word, const JsonTokenType type) noexcept
    {
        for (const auto c : word)
        {
            if (m_position >= m_input.size() || m_input[m_position] != c)
            {
                token.type = JsonTokenType::Invalid;
                return;
            }
            ++m_position;
            ++m_column;
        }

        if (m_position < m_input.size())
        {
            const char next = m_input[m_position];
            const auto unsigned_next = static_cast<unsigned char>(next);

            if (std::isalnum(unsigned_next) || next == '_')
            {
                token.type = JsonTokenType::Invalid;
                return;
            }
        }
        token.type = type;
    }

    void Lexer::read_literal(JsonToken& token) noexcept
    {
        const char c = m_input[m_position];
        switch (c)
        {
        case 't':
            validate_literal(token, "true", JsonTokenType::True);
            break;
        case 'f':
            validate_literal(token, "false", JsonTokenType::False);
            break;
        case 'n':
            validate_literal(token, "null", JsonTokenType::Null);
            break;
        default:
            return;
        }
    }

    void Lexer::read_number(JsonToken& token) noexcept
    {
        const std::size_t start_pos = m_position;

        if (m_position < m_input.size() && m_input[m_position] == '-')
        {
            ++m_position;
            ++m_column;
        }

        if (m_position >= m_input.size())
        {
            token.type = JsonTokenType::Invalid;
            return;
        }

        if (m_input[m_position] == '0')
        {
            ++m_position;
            ++m_column;
            if (m_position < m_input.size() && std::isdigit(static_cast<unsigned char>(m_input[m_position])))
            {
                token.type = JsonTokenType::Invalid;
                return;
            }
        }
        else if (m_input[m_position] >= '1' && m_input[m_position] <= '9')
        {
            while (m_position < m_input.size() && std::isdigit(static_cast<unsigned char>(m_input[m_position])))
            {
                ++m_position;
                ++m_column;
            }
        }
        else
        {
            token.type = JsonTokenType::Invalid;
            return;
        }

        if (m_position < m_input.size() && m_input[m_position] == '.')
        {
            ++m_position;
            ++m_column;

            if (m_position >= m_input.size() || !std::isdigit(static_cast<unsigned char>(m_input[m_position])))
            {
                token.type = JsonTokenType::Invalid;
                return;
            }

            while (m_position < m_input.size() && std::isdigit(static_cast<unsigned char>(m_input[m_position])))
            {
                ++m_position;
                ++m_column;
            }
        }

        if (m_position < m_input.size() && (m_input[m_position] == 'e' || m_input[m_position] == 'E'))
        {
            ++m_position;
            ++m_column;

            if (m_position < m_input.size() && (m_input[m_position] == '+' || m_input[m_position] == '-'))
            {
                ++m_position;
                ++m_column;
            }

            if (m_position >= m_input.size() || !std::isdigit(static_cast<unsigned char>(m_input[m_position])))
            {
                token.type = JsonTokenType::Invalid;
                return;
            }

            while (m_position < m_input.size() && std::isdigit(static_cast<unsigned char>(m_input[m_position])))
            {
                ++m_position;
                ++m_column;
            }
        }

        if (m_position < m_input.size())
        {
            const char next = m_input[m_position];
            const auto unsigned_next = static_cast<unsigned char>(next);

            if (std::isalnum(unsigned_next) || next == '_' || next == '.')
            {
                token.type = JsonTokenType::Invalid;
                return;
            }
        }

        token.type = JsonTokenType::Number;
        token.value = m_input.substr(start_pos, m_position - start_pos);
    }

} // namespace alar::json_lexer