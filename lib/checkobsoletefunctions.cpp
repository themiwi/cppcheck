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

//---------------------------------------------------------------------------
// Obsolete functions
//---------------------------------------------------------------------------

#include "checkobsoletefunctions.h"

//---------------------------------------------------------------------------


void CheckObsoleteFunctions::obsoleteFunctions()
{
    if (!_settings->_checkCodingStyle)
        return;

    // Don't check C# and Java code
    if (_tokenizer->isJavaOrCSharp())
        return;

    for (const Token *tok = _tokenizer->tokens(); tok; tok = tok->next())
    {
        std::list< std::pair<const std::string, const std::string> >::const_iterator it(_obsoleteFunctions.begin()), itend(_obsoleteFunctions.end());
        for (; it!=itend; ++it)
        {
            if (tok->strAt(1) == it->first && tok->strAt(2) == "(" && tok->tokAt(1)->varId() == 0 && !tok->tokAt(0)->isName() && !Token::Match(tok, ".|::|:|,"))
            {
                // If checking an old code base it might be uninteresting to update obsolete functions.
                // Therefore this is "style"
                reportError(tok->tokAt(1), Severity::style, "obsoleteFunctions"+it->first, it->second);
                break;
            }
        }
    }
}
//---------------------------------------------------------------------------

