/* BSD 3-Clause License
 *
 * Copyright © 2008-2019, Jice and the libtcod contributors.
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
#include "color.h"

#include <math.h>

#include "libtcod_int.h"
#include "utility.h"

/* grey levels */
const TCOD_color_t TCOD_black={TCOD_BLACK};
const TCOD_color_t TCOD_darkest_grey={TCOD_DARKEST_GREY};
const TCOD_color_t TCOD_darker_grey={TCOD_DARKER_GREY};
const TCOD_color_t TCOD_dark_grey={TCOD_DARK_GREY};
const TCOD_color_t TCOD_grey={TCOD_GREY};
const TCOD_color_t TCOD_light_grey={TCOD_LIGHT_GREY};
const TCOD_color_t TCOD_lighter_grey={TCOD_LIGHTER_GREY};
const TCOD_color_t TCOD_lightest_grey={TCOD_LIGHTEST_GREY};
const TCOD_color_t TCOD_darkest_gray={TCOD_DARKEST_GREY};
const TCOD_color_t TCOD_darker_gray={TCOD_DARKER_GREY};
const TCOD_color_t TCOD_dark_gray={TCOD_DARK_GREY};
const TCOD_color_t TCOD_gray={TCOD_GREY};
const TCOD_color_t TCOD_light_gray={TCOD_LIGHT_GREY};
const TCOD_color_t TCOD_lighter_gray={TCOD_LIGHTER_GREY};
const TCOD_color_t TCOD_lightest_gray={TCOD_LIGHTEST_GREY};
const TCOD_color_t TCOD_white={TCOD_WHITE};

/* sepia */
const TCOD_color_t TCOD_darkest_sepia={TCOD_DARKEST_SEPIA};
const TCOD_color_t TCOD_darker_sepia={TCOD_DARKER_SEPIA};
const TCOD_color_t TCOD_dark_sepia={TCOD_DARK_SEPIA};
const TCOD_color_t TCOD_sepia={TCOD_SEPIA};
const TCOD_color_t TCOD_light_sepia={TCOD_LIGHT_SEPIA};
const TCOD_color_t TCOD_lighter_sepia={TCOD_LIGHTER_SEPIA};
const TCOD_color_t TCOD_lightest_sepia={TCOD_LIGHTEST_SEPIA};

/* standard colors */
const TCOD_color_t TCOD_red = {TCOD_RED};
const TCOD_color_t TCOD_flame = {TCOD_FLAME};
const TCOD_color_t TCOD_orange = {TCOD_ORANGE};
const TCOD_color_t TCOD_amber = {TCOD_AMBER};
const TCOD_color_t TCOD_yellow = {TCOD_YELLOW};
const TCOD_color_t TCOD_lime = {TCOD_LIME};
const TCOD_color_t TCOD_chartreuse = {TCOD_CHARTREUSE};
const TCOD_color_t TCOD_green = {TCOD_GREEN};
const TCOD_color_t TCOD_sea = {TCOD_SEA};
const TCOD_color_t TCOD_turquoise = {TCOD_TURQUOISE};
const TCOD_color_t TCOD_cyan = {TCOD_CYAN};
const TCOD_color_t TCOD_sky = {TCOD_SKY};
const TCOD_color_t TCOD_azure = {TCOD_AZURE};
const TCOD_color_t TCOD_blue = {TCOD_BLUE};
const TCOD_color_t TCOD_han = {TCOD_HAN};
const TCOD_color_t TCOD_violet = {TCOD_VIOLET};
const TCOD_color_t TCOD_purple = {TCOD_PURPLE};
const TCOD_color_t TCOD_fuchsia = {TCOD_FUCHSIA};
const TCOD_color_t TCOD_magenta = {TCOD_MAGENTA};
const TCOD_color_t TCOD_pink = {TCOD_PINK};
const TCOD_color_t TCOD_crimson = {TCOD_CRIMSON};

/* dark colors */
const TCOD_color_t TCOD_dark_red = {TCOD_DARK_RED};
const TCOD_color_t TCOD_dark_flame = {TCOD_DARK_FLAME};
const TCOD_color_t TCOD_dark_orange = {TCOD_DARK_ORANGE};
const TCOD_color_t TCOD_dark_amber = {TCOD_DARK_AMBER};
const TCOD_color_t TCOD_dark_yellow = {TCOD_DARK_YELLOW};
const TCOD_color_t TCOD_dark_lime = {TCOD_DARK_LIME};
const TCOD_color_t TCOD_dark_chartreuse = {TCOD_DARK_CHARTREUSE};
const TCOD_color_t TCOD_dark_green = {TCOD_DARK_GREEN};
const TCOD_color_t TCOD_dark_sea = {TCOD_DARK_SEA};
const TCOD_color_t TCOD_dark_turquoise = {TCOD_DARK_TURQUOISE};
const TCOD_color_t TCOD_dark_cyan = {TCOD_DARK_CYAN};
const TCOD_color_t TCOD_dark_sky = {TCOD_DARK_SKY};
const TCOD_color_t TCOD_dark_azure = {TCOD_DARK_AZURE};
const TCOD_color_t TCOD_dark_blue = {TCOD_DARK_BLUE};
const TCOD_color_t TCOD_dark_han = {TCOD_DARK_HAN};
const TCOD_color_t TCOD_dark_violet = {TCOD_DARK_VIOLET};
const TCOD_color_t TCOD_dark_purple = {TCOD_DARK_PURPLE};
const TCOD_color_t TCOD_dark_fuchsia = {TCOD_DARK_FUCHSIA};
const TCOD_color_t TCOD_dark_magenta = {TCOD_DARK_MAGENTA};
const TCOD_color_t TCOD_dark_pink = {TCOD_DARK_PINK};
const TCOD_color_t TCOD_dark_crimson = {TCOD_DARK_CRIMSON};

