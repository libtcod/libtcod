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

#include "libtcod.hpp"

TCODText::TCODText(int x, int y, int w, int h, int max_chars){
	data=TCOD_text_init(x,y,w,h,max_chars);
}

TCODText::TCODText(int w, int h, int max_chars){
	data=TCOD_text_init2(w,h,max_chars);
}

TCODText::~TCODText(){
	TCOD_text_delete(data);
}

void TCODText::setPos(int x, int y) {
	TCOD_text_set_pos(data,x,y);
}

void TCODText::setProperties(int cursor_char, int blink_interval, const char * prompt, int tab_size){
	TCOD_text_set_properties(data,cursor_char,blink_interval,prompt,tab_size);	                            
}

void TCODText::setColors(TCODColor fore, TCODColor back, float back_transparency){
	TCOD_color_t foreground = {fore.r,fore.g,fore.b};
	TCOD_color_t background = {back.r,back.g,back.b};
	TCOD_text_set_colors(data,foreground,background,back_transparency);
}

bool TCODText::update(TCOD_key_t key){
	return TCOD_text_update(data,key) != 0;
}

void TCODText::render(TCODConsole * con){
	TCOD_text_render(data,con->data);
}

const char *TCODText::getText(){        
	return TCOD_text_get(data);
}

void TCODText::reset(){
	TCOD_text_reset(data);
}
