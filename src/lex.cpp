/*
* libtcod 1.5.2
* Copyright (c) 2008,2009,2010,2012 Jice & Mingos
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * The name of Jice or Mingos may not be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY JICE AND MINGOS ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL JICE OR MINGOS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "libtcod.hpp"
#include "libtcod_int.h"

TCODLex::TCODLex( const char **_symbols, const char **_keywords, const char *simpleComment,
		const char *commentStart, const char *commentStop, const char *javadocCommentStart,
		const char *_stringDelim, int _flags)
{
	data=(void *)TCOD_lex_new(_symbols,_keywords,simpleComment,commentStart,commentStop,javadocCommentStart,_stringDelim,_flags);

}

TCODLex::TCODLex()
{
	data= (void *)TCOD_lex_new_intern();
}

TCODLex::~TCODLex()
{
	TCOD_lex_delete((TCOD_lex_t *)data);
}

char *TCODLex::getLastJavadoc()
{
	return TCOD_lex_get_last_javadoc((TCOD_lex_t *)data);
}


void TCODLex::setDataBuffer(char *dat)
{
	TCOD_lex_set_data_buffer((TCOD_lex_t *)data,dat);
}

bool TCODLex::setDataFile(const char *_filename)
{
	return TCOD_lex_set_data_file((TCOD_lex_t *)data,_filename) != 0;
}

int TCODLex::parse()
{
	return TCOD_lex_parse((TCOD_lex_t *)data);
}


int TCODLex::parseUntil(int tokenType)
{
	return TCOD_lex_parse_until_token_type((TCOD_lex_t *)data,tokenType);
}

int TCODLex::parseUntil(const char *tokenValue)
{
	return TCOD_lex_parse_until_token_value((TCOD_lex_t *)data,tokenValue);
}

void TCODLex::savepoint(TCODLex *savepoint)
{
	TCOD_lex_savepoint((TCOD_lex_t *)data,(TCOD_lex_t *)(savepoint->data));
}

void TCODLex::restore(TCODLex *savepoint)
{
	TCOD_lex_restore((TCOD_lex_t *)data,(TCOD_lex_t *)(savepoint->data));
}

bool TCODLex::expect(int tokenType)
{
	return TCOD_lex_expect_token_type((TCOD_lex_t *)data,tokenType) != 0;
}

bool TCODLex::expect(int tokenType, const char *tokenValue)
{
	return TCOD_lex_expect_token_value((TCOD_lex_t *)data,tokenType,tokenValue) != 0;
}

