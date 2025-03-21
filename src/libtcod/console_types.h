/* BSD 3-Clause License
 *
 * Copyright © 2008-2025, Jice and the libtcod contributors.
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
#pragma once
#ifndef TCOD_CONSOLE_TYPES_H_
#define TCOD_CONSOLE_TYPES_H_

#include "color.h"
#include "config.h"
#include "console.h"

/***************************************************************************
    @brief Libtcod specific codes representing keys on the keyboard.

    @details
    When no key was pressed (see checkForEvent) : TCOD_NONE (NoKey)

    Special keys:
    - TCODK_ESCAPE (Escape)
    - TCODK_BACKSPACE (Backspace)
    - TCODK_TAB (Tab)
    - TCODK_ENTER (Enter)
    - TCODK_SHIFT (Shift)
    - TCODK_CONTROL (Control)
    - TCODK_ALT (Alt)
    - TCODK_PAUSE (Pause)
    - TCODK_CAPSLOCK (CapsLock)
    - TCODK_PAGEUP (PageUp)
    - TCODK_PAGEDOWN (PageDown)
    - TCODK_END (End)
    - TCODK_HOME (Home)
    - TCODK_UP (Up)
    - TCODK_LEFT (Left)
    - TCODK_RIGHT (Right)
    - TCODK_DOWN (Down)
    - TCODK_PRINTSCREEN (Printscreen)
    - TCODK_INSERT (Insert)
    - TCODK_DELETE (Delete)
    - TCODK_LWIN (Lwin)
    - TCODK_RWIN (Rwin)
    - TCODK_APPS (Apps)
    - TCODK_KPADD (KeypadAdd)
    - TCODK_KPSUB (KeypadSubtract)
    - TCODK_KPDIV (KeypadDivide)
    - TCODK_KPMUL (KeypadMultiply)
    - TCODK_KPDEC (KeypadDecimal)
    - TCODK_KPENTER (KeypadEnter)
    - TCODK_F1 (F1)
    - TCODK_F2 (F2)
    - TCODK_F3 (F3)
    - TCODK_F4 (F4)
    - TCODK_F5 (F5)
    - TCODK_F6 (F6)
    - TCODK_F7 (F7)
    - TCODK_F8 (F8)
    - TCODK_F9 (F9)
    - TCODK_F10 (F10)
    - TCODK_F11 (F11)
    - TCODK_F12 (F12)
    - TCODK_NUMLOCK (Numlock)
    - TCODK_SCROLLLOCK (Scrolllock)
    - TCODK_SPACE (Space)

    Numeric keys:
    - TCODK_0 (Zero)
    - TCODK_1 (One)
    - TCODK_2 (Two)
    - TCODK_3 (Three)
    - TCODK_4 (Four)
    - TCODK_5 (Five)
    - TCODK_6 (Six)
    - TCODK_7 (Seven)
    - TCODK_8 (Eight)
    - TCODK_9 (Nine)
    - TCODK_KP0 (KeypadZero)
    - TCODK_KP1 (KeypadOne)
    - TCODK_KP2 (KeypadTwo)
    - TCODK_KP3 (KeypadThree)
    - TCODK_KP4 (KeypadFour)
    - TCODK_KP5 (KeypadFive)
    - TCODK_KP6 (KeypadSix)
    - TCODK_KP7 (KeypadSeven)
    - TCODK_KP8 (KeypadEight)
    - TCODK_KP9 (KeypadNine)

    Any other (printable) key:
    - TCODK_CHAR (Char)
    - TCODK_TEXT (SDL_TEXTINPUT)

    Codes starting with TCODK_KP represents keys on the numeric keypad (if available).

    @deprecated
    Using libtcod for events means only a limited set of keys are available.
    Use SDL for events to access a complete range of keys.
 */