/* darker colors */
const TCOD_color_t TCOD_darker_red = {TCOD_DARKER_RED};
const TCOD_color_t TCOD_darker_flame = {TCOD_DARKER_FLAME};
const TCOD_color_t TCOD_darker_orange = {TCOD_DARKER_ORANGE};
const TCOD_color_t TCOD_darker_amber = {TCOD_DARKER_AMBER};
const TCOD_color_t TCOD_darker_yellow = {TCOD_DARKER_YELLOW};
const TCOD_color_t TCOD_darker_lime = {TCOD_DARKER_LIME};
const TCOD_color_t TCOD_darker_chartreuse = {TCOD_DARKER_CHARTREUSE};
const TCOD_color_t TCOD_darker_green = {TCOD_DARKER_GREEN};
const TCOD_color_t TCOD_darker_sea = {TCOD_DARKER_SEA};
const TCOD_color_t TCOD_darker_turquoise = {TCOD_DARKER_TURQUOISE};
const TCOD_color_t TCOD_darker_cyan = {TCOD_DARKER_CYAN};
const TCOD_color_t TCOD_darker_sky = {TCOD_DARKER_SKY};
const TCOD_color_t TCOD_darker_azure = {TCOD_DARKER_AZURE};
const TCOD_color_t TCOD_darker_blue = {TCOD_DARKER_BLUE};
const TCOD_color_t TCOD_darker_han = {TCOD_DARKER_HAN};
const TCOD_color_t TCOD_darker_violet = {TCOD_DARKER_VIOLET};
const TCOD_color_t TCOD_darker_purple = {TCOD_DARKER_PURPLE};
const TCOD_color_t TCOD_darker_fuchsia = {TCOD_DARKER_FUCHSIA};
const TCOD_color_t TCOD_darker_magenta = {TCOD_DARKER_MAGENTA};
const TCOD_color_t TCOD_darker_pink = {TCOD_DARKER_PINK};
const TCOD_color_t TCOD_darker_crimson = {TCOD_DARKER_CRIMSON};

/* darkest colors */
const TCOD_color_t TCOD_darkest_red = {TCOD_DARKEST_RED};
const TCOD_color_t TCOD_darkest_flame = {TCOD_DARKEST_FLAME};
const TCOD_color_t TCOD_darkest_orange = {TCOD_DARKEST_ORANGE};
const TCOD_color_t TCOD_darkest_amber = {TCOD_DARKEST_AMBER};
const TCOD_color_t TCOD_darkest_yellow = {TCOD_DARKEST_YELLOW};
const TCOD_color_t TCOD_darkest_lime = {TCOD_DARKEST_LIME};
const TCOD_color_t TCOD_darkest_chartreuse = {TCOD_DARKEST_CHARTREUSE};
const TCOD_color_t TCOD_darkest_green = {TCOD_DARKEST_GREEN};
const TCOD_color_t TCOD_darkest_sea = {TCOD_DARKEST_SEA};
const TCOD_color_t TCOD_darkest_turquoise = {TCOD_DARKEST_TURQUOISE};
const TCOD_color_t TCOD_darkest_cyan = {TCOD_DARKEST_CYAN};
const TCOD_color_t TCOD_darkest_sky = {TCOD_DARKEST_SKY};
const TCOD_color_t TCOD_darkest_azure = {TCOD_DARKEST_AZURE};
const TCOD_color_t TCOD_darkest_blue = {TCOD_DARKEST_BLUE};
const TCOD_color_t TCOD_darkest_han = {TCOD_DARKEST_HAN};
const TCOD_color_t TCOD_darkest_violet = {TCOD_DARKEST_VIOLET};
const TCOD_color_t TCOD_darkest_purple = {TCOD_DARKEST_PURPLE};
const TCOD_color_t TCOD_darkest_fuchsia = {TCOD_DARKEST_FUCHSIA};
const TCOD_color_t TCOD_darkest_magenta = {TCOD_DARKEST_MAGENTA};
const TCOD_color_t TCOD_darkest_pink = {TCOD_DARKEST_PINK};
const TCOD_color_t TCOD_darkest_crimson = {TCOD_DARKEST_CRIMSON};

