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
#include <stdio.h>
#include <string.h>
#include "BlockReader.h"




BlockReader::BlockReader(const char *fname) :
    m_block(),
    m_fileLen(0),
    m_loc(0),
    m_fp(0)
{
    open(fname);
}


BlockReader::BlockReader() :
    m_block(),
    m_fileLen(0),
    m_loc(0),
    m_fp(0)
{

}

BlockReader::~BlockReader()
{
    if (m_fp)
        fclose((FILE*)m_fp);
}

uint8_t BlockReader::next(void)
{
    if (m_loc <= 0 || (m_loc % BLOCKSIZE) == 0)
        read();

    uint8_t rc = 0;

    if (m_loc < m_fileLen)
        rc = m_block[m_loc % BLOCKSIZE];
    m_loc++;
    return rc;
}


void BlockReader::read()
{
    size_t br = fread(m_block, 1, BLOCKSIZE, (FILE *)m_fp);
    if (br >= 0 && br <= BLOCKSIZE)
        m_block[br] = 0;
}

void BlockReader::open(const char *fname)
{
    if (fname)
    {
        m_fp = fopen(fname, "rb");
        if (m_fp)
        {
            fseek((FILE *)m_fp, 0L, SEEK_END);
            m_fileLen = ftell((FILE *)m_fp);
            fseek((FILE *)m_fp, 0L, SEEK_SET);
        }
        else
            puts("failed to open file.");
    }
    else
        puts("Invalid file name.");
}