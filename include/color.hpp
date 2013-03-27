/*
* libtcod 1.6.0
* Copyright (c) 2008,2009,2010,2012,2013 Jice & Mingos
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

#ifndef _TCOD_COLOR_HPP
#define _TCOD_COLOR_HPP

// color constants uses to generate @ColorTable
/**
	@ColorCategory STANDARD COLORS
	@Color red 255,0,0
	@Color flame 255,63,0
	@Color orange 255,127,0
	@Color amber 255,191,0
	@Color yellow 255,255,0,
	@Color lime 191,255,0
	@Color chartreuse 127,255,0
	@Color green 0,255,0
	@Color sea 0,255,127
	@Color turquoise 0,255,191
	@Color cyan 0,255,255
	@Color sky 0,191,255
	@Color azure 0,127,255
	@Color blue 0,0,255
	@Color han 63,0,255
	@Color violet 127,0,255
	@Color purple 191,0,255
	@Color fuchsia 255,0,255
	@Color magenta 255,0,191
	@Color pink 255,0,127
	@Color crimson 255,0,63
	@ColorCategory METALLIC COLORS
	@Color brass 191,151,96
	@Color copper 196,136,124
	@Color gold 229,191,0
	@Color silver 203,203,203
	@ColorCategory MISCELLANEOUS COLORS
	@Color celadon 172,255,175
	@Color peach 255,159,127
	@ColorCategory GREYSCALE & SEPIA
	@Color grey 127,127,127
	@Color sepia 127,101,63
	@ColorCategory BLACK AND WHITE
	@Color black 0,0,0
	@Color white 255,255,255
*/

/**
	@PageName color
	@PageCategory Core
	@PageTitle Colors
	@PageDesc The Doryen library uses 32bits colors. Thus, your OS desktop must use 32bits colors.
A color is defined by its red, green and blue component between 0 and 255.
You can use the following predefined colors (hover over a color to see its full name and R,G,B values):
	@ColorTable
	@CppEx 
		TCODColor::desaturatedRed
		TCODColor::lightestRed
		TCODColor::lighterRed
		TCODColor::lightRed
		TCODColor::red
		TCODColor::darkRed
		TCODColor::darkerRed
		TCODColor::darkestRed
	@CEx 
		TCOD_desaturated_red
		TCOD_lightest_red
		TCOD_lighter_red
		TCOD_light_red
		TCOD_red
		TCOD_dark_red
		TCOD_darker_red
		TCOD_darkest_red
	@PyEx 
		libtcod.desaturated_red
		libtcod.lightest_red
		libtcod.lighter_red
		libtcod.light_red
		libtcod.red
		libtcod.dark_red
		libtcod.darker_red
		libtcod.darkest_red
	@C#Ex 
		TCODColor::desaturatedRed
		TCODColor::lightestRed
		TCODColor::lighterRed
		TCODColor::lightRed
		TCODColor::red
		TCODColor::darkRed
		TCODColor::darkerRed
		TCODColor::darkestRed
	@LuaEx
		tcod.color.desaturatedRed
		tcod.color.lightestRed
		tcod.color.lighterRed
		tcod.color.lightRed
		tcod.color.red
		tcod.color.darkRed
		tcod.color.darkerRed
		tcod.color.darkestRed
 */

class TCODLIB_API TCODColor {
public :
	uint8 r,g,b;

	TCODColor() : r(0),g(0),b(0) {}
	/**
	@PageName color
	@FuncTitle Create your own colors
	@FuncDesc You can create your own colours using a set of constructors, both for RGB and HSV values.
	@CppEx 
		TCODColor myColor(24,64,255); //RGB
		TCODColor myOtherColor(321.0f,0.7f,1.0f); //HSV
	@CEx 
		TCOD_color_t my_color={24,64,255}; <span>/</span>* RGB *<span>/</span>
		TCOD_color_t my_other_color = TCOD_color_RGB(24,64,255); <span>/</span>* RGB too *<span>/</span>
		TCOD_color_t my_yet_another_color = TCOD_color_HSV(321.0f,0.7f,1.0f); <span>/</span>* HSV *<span>/</span>
	@PyEx my_color=libtcod.Color(24,64,255)
	@C#Ex TCODColor myColor = new TCODColor(24,64,255); //RGB
	     TCODColor myColor = new TCODColor(321.0f,0.7f,1.0f); //HSV
	@LuaEx myColor = tcod.Color(24,24,255)
	*/
	TCODColor(uint8 r, uint8 g, uint8 b): r(r), g(g), b(b) {}
	TCODColor(int r, int g, int b): r(r), g(g), b(b) {}
	TCODColor(const TCOD_color_t &col): r(col.r), g(col.g), b(col.b) {}
	TCODColor(float h, float s, float v);