/* light colors */
const TCOD_color_t TCOD_light_red = {TCOD_LIGHT_RED};
const TCOD_color_t TCOD_light_flame = {TCOD_LIGHT_FLAME};
const TCOD_color_t TCOD_light_orange = {TCOD_LIGHT_ORANGE};
const TCOD_color_t TCOD_light_amber = {TCOD_LIGHT_AMBER};
const TCOD_color_t TCOD_light_yellow = {TCOD_LIGHT_YELLOW};
const TCOD_color_t TCOD_light_lime = {TCOD_LIGHT_LIME};
const TCOD_color_t TCOD_light_chartreuse = {TCOD_LIGHT_CHARTREUSE};
const TCOD_color_t TCOD_light_green = {TCOD_LIGHT_GREEN};
const TCOD_color_t TCOD_light_sea = {TCOD_LIGHT_SEA};
const TCOD_color_t TCOD_light_turquoise = {TCOD_LIGHT_TURQUOISE};
const TCOD_color_t TCOD_light_cyan = {TCOD_LIGHT_CYAN};
const TCOD_color_t TCOD_light_sky = {TCOD_LIGHT_SKY};
const TCOD_color_t TCOD_light_azure = {TCOD_LIGHT_AZURE};
const TCOD_color_t TCOD_light_blue = {TCOD_LIGHT_BLUE};
const TCOD_color_t TCOD_light_han = {TCOD_LIGHT_HAN};
const TCOD_color_t TCOD_light_violet = {TCOD_LIGHT_VIOLET};
const TCOD_color_t TCOD_light_purple = {TCOD_LIGHT_PURPLE};
const TCOD_color_t TCOD_light_fuchsia = {TCOD_LIGHT_FUCHSIA};
const TCOD_color_t TCOD_light_magenta = {TCOD_LIGHT_MAGENTA};
const TCOD_color_t TCOD_light_pink = {TCOD_LIGHT_PINK};
const TCOD_color_t TCOD_light_crimson = {TCOD_LIGHT_CRIMSON};

/*lighter colors */
const TCOD_color_t TCOD_lighter_red = {TCOD_LIGHTER_RED};
const TCOD_color_t TCOD_lighter_flame = {TCOD_LIGHTER_FLAME};
const TCOD_color_t TCOD_lighter_orange = {TCOD_LIGHTER_ORANGE};
const TCOD_color_t TCOD_lighter_amber = {TCOD_LIGHTER_AMBER};
const TCOD_color_t TCOD_lighter_yellow = {TCOD_LIGHTER_YELLOW};
const TCOD_color_t TCOD_lighter_lime = {TCOD_LIGHTER_LIME};
const TCOD_color_t TCOD_lighter_chartreuse = {TCOD_LIGHTER_CHARTREUSE};
const TCOD_color_t TCOD_lighter_green = {TCOD_LIGHTER_GREEN};
const TCOD_color_t TCOD_lighter_sea = {TCOD_LIGHTER_SEA};
const TCOD_color_t TCOD_lighter_turquoise = {TCOD_LIGHTER_TURQUOISE};
const TCOD_color_t TCOD_lighter_cyan = {TCOD_LIGHTER_CYAN};
const TCOD_color_t TCOD_lighter_sky = {TCOD_LIGHTER_SKY};
const TCOD_color_t TCOD_lighter_azure = {TCOD_LIGHTER_AZURE};
const TCOD_color_t TCOD_lighter_blue = {TCOD_LIGHTER_BLUE};
const TCOD_color_t TCOD_lighter_han = {TCOD_LIGHTER_HAN};
const TCOD_color_t TCOD_lighter_violet = {TCOD_LIGHTER_VIOLET};
const TCOD_color_t TCOD_lighter_purple = {TCOD_LIGHTER_PURPLE};
const TCOD_color_t TCOD_lighter_fuchsia = {TCOD_LIGHTER_FUCHSIA};
const TCOD_color_t TCOD_lighter_magenta = {TCOD_LIGHTER_MAGENTA};
const TCOD_color_t TCOD_lighter_pink = {TCOD_LIGHTER_PINK};
const TCOD_color_t TCOD_lighter_crimson = {TCOD_LIGHTER_CRIMSON};

