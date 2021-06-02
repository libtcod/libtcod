/* BSD 3-Clause License
 *
 * Copyright © 2008-2021, Jice and the libtcod contributors.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
// clang-format off
#ifndef _TCOD_COLOR_HPP
#define _TCOD_COLOR_HPP

#include <algorithm>

#include "color.h"
#include "utility.h"
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
	uint8_t r,g,b;

	constexpr TCODColor() : r{0}, g{0}, b{0} {}
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
	constexpr TCODColor(uint8_t r_, uint8_t g_, uint8_t b_)
  : r{r_}, g{g_}, b{b_}
  {}
	constexpr TCODColor(int r_, int g_, int b_)
  : r{static_cast<uint8_t>(r_)}, g{static_cast<uint8_t>(g_)}, b{static_cast<uint8_t>(b_)}
  {}
	constexpr TCODColor(const TCOD_color_t &col): r{col.r}, g{col.g}, b{col.b} {}  // Notice: not explicit!
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
		if my_color != libtcod.white : ...
	@C#Ex
		if (myColor.Equal(TCODColor.yellow)) { ... }
		if (myColor.NotEqual(TCODColor.white)) { ... }
	@LuaEx
		if myColor == tcod.color.yellow then ... end
	*/
  bool operator == (const TCODColor& c) const
  {
    return (c.r == r && c.g == g && c.b == b);
  }
  bool operator != (const TCODColor& c) const
  {
    return !(*this == c);
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
  TCODColor operator*(const TCODColor& rhs) const
  {
    return TCODColor(*this, rhs, [](int c1, int c2){ return c1 * c2 / 255; });
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
	@PyEx myDarkishRed = libtcod.light_red * 0.5
	@C#Ex TCODColor myDarkishRed = TCODColor.lightRed.Multiply(0.5f);
	@LuaEx myDarkishRed = tcod.color.lightRed * 0.5
	*/
  TCODColor operator*(float value) const
  {
    return TCODColor(*this, [=](int c){ return static_cast<int>(c * value); });
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
  TCODColor operator+(const TCODColor & rhs) const
  {
    return TCODColor(*this, rhs, [](int c1, int c2){ return c1 + c2; });
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
  TCODColor operator-(const TCODColor& rhs) const
  {
    return TCODColor(*this, rhs, [](int c1, int c2){ return c1 - c2; });
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
	@PyEx my_color = libtcod.color_lerp ( libtcod.dark_grey, libtcod.light_red,coef)
	@C#Ex TCODColor myColor = TCODColor.Interpolate( TCODColor.darkGrey, TCODColor.lightRed, coef );
	@LuaEx myColor = tcod.color.Interpolate( tcod.color.darkGrey, tcod.color.lightRed, coef )
	*/
  static TCODColor lerp(const TCODColor &c1, const TCODColor &c2, float coef)
  {
    return TCODColor(c1, c2, [=](int c, int d){ return c + (d - c) * coef; });
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
	@Param c In the C and Python versions, the color to modify
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
	@Param c	In the C and Python versions, the color to modify
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
	@Param c	In the C and Python versions, the TCOD_color_t from which to read.
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
	static const TCODColor colors [[deprecated]] [TCOD_COLOR_NB][TCOD_COLOR_LEVELS];

	// grey levels
	static const TCODColor black [[deprecated]];
	static const TCODColor darkestGrey [[deprecated]];
	static const TCODColor darkerGrey [[deprecated]];
	static const TCODColor darkGrey [[deprecated]];
	static const TCODColor grey [[deprecated]];
	static const TCODColor lightGrey [[deprecated]];
	static const TCODColor lighterGrey [[deprecated]];
	static const TCODColor lightestGrey [[deprecated]];
	static const TCODColor white [[deprecated]];

	//sepia
	static const TCODColor darkestSepia [[deprecated]];
	static const TCODColor darkerSepia [[deprecated]];
	static const TCODColor darkSepia [[deprecated]];
	static const TCODColor sepia [[deprecated]];
	static const TCODColor lightSepia [[deprecated]];
	static const TCODColor lighterSepia [[deprecated]];
	static const TCODColor lightestSepia [[deprecated]];

	// standard colors
	static const TCODColor red [[deprecated]];
  static const TCODColor flame [[deprecated]];
  static const TCODColor orange [[deprecated]];
  static const TCODColor amber [[deprecated]];
  static const TCODColor yellow [[deprecated]];
  static const TCODColor lime [[deprecated]];
  static const TCODColor chartreuse [[deprecated]];
  static const TCODColor green [[deprecated]];
  static const TCODColor sea [[deprecated]];
  static const TCODColor turquoise [[deprecated]];
  static const TCODColor cyan [[deprecated]];
  static const TCODColor sky [[deprecated]];
  static const TCODColor azure [[deprecated]];
  static const TCODColor blue [[deprecated]];
  static const TCODColor han [[deprecated]];
  static const TCODColor violet [[deprecated]];
  static const TCODColor purple [[deprecated]];
  static const TCODColor fuchsia [[deprecated]];
  static const TCODColor magenta [[deprecated]];
  static const TCODColor pink [[deprecated]];
  static const TCODColor crimson [[deprecated]];

	// dark colors
	static const TCODColor darkRed [[deprecated]];
  static const TCODColor darkFlame [[deprecated]];
  static const TCODColor darkOrange [[deprecated]];
  static const TCODColor darkAmber [[deprecated]];
  static const TCODColor darkYellow [[deprecated]];
  static const TCODColor darkLime [[deprecated]];
  static const TCODColor darkChartreuse [[deprecated]];
  static const TCODColor darkGreen [[deprecated]];
  static const TCODColor darkSea [[deprecated]];
  static const TCODColor darkTurquoise [[deprecated]];
  static const TCODColor darkCyan [[deprecated]];
  static const TCODColor darkSky [[deprecated]];
  static const TCODColor darkAzure [[deprecated]];
  static const TCODColor darkBlue [[deprecated]];
  static const TCODColor darkHan [[deprecated]];
  static const TCODColor darkViolet [[deprecated]];
  static const TCODColor darkPurple [[deprecated]];
  static const TCODColor darkFuchsia [[deprecated]];
  static const TCODColor darkMagenta [[deprecated]];
  static const TCODColor darkPink [[deprecated]];
  static const TCODColor darkCrimson [[deprecated]];

	// darker colors
	static const TCODColor darkerRed [[deprecated]];
  static const TCODColor darkerFlame [[deprecated]];
  static const TCODColor darkerOrange [[deprecated]];
  static const TCODColor darkerAmber [[deprecated]];
  static const TCODColor darkerYellow [[deprecated]];
  static const TCODColor darkerLime [[deprecated]];
  static const TCODColor darkerChartreuse [[deprecated]];
  static const TCODColor darkerGreen [[deprecated]];
  static const TCODColor darkerSea [[deprecated]];
  static const TCODColor darkerTurquoise [[deprecated]];
  static const TCODColor darkerCyan [[deprecated]];
  static const TCODColor darkerSky [[deprecated]];
  static const TCODColor darkerAzure [[deprecated]];
  static const TCODColor darkerBlue [[deprecated]];
  static const TCODColor darkerHan [[deprecated]];
  static const TCODColor darkerViolet [[deprecated]];
  static const TCODColor darkerPurple [[deprecated]];
  static const TCODColor darkerFuchsia [[deprecated]];
  static const TCODColor darkerMagenta [[deprecated]];
  static const TCODColor darkerPink [[deprecated]];
  static const TCODColor darkerCrimson [[deprecated]];

  // darkest colors
  static const TCODColor darkestRed [[deprecated]];
  static const TCODColor darkestFlame [[deprecated]];
  static const TCODColor darkestOrange [[deprecated]];
  static const TCODColor darkestAmber [[deprecated]];
  static const TCODColor darkestYellow [[deprecated]];
  static const TCODColor darkestLime [[deprecated]];
  static const TCODColor darkestChartreuse [[deprecated]];
  static const TCODColor darkestGreen [[deprecated]];
  static const TCODColor darkestSea [[deprecated]];
  static const TCODColor darkestTurquoise [[deprecated]];
  static const TCODColor darkestCyan [[deprecated]];
  static const TCODColor darkestSky [[deprecated]];
  static const TCODColor darkestAzure [[deprecated]];
  static const TCODColor darkestBlue [[deprecated]];
  static const TCODColor darkestHan [[deprecated]];
  static const TCODColor darkestViolet [[deprecated]];
  static const TCODColor darkestPurple [[deprecated]];
  static const TCODColor darkestFuchsia [[deprecated]];
  static const TCODColor darkestMagenta [[deprecated]];
  static const TCODColor darkestPink [[deprecated]];
  static const TCODColor darkestCrimson [[deprecated]];

	// light colors
	static const TCODColor lightRed [[deprecated]];
  static const TCODColor lightFlame [[deprecated]];
  static const TCODColor lightOrange [[deprecated]];
  static const TCODColor lightAmber [[deprecated]];
  static const TCODColor lightYellow [[deprecated]];
  static const TCODColor lightLime [[deprecated]];
  static const TCODColor lightChartreuse [[deprecated]];
  static const TCODColor lightGreen [[deprecated]];
  static const TCODColor lightSea [[deprecated]];
  static const TCODColor lightTurquoise [[deprecated]];
  static const TCODColor lightCyan [[deprecated]];
  static const TCODColor lightSky [[deprecated]];
  static const TCODColor lightAzure [[deprecated]];
  static const TCODColor lightBlue [[deprecated]];
  static const TCODColor lightHan [[deprecated]];
  static const TCODColor lightViolet [[deprecated]];
  static const TCODColor lightPurple [[deprecated]];
  static const TCODColor lightFuchsia [[deprecated]];
  static const TCODColor lightMagenta [[deprecated]];
  static const TCODColor lightPink [[deprecated]];
  static const TCODColor lightCrimson [[deprecated]];

  //lighter colors
  static const TCODColor lighterRed [[deprecated]];
  static const TCODColor lighterFlame [[deprecated]];
  static const TCODColor lighterOrange [[deprecated]];
  static const TCODColor lighterAmber [[deprecated]];
  static const TCODColor lighterYellow [[deprecated]];
  static const TCODColor lighterLime [[deprecated]];
  static const TCODColor lighterChartreuse [[deprecated]];
  static const TCODColor lighterGreen [[deprecated]];
  static const TCODColor lighterSea [[deprecated]];
  static const TCODColor lighterTurquoise [[deprecated]];
  static const TCODColor lighterCyan [[deprecated]];
  static const TCODColor lighterSky [[deprecated]];
  static const TCODColor lighterAzure [[deprecated]];
  static const TCODColor lighterBlue [[deprecated]];
  static const TCODColor lighterHan [[deprecated]];
  static const TCODColor lighterViolet [[deprecated]];
  static const TCODColor lighterPurple [[deprecated]];
  static const TCODColor lighterFuchsia [[deprecated]];
  static const TCODColor lighterMagenta [[deprecated]];
  static const TCODColor lighterPink [[deprecated]];
  static const TCODColor lighterCrimson [[deprecated]];

  // lightest colors
  static const TCODColor lightestRed [[deprecated]];
  static const TCODColor lightestFlame [[deprecated]];
  static const TCODColor lightestOrange [[deprecated]];
  static const TCODColor lightestAmber [[deprecated]];
  static const TCODColor lightestYellow [[deprecated]];
  static const TCODColor lightestLime [[deprecated]];
  static const TCODColor lightestChartreuse [[deprecated]];
  static const TCODColor lightestGreen [[deprecated]];
  static const TCODColor lightestSea [[deprecated]];
  static const TCODColor lightestTurquoise [[deprecated]];
  static const TCODColor lightestCyan [[deprecated]];
  static const TCODColor lightestSky [[deprecated]];
  static const TCODColor lightestAzure [[deprecated]];
  static const TCODColor lightestBlue [[deprecated]];
  static const TCODColor lightestHan [[deprecated]];
  static const TCODColor lightestViolet [[deprecated]];
  static const TCODColor lightestPurple [[deprecated]];
  static const TCODColor lightestFuchsia [[deprecated]];
  static const TCODColor lightestMagenta [[deprecated]];
  static const TCODColor lightestPink [[deprecated]];
  static const TCODColor lightestCrimson [[deprecated]];

	// desaturated colors
  static const TCODColor desaturatedRed [[deprecated]];
  static const TCODColor desaturatedFlame [[deprecated]];
  static const TCODColor desaturatedOrange [[deprecated]];
  static const TCODColor desaturatedAmber [[deprecated]];
  static const TCODColor desaturatedYellow [[deprecated]];
  static const TCODColor desaturatedLime [[deprecated]];
  static const TCODColor desaturatedChartreuse [[deprecated]];
  static const TCODColor desaturatedGreen [[deprecated]];
  static const TCODColor desaturatedSea [[deprecated]];
  static const TCODColor desaturatedTurquoise [[deprecated]];
  static const TCODColor desaturatedCyan [[deprecated]];
  static const TCODColor desaturatedSky [[deprecated]];
  static const TCODColor desaturatedAzure [[deprecated]];
  static const TCODColor desaturatedBlue [[deprecated]];
  static const TCODColor desaturatedHan [[deprecated]];
  static const TCODColor desaturatedViolet [[deprecated]];
  static const TCODColor desaturatedPurple [[deprecated]];
  static const TCODColor desaturatedFuchsia [[deprecated]];
  static const TCODColor desaturatedMagenta [[deprecated]];
  static const TCODColor desaturatedPink [[deprecated]];
  static const TCODColor desaturatedCrimson [[deprecated]];

	// metallic
	static const TCODColor brass [[deprecated]];
	static const TCODColor copper [[deprecated]];
	static const TCODColor gold [[deprecated]];
	static const TCODColor silver [[deprecated]];

	// miscellaneous
	static const TCODColor celadon [[deprecated]];
	static const TCODColor peach [[deprecated]];
 private:
  /**
   *  Return a color transformed by a lambda.
   */
  template <typename F>
  TCODColor(const TCODColor& color, const F& lambda)
  : r{clamp_(lambda(color.r))},
    g{clamp_(lambda(color.g))},
    b{clamp_(lambda(color.b))}
  {}
  /**
   *  Return a color from two colors combined using a lambda.
   */
  template <typename F>
  TCODColor(const TCODColor& color1, const TCODColor& color2, const F& lambda)
  : r{clamp_(lambda(color1.r, color2.r))},
    g{clamp_(lambda(color1.g, color2.g))},
    b{clamp_(lambda(color1.b, color2.b))}
  {}
  /**
   *  Return a color value clamped between 0 to 255.
   */
  template <typename T>
  static constexpr uint8_t clamp_(const T& value) noexcept
  {
    return static_cast<uint8_t>(std::max<T>(0, std::min<T>(value, 255)));
  }
};

TCODLIB_API TCODColor operator *(float value, const TCODColor &c);

#endif
