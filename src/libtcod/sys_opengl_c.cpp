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
/*
 * This renderer is mostly copied and pasted from Antagonist's SkyFire GLSL roguelike engine
 */
#ifndef TCOD_BARE
#include "sys.h"

#include "libtcod_int.h"
#include "console.h"

#include <stdio.h>
#include <string.h>
#include <SDL.h>
#include "sdl2/gl2_ext_.h"

#define CHECKGL(GLcall) GLcall; tcod::sdl2::gl_check()
#define DBGCHECKGL(GLcall) GLcall; tcod::sdl2::gl_debug()

typedef enum {
  Character,
  ForeCol,
  BackCol,
  ConsoleDataEnumSize
} ConsoleDataEnum;
/* JBR04152012 - Made Character a 4 byte value here to support extended characters like other renderers.
   Seems like it should be possible to make it a two byte value using GL_UNSIGNED_SHORT_5_6_5_REV in updateTex,
   but I can't seem to get the math right in the shader code, it always loses precision somewhere,
   resulting in incorrect characters. */
const int ConsoleDataAlignment[3] = {4, 3, 3 };

static const char *TCOD_con_vertex_shader =
#ifndef NDEBUG
"#version 110\n"
#endif
"uniform vec2 termsize; "

"void main(void) "
"{ "

"   gl_Position = gl_Vertex; "

"   gl_TexCoord[0] = gl_MultiTexCoord0; "
"   gl_TexCoord[0].x = gl_TexCoord[0].x*termsize.x; "
"   gl_TexCoord[0].y = gl_TexCoord[0].y*termsize.y; "
"} "
;

static const char *TCOD_con_pixel_shader =
#ifndef NDEBUG
"#version 110\n"
#endif
"uniform sampler2D font; "
"uniform sampler2D term; "
"uniform sampler2D termfcol; "
"uniform sampler2D termbcol; "

"uniform float fontw; "
"uniform vec2 fontcoef; "
"uniform vec2 termsize; "
"uniform vec2 termcoef; "

"void main(void) "
"{ "
"   vec2 rawCoord = gl_TexCoord[0].xy; "                           /* varying from [0, termsize) in x and y */
"   vec2 conPos = floor(rawCoord); "                               /* console integer position */
"   vec2 pixPos = fract(rawCoord); "                               /* pixel offset within console position */
"   pixPos = vec2(pixPos.x*fontcoef.x,pixPos.y*fontcoef.y); "      /* Correct pixel offset for font tex location */

"   vec2 address = vec2(conPos.x*termcoef.x,conPos.y*termcoef.y); "
"   address=address+vec2(0.001, 0.001); "
"   vec4 charvec = texture2D(term,address);"
"   float inchar = (charvec.r * 256.0) + (charvec.g * 256.0 * 256.0);"          /* character */
"   vec4 tcharfcol = texture2D(termfcol, address); "           /* front color */
"   vec4 tcharbcol = texture2D(termbcol, address); "           /* back color */

"   vec4 tchar = vec4(mod(floor(inchar),floor(fontw)),floor(inchar/fontw), 0.0, 0.0); "  /* 1D index to 2D index map for character */

"   gl_FragColor = texture2D(font, vec2((tchar.x*fontcoef.x),(tchar.y*fontcoef.y))+pixPos.xy); "   /* magic func: finds pixel value in font file */
"   gl_FragColor=gl_FragColor.a*tcharfcol+(1.0-gl_FragColor.a)*tcharbcol; "      /* Coloring stage */
"} "
;
/**
 *  Called before creating a window with the older libtcod renderers.
 */
void TCOD_opengl_init_attributes(void) {
  switch (TCOD_ctx.renderer) {
    case TCOD_RENDERER_GLSL:
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                          SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

      break;
    case TCOD_RENDERER_OPENGL:
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                          SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
      break;
    default: break;
  }
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
}