/* lightest colors */
const TCOD_color_t TCOD_lightest_red = {TCOD_LIGHTEST_RED};
const TCOD_color_t TCOD_lightest_flame = {TCOD_LIGHTEST_FLAME};
const TCOD_color_t TCOD_lightest_orange = {TCOD_LIGHTEST_ORANGE};
const TCOD_color_t TCOD_lightest_amber = {TCOD_LIGHTEST_AMBER};
const TCOD_color_t TCOD_lightest_yellow = {TCOD_LIGHTEST_YELLOW};
const TCOD_color_t TCOD_lightest_lime = {TCOD_LIGHTEST_LIME};
const TCOD_color_t TCOD_lightest_chartreuse = {TCOD_LIGHTEST_CHARTREUSE};
const TCOD_color_t TCOD_lightest_green = {TCOD_LIGHTEST_GREEN};
const TCOD_color_t TCOD_lightest_sea = {TCOD_LIGHTEST_SEA};
const TCOD_color_t TCOD_lightest_turquoise = {TCOD_LIGHTEST_TURQUOISE};
const TCOD_color_t TCOD_lightest_cyan = {TCOD_LIGHTEST_CYAN};
const TCOD_color_t TCOD_lightest_sky = {TCOD_LIGHTEST_SKY};
const TCOD_color_t TCOD_lightest_azure = {TCOD_LIGHTEST_AZURE};
const TCOD_color_t TCOD_lightest_blue = {TCOD_LIGHTEST_BLUE};
const TCOD_color_t TCOD_lightest_han = {TCOD_LIGHTEST_HAN};
const TCOD_color_t TCOD_lightest_violet = {TCOD_LIGHTEST_VIOLET};
const TCOD_color_t TCOD_lightest_purple = {TCOD_LIGHTEST_PURPLE};
const TCOD_color_t TCOD_lightest_fuchsia = {TCOD_LIGHTEST_FUCHSIA};
const TCOD_color_t TCOD_lightest_magenta = {TCOD_LIGHTEST_MAGENTA};
const TCOD_color_t TCOD_lightest_pink = {TCOD_LIGHTEST_PINK};
const TCOD_color_t TCOD_lightest_crimson = {TCOD_LIGHTEST_CRIMSON};

/* desaturated colors */
const TCOD_color_t TCOD_desaturated_red = {TCOD_DESATURATED_RED};
const TCOD_color_t TCOD_desaturated_flame = {TCOD_DESATURATED_FLAME};
const TCOD_color_t TCOD_desaturated_orange = {TCOD_DESATURATED_ORANGE};
const TCOD_color_t TCOD_desaturated_amber = {TCOD_DESATURATED_AMBER};
const TCOD_color_t TCOD_desaturated_yellow = {TCOD_DESATURATED_YELLOW};
const TCOD_color_t TCOD_desaturated_lime = {TCOD_DESATURATED_LIME};
const TCOD_color_t TCOD_desaturated_chartreuse = {TCOD_DESATURATED_CHARTREUSE};
const TCOD_color_t TCOD_desaturated_green = {TCOD_DESATURATED_GREEN};
const TCOD_color_t TCOD_desaturated_sea = {TCOD_DESATURATED_SEA};
const TCOD_color_t TCOD_desaturated_turquoise = {TCOD_DESATURATED_TURQUOISE};
const TCOD_color_t TCOD_desaturated_cyan = {TCOD_DESATURATED_CYAN};
const TCOD_color_t TCOD_desaturated_sky = {TCOD_DESATURATED_SKY};
const TCOD_color_t TCOD_desaturated_azure = {TCOD_DESATURATED_AZURE};
const TCOD_color_t TCOD_desaturated_blue = {TCOD_DESATURATED_BLUE};
const TCOD_color_t TCOD_desaturated_han = {TCOD_DESATURATED_HAN};
const TCOD_color_t TCOD_desaturated_violet = {TCOD_DESATURATED_VIOLET};
const TCOD_color_t TCOD_desaturated_purple = {TCOD_DESATURATED_PURPLE};
const TCOD_color_t TCOD_desaturated_fuchsia = {TCOD_DESATURATED_FUCHSIA};
const TCOD_color_t TCOD_desaturated_magenta = {TCOD_DESATURATED_MAGENTA};
const TCOD_color_t TCOD_desaturated_pink = {TCOD_DESATURATED_PINK};
const TCOD_color_t TCOD_desaturated_crimson = {TCOD_DESATURATED_CRIMSON};

/* metallic */
const TCOD_color_t TCOD_brass = {TCOD_BRASS};
const TCOD_color_t TCOD_copper = {TCOD_COPPER};
const TCOD_color_t TCOD_gold = {TCOD_GOLD};
const TCOD_color_t TCOD_silver = {TCOD_SILVER};

/* miscellaneous */
const TCOD_color_t TCOD_celadon = {TCOD_CELADON};
const TCOD_color_t TCOD_peach = {TCOD_PEACH};

