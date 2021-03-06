/*
-------------------------------------------------------------------------------
    Copyright (c) 2020 Charles Carley.

  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
-------------------------------------------------------------------------------
*/
#include "Parser.h"
#include "Catch2.h"

TEST_CASE("Scan1")
{
    const std::string TestFile = std::string(TestDirectory) + "/Scan/Scan1.asm";

    Parser p;

    int sr = p.open(TestFile.c_str());
    EXPECT_EQ(sr, PS_OK);

    Token tok = {};
    sr        = p.scan(tok);
    EXPECT_NE(sr, PS_ERROR);
    EXPECT_EQ(tok.type, TOK_IDENTIFIER);
    EXPECT_EQ(tok.value, "abc");
    sr = p.scan(tok);
    EXPECT_EQ(sr, PS_EOF);
}

TEST_CASE("Scan2")
{
    const std::string TestFile = std::string(TestDirectory) + "/Scan/Scan2.asm";

    Parser p;
    int    sr = p.open(TestFile.c_str());
    EXPECT_EQ(sr, PS_OK);

    Token tok;
    sr = p.scan(tok);
    EXPECT_EQ(tok.type, TOK_LABEL);
    EXPECT_EQ(tok.value, "main");

    sr = p.scan(tok);
    EXPECT_EQ(tok.type, TOK_LABEL);
    EXPECT_EQ(tok.value, "abc");
    sr = p.scan(tok);
    EXPECT_EQ(tok.type, TOK_LABEL);
    EXPECT_EQ(tok.value, "def");

    sr = p.scan(tok);
    EXPECT_EQ(tok.type, TOK_LABEL);
    EXPECT_EQ(tok.value, "ghi");
    sr = p.scan(tok);
    EXPECT_EQ(tok.type, TOK_OPCODE);
    EXPECT_EQ(tok.op, OP_MOV);

    sr = p.scan(tok);
    EXPECT_EQ(tok.type, TOK_REGISTER);
    EXPECT_EQ(tok.reg, 0);

    sr = p.scan(tok);
    EXPECT_EQ(tok.type, TOK_DIGIT);
    EXPECT_EQ(tok.ival.x, 0);

    sr = p.scan(tok);
    EXPECT_EQ(tok.type, PS_OK);
}

TEST_CASE("Scan3")
{
    const std::string TestFile = std::string(TestDirectory) + "/Scan/Scan3.asm";

    Parser p;
    int    sr = p.open(TestFile.c_str());
    EXPECT_EQ(sr, PS_OK);

    Token tok;
    sr = p.scan(tok);
    EXPECT_EQ(tok.type, TOK_LABEL);
    EXPECT_EQ(tok.value, "main");

    sr = p.scan(tok);
    EXPECT_EQ(tok.type, TOK_OPCODE);
    EXPECT_EQ(tok.op, OP_MOV);
    sr = p.scan(tok);
    EXPECT_EQ(tok.type, TOK_REGISTER);
    EXPECT_EQ(tok.reg, 0);
    sr = p.scan(tok);
    EXPECT_EQ(tok.type, TOK_DIGIT);
    EXPECT_EQ(tok.ival.x, 0);

    sr = p.scan(tok);
    EXPECT_EQ(tok.type, TOK_LABEL);
    EXPECT_EQ(tok.value, "abc");

    sr = p.scan(tok);
    EXPECT_EQ(tok.type, TOK_OPCODE);
    EXPECT_EQ(tok.op, OP_MOV);
    sr = p.scan(tok);
    EXPECT_EQ(tok.type, TOK_REGISTER);
    EXPECT_EQ(tok.reg, 1);
    sr = p.scan(tok);
    EXPECT_EQ(tok.type, TOK_DIGIT);
    EXPECT_EQ(tok.ival.x, 5);

    sr = p.scan(tok);
    EXPECT_EQ(tok.type, PS_OK);
}

TEST_CASE("Scan4")
{
    const std::string TestFile = std::string(TestDirectory) + "/Scan/Scan4.asm";

    Parser p;
    int    sr = p.parse(TestFile.c_str());
    EXPECT_EQ(sr, PS_ERROR);
}

TEST_CASE("Scan5")
{
    const std::string TestFile = std::string(TestDirectory) + "/Scan/Scan5.asm";

    Token  tok;
    Parser p;
    int    sr = p.open(TestFile.c_str());
    EXPECT_EQ(sr, PS_OK);

    p.scan(tok);
    EXPECT_EQ(tok.type, TOK_SECTION);
    EXPECT_EQ(tok.value, "data");

    p.scan(tok);
    EXPECT_EQ(tok.type, TOK_LABEL);
    EXPECT_EQ(tok.value, "message1");

    p.scan(tok);
    EXPECT_EQ(tok.type, TOK_SECTION);
    EXPECT_EQ(tok.value, "asciz");

    p.scan(tok);
    EXPECT_EQ(tok.type, TOK_ASCII);
    EXPECT_EQ(tok.value, "Hello World #1");

    p.scan(tok);
    EXPECT_EQ(tok.type, TOK_LABEL);
    EXPECT_EQ(tok.value, "message2");

    p.scan(tok);
    EXPECT_EQ(tok.type, TOK_SECTION);
    EXPECT_EQ(tok.value, "asciz");

    p.scan(tok);
    EXPECT_EQ(tok.type, TOK_ASCII);
    EXPECT_EQ(tok.value, "Hello World #2");

    p.scan(tok);
    EXPECT_EQ(tok.type, TOK_LABEL);
    EXPECT_EQ(tok.value, "message3");

    p.scan(tok);
    EXPECT_EQ(tok.type, TOK_SECTION);
    EXPECT_EQ(tok.value, "asciz");

    p.scan(tok);
    EXPECT_EQ(tok.type, TOK_ASCII);
    EXPECT_EQ(tok.value, "Hello World #3");

    p.scan(tok);
    EXPECT_EQ(tok.type, TOK_LABEL);
    EXPECT_EQ(tok.value, "message4");

    p.scan(tok);
    EXPECT_EQ(tok.type, TOK_SECTION);
    EXPECT_EQ(tok.value, "asciz");

    p.scan(tok);
    EXPECT_EQ(tok.type, TOK_ASCII);
    EXPECT_EQ(tok.value, "Hello World #4");

    p.scan(tok);
    EXPECT_EQ(tok.type, TOK_LABEL);
    EXPECT_EQ(tok.value, "message5");

    p.scan(tok);
    EXPECT_EQ(tok.type, TOK_SECTION);
    EXPECT_EQ(tok.value, "asciz");

    p.scan(tok);
    EXPECT_EQ(tok.type, TOK_ASCII);
    EXPECT_EQ(tok.value, "Hello World #5");
}

TEST_CASE("Scan6")
{
    const std::string TestFile = std::string(TestDirectory) + "/Scan/Scan6.asm";

    Parser p;
    int    sr = p.parse(TestFile.c_str());
    EXPECT_EQ(sr, PS_OK);
}
