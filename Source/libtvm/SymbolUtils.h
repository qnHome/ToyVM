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
#ifndef _SymbolUtils_h_
#define _SymbolUtils_h_

#include <stdint.h>
#include "Declarations.h"
#include "SharedLib.h"

typedef void* LibHandle;
typedef void* LibSymbol;

extern LibHandle LoadSharedLibrary(const str_t& modname, const str_t& moddir);
extern void      UnloadSharedLibrary(LibHandle handle);
extern LibSymbol GetSymbolAddress(LibHandle handle, const str_t& symname);
extern void      FindModuleDirectory(str_t& dest);
extern bool      IsModulePresent(const str_t& modname, const str_t& moddir);
extern void      DisplayModulePath(void);

#endif  //_SymbolUtils_h_