	/**
	@PageName color
	@FuncTitle Compare two colors
	@CppEx 
		if (myColor == TCODColor::yellow) { ... }
		if (myColor != TCODColor::white) { ... }
	@CEx 
		if (TCOD_color_equals(my_color,TCOD_yellow)) { ... }
		if (!TCOD_color_equals(my_color,TCOD_white)) { ... }
	@PyEx 
		if my_color == libtcod.yellow : ...
		if my_color != litbcod.white : ...
	@C#Ex 
		if (myColor.Equal(TCODColor.yellow)) { ... }
		if (myColor.NotEqual(TCODColor.white)) { ... }
	@LuaEx 
		if myColor == tcod.color.yellow then ... end
	*/
	bool operator == (const TCODColor & c) const {
		return (c.r == r && c.g == g && c.b == b);
	}
	bool operator != (const TCODColor & c) const {
		return (c.r != r || c.g != g || c.b != b);
	}

	/**
	@PageName color
	@FuncTitle Multiply two colors
	@FuncDesc c1 = c2 * c3 =>
    c1.r = c2.r * c3.r / 255
    c1.g = c2.g * c3.g / 255
    c1.b = c2.b * c3.b / 255
	darkishRed = darkGrey * red
<table><tr><td style="background-color: rgb(96, 0, 0); width: 60px; height: 30px;"></td><td style="background-color: rgb(96, 96, 96); width: 60px;"></td><td style="background-color: rgb(255, 0, 0); width: 60px;"></td></tr></table>
	@CppEx TCODColor myDarkishRed = TCODColor::darkGrey * TCODColor::lightRed;
	@CEx TCOD_color_t my_darkish_red = TCOD_color_multiply(TCOD_dark_grey, TCOD_light_red);
	@PyEx my_darkish_red = libtcod.dark_grey * libtcod.light_red
	@C#Ex TCODColor myDarkishRed = TCODColor.darkGrey.Multiply(TCODColor.lightRed);
	@LuaEx myDarkishRed = tcod.color.darkGrey * tcod.color.lightRed
	*/
	TCODColor operator * (const TCODColor & a) const {
		TCODColor ret;
		ret.r=(uint8)(((int)r)*a.r/255);
		ret.g=(uint8)(((int)g)*a.g/255);
		ret.b=(uint8)(((int)b)*a.b/255);
		return ret;
	}

	/**
	@PageName color
	@FuncTitle Multiply a color by a float
	@FuncDesc c1 = c2 * v =>
    c1.r = CLAMP(0, 255, c2.r * v)
    c1.g = CLAMP(0, 255, c2.g * v)
    c1.b = CLAMP(0, 255, c2.b * v)
	darkishRed = red * 0.5
<table><tr><td style="background-color: rgb(128, 0, 0); width: 60px; height: 30px;"></td><td style="background-color: rgb(255, 0, 0); width: 60px;"></td><td style="width: 60px;"></td></tr></table>
</tbody>
	@CppEx TCODColor myDarkishRed = TCODColor::lightRed * 0.5f;
	@CEx TCOD_color_t my_darkish_red = TCOD_color_multiply_scalar(TCOD_light_red, 0.5f);
	@PyEx myDarkishRed = litbcod.light_red * 0.5
	@C#Ex TCODColor myDarkishRed = TCODColor.lightRed.Multiply(0.5f);
	@LuaEx myDarkishRed = tcod.color.lightRed * 0.5
	*/
	TCODColor operator *(float value) const {
		TCOD_color_t ret;
		int r,g,b;
		r = (int)(this->r * value);
		g = (int)(this->g * value);
		b = (int)(this->b * value);
		r = CLAMP(0,255,r);
		g = CLAMP(0,255,g);
		b = CLAMP(0,255,b);
		ret.r=(uint8)r;
		ret.g=(uint8)g;
		ret.b=(uint8)b;
		return ret;
	}