typedef enum TCOD_keycode_t {
  TCODK_NONE,
  TCODK_ESCAPE,
  TCODK_BACKSPACE,
  TCODK_TAB,
  TCODK_ENTER,
  TCODK_SHIFT,
  TCODK_CONTROL,
  TCODK_ALT,
  TCODK_PAUSE,
  TCODK_CAPSLOCK,
  TCODK_PAGEUP,
  TCODK_PAGEDOWN,
  TCODK_END,
  TCODK_HOME,
  TCODK_UP,
  TCODK_LEFT,
  TCODK_RIGHT,
  TCODK_DOWN,
  TCODK_PRINTSCREEN,
  TCODK_INSERT,
  TCODK_DELETE,
  TCODK_LWIN,
  TCODK_RWIN,
  TCODK_APPS,
  TCODK_0,
  TCODK_1,
  TCODK_2,
  TCODK_3,
  TCODK_4,
  TCODK_5,
  TCODK_6,
  TCODK_7,
  TCODK_8,
  TCODK_9,
  TCODK_KP0,
  TCODK_KP1,
  TCODK_KP2,
  TCODK_KP3,
  TCODK_KP4,
  TCODK_KP5,
  TCODK_KP6,
  TCODK_KP7,
  TCODK_KP8,
  TCODK_KP9,
  TCODK_KPADD,
  TCODK_KPSUB,
  TCODK_KPDIV,
  TCODK_KPMUL,
  TCODK_KPDEC,
  TCODK_KPENTER,
  TCODK_F1,
  TCODK_F2,
  TCODK_F3,
  TCODK_F4,
  TCODK_F5,
  TCODK_F6,
  TCODK_F7,
  TCODK_F8,
  TCODK_F9,
  TCODK_F10,
  TCODK_F11,
  TCODK_F12,
  TCODK_NUMLOCK,
  TCODK_SCROLLLOCK,
  TCODK_SPACE,
  TCODK_CHAR,
  TCODK_TEXT
} TCOD_keycode_t;

#define TCOD_KEY_TEXT_SIZE 32

/***************************************************************************
    @brief Libtcod key event data, as a keycode or text character.

    @deprecated
    The libtcod keyboard state has several known issues such as missing or broken functionality.
    In its current state it exists only for backwards compatibility.
    These issues should be resolved by using SDL directly for keyboard events.
 */
typedef struct TCOD_key_t {
  /***************************************************************************
      @brief The TCOD_keycode_t enum of the current key.
   */
  TCOD_keycode_t vk;
  /***************************************************************************
      @brief The printable character of a keycode if `vk == TCODK_CHAR`, else `0`.

      @details
      Libtcod 1.6 switched form SDL1 to SDL2 which changed the values returned by this attribute.

      Before 1.6 this value could be affected by modifiers such as the shift key.

      After 1.6 the SDL key symbol is always returned, which will be the same no matter which modifiers are held.

      @deprecated
      The nature of this attribute makes it unsuitable for both printable keys and standard key inputs.
      Use SDL events instead to differentiate between keycodes, symbols, printable characters.
   */
  char c;
  /***************************************************************************
      @brief The UTF-8 text of a key when `vk == TCODK_TEXT`.  Otherwise this will always be `'\0'`.

      @details TCODK_TEXT is always derived from an SDL_TEXTINPUT event.
   */
  char text[TCOD_KEY_TEXT_SIZE];
  /***************************************************************************
      @brief True if is this key was pressed. False if it was released.
   */
  bool pressed;
  /***************************************************************************
      @brief True if left alt was held during this event.
   */
  bool lalt;
  /***************************************************************************
      @brief True if left control was held during this event.
   */
  bool lctrl;
  /***************************************************************************
      @brief True if the left meta key was held during this event.
   */
  bool lmeta;
  /***************************************************************************
      @brief True if right alt was held during this event.
   */
  bool ralt;
  /***************************************************************************
      @brief True if right control was held during this event.
   */
  bool rctrl;
  /***************************************************************************
      @brief True if the right meta key was held during this event.
   */
  bool rmeta;
  /***************************************************************************
      @brief True if shift was held during this event.
   */
  bool shift;
} TCOD_key_t;

/***************************************************************************
    @brief Non-standard special character codes.

    @deprecated
    Modern libtcod programs should always uses the Unicode codepoint of special characters and never this enum.
 */