/* color array */
const TCOD_color_t TCOD_colors[TCOD_COLOR_NB][TCOD_COLOR_LEVELS] = {
 {{TCOD_DESATURATED_RED},{TCOD_LIGHTEST_RED},{TCOD_LIGHTER_RED},{TCOD_LIGHT_RED},{TCOD_RED},{TCOD_DARK_RED},{TCOD_DARKER_RED},{TCOD_DARKEST_RED}},
 {{TCOD_DESATURATED_FLAME},{TCOD_LIGHTEST_FLAME},{TCOD_LIGHTER_FLAME},{TCOD_LIGHT_FLAME},{TCOD_FLAME},{TCOD_DARK_FLAME},{TCOD_DARKER_FLAME},{TCOD_DARKEST_FLAME}},
 {{TCOD_DESATURATED_ORANGE},{TCOD_LIGHTEST_ORANGE},{TCOD_LIGHTER_ORANGE},{TCOD_LIGHT_ORANGE},{TCOD_ORANGE},{TCOD_DARK_ORANGE},{TCOD_DARKER_ORANGE},{TCOD_DARKEST_ORANGE}},
 {{TCOD_DESATURATED_AMBER},{TCOD_LIGHTEST_AMBER},{TCOD_LIGHTER_AMBER},{TCOD_LIGHT_AMBER},{TCOD_AMBER},{TCOD_DARK_AMBER},{TCOD_DARKER_AMBER},{TCOD_DARKEST_AMBER}},
 {{TCOD_DESATURATED_YELLOW},{TCOD_LIGHTEST_YELLOW},{TCOD_LIGHTER_YELLOW},{TCOD_LIGHT_YELLOW},{TCOD_YELLOW},{TCOD_DARK_YELLOW},{TCOD_DARKER_YELLOW},{TCOD_DARKEST_YELLOW}},
 {{TCOD_DESATURATED_LIME},{TCOD_LIGHTEST_LIME},{TCOD_LIGHTER_LIME},{TCOD_LIGHT_LIME},{TCOD_LIME},{TCOD_DARK_LIME},{TCOD_DARKER_LIME},{TCOD_DARKEST_LIME}},
 {{TCOD_DESATURATED_CHARTREUSE},{TCOD_LIGHTEST_CHARTREUSE},{TCOD_LIGHTER_CHARTREUSE},{TCOD_LIGHT_CHARTREUSE},{TCOD_CHARTREUSE},{TCOD_DARK_CHARTREUSE},{TCOD_DARKER_CHARTREUSE},{TCOD_DARKEST_CHARTREUSE}},
 {{TCOD_DESATURATED_GREEN},{TCOD_LIGHTEST_GREEN},{TCOD_LIGHTER_GREEN},{TCOD_LIGHT_GREEN},{TCOD_GREEN},{TCOD_DARK_GREEN},{TCOD_DARKER_GREEN},{TCOD_DARKEST_GREEN}},
 {{TCOD_DESATURATED_SEA},{TCOD_LIGHTEST_SEA},{TCOD_LIGHTER_SEA},{TCOD_LIGHT_SEA},{TCOD_SEA},{TCOD_DARK_SEA},{TCOD_DARKER_SEA},{TCOD_DARKEST_SEA}},
 {{TCOD_DESATURATED_TURQUOISE},{TCOD_LIGHTEST_TURQUOISE},{TCOD_LIGHTER_TURQUOISE},{TCOD_LIGHT_TURQUOISE},{TCOD_TURQUOISE},{TCOD_DARK_TURQUOISE},{TCOD_DARKER_TURQUOISE},{TCOD_DARKEST_TURQUOISE}},
 {{TCOD_DESATURATED_CYAN},{TCOD_LIGHTEST_CYAN},{TCOD_LIGHTER_CYAN},{TCOD_LIGHT_CYAN},{TCOD_CYAN},{TCOD_DARK_CYAN},{TCOD_DARKER_CYAN},{TCOD_DARKEST_CYAN}},
 {{TCOD_DESATURATED_SKY},{TCOD_LIGHTEST_SKY},{TCOD_LIGHTER_SKY},{TCOD_LIGHT_SKY},{TCOD_SKY},{TCOD_DARK_SKY},{TCOD_DARKER_SKY},{TCOD_DARKEST_SKY}},
 {{TCOD_DESATURATED_AZURE},{TCOD_LIGHTEST_AZURE},{TCOD_LIGHTER_AZURE},{TCOD_LIGHT_AZURE},{TCOD_AZURE},{TCOD_DARK_AZURE},{TCOD_DARKER_AZURE},{TCOD_DARKEST_AZURE}},
 {{TCOD_DESATURATED_BLUE},{TCOD_LIGHTEST_BLUE},{TCOD_LIGHTER_BLUE},{TCOD_LIGHT_BLUE},{TCOD_BLUE},{TCOD_DARK_BLUE},{TCOD_DARKER_BLUE},{TCOD_DARKEST_BLUE}},
 {{TCOD_DESATURATED_HAN},{TCOD_LIGHTEST_HAN},{TCOD_LIGHTER_HAN},{TCOD_LIGHT_HAN},{TCOD_HAN},{TCOD_DARK_HAN},{TCOD_DARKER_HAN},{TCOD_DARKEST_HAN}},
 {{TCOD_DESATURATED_VIOLET},{TCOD_LIGHTEST_VIOLET},{TCOD_LIGHTER_VIOLET},{TCOD_LIGHT_VIOLET},{TCOD_VIOLET},{TCOD_DARK_VIOLET},{TCOD_DARKER_VIOLET},{TCOD_DARKEST_VIOLET}},
 {{TCOD_DESATURATED_PURPLE},{TCOD_LIGHTEST_PURPLE},{TCOD_LIGHTER_PURPLE},{TCOD_LIGHT_PURPLE},{TCOD_PURPLE},{TCOD_DARK_PURPLE},{TCOD_DARKER_PURPLE},{TCOD_DARKEST_PURPLE}},
 {{TCOD_DESATURATED_FUCHSIA},{TCOD_LIGHTEST_FUCHSIA},{TCOD_LIGHTER_FUCHSIA},{TCOD_LIGHT_FUCHSIA},{TCOD_FUCHSIA},{TCOD_DARK_FUCHSIA},{TCOD_DARKER_FUCHSIA},{TCOD_DARKEST_FUCHSIA}},
 {{TCOD_DESATURATED_MAGENTA},{TCOD_LIGHTEST_MAGENTA},{TCOD_LIGHTER_MAGENTA},{TCOD_LIGHT_MAGENTA},{TCOD_MAGENTA},{TCOD_DARK_MAGENTA},{TCOD_DARKER_MAGENTA},{TCOD_DARKEST_MAGENTA}},
 {{TCOD_DESATURATED_PINK},{TCOD_LIGHTEST_PINK},{TCOD_LIGHTER_PINK},{TCOD_LIGHT_PINK},{TCOD_PINK},{TCOD_DARK_PINK},{TCOD_DARKER_PINK},{TCOD_DARKEST_PINK}},
 {{TCOD_DESATURATED_CRIMSON},{TCOD_LIGHTEST_CRIMSON},{TCOD_LIGHTER_CRIMSON},{TCOD_LIGHT_CRIMSON},{TCOD_CRIMSON},{TCOD_DARK_CRIMSON},{TCOD_DARKER_CRIMSON},{TCOD_DARKEST_CRIMSON}}
};
/**
 *  \brief Return a new TCOD_color_t from RGB values.
 *
 *  This function is redundant, you should use a braced initializer instead:
 *
 *      TCOD_color_t white = {255, 255, 255};
 */
