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
class TCODLIB_GUI_API Slider : public TextBox {
public :
	Slider(int x,int y,int w, float min, float max, const char *label, const char *tip=NULL);
	virtual ~Slider();
	void render();
	void update(const TCOD_key_t k);
	void setMinMax(float min, float max) { this->min=min;this->max=max; }
	void setCallback(void (*cbk)(Widget *wid, float val, void *data), void *data) { this->cbk=cbk; this->data=data;}
	void setFormat(const char *fmt);
	void setValue(float value);
	void setSensitivity(float sensitivity) { this->sensitivity=sensitivity;}
protected :
	float min,max,value,sensitivity;
	bool onArrows;
	bool drag;
	int dragx;
	int dragy;
	float dragValue;
	char *fmt;
	void (*cbk)(Widget *wid, float val, void *data);
	void *data;

	void valueToText();
	void textToValue();
	void onButtonPress();
	void onButtonRelease();
};

