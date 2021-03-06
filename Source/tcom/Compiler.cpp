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
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>
#include "BinaryWriter.h"
#include "BlockReader.h"
#include "Declarations.h"
#include "Parser.h"
#include "SymbolUtils.h"

using namespace std;

typedef vector<string> strvec_t;

struct ProgramInfo
{
    string   output;
    strvec_t files;
    strvec_t modules;
    bool     disableErrorFmt;
    string   modulePath;
};

void usage(void);

int main(int argc, char **argv)
{
    if (argc <= 1)
    {
        usage();
        return 0;
    }

    ProgramInfo ctx = {};
    int         i;
    for (i = 1; i < argc; ++i)
    {
        if (argv[i][0] == '-')
        {
            switch (argv[i][1])
            {
            case 'h':
                usage();
                exit(0);
                break;
            case 'o':
                if (i + 1 < argc)
                    ctx.output = (argv[++i]);
                break;
            case 'l':
                if (i + 1 < argc)
                    ctx.modules.push_back((argv[++i]));
                break;
            case 'm':
                DisplayModulePath();
                return 0;
                break;
            case 'd':
                ctx.disableErrorFmt = true;
                break;
            default:
                break;
            }
        }
        else
            ctx.files.push_back(argv[i]);
    }

    if (ctx.output.empty())
    {
        usage();
        cout << "missing output file.\n";
        return PS_ERROR;
    }

    if (find(ctx.modules.begin(), ctx.modules.end(), "std") == ctx.modules.end())
        ctx.modules.push_back("std");

    FindModuleDirectory(ctx.modulePath);

    BinaryWriter w(ctx.modulePath);
    for (string file : ctx.files)
    {
        Parser p;
        if (ctx.disableErrorFmt)
            p.disableErrorFormat(true);

        if (p.parse(file.c_str()) != PS_OK)
            return PS_ERROR;

        if (w.mergeLabels(p.getLabels()) != PS_OK)
            return PS_ERROR;

        if (w.mergeDataDeclarations(p.getDataDeclarations()) != PS_OK)
            return PS_ERROR;

        w.mergeInstructions(p.getInstructions());

        // This has not been tested on multiple files yet.
        break;
    }

    if (w.resolve(ctx.modules) != PS_OK)
        return PS_ERROR;
    if (w.open(ctx.output.c_str()) != PS_OK)
        return PS_ERROR;
    if (w.writeHeader() != PS_OK)
        return PS_ERROR;
    if (w.writeSections() != PS_OK)
        return PS_ERROR;
    return 0;
}

void usage(void)
{
    cout << "tcom <options> <input file>\n\n";
    cout << "    options:\n\n";
    cout << "        -h show this message.\n";
    cout << "        -o output file.\n";
    cout << "        -l link library.\n";
    cout << "        -d disable full path when reporting errors.\n";
    cout << "        -m print the module path and exit.\n";
    cout << "\n";
}