	/**
	@PageName color
	@FuncTitle Adding two colors
	@FuncDesc c1 = c1 + c2 => c1.r = MIN(255, c1.r + c2.r)
                  c1.g = MIN(255, c1.g + c2.g)
                  c1.b = MIN(255, c1.b + c2.b)
	lightishRed = red + darkGrey
<table><tr><td style="background-color: rgb(255, 128, 128); width: 60px; height: 30px;"></td><td style="background-color: rgb(255, 0, 0); width: 60px;"></td><td style="background-color: rgb(128, 128, 128); width: 60px;"></td></tr></table>
	@CppEx TCODColor myLightishRed = TCODColor::red + TCODColor::darkGrey
	@CEx TCOD_color_t my_lightish_red = TCOD_color_add(TCOD_red, TCOD_dark_grey);
	@PyEx myLightishRed = libtcod.red + libtcod.dark_grey
	@C#Ex TCODColor myLightishRed = TCODColor.red.Plus(TCODColor.darkGrey)
	@LuaEx myLightishRed = tcod.color.red + tcod.color.darkGrey
	*/
	TCODColor operator + (const TCODColor & a) const {
		TCODColor ret;
		int r=(int)(this->r)+a.r;
		int g=(int)(this->g)+a.g;
		int b=(int)(this->b)+a.b;
		r = MIN(255,r);
		g = MIN(255,g);
		b = MIN(255,b);
		ret.r=(uint8)r;
		ret.g=(uint8)g;
		ret.b=(uint8)b;
		return ret;
	}

	/**
	@PageName color
	@FuncTitle Subtract two colors
	@FuncDesc  c1 = c1 - c2 => c1.r = MAX(0, c1.r - c2.r)
                  c1.g = MAX(0, c1.g - c2.g)
                  c1.b = MAX(0, c1.b - c2.b)
	redish = red - darkGrey
<table><tr><td style="background-color: rgb(127, 0, 0); width: 60px; height: 30px;"></td><td style="background-color: rgb(255, 0, 0); width: 60px;"></td><td style="background-color: rgb(128, 128, 128); width: 60px;"></td></tr></table>
	@CppEx TCODColor myRedish = TCODColor::red - TCODColor::darkGrey
	@CEx TCOD_color_t my_redish = TCOD_color_subtract(TCOD_red, TCOD_dark_grey);
	@PyEx myRedish = libtcod.red - libtcod.dark_grey
	@C#Ex TCODColor myRedish = TCODColor.red.Minus(TCODColor.darkGrey)
	@LuaEx myRedish = tcod.color.red - tcod.color.darkGrey
	*/
	TCODColor operator - (const TCODColor & a) const {
		TCODColor ret;
		int r=(int)(this->r)-a.r;
		int g=(int)(this->g)-a.g;
		int b=(int)(this->b)-a.b;
		r = MAX(0,r);
		g = MAX(0,g);
		b = MAX(0,b);
		ret.r=(uint8)r;
		ret.g=(uint8)g;
		ret.b=(uint8)b;
		return ret;
	}

	/**
	@PageName color
	@FuncTitle Interpolate between two colors
	@FuncDesc   c1 = lerp (c2, c3, coef) => c1.r = c2.r  + (c3.r - c2.r ) * coef
                              c1.g = c2.g  + (c3.g - c2.g ) * coef
                              c1.b = c2.b  + (c3.b - c2.b ) * coef
coef should be between 0.0 and 1.0 but you can as well use other values
<table><tr><td style="background-color: rgb(96, 96, 96); color: rgb(255, 255, 255);" align="center">coef == 0.0f</td><td style="background-color: rgb(96, 96, 96); width: 60px;"></td><td style="background-color: rgb(255, 0, 0); width: 60px;"></td></tr>
	<tr><td style="background-color: rgb(135, 72, 72); color: rgb(255, 255, 255);" align="center">coef == 0.25f</td><td style="background-color: rgb(96, 96, 96); width: 60px;"></td><td style="background-color: rgb(255, 0, 0); width: 60px;"></td></tr>
	<tr><td style="background-color: rgb(175, 48, 48); color: rgb(255, 255, 255);" align="center">coef == 0.5f</td><td style="background-color: rgb(96, 96, 96); width: 60px;"></td><td style="background-color: rgb(255, 0, 0); width: 60px;"></td></tr>

	<tr><td style="background-color: rgb(215, 24, 24); color: rgb(255, 255, 255);" align="center">coef == 0.75f</td><td style="background-color: rgb(96, 96, 96); width: 60px;"></td><td style="background-color: rgb(255, 0, 0); width: 60px;"></td></tr>
	<tr><td style="background-color: rgb(255, 0, 0); color: rgb(255, 255, 255);" align="center">coef == 1.0f</td><td style="background-color: rgb(96, 96, 96); width: 60px;"></td><td style="background-color: rgb(255, 0, 0); width: 60px;"></td></tr></table>
	@CppEx TCODColor myColor = TCODColor::lerp ( TCODColor::darkGrey, TCODColor::lightRed,coef );
	@CEx TCOD_color_t my_color = TCOD_color_lerp ( TCOD_dark_grey, TCOD_light_red,coef);
	@PyEx my_color = libtcod.color_lerp ( libtcod.dark_grey, litbcod.light_red,coef)
	@C#Ex TCODColor myColor = TCODColor.Interpolate( TCODColor.darkGrey, TCODColor.lightRed, coef );
	@LuaEx myColor = tcod.color.Interpolate( tcod.color.darkGrey, tcod.color.lightRed, coef )
	*/
	static TCODColor lerp(const TCODColor &a, const TCODColor &b, float coef) {
		TCODColor ret;
		ret.r=(uint8)(a.r+(b.r-a.r)*coef);
		ret.g=(uint8)(a.g+(b.g-a.g)*coef);
		ret.b=(uint8)(a.b+(b.b-a.b)*coef);
		return ret;
	}