TCOD_color_t TCOD_color_RGB(uint8_t r, uint8_t g, uint8_t b) {
  TCOD_color_t new_color = {r, g, b};
  return new_color;
}
/**
 *  \brief Return a new TCOD_color_t from HSV values.
 *
 *  \param hue The colors hue (in degrees.)
 *  \param saturation The colors saturation (from 0 to 1)
 *  \param value The colors value (from 0 to 1)
 *  \return A new TCOD_color_t struct.
 *
 *  The saturation and value parameters are automatically clamped to 0 and 1.
 *
 *      TCOD_color_t light_blue = TCOD_color_HSV(240.0f, 0.75f, 1.0f);
 *
 *  Use TCOD_color_set_HSV to fill an existing struct with HSV values.
 */
TCOD_color_t TCOD_color_HSV(float hue, float saturation, float value) {
  TCOD_color_t new_color;
  TCOD_color_set_HSV(&new_color, hue, saturation, value);
  return new_color;
}
/**
 *  \brief Return a true value if c1 and c2 are equal.
 */
bool TCOD_color_equals(TCOD_color_t c1, TCOD_color_t  c2) {
  return (c1.r == c2.r && c1.g == c2.g && c1.b == c2.b);
}
/**
 *  \brief Add two colors together and return the result.
 *
 *  \param c1 The first color.
 *  \param c2 The second color.
 *  \return A new TCOD_color_t struct with the result.
 */
TCOD_color_t TCOD_color_add(TCOD_color_t c1, TCOD_color_t c2) {
  TCOD_color_t new_color = {
      (uint8_t)MIN(255, (int)c1.r + c2.r),
      (uint8_t)MIN(255, (int)c1.g + c2.g),
      (uint8_t)MIN(255, (int)c1.b + c2.b)};
  return new_color;
}
/**
 *  \brief Subtract c2 from c1 and return the result.
 *
 *  \param c1 The first color.
 *  \param c2 The second color.
 *  \return A new TCOD_color_t struct with the result.
 */
TCOD_color_t TCOD_color_subtract(TCOD_color_t c1, TCOD_color_t c2) {
  TCOD_color_t new_color = {
      (uint8_t)MAX(0, (int)c1.r - c2.r),
      (uint8_t)MAX(0, (int)c1.g - c2.g),
      (uint8_t)MAX(0, (int)c1.b - c2.b)};
  return new_color;
}
/**
 *  \brief Multiply two colors together and return the result.
 *
 *  \param c1 The first color.
 *  \param c2 The second color.
 *  \return A new TCOD_color_t struct with the result.
 */
TCOD_color_t TCOD_color_multiply(TCOD_color_t c1, TCOD_color_t c2) {
  TCOD_color_t new_color = {
      (uint8_t)(((int)c1.r) * c2.r / 255),
      (uint8_t)(((int)c1.g) * c2.g / 255),
      (uint8_t)(((int)c1.b) * c2.b / 255)};
  return new_color;
}
/**
 *  \brief Multiply a color with a scalar value and return the result.
 *
 *  \param c1 The color to multiply.
 *  \param value The scalar float.
 *  \return A new TCOD_color_t struct with the result.
 */
