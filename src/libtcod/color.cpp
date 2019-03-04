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
#include "color.hpp"

#include "libtcod_int.h"

// grey levels
const TCODColor TCODColor::black(TCOD_BLACK);
const TCODColor TCODColor::darkestGrey(TCOD_DARKEST_GREY);
const TCODColor TCODColor::darkerGrey(TCOD_DARKER_GREY);
const TCODColor TCODColor::darkGrey(TCOD_DARK_GREY);
const TCODColor TCODColor::grey(TCOD_GREY);
const TCODColor TCODColor::lightGrey(TCOD_LIGHT_GREY);
const TCODColor TCODColor::lighterGrey(TCOD_LIGHTER_GREY);
const TCODColor TCODColor::lightestGrey(TCOD_LIGHTEST_GREY);
const TCODColor TCODColor::white(TCOD_WHITE);

//sepia
const TCODColor TCODColor::darkestSepia(TCOD_DARKEST_SEPIA);
const TCODColor TCODColor::darkerSepia(TCOD_DARKER_SEPIA);
const TCODColor TCODColor::darkSepia(TCOD_DARK_SEPIA);
const TCODColor TCODColor::sepia(TCOD_SEPIA);
const TCODColor TCODColor::lightSepia(TCOD_LIGHT_SEPIA);
const TCODColor TCODColor::lighterSepia(TCOD_LIGHTER_SEPIA);
const TCODColor TCODColor::lightestSepia(TCOD_LIGHTEST_SEPIA);

// standard colors
const TCODColor TCODColor::red(TCOD_RED);
const TCODColor TCODColor::flame(TCOD_FLAME);
const TCODColor TCODColor::orange(TCOD_ORANGE);
const TCODColor TCODColor::amber(TCOD_AMBER);
const TCODColor TCODColor::yellow(TCOD_YELLOW);
const TCODColor TCODColor::lime(TCOD_LIME);
const TCODColor TCODColor::chartreuse(TCOD_CHARTREUSE);
const TCODColor TCODColor::green(TCOD_GREEN);
const TCODColor TCODColor::sea(TCOD_SEA);
const TCODColor TCODColor::turquoise(TCOD_TURQUOISE);
const TCODColor TCODColor::cyan(TCOD_CYAN);
const TCODColor TCODColor::sky(TCOD_SKY);
const TCODColor TCODColor::azure(TCOD_AZURE);
const TCODColor TCODColor::blue(TCOD_BLUE);
const TCODColor TCODColor::han(TCOD_HAN);
const TCODColor TCODColor::violet(TCOD_VIOLET);
const TCODColor TCODColor::purple(TCOD_PURPLE);
const TCODColor TCODColor::fuchsia(TCOD_FUCHSIA);
const TCODColor TCODColor::magenta(TCOD_MAGENTA);
const TCODColor TCODColor::pink(TCOD_PINK);
const TCODColor TCODColor::crimson(TCOD_CRIMSON);

// dark colors
const TCODColor TCODColor::darkRed(TCOD_DARK_RED);
const TCODColor TCODColor::darkFlame(TCOD_DARK_FLAME);
const TCODColor TCODColor::darkOrange(TCOD_DARK_ORANGE);
const TCODColor TCODColor::darkAmber(TCOD_DARK_AMBER);
const TCODColor TCODColor::darkYellow(TCOD_DARK_YELLOW);
const TCODColor TCODColor::darkLime(TCOD_DARK_LIME);
const TCODColor TCODColor::darkChartreuse(TCOD_DARK_CHARTREUSE);
const TCODColor TCODColor::darkGreen(TCOD_DARK_GREEN);
const TCODColor TCODColor::darkSea(TCOD_DARK_SEA);
const TCODColor TCODColor::darkTurquoise(TCOD_DARK_TURQUOISE);
const TCODColor TCODColor::darkCyan(TCOD_DARK_CYAN);
const TCODColor TCODColor::darkSky(TCOD_DARK_SKY);
const TCODColor TCODColor::darkAzure(TCOD_DARK_AZURE);
const TCODColor TCODColor::darkBlue(TCOD_DARK_BLUE);
const TCODColor TCODColor::darkHan(TCOD_DARK_HAN);
const TCODColor TCODColor::darkViolet(TCOD_DARK_VIOLET);
const TCODColor TCODColor::darkPurple(TCOD_DARK_PURPLE);
const TCODColor TCODColor::darkFuchsia(TCOD_DARK_FUCHSIA);
const TCODColor TCODColor::darkMagenta(TCOD_DARK_MAGENTA);
const TCODColor TCODColor::darkPink(TCOD_DARK_PINK);
const TCODColor TCODColor::darkCrimson(TCOD_DARK_CRIMSON);

