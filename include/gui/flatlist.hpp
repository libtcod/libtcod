/*
* libtcod 1.6.3
* Copyright (c) 2008,2009,2010,2012,2013,2016,2017 Jice & Mingos & rmtew
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
* THIS SOFTWARE IS PROVIDED BY JICE, MINGOS AND RMTEW ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL JICE, MINGOS OR RMTEW BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
class TCODLIB_GUI_API FlatList : public TextBox {
public :
	FlatList(int x,int y,int w, const char **list, const char *label, const char *tip=NULL);
	virtual ~FlatList();
	void render();
	void update(const TCOD_key_t k);
	void setCallback(void (*cbk)(Widget *wid, const char * val, void *data), void *data) { this->cbk=cbk; this->data=data;}
	void setValue(const char * value);
	void setList(const char **list);
protected :
	const char **value;
	const char **list;
	bool onLeftArrow;
	bool onRightArrow;
	void (*cbk)(Widget *wid, const char *val, void *data);
	void *data;

	void valueToText();
	void textToValue();
	void onButtonClick();
};

