#pragma once

#include <optional>
#include <stack>
#include <string>
#include <string_view>

#include "alar_json/json_value.hpp"
#include "lexer/json_lexer.hpp"
#include "json_parser_error.hpp"
#include "json_result.hpp"

namespace alar::json_parser
{
    using json_parser_error::JsonParseError;
    using json_parser_error::JsonParseErrorType;

    using Result_t = detail::Result<JsonValue, JsonParseError>;

    class Parser
    {
    public:
        enum class ContextType
        {
            Object,
            Array
        };

        enum class ContextState
        {
            ObjectExpectKeyOrEnd,
            ObjectExpectKey,
            ObjectExpectColon,
            ObjectExpectValue,
            ObjectExpectCommaOrEnd,

            ArrayExpectValueOrEnd,
            ArrayExpectValue,
            ArrayExpectCommaOrEnd
        };

        struct Context
        {
            ContextType type;
            ContextState state;

            JsonValue value;

            std::string key;
        };

        explicit Parser(std::string_view input) noexcept;

        Result_t parse() noexcept;

    private:
        void parse_value() noexcept;
        void parse_number() noexcept;
        void parse_string() noexcept;
        void parse_literal() noexcept;

        void store_value(JsonValue value) noexcept;
        void close_context() noexcept;

        [[nodiscard]]
        bool has_error() const noexcept;

        std::stack<Context> m_context_st;
        json_lexer::JsonToken m_current_token{};
        json_lexer::Lexer m_lexer;
        std::optional<JsonValue> m_result;
        std::optional<JsonParseError> m_error;
    };
} // namespace alar::json_parser