// darker colors
const TCODColor TCODColor::darkerRed(TCOD_DARKER_RED);
const TCODColor TCODColor::darkerFlame(TCOD_DARKER_FLAME);
const TCODColor TCODColor::darkerOrange(TCOD_DARKER_ORANGE);
const TCODColor TCODColor::darkerAmber(TCOD_DARKER_AMBER);
const TCODColor TCODColor::darkerYellow(TCOD_DARKER_YELLOW);
const TCODColor TCODColor::darkerLime(TCOD_DARKER_LIME);
const TCODColor TCODColor::darkerChartreuse(TCOD_DARKER_CHARTREUSE);
const TCODColor TCODColor::darkerGreen(TCOD_DARKER_GREEN);
const TCODColor TCODColor::darkerSea(TCOD_DARKER_SEA);
const TCODColor TCODColor::darkerTurquoise(TCOD_DARKER_TURQUOISE);
const TCODColor TCODColor::darkerCyan(TCOD_DARKER_CYAN);
const TCODColor TCODColor::darkerSky(TCOD_DARKER_SKY);
const TCODColor TCODColor::darkerAzure(TCOD_DARKER_AZURE);
const TCODColor TCODColor::darkerBlue(TCOD_DARKER_BLUE);
const TCODColor TCODColor::darkerHan(TCOD_DARKER_HAN);
const TCODColor TCODColor::darkerViolet(TCOD_DARKER_VIOLET);
const TCODColor TCODColor::darkerPurple(TCOD_DARKER_PURPLE);
const TCODColor TCODColor::darkerFuchsia(TCOD_DARKER_FUCHSIA);
const TCODColor TCODColor::darkerMagenta(TCOD_DARKER_MAGENTA);
const TCODColor TCODColor::darkerPink(TCOD_DARKER_PINK);
const TCODColor TCODColor::darkerCrimson(TCOD_DARKER_CRIMSON);

// darkest colors
const TCODColor TCODColor::darkestRed(TCOD_DARKEST_RED);
const TCODColor TCODColor::darkestFlame(TCOD_DARKEST_FLAME);
const TCODColor TCODColor::darkestOrange(TCOD_DARKEST_ORANGE);
const TCODColor TCODColor::darkestAmber(TCOD_DARKEST_AMBER);
const TCODColor TCODColor::darkestYellow(TCOD_DARKEST_YELLOW);
const TCODColor TCODColor::darkestLime(TCOD_DARKEST_LIME);
const TCODColor TCODColor::darkestChartreuse(TCOD_DARKEST_CHARTREUSE);
const TCODColor TCODColor::darkestGreen(TCOD_DARKEST_GREEN);
const TCODColor TCODColor::darkestSea(TCOD_DARKEST_SEA);
const TCODColor TCODColor::darkestTurquoise(TCOD_DARKEST_TURQUOISE);
const TCODColor TCODColor::darkestCyan(TCOD_DARKEST_CYAN);
const TCODColor TCODColor::darkestSky(TCOD_DARKEST_SKY);
const TCODColor TCODColor::darkestAzure(TCOD_DARKEST_AZURE);
const TCODColor TCODColor::darkestBlue(TCOD_DARKEST_BLUE);
const TCODColor TCODColor::darkestHan(TCOD_DARKEST_HAN);
const TCODColor TCODColor::darkestViolet(TCOD_DARKEST_VIOLET);
const TCODColor TCODColor::darkestPurple(TCOD_DARKEST_PURPLE);
const TCODColor TCODColor::darkestFuchsia(TCOD_DARKEST_FUCHSIA);
const TCODColor TCODColor::darkestMagenta(TCOD_DARKEST_MAGENTA);
const TCODColor TCODColor::darkestPink(TCOD_DARKEST_PINK);
const TCODColor TCODColor::darkestCrimson(TCOD_DARKEST_CRIMSON);