	/**
	@PageName color
	@FuncTitle Define a color by its hue, saturation and value
	@FuncDesc After this function is called, the r,g,b fields of the color are calculated according to the h,s,v parameters.
	@Cpp void TCODColor::setHSV(float h, float s, float v)
	@C void TCOD_color_set_HSV(TCOD_color_t *c,float h, float s, float v)
	@Py color_set_hsv(c,h,s,v)
	@C# void TCODColor::setHSV(float h, float s, float v)
	@Lua Color:setHSV( h, s ,v )
	@Param c In the C and python versions, the color to modify
	@Param h,s,v Color components in the HSV space
		0.0 <= h < 360.0
		0.0 <= s <= 1.0
		0.0 <= v <= 1.0
	*/
	void setHSV(float h, float s, float v);

	/**
	@PageName color
	@FuncTitle Define a color's hue, saturation or lightness
	@FuncDesc These functions set only a single component in the HSV color space.
	@Cpp 
		void TCODColor::setHue (float h)
		void TCODColor::setSaturation (float s)
		void TCODColor::setValue (float v)
	@C 
		void TCOD_color_set_hue (TCOD_color_t *c, float h)
		void TCOD_color_set_saturation (TCOD_color_t *c, float s)
		void TCOD_color_set_value (TCOD_color_t *c, float v)
	@Lua 
		Color:setHue(h)
		Color:setSaturation(s)
		Color:setValue(v)
	@Param h,s,v	Color components in the HSV space
	@Param c	In the C and python versions, the color to modify
	*/
	void setHue (float h);
	void setSaturation (float s);
	void setValue (float v);

	/**
	@PageName color
	@FuncTitle Get a color hue, saturation and value components
	@Cpp void TCODColor::getHSV(float *h, float *s, float *v) const
	@C void TCOD_color_get_HSV(TCOD_color_t c,float * h, float * s, float * v)
	@Py color_get_HSV(c) # returns [h,s,v]
	@C# void TCODColor::getHSV(out float h, out float s, out float v)
	@Lua Color:getHSV() -- returns h,s,v
	@Param c	In the C and python versions, the TCOD_color_t from which to read.
	@Param  h,s,v	Color components in the HSV space
		0.0 <= h < 360.0
		0.0 <= s <= 1.0
		0.0 <= v <= 1.0
	*/
	void getHSV(float *h, float *s, float *v) const;

	/**
	@PageName color
	@FuncTitle Get a color's hue, saturation or value
	@FuncDesc Should you need to extract only one of the HSV components, these functions are what you should call. Note that if you need all three values, it's way less burdensome for the CPU to call TCODColor::getHSV().
	@Cpp 
		float TCODColor::getHue ()
		float TCODColor::getSaturation ()
		float TCODColor::getValue ()
	@C 
		float TCOD_color_get_hue (TCOD_color_t c)
		float TCOD_color_get_saturation (TCOD_color_t c)
		float TCOD_color_get_value (TCOD_color_t c)
	@Lua
		Color:getHue()
		Color:getSaturation()
		Color:getValue()
	@C#
		float TCODColor::getHue()
		float TCODColor::getSaturation()
		float TCODColor::getValue()
	@Param c	the TCOD_color_t from which to read
	*/
	float getHue ();
	float getSaturation ();
	float getValue ();