TCOD_color_t TCOD_color_multiply_scalar(TCOD_color_t c1, float value) {
  TCOD_color_t new_color = {
      (uint8_t)CLAMP(0.0f, 255.0f, (float)c1.r * value),
      (uint8_t)CLAMP(0.0f, 255.0f, (float)c1.g * value),
      (uint8_t)CLAMP(0.0f, 255.0f, (float)c1.b * value)};
  return new_color;
}
/**
 *  \brief Interpolate two colors together and return the result.
 *
 *  \param c1 The first color (where coef if 0)
 *  \param c2 The second color (where coef if 1)
 *  \param coef The coefficient.
 *  \return A new TCOD_color_t struct with the result.
 */
TCOD_color_t TCOD_color_lerp(TCOD_color_t c1, TCOD_color_t c2, float coef) {
  TCOD_color_t new_color = {
      (uint8_t)(c1.r + (c2.r - c1.r) * coef),
      (uint8_t)(c1.g + (c2.g - c1.g) * coef),
      (uint8_t)(c1.b + (c2.b - c1.b) * coef)};
  return new_color;
}
/* Return floor modulo for double values. */
static double fabsmod(double x, double n) {
  double m = fmod(x, n);
  return m < 0 ? m+n : m;
}

/**
 *  \brief Sets a colors values from HSV values.
 *
 *  \param color The color to be changed.
 *  \param hue The colors hue (in degrees.)
 *  \param saturation The colors saturation (from 0 to 1)
 *  \param value The colors value (from 0 to 1)
 */
void TCOD_color_set_HSV(TCOD_color_t *color,
                        float hue, float saturation, float value) {
  int hue_section;
  float hue_fraction, p, q, t;

  saturation = CLAMP(0.0f, 1.0f, saturation);
  value = CLAMP(0.0f, 1.0f, value);
  if( saturation == 0.0f ) { /* achromatic (grey) */
    color->r = color->g = color->b = (uint8_t)(value * 255.0f + 0.5f);
    return;
  }

  hue = (float)fabsmod(hue, 360.0f);
  hue /= 60.0f;  /* sector 0 to 5 */
  hue_section = (int)floor(hue);
  hue_fraction = hue - hue_section; /* fraction between sections */
  p = value * (1 - saturation);
  q = value * (1 - saturation * hue_fraction);
  t = value * (1 - saturation * (1 - hue_fraction));

  switch (hue_section) {
    default:
    case 0: /* red/yellow */
      color->r = (uint8_t)(value * 255.0f + 0.5f);
      color->g = (uint8_t)(t * 255.0f + 0.5f);
      color->b = (uint8_t)(p * 255.0f + 0.5f);
      break;
    case 1: /* yellow/green */
      color->r = (uint8_t)(q * 255.0f + 0.5f);
      color->g = (uint8_t)(value * 255.0f + 0.5f);
      color->b = (uint8_t)(p * 255.0f + 0.5f);
      break;
    case 2: /* green/cyan */
      color->r = (uint8_t)(p * 255.0f + 0.5f);
      color->g = (uint8_t)(value * 255.0f + 0.5f);
      color->b = (uint8_t)(t * 255.0f + 0.5f);
      break;
    case 3: /* cyan/blue */
      color->r = (uint8_t)(p * 255.0f + 0.5f);
      color->g = (uint8_t)(q * 255.0f + 0.5f);
      color->b = (uint8_t)(value * 255.0f + 0.5f);
      break;
    case 4: /* blue/purple */
      color->r = (uint8_t)(t * 255.0f + 0.5f);
      color->g = (uint8_t)(p * 255.0f + 0.5f);
      color->b = (uint8_t)(value * 255.0f + 0.5f);
      break;
    case 5: /* purple/red */
      color->r = (uint8_t)(value * 255.0f + 0.5f);
      color->g = (uint8_t)(p * 255.0f + 0.5f);
      color->b = (uint8_t)(q * 255.0f + 0.5f);
      break;
  }
}
/**
 *  \brief Get a set of HSV values from a color.
 *
 *  \param color The color
 *  \param hue Pointer to a float, filled with the hue. (degrees)
 *  \param saturation Pointer to a float, filled with the saturation. (0 to 1)
 *  \param value Pointer to a float, filled with the value. (0 to 1)
 *
 *  The hue, saturation, and value parameters can not be NULL pointers,
 */
void TCOD_color_get_HSV(TCOD_color_t color,
                        float *hue, float *saturation, float *value) {
  *hue = TCOD_color_get_hue(color);
  *saturation = TCOD_color_get_saturation(color);
  *value = TCOD_color_get_value(color);
  return;
}
/**
 *  \brief Return a colors hue.
 *
 *  \param color A color struct.
 *  \return The colors hue. (degrees)
 */