typedef enum TCOD_chars_t {
  /* single walls */
  TCOD_CHAR_HLINE TCOD_DEPRECATED_ENUM = 196,
  TCOD_CHAR_VLINE TCOD_DEPRECATED_ENUM = 179,
  TCOD_CHAR_NE TCOD_DEPRECATED_ENUM = 191,
  TCOD_CHAR_NW TCOD_DEPRECATED_ENUM = 218,
  TCOD_CHAR_SE TCOD_DEPRECATED_ENUM = 217,
  TCOD_CHAR_SW TCOD_DEPRECATED_ENUM = 192,
  TCOD_CHAR_TEEW TCOD_DEPRECATED_ENUM = 180,
  TCOD_CHAR_TEEE TCOD_DEPRECATED_ENUM = 195,
  TCOD_CHAR_TEEN TCOD_DEPRECATED_ENUM = 193,
  TCOD_CHAR_TEES TCOD_DEPRECATED_ENUM = 194,
  TCOD_CHAR_CROSS TCOD_DEPRECATED_ENUM = 197,
  /* double walls */
  TCOD_CHAR_DHLINE TCOD_DEPRECATED_ENUM = 205,
  TCOD_CHAR_DVLINE TCOD_DEPRECATED_ENUM = 186,
  TCOD_CHAR_DNE TCOD_DEPRECATED_ENUM = 187,
  TCOD_CHAR_DNW TCOD_DEPRECATED_ENUM = 201,
  TCOD_CHAR_DSE TCOD_DEPRECATED_ENUM = 188,
  TCOD_CHAR_DSW TCOD_DEPRECATED_ENUM = 200,
  TCOD_CHAR_DTEEW TCOD_DEPRECATED_ENUM = 185,
  TCOD_CHAR_DTEEE TCOD_DEPRECATED_ENUM = 204,
  TCOD_CHAR_DTEEN TCOD_DEPRECATED_ENUM = 202,
  TCOD_CHAR_DTEES TCOD_DEPRECATED_ENUM = 203,
  TCOD_CHAR_DCROSS TCOD_DEPRECATED_ENUM = 206,
  /* blocks */
  TCOD_CHAR_BLOCK1 TCOD_DEPRECATED_ENUM = 176,
  TCOD_CHAR_BLOCK2 TCOD_DEPRECATED_ENUM = 177,
  TCOD_CHAR_BLOCK3 TCOD_DEPRECATED_ENUM = 178,
  /* arrows */
  TCOD_CHAR_ARROW_N TCOD_DEPRECATED_ENUM = 24,
  TCOD_CHAR_ARROW_S TCOD_DEPRECATED_ENUM = 25,
  TCOD_CHAR_ARROW_E TCOD_DEPRECATED_ENUM = 26,
  TCOD_CHAR_ARROW_W TCOD_DEPRECATED_ENUM = 27,
  /* arrows without tail */
  TCOD_CHAR_ARROW2_N TCOD_DEPRECATED_ENUM = 30,
  TCOD_CHAR_ARROW2_S TCOD_DEPRECATED_ENUM = 31,
  TCOD_CHAR_ARROW2_E TCOD_DEPRECATED_ENUM = 16,
  TCOD_CHAR_ARROW2_W TCOD_DEPRECATED_ENUM = 17,
  /* double arrows */
  TCOD_CHAR_DARROW_H TCOD_DEPRECATED_ENUM = 29,
  TCOD_CHAR_DARROW_V TCOD_DEPRECATED_ENUM = 18,
  /* GUI stuff */
  TCOD_CHAR_CHECKBOX_UNSET TCOD_DEPRECATED_ENUM = 224,
  TCOD_CHAR_CHECKBOX_SET TCOD_DEPRECATED_ENUM = 225,
  TCOD_CHAR_RADIO_UNSET TCOD_DEPRECATED_ENUM = 9,
  TCOD_CHAR_RADIO_SET TCOD_DEPRECATED_ENUM = 10,
  /* sub-pixel resolution kit */
  TCOD_CHAR_SUBP_NW TCOD_DEPRECATED_ENUM = 226,
  TCOD_CHAR_SUBP_NE TCOD_DEPRECATED_ENUM = 227,
  TCOD_CHAR_SUBP_N TCOD_DEPRECATED_ENUM = 228,
  TCOD_CHAR_SUBP_SE TCOD_DEPRECATED_ENUM = 229,
  TCOD_CHAR_SUBP_DIAG TCOD_DEPRECATED_ENUM = 230,
  TCOD_CHAR_SUBP_E TCOD_DEPRECATED_ENUM = 231,
  TCOD_CHAR_SUBP_SW TCOD_DEPRECATED_ENUM = 232,
  /* miscellaneous */
  TCOD_CHAR_SMILIE TCOD_DEPRECATED_ENUM = 1,
  TCOD_CHAR_SMILIE_INV TCOD_DEPRECATED_ENUM = 2,
  TCOD_CHAR_HEART TCOD_DEPRECATED_ENUM = 3,
  TCOD_CHAR_DIAMOND TCOD_DEPRECATED_ENUM = 4,
  TCOD_CHAR_CLUB TCOD_DEPRECATED_ENUM = 5,
  TCOD_CHAR_SPADE TCOD_DEPRECATED_ENUM = 6,
  TCOD_CHAR_BULLET TCOD_DEPRECATED_ENUM = 7,
  TCOD_CHAR_BULLET_INV TCOD_DEPRECATED_ENUM = 8,
  TCOD_CHAR_MALE TCOD_DEPRECATED_ENUM = 11,
  TCOD_CHAR_FEMALE TCOD_DEPRECATED_ENUM = 12,
  TCOD_CHAR_NOTE TCOD_DEPRECATED_ENUM = 13,
  TCOD_CHAR_NOTE_DOUBLE TCOD_DEPRECATED_ENUM = 14,
  TCOD_CHAR_LIGHT TCOD_DEPRECATED_ENUM = 15,
  TCOD_CHAR_EXCLAM_DOUBLE TCOD_DEPRECATED_ENUM = 19,
  TCOD_CHAR_PILCROW TCOD_DEPRECATED_ENUM = 20,
  TCOD_CHAR_SECTION TCOD_DEPRECATED_ENUM = 21,
  TCOD_CHAR_POUND TCOD_DEPRECATED_ENUM = 156,
  TCOD_CHAR_MULTIPLICATION TCOD_DEPRECATED_ENUM = 158,
  TCOD_CHAR_FUNCTION TCOD_DEPRECATED_ENUM = 159,
  TCOD_CHAR_RESERVED TCOD_DEPRECATED_ENUM = 169,
  TCOD_CHAR_HALF TCOD_DEPRECATED_ENUM = 171,
  TCOD_CHAR_ONE_QUARTER TCOD_DEPRECATED_ENUM = 172,
  TCOD_CHAR_COPYRIGHT TCOD_DEPRECATED_ENUM = 184,
  TCOD_CHAR_CENT TCOD_DEPRECATED_ENUM = 189,
  TCOD_CHAR_YEN TCOD_DEPRECATED_ENUM = 190,
  TCOD_CHAR_CURRENCY TCOD_DEPRECATED_ENUM = 207,
  TCOD_CHAR_THREE_QUARTERS TCOD_DEPRECATED_ENUM = 243,
  TCOD_CHAR_DIVISION TCOD_DEPRECATED_ENUM = 246,
  TCOD_CHAR_GRADE TCOD_DEPRECATED_ENUM = 248,
  TCOD_CHAR_UMLAUT TCOD_DEPRECATED_ENUM = 249,
  TCOD_CHAR_POW1 TCOD_DEPRECATED_ENUM = 251,
  TCOD_CHAR_POW3 TCOD_DEPRECATED_ENUM = 252,
  TCOD_CHAR_POW2 TCOD_DEPRECATED_ENUM = 253,
  TCOD_CHAR_BULLET_SQUARE TCOD_DEPRECATED_ENUM = 254,
  /* diacritics */
} TCOD_chars_t TCOD_DEPRECATED_ENUM;