/* console size (power of 2 and cells) */
static int POTconwidth, POTconheight, conwidth, conheight;
/* programs and shaders handles */
static GLuint conProgram, conVertShader, conFragShader;
/* font texture handle */
static GLuint font_tex;
/* font power of 2 size and pixels */
static int POTfontwidth,POTfontheight, fontwidth,fontheight;
/* console data */
static GLuint Tex[ConsoleDataEnumSize];
static unsigned char *data[ConsoleDataEnumSize];
static bool dirty[ConsoleDataEnumSize];

static SDL_GLContext glcontext;

void TCOD_opengl_uninit_state()
{
  SDL_GL_DeleteContext(glcontext);
}
/**
 *  Called after creating an OpenGL window.
 */
bool TCOD_opengl_init_state(int conw, int conh, SDL_Surface* font)
{
  SDL_PixelFormat *my_format =
      SDL_AllocFormat(SDL_GetWindowPixelFormat(window));

  glcontext = SDL_GL_CreateContext(window);

  if(!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
    throw std::runtime_error("Failed to invoke the GLAD loader.");
  }
  // check OpenGL extensions
  if (TCOD_ctx.renderer == TCOD_RENDERER_GLSL) {
    if (!GLAD_GL_VERSION_2_0) {
      TCOD_LOG(("Falling back to fixed pipeline...\n"));
      TCOD_ctx.renderer = TCOD_RENDERER_OPENGL;
    }
  }
  // set OpenGL state
  glEnable(GL_TEXTURE_2D);
  glClearColor(1.0f, 1.0f, 0.0f, 0.0f);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glClear(GL_COLOR_BUFFER_BIT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if (TCOD_ctx.renderer == TCOD_RENDERER_GLSL) {
    glOrtho(0, conw, 0, conh, -1.0, 1.0);
    glDisable(GL_BLEND);
  } else {
    glOrtho(0, conw, conh, 0.0, -1.0, 1.0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  }

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  if (!TCOD_ctx.fullscreen) {
    SDL_GL_SetSwapInterval(0); // turn vsync off in windowed mode
  }
  // compute pot size
  conwidth = conw;
  conheight = conh;
  POTconwidth = POTconheight = 1;
  while (POTconwidth < conw) { POTconwidth *= 2; }
  while (POTconheight < conh) { POTconheight *= 2; }
  // convert font for OpenGL
  uint32_t rmask, gmask, bmask, amask;
  SDL_Surface *temp;
  SDL_Surface *temp_alpha;

  #if SDL_BYTEORDER == SDL_BIG_ENDIAN
  rmask = 0xff000000;
  gmask = 0x00ff0000;
  bmask = 0x0000ff00;
  amask = 0x000000ff;
  #else
  rmask = 0x000000ff;
  gmask = 0x0000ff00;
  bmask = 0x00ff0000;
  amask = 0xff000000;
  #endif

  fontwidth = font->w;
  fontheight = font->h;
  POTfontwidth = POTfontheight=1;
  while (POTfontwidth < fontwidth) { POTfontwidth *= 2; }
  while (POTfontheight < fontheight) { POTfontheight *= 2; }

  SDL_SetColorKey(font, 1, SDL_MapRGB(font->format, 0, 0, 0));
  my_format->Amask = amask;
  temp_alpha = SDL_ConvertSurface(font, my_format, 0);
  SDL_FreeFormat(my_format);

  temp = SDL_CreateRGBSurface(SDL_SWSURFACE, POTfontwidth, POTfontheight, 32,
                              bmask, gmask, rmask, amask); // BGRA

  SDL_BlitSurface(temp_alpha, NULL, temp, NULL);
  SDL_FreeSurface(temp_alpha);

  CHECKGL(glGenTextures(1, &font_tex));
  CHECKGL(glBindTexture(GL_TEXTURE_2D, font_tex));
  SDL_LockSurface(temp);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  CHECKGL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, temp->w, temp->h, 0, GL_BGRA,
                       GL_UNSIGNED_BYTE, temp->pixels));
  SDL_UnlockSurface(temp);
  SDL_FreeSurface(temp);
  return true;
}
/**
 *  Return a `type` shader from the `txt` source.
 *
 *  Returns 0 non error.
 */
