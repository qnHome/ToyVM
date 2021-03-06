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
#include "ConsoleCurses.h"
#include <fcntl.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "MemoryStream.h"

#define KEY_SHIFT_DOWN_1 126 // f8 + Shift   OSX
#define KEY_SHIFT_DOWN_2 336 // down + Shift xterm
#define KEY_SHIFT_DOWN_3 284 // f8 + Shift   xterm


ConsoleCurses::ConsoleCurses() :
    m_buffer(nullptr),
    m_colorBuffer(nullptr),
    m_stdout(nullptr)
{
}

ConsoleCurses ::~ConsoleCurses()
{
    delete[] m_buffer;
    delete[] m_colorBuffer;

    endwin();
}

void ConsoleCurses::clear()
{
    if (m_buffer)
    {
        memset(m_buffer, ' ', m_size);
        memset(m_colorBuffer, CS_BLACK, m_size);
    }
}

void ConsoleCurses::switchOutput(bool on)
{
    if (on)
        m_stdout = freopen("/tmp/tdbg_stdout", "w", stdout);
    else
    {
        if (m_stdout != nullptr)
            fclose(m_stdout);

        freopen("/dev/tty", "w", stdout);

        readRedirectedOutput("/tmp/tdbg_stdout");
    }
}

void ConsoleCurses::readRedirectedOutput(const str_t &_path)
{
    FILE *fp = fopen(_path.c_str(), "r");
    if (fp)
    {
        fseek(fp, 0L, SEEK_END);
        long len = ftell(fp);
        if (len > 0)
        {
            fseek(fp, 0L, SEEK_SET);
            MemoryStream ms;
            ms.reserve(len);

            len = fread(ms.ptr(), 1, len, fp);
            if (len > 0)
                m_std += str_t((char *)ms.ptr(), len);
        }

        fclose(fp);
    }
}

int ConsoleCurses::nextCommand()
{
    int ch = getch(), rc = CCS_NO_INPUT;
    switch (ch)
    {
    case 'q':
        rc = CCS_QUIT;
        break;
    case 'r':
        rc = CCS_RESTART;
        break;
    case KEY_F(7):
    case 'b':
        rc = CCS_ADD_BREAKPOINT;
        break;
    case KEY_F(5):
    case 'c':
        rc = CCS_CONTINUE;
        break;
    case KEY_F(8):
    case KEY_DOWN:
        rc = CCS_STEP;
        break;

    // fall back (not guaranteed)
    // F4, F8+Shift, Down+Shift
    case KEY_F(4):
    case KEY_SHIFT_DOWN_1:
    case KEY_SHIFT_DOWN_2:
    case KEY_SHIFT_DOWN_3:
        rc = CCS_STEP_OUT;
        break;
    }
    return rc;
}

void ConsoleCurses::setCursorPosition(int x, int y)
{
    move(x, y);
}

void ConsoleCurses::showCursor(bool doit)
{
    curs_set(doit ? 1 : 0);
}

void ConsoleCurses::flush()
{
    move(0, 0);
    curs_set(0);
    int32_t c = 0, p;  // > 255

    size_t i, j, k;
    for (i = 0; i < m_displayRect.h; ++i)
    {
        for (j = 0; j < m_displayRect.w; ++j)
        {
            k = j + (i * m_displayRect.w);

            if (m_colorBuffer[k] != c)
                c = m_colorBuffer[k];

            p = COLOR_PAIR(c);
            move(i, j);
            delch();

            attron(p);
            insch(m_buffer[k]);
            attroff(p);
        }
    }
    refresh();
}


void ConsoleCurses::writeChar(char ch, uint8_t col, size_t k)
{
    if (k < m_size)
    {
        m_buffer[k]      = ch;
        m_colorBuffer[k] = col;
    }
}

uint8_t ConsoleCurses::getSwappedColor(uint8_t inp)
{
    switch (inp)
    {
    case CS_GREEN:
        inp = CS_GREEN;
        break;
    case CS_DARKGREEN:
        inp = CS_DARKGREEN;
        break;
    case CS_BLUE:  // blue and red are swapped here
        inp = CS_RED;
        break;
    case CS_DARKBLUE:
        inp = CS_DARKRED;
        break;
    case CS_RED:
        inp = CS_BLUE;
        break;
    case CS_DARKRED:
        inp = CS_DARKBLUE;
        break;
    case CS_DARKYELLOW:  // yellow and cyan are swapped here
        inp = CS_DARKCYAN;
        break;
    case CS_YELLOW:
        inp = CS_CYAN;
        break;
    case CS_DARKCYAN:
        inp = CS_DARKYELLOW;
        break;
    case CS_CYAN:
        inp = CS_YELLOW;
        break;
    case CS_DARKMAGENTA:
        inp = CS_DARKMAGENTA;
        break;
    case CS_MAGENTA:
        inp = CS_MAGENTA;
        break;
    case CS_GREY:
        inp = CS_GREY;
        break;
    case CS_LIGHT_GREY:
        inp = CS_LIGHT_GREY;
        break;
    case CS_WHITE:
        inp = CS_WHITE;
        break;
    case CS_BLACK:
        inp = CS_BLACK;
        break;
    }
    return inp;
}

int ConsoleCurses::create()
{
    initscr();  // never exits on failure
    keypad(stdscr, 1);
    noecho();
    curs_set(0);

    start_color();
    m_supportsColor = has_colors() != 0;
    use_default_colors();

    if (m_supportsColor)
    {
        uint8_t i, j, c;
        for (i = 0; i < 16; ++i)
        {
            for (j = 0; j < 16; ++j)
            {
                c = j + i * 16;
                uint8_t fg = getSwappedColor(j);
                uint8_t bg = getSwappedColor(i);
                init_pair(c, fg, bg == 0 ? -1 : bg);
            }
        }
    }


    int16_t r, c;
    getmaxyx(stdscr, r, c);
    if (r <= 0 || c <= 0)
    {
        printf("getmaxyx returned an invalid screen size\n");
        return -1;
    }

    m_displayRect = {0, 0, (int16_t)c, (int16_t)r};
    m_size        = (size_t)r * (size_t)c;

    m_buffer      = new uint8_t[m_size];
    m_colorBuffer = new uint8_t[m_size];

    memset(m_buffer, ' ', m_size);
    memset(m_colorBuffer, CS_BLACK, m_size);
    return 0;
}