#if defined(_MSC_VER) && !defined(__clang__)
#pragma deprecated(TCOD_CHAR_HLINE)
#pragma deprecated(TCOD_CHAR_VLINE)
#pragma deprecated(TCOD_CHAR_NE)
#pragma deprecated(TCOD_CHAR_NW)
#pragma deprecated(TCOD_CHAR_SE)
#pragma deprecated(TCOD_CHAR_SW)
#pragma deprecated(TCOD_CHAR_TEEW)
#pragma deprecated(TCOD_CHAR_TEEE)
#pragma deprecated(TCOD_CHAR_TEEN)
#pragma deprecated(TCOD_CHAR_TEES)
#pragma deprecated(TCOD_CHAR_CROSS)
#pragma deprecated(TCOD_CHAR_DHLINE)
#pragma deprecated(TCOD_CHAR_DVLINE)
#pragma deprecated(TCOD_CHAR_DNE)
#pragma deprecated(TCOD_CHAR_DNW)
#pragma deprecated(TCOD_CHAR_DSE)
#pragma deprecated(TCOD_CHAR_DSW)
#pragma deprecated(TCOD_CHAR_DTEEW)
#pragma deprecated(TCOD_CHAR_DTEEE)
#pragma deprecated(TCOD_CHAR_DTEEN)
#pragma deprecated(TCOD_CHAR_DTEES)
#pragma deprecated(TCOD_CHAR_DCROSS)
#pragma deprecated(TCOD_CHAR_BLOCK1)
#pragma deprecated(TCOD_CHAR_BLOCK2)
#pragma deprecated(TCOD_CHAR_BLOCK3)
#pragma deprecated(TCOD_CHAR_ARROW_N)
#pragma deprecated(TCOD_CHAR_ARROW_S)
#pragma deprecated(TCOD_CHAR_ARROW_E)
#pragma deprecated(TCOD_CHAR_ARROW_W)
#pragma deprecated(TCOD_CHAR_ARROW2_N)
#pragma deprecated(TCOD_CHAR_ARROW2_S)
#pragma deprecated(TCOD_CHAR_ARROW2_E)
#pragma deprecated(TCOD_CHAR_ARROW2_W)
#pragma deprecated(TCOD_CHAR_DARROW_H)
#pragma deprecated(TCOD_CHAR_DARROW_V)
#pragma deprecated(TCOD_CHAR_CHECKBOX_UNSET)
#pragma deprecated(TCOD_CHAR_CHECKBOX_SET)
#pragma deprecated(TCOD_CHAR_RADIO_UNSET)
#pragma deprecated(TCOD_CHAR_RADIO_SET)
#pragma deprecated(TCOD_CHAR_SUBP_NW)
#pragma deprecated(TCOD_CHAR_SUBP_NE)
#pragma deprecated(TCOD_CHAR_SUBP_N)
#pragma deprecated(TCOD_CHAR_SUBP_SE)
#pragma deprecated(TCOD_CHAR_SUBP_DIAG)
#pragma deprecated(TCOD_CHAR_SUBP_E)
#pragma deprecated(TCOD_CHAR_SUBP_SW)
#pragma deprecated(TCOD_CHAR_SMILIE)
#pragma deprecated(TCOD_CHAR_SMILIE_INV)
#pragma deprecated(TCOD_CHAR_HEART)
#pragma deprecated(TCOD_CHAR_DIAMOND)
#pragma deprecated(TCOD_CHAR_CLUB)
#pragma deprecated(TCOD_CHAR_SPADE)
#pragma deprecated(TCOD_CHAR_BULLET)
#pragma deprecated(TCOD_CHAR_BULLET_INV)
#pragma deprecated(TCOD_CHAR_MALE)
#pragma deprecated(TCOD_CHAR_FEMALE)
#pragma deprecated(TCOD_CHAR_NOTE)
#pragma deprecated(TCOD_CHAR_NOTE_DOUBLE)
#pragma deprecated(TCOD_CHAR_LIGHT)
#pragma deprecated(TCOD_CHAR_EXCLAM_DOUBLE)
#pragma deprecated(TCOD_CHAR_PILCROW)
#pragma deprecated(TCOD_CHAR_SECTION)
#pragma deprecated(TCOD_CHAR_POUND)
#pragma deprecated(TCOD_CHAR_MULTIPLICATION)
#pragma deprecated(TCOD_CHAR_FUNCTION)
#pragma deprecated(TCOD_CHAR_RESERVED)
#pragma deprecated(TCOD_CHAR_HALF)
#pragma deprecated(TCOD_CHAR_ONE_QUARTER)
#pragma deprecated(TCOD_CHAR_COPYRIGHT)
#pragma deprecated(TCOD_CHAR_CENT)
#pragma deprecated(TCOD_CHAR_YEN)
#pragma deprecated(TCOD_CHAR_CURRENCY)
#pragma deprecated(TCOD_CHAR_THREE_QUARTERS)
#pragma deprecated(TCOD_CHAR_DIVISION)
#pragma deprecated(TCOD_CHAR_GRADE)
#pragma deprecated(TCOD_CHAR_UMLAUT)
#pragma deprecated(TCOD_CHAR_POW1)
#pragma deprecated(TCOD_CHAR_POW3)
#pragma deprecated(TCOD_CHAR_POW2)
#pragma deprecated(TCOD_CHAR_BULLET_SQUARE)
#endif  // _MSC_VER
/***************************************************************************
    @brief Bitwise flags used for functions such as TCOD_console_check_for_keypress()
    This was replaced by the equivalent values of TCOD_event_t.
 */