static GLuint loadShader(const char* txt, GLuint type)
{
  GLuint v = glCreateShader(type);
  glShaderSource(v, 1, &txt, 0);
  glCompileShader(v);
  int success;
  glGetShaderiv(v, GL_COMPILE_STATUS, &success);
  if(success != GL_TRUE) {
    // something went wrong
    int infologLength = 0;
    glGetShaderiv(v, GL_INFO_LOG_LENGTH, &infologLength);
    if(infologLength > 0) {
      int charsWritten = 0;
      char* infoLog = static_cast<char*>(malloc(infologLength));
      glGetProgramInfoLog(v, infologLength, &charsWritten, infoLog);
      printf("GLSL ERROR : %s\n", infoLog);
      free(infoLog);
    }
    return 0;
  }
  return v;
}
/**
 *  Compile a program from sources and output the shaders and program, returns
 *  true if successful.
 *
 *  `vertShaderCode` and `fragShaderCode` point to the source code for the
 *  shaders.
 *
 *  Shaders will be output to `vertShader` and `fragShader`.
 *
 *  Program will be output to `prog`.
 */
static bool loadProgram(const char* vertShaderCode, const char* fragShaderCode,
      GLuint* vertShader, GLuint* fragShader, GLuint* prog)
{
  // Create and load Program and Shaders
  *prog = DBGCHECKGL(glCreateProgram());

  *vertShader = loadShader(vertShaderCode, GL_VERTEX_SHADER);
  if (*vertShader == 0) { return false; }
  glAttachShader(*prog, *vertShader);

  *fragShader = loadShader(fragShaderCode, GL_FRAGMENT_SHADER);
  if (*fragShader == 0) { return false; }
  glAttachShader(*prog, *fragShader);

  glLinkProgram(*prog);

  int success;
  glGetProgramiv(*prog, GL_LINK_STATUS, &success);
  if(success != GL_TRUE) {
    /* something went wrong */
    int infologLength = 0;
    int charsWritten = 0;
    glGetProgramiv(*prog, GL_INFO_LOG_LENGTH, &infologLength);
    if (infologLength > 0) {
      char* infoLog = static_cast<char*>(malloc(infologLength));
      glGetProgramInfoLog(*prog, infologLength, &charsWritten, infoLog);
      printf("OPENGL ERROR: Program link Error");
      printf("%s\n", infoLog);
      free(infoLog);
    }
    return false;
  }
  return true;
}
/**
 *  Load built-in shaders, returns true if successful.
 */
bool TCOD_opengl_init_shaders(void)
{
  if (TCOD_ctx.renderer == TCOD_RENDERER_GLSL) {
    if (!loadProgram(TCOD_con_vertex_shader, TCOD_con_pixel_shader,
                     &conVertShader, &conFragShader, &conProgram)) {
      return false;
    }
  }
  // Host side data init
  for(int i = 0; i < ConsoleDataEnumSize; ++i)
  {
    data[i] = static_cast<unsigned char*>(
        calloc(conwidth * conheight, ConsoleDataAlignment[i])
    );
    dirty[i] = true;
  }
  // Initialize ForeCol to 255, 255, 255, 255
  TCOD_color_t* fCol = reinterpret_cast<TCOD_color_t*>(data[ForeCol]);
  for(int i = 0; i < conwidth * conheight; ++i)
  {
      fCol[i].r=255;
      fCol[i].g=255;
      fCol[i].b=255;
  }
  // Generate Textures
  glGenTextures(3, Tex);

  // Character Texture
  CHECKGL(glBindTexture(GL_TEXTURE_2D, Tex[Character]));

  CHECKGL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
  CHECKGL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

  CHECKGL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, POTconwidth, POTconheight, 0,
                       GL_LUMINANCE, GL_UNSIGNED_BYTE, 0));

  // ForeCol Texture
  CHECKGL(glBindTexture(GL_TEXTURE_2D, Tex[ForeCol]));

  CHECKGL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
  CHECKGL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

  CHECKGL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, POTconwidth, POTconheight, 0,
                       GL_RGB, GL_UNSIGNED_BYTE, 0));

  // BackCol Texture
  CHECKGL(glBindTexture(GL_TEXTURE_2D, Tex[BackCol]));

  CHECKGL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
  CHECKGL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

  CHECKGL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, POTconwidth, POTconheight, 0,
                       GL_RGB, GL_UNSIGNED_BYTE, 0));

  CHECKGL(glBindTexture(GL_TEXTURE_2D, 0));

  return true;
}