	/**
	@PageName color
	@FuncTitle Shift a color's hue up or down
	@FuncDesc The hue shift value is the number of grades the color's hue will be shifted. The value can be negative for shift left, or positive for shift right.
		Resulting values H < 0 and H >= 360 are handled automatically.
	@Cpp void TCODColor::shiftHue (float hshift)
	@C void TCOD_color_shift_hue (TCOD_color_t *c, float hshift)
	@C# TCODColor::shiftHue(float hshift)
	@Lua Color:shiftHue(hshift)
	@Param c	The color to modify
	@Param hshift	The hue shift value
	*/
	void shiftHue (float hshift);

	/**
	@PageName color
	@FuncTitle Scale a color's saturation and value
	@Cpp void TCODColor::scaleHSV (float sscale, float vscale)
	@C void TCOD_color_scale_HSV (TCOD_color_t *c, float scoef, float vcoef)
	@Py color_scale_HSV(c, scoef, vcoef)
	@C# TCODColor::scaleHSV (float sscale, float vscale)
	@Lua Color:scaleHSV(sscale,vscale)
	@Param c	The color to modify
	@Param sscale	saturation multiplier (1.0f for no change)
	@Param vscale	value multiplier (1.0f for no change)
	*/
	void scaleHSV (float sscale, float vscale);

	/**
	@PageName color
	@FuncTitle Generate a smooth color map
	@FuncDesc You can define a color map from an array of color keys. Colors will be interpolated between the keys.
	0 -> black
	4 -> red
	8 -> white
	Result :
	<table>
	<tbody><tr><td class="code"><pre>map[0]</pre></td><td style="background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); width: 50px;" align="center">&nbsp;</td><td>black</td></tr>

	<tr><td class="code"><pre>map[1]</pre></td><td style="background-color: rgb(63, 0, 0); color: rgb(255, 255, 255); width: 50px;" align="center">&nbsp;</td></tr>
	<tr><td class="code"><pre>map[2]</pre></td><td style="background-color: rgb(127, 0, 0); color: rgb(255, 255, 255); width: 50px;" align="center">&nbsp;</td></tr>
	<tr><td class="code"><pre>map[3]</pre></td><td style="background-color: rgb(191, 0, 0); color: rgb(255, 255, 255); width: 50px;" align="center">&nbsp;</td></tr>
	<tr><td class="code"><pre>map[4]</pre></td><td style="background-color: rgb(255, 0, 0); color: rgb(255, 255, 255); width: 50px;" align="center">&nbsp;</td><td>red</td></tr>
	<tr><td class="code"><pre>map[5]</pre></td><td style="background-color: rgb(255, 63, 63); color: rgb(255, 255, 255); width: 50px;" align="center">&nbsp;</td></tr>
	<tr><td class="code"><pre>map[6]</pre></td><td style="background-color: rgb(255, 127, 127); color: rgb(255, 255, 255); width: 50px;" align="center">&nbsp;</td></tr>

	<tr><td class="code"><pre>map[7]</pre></td><td style="background-color: rgb(255, 191, 191); color: rgb(255, 255, 255); width: 50px;" align="center">&nbsp;</td></tr>
	<tr><td class="code"><pre>map[8]</pre></td><td style="background-color: rgb(255, 255, 255); color: rgb(255, 255, 255); width: 50px;" align="center">&nbsp;</td><td>white</td></tr>
</tbody></table>
	@Cpp static void genMap(TCODColor *map, int nbKey, TCODColor const *keyColor, int const *keyIndex)
	@C void TCOD_gen_map(TCOD_color_t *map, int nb_key, TCOD_color_t const *key_color, int const *key_index)
	@Py color_gen_map(keyColor,keyIndex) # returns an array of colors
	@Param map	An array of colors to be filled by the function.
	@Param nbKey	Number of color keys
	@Param keyColor	Array of nbKey colors containing the color of each key
	@Param keyIndex	Array of nbKey integers containing the index of each key.
		If you want to fill the map array, keyIndex[0] must be 0 and keyIndex[nbKey-1] is the number of elements in map minus 1 but you can also use the function to fill only a part of the map array.
	@CppEx 
		int idx[] = { 0, 4, 8 }; // indexes of the keys
		TCODColor col[] = { TCODColor( 0,0,0 ), TCODColor(255,0,0), TCODColor(255,255,255) }; // colors : black, red, white
		TCODColor map[9];
		TCODColor::genMap(map,3,col,idx);
	@CEx 
		int idx[] = { 0, 4, 8 }; // indexes of the keys
		TCOD_color_t col[] = { { 0,0,0 }, {255,0,0}, {255,255,255} }; // colors : black, red, white
		TCOD_color_t map[9];
		TCOD_color_gen_map(map,3,col,idx);
	@PyEx 
		idx = [ 0, 4, 8 ] # indexes of the keys
		col = [ libtcod.Color( 0,0,0 ), libtcod.Color( 255,0,0 ), libtcod.Color(255,255,255) ] # colors : black, red, white
		map=libtcod.color_gen_map(col,idx)
	*/
	static void genMap(TCODColor *map, int nbKey, TCODColor const *keyColor, int const *keyIndex);