// light colors
const TCODColor TCODColor::lightRed(TCOD_LIGHT_RED);
const TCODColor TCODColor::lightFlame(TCOD_LIGHT_FLAME);
const TCODColor TCODColor::lightOrange(TCOD_LIGHT_ORANGE);
const TCODColor TCODColor::lightAmber(TCOD_LIGHT_AMBER);
const TCODColor TCODColor::lightYellow(TCOD_LIGHT_YELLOW);
const TCODColor TCODColor::lightLime(TCOD_LIGHT_LIME);
const TCODColor TCODColor::lightChartreuse(TCOD_LIGHT_CHARTREUSE);
const TCODColor TCODColor::lightGreen(TCOD_LIGHT_GREEN);
const TCODColor TCODColor::lightSea(TCOD_LIGHT_SEA);
const TCODColor TCODColor::lightTurquoise(TCOD_LIGHT_TURQUOISE);
const TCODColor TCODColor::lightCyan(TCOD_LIGHT_CYAN);
const TCODColor TCODColor::lightSky(TCOD_LIGHT_SKY);
const TCODColor TCODColor::lightAzure(TCOD_LIGHT_AZURE);
const TCODColor TCODColor::lightBlue(TCOD_LIGHT_BLUE);
const TCODColor TCODColor::lightHan(TCOD_LIGHT_HAN);
const TCODColor TCODColor::lightViolet(TCOD_LIGHT_VIOLET);
const TCODColor TCODColor::lightPurple(TCOD_LIGHT_PURPLE);
const TCODColor TCODColor::lightFuchsia(TCOD_LIGHT_FUCHSIA);
const TCODColor TCODColor::lightMagenta(TCOD_LIGHT_MAGENTA);
const TCODColor TCODColor::lightPink(TCOD_LIGHT_PINK);
const TCODColor TCODColor::lightCrimson(TCOD_LIGHT_CRIMSON);

// lighter colors
const TCODColor TCODColor::lighterRed(TCOD_LIGHTER_RED);
const TCODColor TCODColor::lighterFlame(TCOD_LIGHTER_FLAME);
const TCODColor TCODColor::lighterOrange(TCOD_LIGHTER_ORANGE);
const TCODColor TCODColor::lighterAmber(TCOD_LIGHTER_AMBER);
const TCODColor TCODColor::lighterYellow(TCOD_LIGHTER_YELLOW);
const TCODColor TCODColor::lighterLime(TCOD_LIGHTER_LIME);
const TCODColor TCODColor::lighterChartreuse(TCOD_LIGHTER_CHARTREUSE);
const TCODColor TCODColor::lighterGreen(TCOD_LIGHTER_GREEN);
const TCODColor TCODColor::lighterSea(TCOD_LIGHTER_SEA);
const TCODColor TCODColor::lighterTurquoise(TCOD_LIGHTER_TURQUOISE);
const TCODColor TCODColor::lighterCyan(TCOD_LIGHTER_CYAN);
const TCODColor TCODColor::lighterSky(TCOD_LIGHTER_SKY);
const TCODColor TCODColor::lighterAzure(TCOD_LIGHTER_AZURE);
const TCODColor TCODColor::lighterBlue(TCOD_LIGHTER_BLUE);
const TCODColor TCODColor::lighterHan(TCOD_LIGHTER_HAN);
const TCODColor TCODColor::lighterViolet(TCOD_LIGHTER_VIOLET);
const TCODColor TCODColor::lighterPurple(TCOD_LIGHTER_PURPLE);
const TCODColor TCODColor::lighterFuchsia(TCOD_LIGHTER_FUCHSIA);
const TCODColor TCODColor::lighterMagenta(TCOD_LIGHTER_MAGENTA);
const TCODColor TCODColor::lighterPink(TCOD_LIGHTER_PINK);
const TCODColor TCODColor::lighterCrimson(TCOD_LIGHTER_CRIMSON);