static bool updateTex(ConsoleDataEnum dataType)
{
  GLenum Type = 0;
  DBGCHECKGL(glBindTexture(GL_TEXTURE_2D, Tex[dataType]));

  switch(ConsoleDataAlignment[dataType]) {
    case 1:
      Type = GL_RED;
      break;
  /*case 2:
    Type = GL_RG;
    break; */
    case 3:
      Type = GL_RGB;
      break;
    case 4:
      Type = GL_RGBA;
      break;
  }
  /*glPixelStorei(GL_UNPACK_ALIGNMENT, 1); */
  DBGCHECKGL(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, conwidth, conheight,
                             Type, GL_UNSIGNED_BYTE, data[dataType]));

  DBGCHECKGL(glBindTexture(GL_TEXTURE_2D, 0));
  return true;
}

static void updateChar(ConsoleDataEnum dataType, int BufferPos,
                       unsigned char *c, int length, int offset)
{
  dirty[dataType] = true; // Set dirty so Texture gets updated next frame
  for(int i = 0; i < length; ++i) {
    data[dataType][BufferPos * ConsoleDataAlignment[dataType]
                   + i + offset] = c[i];
  }
}

void TCOD_opengl_putchar_ex(int x, int y, int c,
                            TCOD_color_t fore, TCOD_color_t back)
{
  int loc = x + y * conwidth;
  if (TCOD_ctx.renderer == TCOD_RENDERER_GLSL) {
    updateChar(Character, loc, reinterpret_cast<unsigned char*>(&c),
               ConsoleDataAlignment[Character], 0);
    updateChar(ForeCol, loc, &fore.r, ConsoleDataAlignment[ForeCol], 0);
  }
  updateChar(BackCol, loc, &back.r, ConsoleDataAlignment[BackCol], 0);
}