	// color array
	static const TCODColor colors[TCOD_COLOR_NB][TCOD_COLOR_LEVELS];

	// grey levels
	static const TCODColor black;
	static const TCODColor darkestGrey;
	static const TCODColor darkerGrey;
	static const TCODColor darkGrey;
	static const TCODColor grey;
	static const TCODColor lightGrey;
	static const TCODColor lighterGrey;
	static const TCODColor lightestGrey;
	static const TCODColor white;

	//sepia
	static const TCODColor darkestSepia;
	static const TCODColor darkerSepia;
	static const TCODColor darkSepia;
	static const TCODColor sepia;
	static const TCODColor lightSepia;
	static const TCODColor lighterSepia;
	static const TCODColor lightestSepia;

	// standard colors
	static const TCODColor red;
  static const TCODColor flame;
  static const TCODColor orange;
  static const TCODColor amber;
  static const TCODColor yellow;
  static const TCODColor lime;
  static const TCODColor chartreuse;
  static const TCODColor green;
  static const TCODColor sea;
  static const TCODColor turquoise;
  static const TCODColor cyan;
  static const TCODColor sky;
  static const TCODColor azure;
  static const TCODColor blue;
  static const TCODColor han;
  static const TCODColor violet;
  static const TCODColor purple;
  static const TCODColor fuchsia;
  static const TCODColor magenta;
  static const TCODColor pink;
  static const TCODColor crimson;

	// dark colors
	static const TCODColor darkRed;
  static const TCODColor darkFlame;
  static const TCODColor darkOrange;
  static const TCODColor darkAmber;
  static const TCODColor darkYellow;
  static const TCODColor darkLime;
  static const TCODColor darkChartreuse;
  static const TCODColor darkGreen;
  static const TCODColor darkSea;
  static const TCODColor darkTurquoise;
  static const TCODColor darkCyan;
  static const TCODColor darkSky;
  static const TCODColor darkAzure;
  static const TCODColor darkBlue;
  static const TCODColor darkHan;
  static const TCODColor darkViolet;
  static const TCODColor darkPurple;
  static const TCODColor darkFuchsia;
  static const TCODColor darkMagenta;
  static const TCODColor darkPink;
  static const TCODColor darkCrimson;

	// darker colors
	static const TCODColor darkerRed;
  static const TCODColor darkerFlame;
  static const TCODColor darkerOrange;
  static const TCODColor darkerAmber;
  static const TCODColor darkerYellow;
  static const TCODColor darkerLime;
  static const TCODColor darkerChartreuse;
  static const TCODColor darkerGreen;
  static const TCODColor darkerSea;
  static const TCODColor darkerTurquoise;
  static const TCODColor darkerCyan;
  static const TCODColor darkerSky;
  static const TCODColor darkerAzure;
  static const TCODColor darkerBlue;
  static const TCODColor darkerHan;
  static const TCODColor darkerViolet;
  static const TCODColor darkerPurple;
  static const TCODColor darkerFuchsia;
  static const TCODColor darkerMagenta;
  static const TCODColor darkerPink;
  static const TCODColor darkerCrimson;

