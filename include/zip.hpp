/*
* libtcod 1.4.1
* Copyright (c) 2008,2009 Jice
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * The name of Jice may not be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY Jice ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL Jice BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _TCOD_ZIP_HPP
#define _TCOD_ZIP_HPP

class TCODLIB_API TCODZip {
public :
	TCODZip();
	~TCODZip();

	// output interface
	void putChar(char val);
	void putInt(int val);
	void putFloat(float val);
	void putString(const char *val);
	void putData(int nbBytes, const void *data);
	void putColor(const TCODColor *val);
	void putImage(const TCODImage *val);
	void putConsole(const TCODConsole *val);
	int saveToFile(const char *filename);

	// input interface
	int loadFromFile(const char *filename);
	char getChar();
	int getInt();
	float getFloat();
	const char *getString();
	TCODColor getColor();
	TCODImage *getImage();
	TCODConsole *getConsole();
	int getData(int nbBytes, void *data);
protected :
	TCOD_zip_t data;
};

#endif