bool TCOD_opengl_render(
    int oldFade,
    bool*, // ascii_updated
    TCOD_Console* console,
    TCOD_Console* cache)
{
  int fade = TCOD_console_get_fade();
  bool track_changes = (cache && oldFade == fade);
  // update OpenGL data
  for (int y = 0; y < conheight; ++y) {
    for (int x = 0; x < conwidth; ++x) {
      int i = y * conwidth + x;
      if (track_changes && console->tiles[i] == cache->tiles[i]) {
        continue;
      }
      TCOD_opengl_putchar_ex(
          x, y,
          TCOD_get_tileid_for_charcode_(console->tiles[i].ch),
          tcod::ColorRGB(console->tiles[i].fg),
          tcod::ColorRGB(console->tiles[i].bg)
      );
    }
  }
  /* check if any of the textures have changed since they were last uploaded */
  for(int i = 0; i < ConsoleDataEnumSize; ++i)
  {
    if (dirty[i]) {
      updateTex(static_cast<ConsoleDataEnum>(i));
      dirty[i] = false;
    }
  }
  if (TCOD_ctx.renderer == TCOD_RENDERER_OPENGL) {
    /* fixed pipeline for video cards without pixel shader support */
    /* draw the background as a single quad */
    float texw = static_cast<float>(conwidth) / POTconwidth;
    float texh = static_cast<float>(conheight) / POTconheight;
    float fonw = static_cast<float>(fontwidth) / (TCOD_ctx.fontNbCharHoriz
                                                  * POTfontwidth);
    float fonh = static_cast<float>(fontheight) / (TCOD_ctx.fontNbCharVertic
                                                   * POTfontheight);
    DBGCHECKGL(glBindTexture(GL_TEXTURE_2D, Tex[BackCol]));
    DBGCHECKGL(
        glBegin(GL_QUADS);
        glColor3f(1.0, 1.0, 1.0);
        glTexCoord2f(0.0, 0.0);
        glVertex2i(0, 0);
        glTexCoord2f(0.0, texh);
        glVertex2i(0, conheight);
        glTexCoord2f(texw, texh);
        glVertex2i(conwidth, conheight);
        glTexCoord2f(texw, 0.0);
        glVertex2i(conwidth, 0);
        glEnd()
    );
    // draw the characters (one quad per cell)
    DBGCHECKGL(glBindTexture(GL_TEXTURE_2D, font_tex));

    for (int y = 0; y < conheight; ++y) {
      for (int x = 0; x < conwidth; ++x) {
        int i = y * conwidth + x;
        if (console->tiles[i].ch == ' ') { continue; }
        const TCOD_ColorRGB f = tcod::ColorRGB(console->tiles[i].fg);
        const TCOD_ColorRGB b = tcod::ColorRGB(console->tiles[i].bg);
        // only draw character if foreground color != background color
        if (f == b) { continue; }
        int destx = x;/* *TCOD_font_width; */
        int desty = y;/* *TCOD_font_height; */
        if (TCOD_ctx.fullscreen) {
          destx += TCOD_ctx.fullscreen_offsetx / TCOD_ctx.font_width;
          desty += TCOD_ctx.fullscreen_offsety / TCOD_ctx.font_height;
        }
        // draw foreground
        int ascii = TCOD_ctx.ascii_to_tcod[console->tiles[i].ch];
        int srcx = ascii % TCOD_ctx.fontNbCharHoriz;
        int srcy = ascii / TCOD_ctx.fontNbCharHoriz;
        glBegin(GL_QUADS);
        glColor3f(f.r / 255.0f, f.g / 255.0f, f.b / 255.0f);
        glTexCoord2f(srcx * fonw, srcy * fonh);
        glVertex2i(destx, desty);
        glTexCoord2f(srcx * fonw, (srcy + 1) * fonh);
        glVertex2i(destx, desty + 1);
        glTexCoord2f((srcx + 1) * fonw, (srcy + 1) * fonh);
        glVertex2i(destx + 1, desty + 1);
        glTexCoord2f((srcx + 1) * fonw, srcy * fonh);
        glVertex2i(destx+1, desty);
        glEnd();
      }
    }
    DBGCHECKGL(glBindTexture(GL_TEXTURE_2D, 0));
  } else {
    // actual rendering
    DBGCHECKGL(glUseProgram(conProgram));
    // Technically all these glUniform calls can be moved to SFConsole() when
    // the shader is loaded.
    // None of these change.
    // The Textures still need to bind to the same # Activetexture throughout
    // though.
    DBGCHECKGL(
        glUniform2f(
            glGetUniformLocation(conProgram,"termsize"),
            conwidth,
            conheight
        )
    );
    DBGCHECKGL(
        glUniform2f(
            glGetUniformLocation(conProgram,"termcoef"),
            1.0f / POTconwidth,
            1.0f / POTconheight
        )
    );
    DBGCHECKGL(
        glUniform1f(
            glGetUniformLocation(conProgram,"fontw"),
            static_cast<float>(TCOD_ctx.fontNbCharHoriz)
        )
    );
    DBGCHECKGL(
        glUniform2f(
            glGetUniformLocation(conProgram,"fontcoef"),
            static_cast<float>(fontwidth) / (POTfontwidth
                                             * TCOD_ctx.fontNbCharHoriz),
            static_cast<float>(fontheight) / (POTfontheight
                                              * TCOD_ctx.fontNbCharVertic)
        )
    );

    DBGCHECKGL(glActiveTexture(GL_TEXTURE0));
    DBGCHECKGL(glBindTexture(GL_TEXTURE_2D, font_tex));
    DBGCHECKGL(glUniform1i(glGetUniformLocation(conProgram,"font"), 0));

    DBGCHECKGL(glActiveTexture(GL_TEXTURE1));
    DBGCHECKGL(glBindTexture(GL_TEXTURE_2D, Tex[Character]));
    DBGCHECKGL(glUniform1i(glGetUniformLocation(conProgram, "term"), 1));

    DBGCHECKGL(glActiveTexture(GL_TEXTURE2));
    DBGCHECKGL(glBindTexture(GL_TEXTURE_2D, Tex[ForeCol]));
    DBGCHECKGL(glUniform1i(glGetUniformLocation(conProgram, "termfcol"), 2));

    DBGCHECKGL(glActiveTexture(GL_TEXTURE3));
    DBGCHECKGL(glBindTexture(GL_TEXTURE_2D, Tex[BackCol]));
    DBGCHECKGL(glUniform1i(glGetUniformLocation(conProgram, "termbcol"), 3));

    /*DBGCHECKGL(shader->Validate()); */

    DBGCHECKGL(
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-1.0f, -1.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(1.0f, -1.0f, 0.0f);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(1.0f, 1.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-1.0f, 1.0f, 0.0f);
        glEnd()
    );
    DBGCHECKGL(glBindTexture(GL_TEXTURE_2D, 0));
    DBGCHECKGL(glUseProgram(0));
  }
  /* fading overlay */
  if (fade != 255) {
    int x = 0;
    int y = 0;
    if (TCOD_ctx.fullscreen) {
      x = TCOD_ctx.fullscreen_offsetx / TCOD_ctx.font_width;
      y = TCOD_ctx.fullscreen_offsety / TCOD_ctx.font_height;
    }
    glBegin(GL_QUADS);
    glColor4f(
        TCOD_ctx.fading_color.r / 255.0f,
        TCOD_ctx.fading_color.g / 255.0f,
        TCOD_ctx.fading_color.b / 255.0f,
        1.0f - fade / 255.0f
    );
    glVertex2i(x, y);
    glVertex2i(x, y + conheight);
    glVertex2i(x + conwidth, y + conheight);
    glVertex2i(x + conwidth, y);
    glEnd();
  }
  return true;
}
void TCOD_opengl_swap(void)
{
  SDL_GL_SwapWindow(window);
}
/**
 *  Return an SDL_Surface screenshot of the OpenGL surface.
 */