// lightest colors
const TCODColor TCODColor::lightestRed(TCOD_LIGHTEST_RED);
const TCODColor TCODColor::lightestFlame(TCOD_LIGHTEST_FLAME);
const TCODColor TCODColor::lightestOrange(TCOD_LIGHTEST_ORANGE);
const TCODColor TCODColor::lightestAmber(TCOD_LIGHTEST_AMBER);
const TCODColor TCODColor::lightestYellow(TCOD_LIGHTEST_YELLOW);
const TCODColor TCODColor::lightestLime(TCOD_LIGHTEST_LIME);
const TCODColor TCODColor::lightestChartreuse(TCOD_LIGHTEST_CHARTREUSE);
const TCODColor TCODColor::lightestGreen(TCOD_LIGHTEST_GREEN);
const TCODColor TCODColor::lightestSea(TCOD_LIGHTEST_SEA);
const TCODColor TCODColor::lightestTurquoise(TCOD_LIGHTEST_TURQUOISE);
const TCODColor TCODColor::lightestCyan(TCOD_LIGHTEST_CYAN);
const TCODColor TCODColor::lightestSky(TCOD_LIGHTEST_SKY);
const TCODColor TCODColor::lightestAzure(TCOD_LIGHTEST_AZURE);
const TCODColor TCODColor::lightestBlue(TCOD_LIGHTEST_BLUE);
const TCODColor TCODColor::lightestHan(TCOD_LIGHTEST_HAN);
const TCODColor TCODColor::lightestViolet(TCOD_LIGHTEST_VIOLET);
const TCODColor TCODColor::lightestPurple(TCOD_LIGHTEST_PURPLE);
const TCODColor TCODColor::lightestFuchsia(TCOD_LIGHTEST_FUCHSIA);
const TCODColor TCODColor::lightestMagenta(TCOD_LIGHTEST_MAGENTA);
const TCODColor TCODColor::lightestPink(TCOD_LIGHTEST_PINK);
const TCODColor TCODColor::lightestCrimson(TCOD_LIGHTEST_CRIMSON);

// desaturated colors
const TCODColor TCODColor::desaturatedRed(TCOD_DESATURATED_RED);
const TCODColor TCODColor::desaturatedFlame(TCOD_DESATURATED_FLAME);
const TCODColor TCODColor::desaturatedOrange(TCOD_DESATURATED_ORANGE);
const TCODColor TCODColor::desaturatedAmber(TCOD_DESATURATED_AMBER);
const TCODColor TCODColor::desaturatedYellow(TCOD_DESATURATED_YELLOW);
const TCODColor TCODColor::desaturatedLime(TCOD_DESATURATED_LIME);
const TCODColor TCODColor::desaturatedChartreuse(TCOD_DESATURATED_CHARTREUSE);
const TCODColor TCODColor::desaturatedGreen(TCOD_DESATURATED_GREEN);
const TCODColor TCODColor::desaturatedSea(TCOD_DESATURATED_SEA);
const TCODColor TCODColor::desaturatedTurquoise(TCOD_DESATURATED_TURQUOISE);
const TCODColor TCODColor::desaturatedCyan(TCOD_DESATURATED_CYAN);
const TCODColor TCODColor::desaturatedSky(TCOD_DESATURATED_SKY);
const TCODColor TCODColor::desaturatedAzure(TCOD_DESATURATED_AZURE);
const TCODColor TCODColor::desaturatedBlue(TCOD_DESATURATED_BLUE);
const TCODColor TCODColor::desaturatedHan(TCOD_DESATURATED_HAN);
const TCODColor TCODColor::desaturatedViolet(TCOD_DESATURATED_VIOLET);
const TCODColor TCODColor::desaturatedPurple(TCOD_DESATURATED_PURPLE);
const TCODColor TCODColor::desaturatedFuchsia(TCOD_DESATURATED_FUCHSIA);
const TCODColor TCODColor::desaturatedMagenta(TCOD_DESATURATED_MAGENTA);
const TCODColor TCODColor::desaturatedPink(TCOD_DESATURATED_PINK);
const TCODColor TCODColor::desaturatedCrimson(TCOD_DESATURATED_CRIMSON);

//special
const TCODColor TCODColor::brass(TCOD_BRASS);
const TCODColor TCODColor::copper(TCOD_COPPER);
const TCODColor TCODColor::gold(TCOD_GOLD);
const TCODColor TCODColor::silver(TCOD_SILVER);