typedef enum {
  TCOD_KEY_PRESSED = 1,
  TCOD_KEY_RELEASED = 2,
} TCOD_key_status_t;
/**
 *  These font flags can be OR'd together into a bit-field and passed to
 *  TCOD_console_set_custom_font
 */
typedef enum {
  /** Tiles are arranged in column-major order.
   *
   *       0 3 6
   *       1 4 7
   *       2 5 8
   */
  TCOD_FONT_LAYOUT_ASCII_INCOL = 1,
  /** Tiles are arranged in row-major order.
   *
   *       0 1 2
   *       3 4 5
   *       6 7 8
   */
  TCOD_FONT_LAYOUT_ASCII_INROW = 2,
  /** Converts all tiles into a monochrome gradient. */
  TCOD_FONT_TYPE_GREYSCALE = 4,
  TCOD_FONT_TYPE_GRAYSCALE = 4,
  /** A unique layout used by some of libtcod's fonts. */
  TCOD_FONT_LAYOUT_TCOD = 8,
  /**
   *  Decode a code page 437 tileset into Unicode code-points.
   *  \rst
   *  .. versionadded:: 1.10
   *  \endrst
   */
  TCOD_FONT_LAYOUT_CP437 = 16,
} TCOD_font_flags_t;
/***************************************************************************
    @brief Libtcod rendering modes.
 */
