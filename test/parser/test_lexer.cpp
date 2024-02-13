/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
#include "natrix/parser/lexer.h"
#include "natrix/parser/source.h"

struct EToken {
    TokenType type;
    const std::string &lexeme;
};

static void PrintTo(const Token& token, std::ostream* os) {
    StringBuilder sb = sb_init();
    token_to_string(&token, &sb);
    *os << sb.str;
    sb_free(&sb);
}

static void PrintTo(const EToken& token, std::ostream* os) {
    StringBuilder sb = sb_init();
    sb_append_str(&sb, token_get_type_name(token.type));
    sb_append_str(&sb, "(\"");
    sb_append_escaped_str(&sb, token.lexeme.c_str());
    sb_append_str(&sb, "\")");
    *os << sb.str;
    sb_free(&sb);
}

static bool operator==(const Token &lhs, EToken rhs) {
    return lhs.type == rhs.type && std::string(lhs.start, lhs.end - lhs.start) == rhs.lexeme;
}

TEST(LexerTest, EmptySource) {
    Lexer lexer;
    lexer_init(&lexer, "\n");
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_EOF, ""}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_EOF, ""}));
    EXPECT_EQ(lexer_error_message(&lexer), nullptr);
}

TEST(LexerTest, SingleComment) {
    Lexer lexer;
    lexer_init(&lexer, "# comment\n");
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_EOF, ""}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_EOF, ""}));
    EXPECT_EQ(lexer_error_message(&lexer), nullptr);
}

TEST(LexerTest, SingleCommentWithSpaces) {
    Lexer lexer;
    lexer_init(&lexer, "    # comment\n");
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_EOF, ""}));
    EXPECT_EQ(lexer_error_message(&lexer), nullptr);
}

TEST(LexerTest, CommentedIntLiteral) {
    Lexer lexer;
    lexer_init(&lexer, "42    # comment\n");
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_INT_LITERAL, "42"}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_NEWLINE, "# comment\n"}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_EOF, ""}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_EOF, ""}));
    EXPECT_EQ(lexer_error_message(&lexer), nullptr);
}

TEST(LexerTest, SymbolsWithCommentOnNextLine) {
    Lexer lexer;
    lexer_init(&lexer, "/-\n# comment\n");
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_SLASH, "/"}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_MINUS, "-"}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_NEWLINE, "\n"}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_EOF, ""}));
    EXPECT_EQ(lexer_error_message(&lexer), nullptr);
}

TEST(LexerTest, SimpleExpression) {
    Lexer lexer;
    lexer_init(&lexer, "(_x2 + 10) * 3\n");
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_LPAREN, "("}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_IDENTIFIER, "_x2"}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_PLUS, "+"}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_INT_LITERAL, "10"}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_RPAREN, ")"}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_STAR, "*"}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_INT_LITERAL, "3"}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_NEWLINE, "\n"}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_EOF, ""}));
    EXPECT_EQ(lexer_error_message(&lexer), nullptr);
}

TEST(LexerTest, UnexpectedCharacter) {
    Lexer lexer;
    lexer_init(&lexer, "4  `\n");
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_INT_LITERAL, "4"}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_ERROR, "`"}));
    EXPECT_STREQ(lexer_error_message(&lexer), "unexpected character");
}

TEST(LexerTest, Indent) {
    Lexer lexer;
    lexer_init(&lexer, "1\n  2\n   3\n   4  #comment\n      # empty line\n\n5 \n");
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_INT_LITERAL, "1"}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_NEWLINE, "\n"}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_INDENT, "  "}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_INT_LITERAL, "2"}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_NEWLINE, "\n"}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_INDENT, " "}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_INT_LITERAL, "3"}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_NEWLINE, "\n"}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_INT_LITERAL, "4"}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_NEWLINE, "#comment\n"}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_DEDENT, ""}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_DEDENT, ""}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_INT_LITERAL, "5"}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_NEWLINE, "\n"}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_EOF, ""}));
    EXPECT_EQ(lexer_error_message(&lexer), nullptr);
}

TEST(LexerTest, DedentError) {
    Lexer lexer;
    lexer_init(&lexer, "1\n  2\n 3\n");
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_INT_LITERAL, "1"}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_NEWLINE, "\n"}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_INDENT, "  "}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_INT_LITERAL, "2"}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_NEWLINE, "\n"}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_ERROR, " "}));
    EXPECT_STREQ(lexer_error_message(&lexer), "unindent does not match any outer indentation level");
}

TEST(LexerTest, IndentTooDeepError) {
    Source src = source_from_file("test_indent_too_deep.ntx");
    ASSERT_NE(src.start, nullptr);
    Lexer lexer;
    lexer_init(&lexer, src.start);
    Token token = {.type = TOKEN_INT_LITERAL};
    while (token.type != TOKEN_EOF & token.type != TOKEN_ERROR) {
        token = lexer_next_token(&lexer);
    }
    EXPECT_EQ(token.type, TOKEN_ERROR);
    EXPECT_EQ(2259, token.start - src.start);
    EXPECT_EQ(1, token.end - token.start);
    EXPECT_STREQ(lexer_error_message(&lexer), "too many indentation levels");
    source_free(&src);
}

TEST(LexerTest, Keywords) {
    Lexer lexer;
    lexer_init(&lexer, "i if ifi e el els else elsew eli elif elif1\n");
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_IDENTIFIER, "i"}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_KW_IF, "if"}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_IDENTIFIER, "ifi"}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_IDENTIFIER, "e"}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_IDENTIFIER, "el"}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_IDENTIFIER, "els"}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_KW_ELSE, "else"}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_IDENTIFIER, "elsew"}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_IDENTIFIER, "eli"}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_KW_ELIF, "elif"}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_IDENTIFIER, "elif1"}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_NEWLINE, "\n"}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_EOF, ""}));
    EXPECT_EQ(lexer_error_message(&lexer), nullptr);
}

TEST(LexerTest, CmpOp) {
    Lexer lexer;
    lexer_init(&lexer, "= == != > >= <= < !a\n");
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_EQUALS, "="}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_EQ, "=="}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_NE, "!="}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_GT, ">"}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_GE, ">="}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_LE, "<="}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_LT, "<"}));
    EXPECT_EQ(lexer_next_token(&lexer), (EToken{TOKEN_ERROR, "!"}));
    EXPECT_STREQ(lexer_error_message(&lexer), "invalid syntax");
}
