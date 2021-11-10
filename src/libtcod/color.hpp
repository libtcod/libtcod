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

  // clang-format on

  /***************************************************************************
      @brief Allow explicit conversions to TCOD_ColorRGB.
      \rst
      .. versionadded:: 1.19
      \endrst
   */
  [[nodiscard]] constexpr explicit operator TCOD_ColorRGB() const noexcept { return {r, g, b}; }
  /***************************************************************************
      @brief Allow explicit conversions to TCOD_ColorRGBA.
      \rst
      .. versionadded:: 1.19
      \endrst
   */
  [[nodiscard]] constexpr explicit operator TCOD_ColorRGBA() const noexcept { return {r, g, b, 255}; }
  /***************************************************************************
      @brief Allow explicit conversions to tcod::ColorRGB.
      \rst
      .. versionadded:: 1.19
      \endrst
   */
  [[nodiscard]] constexpr explicit operator tcod::ColorRGB() const noexcept { return {r, g, b}; };
  /***************************************************************************
      @brief Allow explicit conversions to tcod::ColorRGBA.
      \rst
      .. versionadded:: 1.19
      \endrst
   */
  [[nodiscard]] constexpr explicit operator tcod::ColorRGBA() const noexcept { return {r, g, b, 255}; };

  // color array
  static const TCODColor colors [[deprecated]][TCOD_COLOR_NB][TCOD_COLOR_LEVELS];

  // grey levels
  static const TCODColor black [[deprecated("Replace with tcod::ColorRGB{0, 0, 0}")]];
  static const TCODColor darkestGrey [[deprecated("Replace with tcod::ColorRGB{31, 31, 31}")]];
  static const TCODColor darkerGrey [[deprecated("Replace with tcod::ColorRGB{63, 63, 63}")]];
  static const TCODColor darkGrey [[deprecated("Replace with tcod::ColorRGB{95, 95, 95}")]];
  static const TCODColor grey [[deprecated("Replace with tcod::ColorRGB{127, 127, 127}")]];
  static const TCODColor lightGrey [[deprecated("Replace with tcod::ColorRGB{159, 159, 159}")]];
  static const TCODColor lighterGrey [[deprecated("Replace with tcod::ColorRGB{191, 191, 191}")]];
  static const TCODColor lightestGrey [[deprecated("Replace with tcod::ColorRGB{223, 223, 223}")]];
  static const TCODColor white [[deprecated("Replace with tcod::ColorRGB{255, 255, 255}")]];

  // sepia
  static const TCODColor darkestSepia [[deprecated("Replace with tcod::ColorRGB{31, 24, 15}")]];
  static const TCODColor darkerSepia [[deprecated("Replace with tcod::ColorRGB{63, 50, 31}")]];
  static const TCODColor darkSepia [[deprecated("Replace with tcod::ColorRGB{94, 75, 47}")]];
  static const TCODColor sepia [[deprecated("Replace with tcod::ColorRGB{127, 101, 63}")]];
  static const TCODColor lightSepia [[deprecated("Replace with tcod::ColorRGB{158, 134, 100}")]];
  static const TCODColor lighterSepia [[deprecated("Replace with tcod::ColorRGB{191, 171, 143}")]];
  static const TCODColor lightestSepia [[deprecated("Replace with tcod::ColorRGB{222, 211, 195}")]];

  // standard colors
  static const TCODColor red [[deprecated("Replace with tcod::ColorRGB{255, 0, 0}")]];
  static const TCODColor flame [[deprecated("Replace with tcod::ColorRGB{255, 63, 0}")]];
  static const TCODColor orange [[deprecated("Replace with tcod::ColorRGB{255, 127, 0}")]];
  static const TCODColor amber [[deprecated("Replace with tcod::ColorRGB{255, 191, 0}")]];
  static const TCODColor yellow [[deprecated("Replace with tcod::ColorRGB{255, 255, 0}")]];
  static const TCODColor lime [[deprecated("Replace with tcod::ColorRGB{191, 255, 0}")]];
  static const TCODColor chartreuse [[deprecated("Replace with tcod::ColorRGB{127, 255, 0}")]];
  static const TCODColor green [[deprecated("Replace with tcod::ColorRGB{0, 255, 0}")]];
  static const TCODColor sea [[deprecated("Replace with tcod::ColorRGB{0, 255, 127}")]];
  static const TCODColor turquoise [[deprecated("Replace with tcod::ColorRGB{0, 255, 191}")]];
  static const TCODColor cyan [[deprecated("Replace with tcod::ColorRGB{0, 255, 255}")]];
  static const TCODColor sky [[deprecated("Replace with tcod::ColorRGB{0, 191, 255}")]];
  static const TCODColor azure [[deprecated("Replace with tcod::ColorRGB{0, 127, 255}")]];
  static const TCODColor blue [[deprecated("Replace with tcod::ColorRGB{0, 0, 255}")]];
  static const TCODColor han [[deprecated("Replace with tcod::ColorRGB{63, 0, 255}")]];
  static const TCODColor violet [[deprecated("Replace with tcod::ColorRGB{127, 0, 255}")]];
  static const TCODColor purple [[deprecated("Replace with tcod::ColorRGB{191, 0, 255}")]];
  static const TCODColor fuchsia [[deprecated("Replace with tcod::ColorRGB{255, 0, 255}")]];
  static const TCODColor magenta [[deprecated("Replace with tcod::ColorRGB{255, 0, 191}")]];
  static const TCODColor pink [[deprecated("Replace with tcod::ColorRGB{255, 0, 127}")]];
  static const TCODColor crimson [[deprecated("Replace with tcod::ColorRGB{255, 0, 63}")]];

  // dark colors
  static const TCODColor darkRed [[deprecated("Replace with tcod::ColorRGB{191, 0, 0}")]];
  static const TCODColor darkFlame [[deprecated("Replace with tcod::ColorRGB{191, 47, 0}")]];
  static const TCODColor darkOrange [[deprecated("Replace with tcod::ColorRGB{191, 95, 0}")]];
  static const TCODColor darkAmber [[deprecated("Replace with tcod::ColorRGB{191, 143, 0}")]];
  static const TCODColor darkYellow [[deprecated("Replace with tcod::ColorRGB{191, 191, 0}")]];
  static const TCODColor darkLime [[deprecated("Replace with tcod::ColorRGB{143, 191, 0}")]];
  static const TCODColor darkChartreuse [[deprecated("Replace with tcod::ColorRGB{95, 191, 0}")]];
  static const TCODColor darkGreen [[deprecated("Replace with tcod::ColorRGB{0, 191, 0}")]];
  static const TCODColor darkSea [[deprecated("Replace with tcod::ColorRGB{0, 191, 95}")]];
  static const TCODColor darkTurquoise [[deprecated("Replace with tcod::ColorRGB{0, 191, 143}")]];
  static const TCODColor darkCyan [[deprecated("Replace with tcod::ColorRGB{0, 191, 191}")]];
  static const TCODColor darkSky [[deprecated("Replace with tcod::ColorRGB{0, 143, 191}")]];
  static const TCODColor darkAzure [[deprecated("Replace with tcod::ColorRGB{0, 95, 191}")]];
  static const TCODColor darkBlue [[deprecated("Replace with tcod::ColorRGB{0, 0, 191}")]];
  static const TCODColor darkHan [[deprecated("Replace with tcod::ColorRGB{47, 0, 191}")]];
  static const TCODColor darkViolet [[deprecated("Replace with tcod::ColorRGB{95, 0, 191}")]];
  static const TCODColor darkPurple [[deprecated("Replace with tcod::ColorRGB{143, 0, 191}")]];
  static const TCODColor darkFuchsia [[deprecated("Replace with tcod::ColorRGB{191, 0, 191}")]];
  static const TCODColor darkMagenta [[deprecated("Replace with tcod::ColorRGB{191, 0, 143}")]];
  static const TCODColor darkPink [[deprecated("Replace with tcod::ColorRGB{191, 0, 95}")]];
  static const TCODColor darkCrimson [[deprecated("Replace with tcod::ColorRGB{191, 0, 47}")]];

  // darker colors
  static const TCODColor darkerRed [[deprecated("Replace with tcod::ColorRGB{127, 0, 0}")]];
  static const TCODColor darkerFlame [[deprecated("Replace with tcod::ColorRGB{127, 31, 0}")]];
  static const TCODColor darkerOrange [[deprecated("Replace with tcod::ColorRGB{127, 63, 0}")]];
  static const TCODColor darkerAmber [[deprecated("Replace with tcod::ColorRGB{127, 95, 0}")]];
  static const TCODColor darkerYellow [[deprecated("Replace with tcod::ColorRGB{127, 127, 0}")]];
  static const TCODColor darkerLime [[deprecated("Replace with tcod::ColorRGB{95, 127, 0}")]];
  static const TCODColor darkerChartreuse [[deprecated("Replace with tcod::ColorRGB{63, 127, 0}")]];
  static const TCODColor darkerGreen [[deprecated("Replace with tcod::ColorRGB{0, 127, 0}")]];
  static const TCODColor darkerSea [[deprecated("Replace with tcod::ColorRGB{0, 127, 63}")]];
  static const TCODColor darkerTurquoise [[deprecated("Replace with tcod::ColorRGB{0, 127, 95}")]];
  static const TCODColor darkerCyan [[deprecated("Replace with tcod::ColorRGB{0, 127, 127}")]];
  static const TCODColor darkerSky [[deprecated("Replace with tcod::ColorRGB{0, 95, 127}")]];
  static const TCODColor darkerAzure [[deprecated("Replace with tcod::ColorRGB{0, 63, 127}")]];
  static const TCODColor darkerBlue [[deprecated("Replace with tcod::ColorRGB{0, 0, 127}")]];
  static const TCODColor darkerHan [[deprecated("Replace with tcod::ColorRGB{31, 0, 127}")]];
  static const TCODColor darkerViolet [[deprecated("Replace with tcod::ColorRGB{63, 0, 127}")]];
  static const TCODColor darkerPurple [[deprecated("Replace with tcod::ColorRGB{95, 0, 127}")]];
  static const TCODColor darkerFuchsia [[deprecated("Replace with tcod::ColorRGB{127, 0, 127}")]];
  static const TCODColor darkerMagenta [[deprecated("Replace with tcod::ColorRGB{127, 0, 95}")]];
  static const TCODColor darkerPink [[deprecated("Replace with tcod::ColorRGB{127, 0, 63}")]];
  static const TCODColor darkerCrimson [[deprecated("Replace with tcod::ColorRGB{127, 0, 31}")]];

  // darkest colors
  static const TCODColor darkestRed [[deprecated("Replace with tcod::ColorRGB{63, 0, 0}")]];
  static const TCODColor darkestFlame [[deprecated("Replace with tcod::ColorRGB{63, 15, 0}")]];
  static const TCODColor darkestOrange [[deprecated("Replace with tcod::ColorRGB{63, 31, 0}")]];
  static const TCODColor darkestAmber [[deprecated("Replace with tcod::ColorRGB{63, 47, 0}")]];
  static const TCODColor darkestYellow [[deprecated("Replace with tcod::ColorRGB{63, 63, 0}")]];
  static const TCODColor darkestLime [[deprecated("Replace with tcod::ColorRGB{47, 63, 0}")]];
  static const TCODColor darkestChartreuse [[deprecated("Replace with tcod::ColorRGB{31, 63, 0}")]];
  static const TCODColor darkestGreen [[deprecated("Replace with tcod::ColorRGB{0, 63, 0}")]];
  static const TCODColor darkestSea [[deprecated("Replace with tcod::ColorRGB{0, 63, 31}")]];
  static const TCODColor darkestTurquoise [[deprecated("Replace with tcod::ColorRGB{0, 63, 47}")]];
  static const TCODColor darkestCyan [[deprecated("Replace with tcod::ColorRGB{0, 63, 63}")]];
  static const TCODColor darkestSky [[deprecated("Replace with tcod::ColorRGB{0, 47, 63}")]];
  static const TCODColor darkestAzure [[deprecated("Replace with tcod::ColorRGB{0, 31, 63}")]];
  static const TCODColor darkestBlue [[deprecated("Replace with tcod::ColorRGB{0, 0, 63}")]];
  static const TCODColor darkestHan [[deprecated("Replace with tcod::ColorRGB{15, 0, 63}")]];
  static const TCODColor darkestViolet [[deprecated("Replace with tcod::ColorRGB{31, 0, 63}")]];
  static const TCODColor darkestPurple [[deprecated("Replace with tcod::ColorRGB{47, 0, 63}")]];
  static const TCODColor darkestFuchsia [[deprecated("Replace with tcod::ColorRGB{63, 0, 63}")]];
  static const TCODColor darkestMagenta [[deprecated("Replace with tcod::ColorRGB{63, 0, 47}")]];
  static const TCODColor darkestPink [[deprecated("Replace with tcod::ColorRGB{63, 0, 31}")]];
  static const TCODColor darkestCrimson [[deprecated("Replace with tcod::ColorRGB{63, 0, 15}")]];

  // light colors
  static const TCODColor lightRed [[deprecated("Replace with tcod::ColorRGB{255, 63, 63}")]];
  static const TCODColor lightFlame [[deprecated("Replace with tcod::ColorRGB{255, 111, 63}")]];
  static const TCODColor lightOrange [[deprecated("Replace with tcod::ColorRGB{255, 159, 63}")]];
  static const TCODColor lightAmber [[deprecated("Replace with tcod::ColorRGB{255, 207, 63}")]];
  static const TCODColor lightYellow [[deprecated("Replace with tcod::ColorRGB{255, 255, 63}")]];
  static const TCODColor lightLime [[deprecated("Replace with tcod::ColorRGB{207, 255, 63}")]];
  static const TCODColor lightChartreuse [[deprecated("Replace with tcod::ColorRGB{159, 255, 63}")]];
  static const TCODColor lightGreen [[deprecated("Replace with tcod::ColorRGB{63, 255, 63}")]];
  static const TCODColor lightSea [[deprecated("Replace with tcod::ColorRGB{63, 255, 159}")]];
  static const TCODColor lightTurquoise [[deprecated("Replace with tcod::ColorRGB{63, 255, 207}")]];
  static const TCODColor lightCyan [[deprecated("Replace with tcod::ColorRGB{63, 255, 255}")]];
  static const TCODColor lightSky [[deprecated("Replace with tcod::ColorRGB{63, 207, 255}")]];
  static const TCODColor lightAzure [[deprecated("Replace with tcod::ColorRGB{63, 159, 255}")]];
  static const TCODColor lightBlue [[deprecated("Replace with tcod::ColorRGB{63, 63, 255}")]];
  static const TCODColor lightHan [[deprecated("Replace with tcod::ColorRGB{111, 63, 255}")]];
  static const TCODColor lightViolet [[deprecated("Replace with tcod::ColorRGB{159, 63, 255}")]];
  static const TCODColor lightPurple [[deprecated("Replace with tcod::ColorRGB{207, 63, 255}")]];
  static const TCODColor lightFuchsia [[deprecated("Replace with tcod::ColorRGB{255, 63, 255}")]];
  static const TCODColor lightMagenta [[deprecated("Replace with tcod::ColorRGB{255, 63, 207}")]];
  static const TCODColor lightPink [[deprecated("Replace with tcod::ColorRGB{255, 63, 159}")]];
  static const TCODColor lightCrimson [[deprecated("Replace with tcod::ColorRGB{255, 63, 111}")]];

  // lighter colors
  static const TCODColor lighterRed [[deprecated("Replace with tcod::ColorRGB{255, 127, 127}")]];
  static const TCODColor lighterFlame [[deprecated("Replace with tcod::ColorRGB{255, 159, 127}")]];
  static const TCODColor lighterOrange [[deprecated("Replace with tcod::ColorRGB{255, 191, 127}")]];
  static const TCODColor lighterAmber [[deprecated("Replace with tcod::ColorRGB{255, 223, 127}")]];
  static const TCODColor lighterYellow [[deprecated("Replace with tcod::ColorRGB{255, 255, 127}")]];
  static const TCODColor lighterLime [[deprecated("Replace with tcod::ColorRGB{223, 255, 127}")]];
  static const TCODColor lighterChartreuse [[deprecated("Replace with tcod::ColorRGB{191, 255, 127}")]];
  static const TCODColor lighterGreen [[deprecated("Replace with tcod::ColorRGB{127, 255, 127}")]];
  static const TCODColor lighterSea [[deprecated("Replace with tcod::ColorRGB{127, 255, 191}")]];
  static const TCODColor lighterTurquoise [[deprecated("Replace with tcod::ColorRGB{127, 255, 223}")]];
  static const TCODColor lighterCyan [[deprecated("Replace with tcod::ColorRGB{127, 255, 255}")]];
  static const TCODColor lighterSky [[deprecated("Replace with tcod::ColorRGB{127, 223, 255}")]];
  static const TCODColor lighterAzure [[deprecated("Replace with tcod::ColorRGB{127, 191, 255}")]];
  static const TCODColor lighterBlue [[deprecated("Replace with tcod::ColorRGB{127, 127, 255}")]];
  static const TCODColor lighterHan [[deprecated("Replace with tcod::ColorRGB{159, 127, 255}")]];
  static const TCODColor lighterViolet [[deprecated("Replace with tcod::ColorRGB{191, 127, 255}")]];
  static const TCODColor lighterPurple [[deprecated("Replace with tcod::ColorRGB{223, 127, 255}")]];
  static const TCODColor lighterFuchsia [[deprecated("Replace with tcod::ColorRGB{255, 127, 255}")]];
  static const TCODColor lighterMagenta [[deprecated("Replace with tcod::ColorRGB{255, 127, 223}")]];
  static const TCODColor lighterPink [[deprecated("Replace with tcod::ColorRGB{255, 127, 191}")]];
  static const TCODColor lighterCrimson [[deprecated("Replace with tcod::ColorRGB{255, 127, 159}")]];

  // lightest colors
  static const TCODColor lightestRed [[deprecated("Replace with tcod::ColorRGB{255, 191, 191}")]];
  static const TCODColor lightestFlame [[deprecated("Replace with tcod::ColorRGB{255, 207, 191}")]];
  static const TCODColor lightestOrange [[deprecated("Replace with tcod::ColorRGB{255, 223, 191}")]];
  static const TCODColor lightestAmber [[deprecated("Replace with tcod::ColorRGB{255, 239, 191}")]];
  static const TCODColor lightestYellow [[deprecated("Replace with tcod::ColorRGB{255, 255, 191}")]];
  static const TCODColor lightestLime [[deprecated("Replace with tcod::ColorRGB{239, 255, 191}")]];
  static const TCODColor lightestChartreuse [[deprecated("Replace with tcod::ColorRGB{223, 255, 191}")]];
  static const TCODColor lightestGreen [[deprecated("Replace with tcod::ColorRGB{191, 255, 191}")]];
  static const TCODColor lightestSea [[deprecated("Replace with tcod::ColorRGB{191, 255, 223}")]];
  static const TCODColor lightestTurquoise [[deprecated("Replace with tcod::ColorRGB{191, 255, 239}")]];
  static const TCODColor lightestCyan [[deprecated("Replace with tcod::ColorRGB{191, 255, 255}")]];
  static const TCODColor lightestSky [[deprecated("Replace with tcod::ColorRGB{191, 239, 255}")]];
  static const TCODColor lightestAzure [[deprecated("Replace with tcod::ColorRGB{191, 223, 255}")]];
  static const TCODColor lightestBlue [[deprecated("Replace with tcod::ColorRGB{191, 191, 255}")]];
  static const TCODColor lightestHan [[deprecated("Replace with tcod::ColorRGB{207, 191, 255}")]];
  static const TCODColor lightestViolet [[deprecated("Replace with tcod::ColorRGB{223, 191, 255}")]];
  static const TCODColor lightestPurple [[deprecated("Replace with tcod::ColorRGB{239, 191, 255}")]];
  static const TCODColor lightestFuchsia [[deprecated("Replace with tcod::ColorRGB{255, 191, 255}")]];
  static const TCODColor lightestMagenta [[deprecated("Replace with tcod::ColorRGB{255, 191, 239}")]];
  static const TCODColor lightestPink [[deprecated("Replace with tcod::ColorRGB{255, 191, 223}")]];
  static const TCODColor lightestCrimson [[deprecated("Replace with tcod::ColorRGB{255, 191, 207}")]];

  // desaturated colors
  static const TCODColor desaturatedRed [[deprecated("Replace with tcod::ColorRGB{127, 63, 63}")]];
  static const TCODColor desaturatedFlame [[deprecated("Replace with tcod::ColorRGB{127, 79, 63}")]];
  static const TCODColor desaturatedOrange [[deprecated("Replace with tcod::ColorRGB{127, 95, 63}")]];
  static const TCODColor desaturatedAmber [[deprecated("Replace with tcod::ColorRGB{127, 111, 63}")]];
  static const TCODColor desaturatedYellow [[deprecated("Replace with tcod::ColorRGB{127, 127, 63}")]];
  static const TCODColor desaturatedLime [[deprecated("Replace with tcod::ColorRGB{111, 127, 63}")]];
  static const TCODColor desaturatedChartreuse [[deprecated("Replace with tcod::ColorRGB{95, 127, 63}")]];
  static const TCODColor desaturatedGreen [[deprecated("Replace with tcod::ColorRGB{63, 127, 63}")]];
  static const TCODColor desaturatedSea [[deprecated("Replace with tcod::ColorRGB{63, 127, 95}")]];
  static const TCODColor desaturatedTurquoise [[deprecated("Replace with tcod::ColorRGB{63, 127, 111}")]];
  static const TCODColor desaturatedCyan [[deprecated("Replace with tcod::ColorRGB{63, 127, 127}")]];
  static const TCODColor desaturatedSky [[deprecated("Replace with tcod::ColorRGB{63, 111, 127}")]];
  static const TCODColor desaturatedAzure [[deprecated("Replace with tcod::ColorRGB{63, 95, 127}")]];
  static const TCODColor desaturatedBlue [[deprecated("Replace with tcod::ColorRGB{63, 63, 127}")]];
  static const TCODColor desaturatedHan [[deprecated("Replace with tcod::ColorRGB{79, 63, 127}")]];
  static const TCODColor desaturatedViolet [[deprecated("Replace with tcod::ColorRGB{95, 63, 127}")]];
  static const TCODColor desaturatedPurple [[deprecated("Replace with tcod::ColorRGB{111, 63, 127}")]];
  static const TCODColor desaturatedFuchsia [[deprecated("Replace with tcod::ColorRGB{127, 63, 127}")]];
  static const TCODColor desaturatedMagenta [[deprecated("Replace with tcod::ColorRGB{127, 63, 111}")]];
  static const TCODColor desaturatedPink [[deprecated("Replace with tcod::ColorRGB{127, 63, 95}")]];
  static const TCODColor desaturatedCrimson [[deprecated("Replace with tcod::ColorRGB{127, 63, 79}")]];

  // metallic
  static const TCODColor brass [[deprecated("Replace with tcod::ColorRGB{191, 151, 96}")]];
  static const TCODColor copper [[deprecated("Replace with tcod::ColorRGB{197, 136, 124}")]];
  static const TCODColor gold [[deprecated("Replace with tcod::ColorRGB{229, 191, 0}")]];
  static const TCODColor silver [[deprecated("Replace with tcod::ColorRGB{203, 203, 203}")]];

  // miscellaneous
  static const TCODColor celadon [[deprecated("Replace with tcod::ColorRGB{172, 255, 175}")]];
  static const TCODColor peach [[deprecated("Replace with tcod::ColorRGB{255, 159, 127}")]];

 private:
  /**
   *  Return a color transformed by a lambda.
   */
  template <typename F>
  TCODColor(const TCODColor& color, const F& lambda)
      : r{clamp_(lambda(color.r))}, g{clamp_(lambda(color.g))}, b{clamp_(lambda(color.b))} {}
  /**
   *  Return a color from two colors combined using a lambda.
   */
  template <typename F>
  TCODColor(const TCODColor& color1, const TCODColor& color2, const F& lambda)
      : r{clamp_(lambda(color1.r, color2.r))},
        g{clamp_(lambda(color1.g, color2.g))},
        b{clamp_(lambda(color1.b, color2.b))} {}
  /**
   *  Return a color value clamped between 0 to 255.
   */
  template <typename T>
  static constexpr uint8_t clamp_(const T& value) noexcept {
    return static_cast<uint8_t>(std::max<T>(0, std::min<T>(value, 255)));
  }
};

TCODLIB_API TCODColor operator*(float value, const TCODColor& c);

#endif  // _TCOD_COLOR_HPP
