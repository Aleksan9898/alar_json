#include "lexer/json_lexer.hpp"
#include <gtest/gtest.h>

namespace alar::json_lexer::testing
{

    using namespace alar::json_lexer;

    TEST(JsonLexerTest, StructuralTokensAndCoordinates)
    {
        Lexer lexer("{\n  \"key\": [10, true]\n}");

        auto t = lexer.next_token();
        EXPECT_EQ(t.type, JsonTokenType::ObjectBegin);
        EXPECT_EQ(t.line, 1);
        EXPECT_EQ(t.column, 1);

        t = lexer.next_token();
        EXPECT_EQ(t.type, JsonTokenType::String);
        EXPECT_EQ(t.value, "key");
        EXPECT_EQ(t.line, 2);
        EXPECT_EQ(t.column, 3);

        t = lexer.next_token();
        EXPECT_EQ(t.type, JsonTokenType::Colon);

        t = lexer.next_token();
        EXPECT_EQ(t.type, JsonTokenType::ArrayBegin);

        t = lexer.next_token();
        EXPECT_EQ(t.type, JsonTokenType::Number);
        EXPECT_EQ(t.value, "10");

        t = lexer.next_token();
        EXPECT_EQ(t.type, JsonTokenType::Comma);

        t = lexer.next_token();
        EXPECT_EQ(t.type, JsonTokenType::True);

        t = lexer.next_token();
        EXPECT_EQ(t.type, JsonTokenType::ArrayEnd);

        t = lexer.next_token();
        EXPECT_EQ(t.type, JsonTokenType::ObjectEnd);

        t = lexer.next_token();
        EXPECT_EQ(t.type, JsonTokenType::EndOfFile);
    }

    TEST(JsonLexerTest, ValidLiterals)
    {
        Lexer lexer("true false null");

        EXPECT_EQ(lexer.next_token().type, JsonTokenType::True);
        EXPECT_EQ(lexer.next_token().type, JsonTokenType::False);
        EXPECT_EQ(lexer.next_token().type, JsonTokenType::Null);
        EXPECT_EQ(lexer.next_token().type, JsonTokenType::EndOfFile);
    }

    TEST(JsonLexerTest, InvalidLiteralWordBoundaries)
    {
        Lexer l1("truefalse");
        EXPECT_EQ(l1.next_token().type, JsonTokenType::Invalid);

        Lexer l2("true_id");
        EXPECT_EQ(l2.next_token().type, JsonTokenType::Invalid);

        Lexer l3("null123");
        EXPECT_EQ(l3.next_token().type, JsonTokenType::Invalid);
    }

    TEST(JsonLexerTest, ValidNumbers)
    {
        const std::string_view valid_numbers[] = {"0",
                                                  "-0",
                                                  "123",
                                                  "-456",
                                                  "0.123",
                                                  "-0.5",
                                                  "12.34",
                                                  "1e10",
                                                  "1E+10",
                                                  "1.5e-3",
                                                  "-0.5E2"};

        for (const auto num_str : valid_numbers) {
            Lexer lexer(num_str);
            const auto token = lexer.next_token();
            EXPECT_EQ(token.type, JsonTokenType::Number);
            EXPECT_EQ(token.value, num_str);
        }
    }

    TEST(JsonLexerTest, NumberFollowedByComma)
    {
        Lexer lexer("0,5");

        auto t1 = lexer.next_token();
        EXPECT_EQ(t1.type, JsonTokenType::Number);
        EXPECT_EQ(t1.value, "0");

        auto t2 = lexer.next_token();
        EXPECT_EQ(t2.type, JsonTokenType::Comma);

        auto t3 = lexer.next_token();
        EXPECT_EQ(t3.type, JsonTokenType::Number);
        EXPECT_EQ(t3.value, "5");
    }

    TEST(JsonLexerTest, InvalidNumbers)
    {
        const std::string_view invalid_numbers[] = {
            "012", "-05", "+15", ".5", "5.", "1e", "1e+", "0x1F", "100_px", "1.2.3"};

        for (const auto num_str : invalid_numbers) {
            SCOPED_TRACE(num_str);
            Lexer lexer(num_str);
            const auto token = lexer.next_token();
            EXPECT_EQ(token.type, JsonTokenType::Invalid) << "Failed for input: " << num_str;
        }
    }

    TEST(JsonLexerTest, ValidStrings)
    {
        Lexer lexer(R"("hello" "hello\nworld" "unicode: \u0041" "utf8: привет")");

        auto t = lexer.next_token();
        EXPECT_EQ(t.type, JsonTokenType::String);
        EXPECT_EQ(t.value, "hello");

        t = lexer.next_token();
        EXPECT_EQ(t.type, JsonTokenType::String);
        EXPECT_EQ(t.value, R"(hello\nworld)");

        t = lexer.next_token();
        EXPECT_EQ(t.type, JsonTokenType::String);
        EXPECT_EQ(t.value, R"(unicode: \u0041)");

        t = lexer.next_token();
        EXPECT_EQ(t.type, JsonTokenType::String);
        EXPECT_EQ(t.value, "utf8: привет");
    }

    TEST(JsonLexerTest, InvalidStrings)
    {
        Lexer l1("\"unclosed string");
        EXPECT_EQ(l1.next_token().type, JsonTokenType::Invalid);

        Lexer l2("\"\t\"");
        EXPECT_EQ(l2.next_token().type, JsonTokenType::Invalid);

        Lexer l3(R"("\u001Z")");
        EXPECT_EQ(l3.next_token().type, JsonTokenType::Invalid);
    }

} // namespace alar::lexer::testing