//miscellaneous
const TCODColor TCODColor::celadon(TCOD_CELADON);
const TCODColor TCODColor::peach(TCOD_PEACH);

#ifndef TCOD_HAIKU
// color array
const TCODColor TCODColor::colors[TCOD_COLOR_NB][TCOD_COLOR_LEVELS] = {
 {TCODColor(TCOD_DESATURATED_RED),TCODColor(TCOD_LIGHTEST_RED),TCODColor(TCOD_LIGHTER_RED),TCODColor(TCOD_LIGHT_RED),TCODColor(TCOD_RED),TCODColor(TCOD_DARK_RED),TCODColor(TCOD_DARKER_RED),TCODColor(TCOD_DARKEST_RED)},
 {TCODColor(TCOD_DESATURATED_FLAME),TCODColor(TCOD_LIGHTEST_FLAME),TCODColor(TCOD_LIGHTER_FLAME),TCODColor(TCOD_LIGHT_FLAME),TCODColor(TCOD_FLAME),TCODColor(TCOD_DARK_FLAME),TCODColor(TCOD_DARKER_FLAME),TCODColor(TCOD_DARKEST_FLAME)},
 {TCODColor(TCOD_DESATURATED_ORANGE),TCODColor(TCOD_LIGHTEST_ORANGE),TCODColor(TCOD_LIGHTER_ORANGE),TCODColor(TCOD_LIGHT_ORANGE),TCODColor(TCOD_ORANGE),TCODColor(TCOD_DARK_ORANGE),TCODColor(TCOD_DARKER_ORANGE),TCODColor(TCOD_DARKEST_ORANGE)},
 {TCODColor(TCOD_DESATURATED_AMBER),TCODColor(TCOD_LIGHTEST_AMBER),TCODColor(TCOD_LIGHTER_AMBER),TCODColor(TCOD_LIGHT_AMBER),TCODColor(TCOD_AMBER),TCODColor(TCOD_DARK_AMBER),TCODColor(TCOD_DARKER_AMBER),TCODColor(TCOD_DARKEST_AMBER)},
 {TCODColor(TCOD_DESATURATED_YELLOW),TCODColor(TCOD_LIGHTEST_YELLOW),TCODColor(TCOD_LIGHTER_YELLOW),TCODColor(TCOD_LIGHT_YELLOW),TCODColor(TCOD_YELLOW),TCODColor(TCOD_DARK_YELLOW),TCODColor(TCOD_DARKER_YELLOW),TCODColor(TCOD_DARKEST_YELLOW)},
 {TCODColor(TCOD_DESATURATED_LIME),TCODColor(TCOD_LIGHTEST_LIME),TCODColor(TCOD_LIGHTER_LIME),TCODColor(TCOD_LIGHT_LIME),TCODColor(TCOD_LIME),TCODColor(TCOD_DARK_LIME),TCODColor(TCOD_DARKER_LIME),TCODColor(TCOD_DARKEST_LIME)},
 {TCODColor(TCOD_DESATURATED_CHARTREUSE),TCODColor(TCOD_LIGHTEST_CHARTREUSE),TCODColor(TCOD_LIGHTER_CHARTREUSE),TCODColor(TCOD_LIGHT_CHARTREUSE),TCODColor(TCOD_CHARTREUSE),TCODColor(TCOD_DARK_CHARTREUSE),TCODColor(TCOD_DARKER_CHARTREUSE),TCODColor(TCOD_DARKEST_CHARTREUSE)},
 {TCODColor(TCOD_DESATURATED_GREEN),TCODColor(TCOD_LIGHTEST_GREEN),TCODColor(TCOD_LIGHTER_GREEN),TCODColor(TCOD_LIGHT_GREEN),TCODColor(TCOD_GREEN),TCODColor(TCOD_DARK_GREEN),TCODColor(TCOD_DARKER_GREEN),TCODColor(TCOD_DARKEST_GREEN)},
 {TCODColor(TCOD_DESATURATED_SEA),TCODColor(TCOD_LIGHTEST_SEA),TCODColor(TCOD_LIGHTER_SEA),TCODColor(TCOD_LIGHT_SEA),TCODColor(TCOD_SEA),TCODColor(TCOD_DARK_SEA),TCODColor(TCOD_DARKER_SEA),TCODColor(TCOD_DARKEST_SEA)},
 {TCODColor(TCOD_DESATURATED_TURQUOISE),TCODColor(TCOD_LIGHTEST_TURQUOISE),TCODColor(TCOD_LIGHTER_TURQUOISE),TCODColor(TCOD_LIGHT_TURQUOISE),TCODColor(TCOD_TURQUOISE),TCODColor(TCOD_DARK_TURQUOISE),TCODColor(TCOD_DARKER_TURQUOISE),TCODColor(TCOD_DARKEST_TURQUOISE)},
 {TCODColor(TCOD_DESATURATED_CYAN),TCODColor(TCOD_LIGHTEST_CYAN),TCODColor(TCOD_LIGHTER_CYAN),TCODColor(TCOD_LIGHT_CYAN),TCODColor(TCOD_CYAN),TCODColor(TCOD_DARK_CYAN),TCODColor(TCOD_DARKER_CYAN),TCODColor(TCOD_DARKEST_CYAN)},
 {TCODColor(TCOD_DESATURATED_SKY),TCODColor(TCOD_LIGHTEST_SKY),TCODColor(TCOD_LIGHTER_SKY),TCODColor(TCOD_LIGHT_SKY),TCODColor(TCOD_SKY),TCODColor(TCOD_DARK_SKY),TCODColor(TCOD_DARKER_SKY),TCODColor(TCOD_DARKEST_SKY)},
 {TCODColor(TCOD_DESATURATED_AZURE),TCODColor(TCOD_LIGHTEST_AZURE),TCODColor(TCOD_LIGHTER_AZURE),TCODColor(TCOD_LIGHT_AZURE),TCODColor(TCOD_AZURE),TCODColor(TCOD_DARK_AZURE),TCODColor(TCOD_DARKER_AZURE),TCODColor(TCOD_DARKEST_AZURE)},
 {TCODColor(TCOD_DESATURATED_BLUE),TCODColor(TCOD_LIGHTEST_BLUE),TCODColor(TCOD_LIGHTER_BLUE),TCODColor(TCOD_LIGHT_BLUE),TCODColor(TCOD_BLUE),TCODColor(TCOD_DARK_BLUE),TCODColor(TCOD_DARKER_BLUE),TCODColor(TCOD_DARKEST_BLUE)},
 {TCODColor(TCOD_DESATURATED_HAN),TCODColor(TCOD_LIGHTEST_HAN),TCODColor(TCOD_LIGHTER_HAN),TCODColor(TCOD_LIGHT_HAN),TCODColor(TCOD_HAN),TCODColor(TCOD_DARK_HAN),TCODColor(TCOD_DARKER_HAN),TCODColor(TCOD_DARKEST_HAN)},
 {TCODColor(TCOD_DESATURATED_VIOLET),TCODColor(TCOD_LIGHTEST_VIOLET),TCODColor(TCOD_LIGHTER_VIOLET),TCODColor(TCOD_LIGHT_VIOLET),TCODColor(TCOD_VIOLET),TCODColor(TCOD_DARK_VIOLET),TCODColor(TCOD_DARKER_VIOLET),TCODColor(TCOD_DARKEST_VIOLET)},
 {TCODColor(TCOD_DESATURATED_PURPLE),TCODColor(TCOD_LIGHTEST_PURPLE),TCODColor(TCOD_LIGHTER_PURPLE),TCODColor(TCOD_LIGHT_PURPLE),TCODColor(TCOD_PURPLE),TCODColor(TCOD_DARK_PURPLE),TCODColor(TCOD_DARKER_PURPLE),TCODColor(TCOD_DARKEST_PURPLE)},
 {TCODColor(TCOD_DESATURATED_FUCHSIA),TCODColor(TCOD_LIGHTEST_FUCHSIA),TCODColor(TCOD_LIGHTER_FUCHSIA),TCODColor(TCOD_LIGHT_FUCHSIA),TCODColor(TCOD_FUCHSIA),TCODColor(TCOD_DARK_FUCHSIA),TCODColor(TCOD_DARKER_FUCHSIA),TCODColor(TCOD_DARKEST_FUCHSIA)},
 {TCODColor(TCOD_DESATURATED_MAGENTA),TCODColor(TCOD_LIGHTEST_MAGENTA),TCODColor(TCOD_LIGHTER_MAGENTA),TCODColor(TCOD_LIGHT_MAGENTA),TCODColor(TCOD_MAGENTA),TCODColor(TCOD_DARK_MAGENTA),TCODColor(TCOD_DARKER_MAGENTA),TCODColor(TCOD_DARKEST_MAGENTA)},
 {TCODColor(TCOD_DESATURATED_PINK),TCODColor(TCOD_LIGHTEST_PINK),TCODColor(TCOD_LIGHTER_PINK),TCODColor(TCOD_LIGHT_PINK),TCODColor(TCOD_PINK),TCODColor(TCOD_DARK_PINK),TCODColor(TCOD_DARKER_PINK),TCODColor(TCOD_DARKEST_PINK)},
 {TCODColor(TCOD_DESATURATED_CRIMSON),TCODColor(TCOD_LIGHTEST_CRIMSON),TCODColor(TCOD_LIGHTER_CRIMSON),TCODColor(TCOD_LIGHT_CRIMSON),TCODColor(TCOD_CRIMSON),TCODColor(TCOD_DARK_CRIMSON),TCODColor(TCOD_DARKER_CRIMSON),TCODColor(TCOD_DARKEST_CRIMSON)}
};
#endif

