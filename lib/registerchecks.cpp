/*
 * Cppcheck - A tool for static C/C++ code analysis
 * Copyright (C) 2007-2011 Daniel Marjamäki and Cppcheck team.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "registerchecks.h"

#include "checkautovariables.h"
#include "checkbufferoverrun.h"
#include "checkclass.h"
#include "checkexceptionsafety.h"
#include "check.h"
#include "checkmemoryleak.h"
#include "checknullpointer.h"
#include "checkobsoletefunctions.h"
#include "checkother.h"
#include "checkpostfixoperator.h"
#include "checkstl.h"
#include "checkuninitvar.h"
#include "checkunusedfunctions.h"

#include <cassert>
#include <cstdlib>
#include <iostream>

RegisterChecks::RegisterChecks()
{
    Check::ChecksList& checks = Check::instances();
    assert(checks.empty());

    // Every built-in Check class must be listed explicitly here
    Check* checksarr[] =
    {
        new CheckAutoVariables()
        ,new CheckBufferOverrun()
        ,new CheckClass()
        ,new CheckExceptionSafety()
        ,new CheckMemoryLeakInClass()
        ,new CheckMemoryLeakInFunction()
        ,new CheckMemoryLeakNoVar()
        ,new CheckMemoryLeakStructMember()
        ,new CheckNullPointer()
        ,new CheckObsoleteFunctions()
        ,new CheckOther()
        ,new CheckPostfixOperator()
        ,new CheckStl()
        ,new CheckUninitVar()
        ,new CheckUnusedFunctions()
    };

    const size_t n = sizeof(checksarr)/sizeof(Check*);
    _builtins.insert(checksarr, checksarr + n);
    checks.insert(checksarr, checksarr + n);
}

RegisterChecks::~RegisterChecks()
{
    Check::ChecksList::iterator f,
          e = _builtins.end();
    // Remove from Check::instances and deallocate
    for (f = _builtins.begin(); f != e; ++f)
    {
        Check::instances().erase(*f);
        delete *f;
    }
    _builtins.clear();
}