SDL_Surface* TCOD_opengl_get_screen(void)
{
  int offx = 0;
  int offy = 0;
  /* allocate a pixel buffer */
  int pixw = TCOD_ctx.root->w * TCOD_ctx.font_width;
  int pixh = TCOD_ctx.root->h * TCOD_ctx.font_height;
  SDL_Surface* surf = TCOD_sys_get_surface(pixw, pixh, false);
  if (TCOD_ctx.fullscreen) {
    offx = TCOD_ctx.fullscreen_offsetx;
    offy = TCOD_ctx.fullscreen_offsety;
  }
  /* get pixel data from opengl */
  glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);
  glPixelStorei(GL_PACK_ROW_LENGTH, 0);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glReadPixels(offx, offy, pixw, pixh, GL_RGB, GL_UNSIGNED_BYTE, surf->pixels);
  glPopClientAttrib();
  /* vertical flip (opengl has lower-left origin, SDL upper left) */
  uint32_t mask = (surf->format->Rmask | surf->format->Gmask
                   | surf->format->Bmask);
  uint32_t nmask = ~mask;
  for (int x = 0; x < surf->w; ++x) {
    for (int y = 0; y < surf->h / 2; ++y) {
      int offsrc = x * 3 + y * surf->pitch;
      int offdst = x * 3 + (surf->h - 1 - y) * surf->pitch;
      uint32_t *pixsrc = reinterpret_cast<uint32_t*>(
          (static_cast<uint8_t*>(surf->pixels)) + offsrc
      );
      uint32_t *pixdst = reinterpret_cast<uint32_t*>(
          (static_cast<uint8_t*>(surf->pixels)) + offdst
      );
      uint32_t tmp = *pixsrc;
      *pixsrc = ((*pixsrc) & nmask) | ((*pixdst) & mask);
      *pixdst = ((*pixdst) & nmask) | (tmp & mask);
    }
  }
  return surf;
}
#endif // TCOD_BARE