TCODColor::TCODColor(float h, float s, float v) {
	//setHSV(h,s,v);
	TCOD_color_t c = TCOD_color_HSV(h,s,v);
	r = c.r; g = c.g; b = c.b;
}

void TCODColor::setHSV(float h, float s, float v) {
	TCOD_color_t c;
	TCOD_color_set_HSV(&c,h,s,v);
	r = c.r; g = c.g; b = c.b;
}

void TCODColor::setHue (float h) {
	TCOD_color_t c = { r, g, b };
	TCOD_color_set_hue (&c, h);
	r = c.r; g = c.g; b = c.b;
}

void TCODColor::setSaturation (float s) {
	TCOD_color_t c = { r, g, b };
	TCOD_color_set_saturation (&c, s);
	r = c.r; g = c.g; b = c.b;
}

void TCODColor::setValue (float v) {
	TCOD_color_t c = { r, g, b };
	TCOD_color_set_value (&c, v);
	r = c.r; g = c.g; b = c.b;
}

void TCODColor::getHSV(float *h, float *s, float *v) const {
  TCOD_color_t c={r,g,b};
  TCOD_color_get_HSV(c,h,s,v);
}

float TCODColor::getHue () {
	TCOD_color_t c = { r, g, b };
	return TCOD_color_get_hue(c);
}