typedef enum TCOD_renderer_t {
  /***************************************************************************
      @brief Alias for TCOD_RENDERER_OPENGL2.
   */
  TCOD_RENDERER_GLSL,
  /***************************************************************************
      An OpenGL 1.1 implementation.

      Performs worse than TCOD_RENDERER_GLSL without many benefits.

      \rst
      .. deprecated:: 1.23
          This renderer has been removed.
      \endrst
   */
  TCOD_RENDERER_OPENGL,
  /***************************************************************************
      A software based renderer.

      The font file is loaded into RAM instead of VRAM in this implementation.

      \rst
      .. deprecated:: 1.23
          This renderer has been removed.
      \endrst
   */
  TCOD_RENDERER_SDL,
  /***************************************************************************
      A new SDL2 renderer.  Allows the window to be resized.

      You may set `SDL_HINT_RENDER_SCALE_QUALITY` to determine the tileset
      upscaling filter.  Either nearest or linear.  The hint will only take
      effect if it's set before this renderer is created.

      \rst
      .. versionadded:: 1.8
      \endrst
   */
  TCOD_RENDERER_SDL2,
  /***************************************************************************
      A new OpenGL 2.0 core renderer.  Allows the window to be resized.

      You may set `SDL_HINT_RENDER_SCALE_QUALITY` to determine the tileset
      upscaling filter.  Either nearest or linear.  The hint will take effect
      on the next frame.

      \rst
      .. versionadded:: 1.9

      .. versionchanged:: 1.11
          This renderer now uses OpenGL 2.0 instead of 2.1.

      .. versionchanged:: 1.16
          Now checks the `SDL_HINT_RENDER_SCALE_QUALITY` hint.

      .. deprecated:: 1.23
          This renderer has been removed.
      \endrst
   */
  TCOD_RENDERER_OPENGL2,
  /***************************************************************************
      @brief A renderer targeting modern XTerm terminals with 24-bit color support.

      This is an experimental renderer with partial support for XTerm and SSH.
      This will work best on those terminals.

      Terminal inputs and events will be passed to SDL's event system.

      There is poor support for ANSI escapes on Windows 10.
      It is not recommended to use this renderer on Windows.

      \rst
      .. versionadded:: 1.20
      \endrst
   */
  TCOD_RENDERER_XTERM,
  TCOD_NB_RENDERERS,
} TCOD_renderer_t;
#endif  // TCOD_CONSOLE_TYPES_H_