float TCOD_color_get_hue(TCOD_color_t color) {
  uint8_t max = MAX(color.r, MAX(color.g, color.b));
  uint8_t min = MIN(color.r, MIN(color.g, color.b));
  float delta = (float)max - (float)min;
  float hue; /* degrees */
  if (delta == 0.0f) { return 0.0f; } /* achromatic, including black */
  if (color.r == max) {
    hue = (float)(color.g - color.b) / delta;
  } else if (color.g == max) {
    hue = 2.0f + (float)(color.b - color.r) / delta;
  } else {
    hue = 4.0f + (float)(color.r - color.g) / delta;
  }
  hue *= 60.0f;
  hue = (float)fabsmod(hue, 360.0f);
  return hue;
}
/**
 *  \brief Change a colors hue.
 *
 *  \param color Pointer to a color struct.
 *  \param hue The hue in degrees.
 */
void TCOD_color_set_hue(TCOD_color_t *color, float hue) {
  TCOD_color_set_HSV(color,
                     hue,
                     TCOD_color_get_saturation(*color),
                     TCOD_color_get_value(*color));
}
/**
 *  \brief Return a colors saturation.
 *
 *  \param color A color struct.
 *  \return The colors saturation. (0 to 1)
 */
float TCOD_color_get_saturation (TCOD_color_t color) {
  float max = (float)(MAX(color.r, MAX(color.g, color.b))) / 255.0f;
  float min = (float)(MIN(color.r, MIN(color.g, color.b))) / 255.0f;
  float delta = max - min;
  if (max == 0.0f) { return 0.0f; }
  return delta / max;
}
/**
 *  \brief Change a colors saturation.
 *
 *  \param color Pointer to a color struct.
 *  \param saturation The desired saturation value.
 */
void TCOD_color_set_saturation(TCOD_color_t *color, float saturation) {
  TCOD_color_set_HSV(color,
                     TCOD_color_get_hue(*color),
                     saturation,
                     TCOD_color_get_value(*color));
}
/**
 *  \brief Get a colors value.
 *
 *  \param color A color struct.
 *  \return The colors value. (0 to 1)
 */
float TCOD_color_get_value(TCOD_color_t color) {
  return (float)(MAX(color.r, MAX(color.g, color.b))) / 255.0f;
}
/**
 *  \brief Change a colors value.
 *
 *  \param color Pointer to a color struct.
 *  \param value The desired value.
 */
void TCOD_color_set_value(TCOD_color_t *color, float value) {
  TCOD_color_set_HSV(color,
                     TCOD_color_get_hue(*color),
                     TCOD_color_get_saturation(*color),
                     value);
}
/**
 *  \brief Shift a colors hue by an amount.
 *
 *  \param color Pointer to a color struct.
 *  \param hue_shift The distance to shift the hue, in degrees.
 */
void TCOD_color_shift_hue(TCOD_color_t *color, float hue_shift) {
  if (hue_shift == 0.0f) { return; }
  TCOD_color_set_HSV(color,
                     TCOD_color_get_hue(*color) + hue_shift,
                     TCOD_color_get_saturation(*color),
                     TCOD_color_get_value(*color));
}
/**
 *  \brief Scale a colors saturation and value.
 *
 *  \param color Pointer to a color struct.
 *  \param saturation_coef Multiplier for this colors saturation.
 *  \param value_coef Multiplier for this colors value.
 */
void TCOD_color_scale_HSV(TCOD_color_t *color,
                          float saturation_coef, float value_coef) {
  TCOD_color_set_HSV(color,
                     TCOD_color_get_hue(*color),
                     TCOD_color_get_saturation(*color) * saturation_coef,
                     TCOD_color_get_value(*color) * value_coef);
}
/**
 *  \brief Generate an interpolated gradient of colors.
 *
 *  \param map Array to fill with the new gradient.
 *  \param nb_key The array size of the key_color and key_index parameters.
 *  \param key_color An array of colors to use, in order.
 *  \param key_index An array mapping key_color items to the map array.
 *
 *  \verbatim embed:rst:leading-asterisk
 *  .. code-block:: c
 *
 *    TCOD_color_t[256] gradient;
 *    TCOD_color_t[4] key_color = {TCOD_black, TCOD_dark_amber,
 *                                 TCOD_cyan, TCOD_white};
 *    int[4] key_index = {0, 64, 192, 255};
 *    TCOD_color_gen_map(&gradient, 4, &key_color, &key_index);
 *  \endverbatim
 */
void TCOD_color_gen_map(TCOD_color_t *map, int nb_key,
                        const TCOD_color_t *key_color, const int *key_index) {
  int segment = 0;
  for (segment = 0; segment < nb_key - 1; ++segment) {
    int idx_start = key_index[segment];
    int idx_end = key_index[segment + 1];
    int idx;
    for (idx = idx_start; idx <= idx_end; ++idx) {
      map[idx] = TCOD_color_lerp(
          key_color[segment],
          key_color[segment + 1],
          (float)(idx - idx_start) / (idx_end - idx_start));
    }
  }
}