float TCODColor::getSaturation () {
	TCOD_color_t c = { r, g, b };
	return TCOD_color_get_saturation(c);
}

float TCODColor::getValue () {
	TCOD_color_t c = { r, g, b };
	return TCOD_color_get_value(c);
}

void TCODColor::shiftHue (float hshift) {
	TCOD_color_t c = { r, g, b };
	TCOD_color_shift_hue (&c, hshift);
	r = c.r; g = c.g; b = c.b;
}

void TCODColor::scaleHSV (float sscale, float vscale) {
  TCOD_color_t c = {r,g,b};
  TCOD_color_scale_HSV(&c,sscale,vscale);
  r = c.r; g = c.g; b = c.b;
}

// non member operators
TCODColor operator *(float value, const TCODColor &c) {
	return c*value;
}

void TCODColor::genMap(TCODColor *map, int nbKey, TCODColor const *keyColor,
                       int const *keyIndex)
{
  for (int segment = 0; segment < nbKey - 1; ++segment) {
    int idxStart = keyIndex[segment];
    int idxEnd = keyIndex[segment + 1];
    for (int idx = idxStart; idx <= idxEnd; ++idx) {
      map[idx] = TCODColor::lerp(
          keyColor[segment],
          keyColor[segment + 1],
          static_cast<float>(idx - idxStart) / (idxEnd - idxStart)
      );
    }
  }
}
