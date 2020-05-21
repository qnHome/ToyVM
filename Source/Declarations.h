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
#ifndef _Declarations_h_
#define _Declarations_h_

#include <stdint.h>
#include <string.h>
#include <string>

#define INS_ARG 3
#define MAX_KWD 6

// return value for actions that need to scan
// for more information before returning a token.
#define CONTINUE -1
// Common error return code
#define UNDEFINED -1

#define TYPE_ID4(a, b, c, d) ((int)(d) << 24 | (int)(c) << 16 | (b) << 8 | (a))
#define TYPE_ID2(a, b) ((b) << 8 | (a))

typedef union Register {
    uint8_t  b[8];
    uint16_t w[4];
    uint32_t l[2];
    uint64_t x;
} Register;

enum ProgramFlags
{
    PF_E = 1 << 0,
    PF_G = 1 << 1,
    PF_L = 1 << 2,
};

enum ParseResult
{
    PS_ERROR = -10,
    PS_OK,
};

struct Token
{
    uint8_t     op;
    uint8_t     reg;
    Register    ival;
    int32_t     type;
    std::string value;
    int32_t     index;
    bool        hasComma;
};

enum TokenCode
{
    TOK_OPCODE,
    TOK_REGISTER,
    TOK_IDENTIFIER,
    TOK_DIGIT,
    TOK_LABEL,
    TOK_SECTION,
    TOK_EOL,
    TOK_MAX,
};

enum ParserState
{
    ST_INITIAL = 0,
    ST_ID,
    ST_DIGIT,
    ST_SECTION,
    ST_CONTINUE,
    ST_MAX,
};

enum Opcode
{
    OP_BEG,
    OP_RET,   // ret
    OP_MOV,   // mov r(n), src
    OP_CALL,  // call address
    OP_INC,   // inc, r(n)
    OP_DEC,   // dec, r(n)
    OP_CMP,   // cmp, r(n), src
    OP_JMP,   // jump
    OP_JEQ,   // jump ==
    OP_JNE,   // jump !
    OP_JLT,   // jump <
    OP_JGT,   // jump >
    OP_JLE,   // jump <=
    OP_JGE,   // jump >=
    OP_ADD,   // add r(n), src
    OP_SUB,   // sub r(n), src
    OP_MUL,   // mul r(n), src
    OP_DIV,   // div r(n), src
    OP_SHR,   // shr r(n), src
    OP_SHL,   // shl r(n), src
              // ---- debugging ----
    OP_PRG,   // print register
    OP_PRGI,  // print all registers
    OP_MAX
};

enum ArgType
{
    AT_REG,
    AT_LIT,
    AT_ADDR,
    AT_REGLIT,
    AT_NULL,
};

typedef char Keyword[MAX_KWD + 1];

struct KeywordMap
{
    Keyword word;
    uint8_t op;
    uint8_t narg;
    uint8_t argv[INS_ARG];
};

enum SectionCodes
{
    SEC_DAT = 0xFF,
    SEC_TXT,
    SEC_STR,
};

enum InstructionFlags
{
    IF_DREG = (1 << 0),
    IF_DLIT = (1 << 1),
    IF_SREG = (1 << 2),
    IF_SLIT = (1 << 3),
    IF_ADDR = (1 << 4),
    IF_MAX  = IF_DREG | IF_DLIT | IF_SREG | IF_SLIT | IF_ADDR
};

struct TVMHeader
{
    uint16_t code;
    uint8_t  flags;
    uint8_t  txt;
    uint32_t dat;
    uint64_t str;
};

struct TVMSection
{
    uint16_t code;
    uint16_t flags;
    uint64_t entry;
    uint32_t pad;
    uint32_t size;
    uint32_t start;
    uint64_t padding;
};

struct Instruction
{
    uint8_t     op;
    uint8_t     flags;
    uint8_t     argc;
    uint64_t    argv[INS_ARG];
    uint64_t    label;
    std::string labelName;
};

struct ExecInstruction
{
    uint8_t  op;
    uint8_t  flags;
    uint8_t  argc;
    uint64_t argv[INS_ARG];
};

#define _RELITAVE_TIME_CHECK_BEGIN                                    \
    {                                                                 \
        chrono::high_resolution_clock::time_point begintick, endtick; \
        begintick = chrono::high_resolution_clock().now();

#define _RELITAVE_TIME_CHECK_END                                      \
    endtick = chrono::high_resolution_clock().now();                  \
    cout << __FUNCTION__ << " exec("                                  \
         << fixed << setprecision(6)                                  \
         << ((chrono::duration<double>(endtick - begintick).count())) \
         << "s)"                                                      \
         << endl;                                                     \
    }

#endif  // _Declarations_h_