  // darkest colors
  static const TCODColor darkestRed;
  static const TCODColor darkestFlame;
  static const TCODColor darkestOrange;
  static const TCODColor darkestAmber;
  static const TCODColor darkestYellow;
  static const TCODColor darkestLime;
  static const TCODColor darkestChartreuse;
  static const TCODColor darkestGreen;
  static const TCODColor darkestSea;
  static const TCODColor darkestTurquoise;
  static const TCODColor darkestCyan;
  static const TCODColor darkestSky;
  static const TCODColor darkestAzure;
  static const TCODColor darkestBlue;
  static const TCODColor darkestHan;
  static const TCODColor darkestViolet;
  static const TCODColor darkestPurple;
  static const TCODColor darkestFuchsia;
  static const TCODColor darkestMagenta;
  static const TCODColor darkestPink;
  static const TCODColor darkestCrimson;

	// light colors
	static const TCODColor lightRed;
  static const TCODColor lightFlame;
  static const TCODColor lightOrange;
  static const TCODColor lightAmber;
  static const TCODColor lightYellow;
  static const TCODColor lightLime;
  static const TCODColor lightChartreuse;
  static const TCODColor lightGreen;
  static const TCODColor lightSea;
  static const TCODColor lightTurquoise;
  static const TCODColor lightCyan;
  static const TCODColor lightSky;
  static const TCODColor lightAzure;
  static const TCODColor lightBlue;
  static const TCODColor lightHan;
  static const TCODColor lightViolet;
  static const TCODColor lightPurple;
  static const TCODColor lightFuchsia;
  static const TCODColor lightMagenta;
  static const TCODColor lightPink;
  static const TCODColor lightCrimson;

  //lighter colors
  static const TCODColor lighterRed;
  static const TCODColor lighterFlame;
  static const TCODColor lighterOrange;
  static const TCODColor lighterAmber;
  static const TCODColor lighterYellow;
  static const TCODColor lighterLime;
  static const TCODColor lighterChartreuse;
  static const TCODColor lighterGreen;
  static const TCODColor lighterSea;
  static const TCODColor lighterTurquoise;
  static const TCODColor lighterCyan;
  static const TCODColor lighterSky;
  static const TCODColor lighterAzure;
  static const TCODColor lighterBlue;
  static const TCODColor lighterHan;
  static const TCODColor lighterViolet;
  static const TCODColor lighterPurple;
  static const TCODColor lighterFuchsia;
  static const TCODColor lighterMagenta;
  static const TCODColor lighterPink;
  static const TCODColor lighterCrimson;

  // lightest colors
  static const TCODColor lightestRed;
  static const TCODColor lightestFlame;
  static const TCODColor lightestOrange;
  static const TCODColor lightestAmber;
  static const TCODColor lightestYellow;
  static const TCODColor lightestLime;
  static const TCODColor lightestChartreuse;
  static const TCODColor lightestGreen;
  static const TCODColor lightestSea;
  static const TCODColor lightestTurquoise;
  static const TCODColor lightestCyan;
  static const TCODColor lightestSky;
  static const TCODColor lightestAzure;
  static const TCODColor lightestBlue;
  static const TCODColor lightestHan;
  static const TCODColor lightestViolet;
  static const TCODColor lightestPurple;
  static const TCODColor lightestFuchsia;
  static const TCODColor lightestMagenta;
  static const TCODColor lightestPink;
  static const TCODColor lightestCrimson;

	// desaturated colors
  static const TCODColor desaturatedRed;
  static const TCODColor desaturatedFlame;
  static const TCODColor desaturatedOrange;
  static const TCODColor desaturatedAmber;
  static const TCODColor desaturatedYellow;
  static const TCODColor desaturatedLime;
  static const TCODColor desaturatedChartreuse;
  static const TCODColor desaturatedGreen;
  static const TCODColor desaturatedSea;
  static const TCODColor desaturatedTurquoise;
  static const TCODColor desaturatedCyan;
  static const TCODColor desaturatedSky;
  static const TCODColor desaturatedAzure;
  static const TCODColor desaturatedBlue;
  static const TCODColor desaturatedHan;
  static const TCODColor desaturatedViolet;
  static const TCODColor desaturatedPurple;
  static const TCODColor desaturatedFuchsia;
  static const TCODColor desaturatedMagenta;
  static const TCODColor desaturatedPink;
  static const TCODColor desaturatedCrimson;

	// metallic
	static const TCODColor brass;
	static const TCODColor copper;
	static const TCODColor gold;
	static const TCODColor silver;

	// miscellaneous
	static const TCODColor celadon;
	static const TCODColor peach;
};

TCODLIB_API TCODColor operator *(float value, const TCODColor &c);

#endif
