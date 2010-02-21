/*
* libtcod 1.5.1
* Copyright (c) 2008,2009,2010 Jice & Mingos
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

/*
 * This renderer is mostly copied and pasted from Antagonist's SkyFire GLSL roguelike engine
 */ 

#include "libtcod.h"
#include "libtcod_int.h"
#ifndef NO_OPENGL
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

#define CHECKGL( GLcall )                               		\
    GLcall;                                             		\
    if(!_CheckGL_Error( #GLcall, __FILE__, __LINE__))     		\
    return false;

#ifdef NDEBUG
#define DBGCHECKGL(GLcall) GLcall
#else
#define DBGCHECKGL CHECKGL
#endif

typedef  enum 
{
	Character,
	ForeCol,
	BackCol,
	ConsoleDataEnumSize
} ConsoleDataEnum;
const int ConsoleDataAlignment[3] = {1, 3, 3 };

static const char *TCOD_con_vertex_shader =
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
"uniform sampler2D font; "
"uniform sampler2D term; "
"uniform sampler2D termfcol; "
"uniform sampler2D termbcol; "

"uniform vec2 fontsize; "
"uniform vec2 termsize; "
"uniform vec2 POTtermsize; "

"void main(void) "
"{ "
"   vec2 termscale = vec2(termsize.x/POTtermsize.x, termsize.y/POTtermsize.y); "
"   vec2 rawCoord = gl_TexCoord[0].xy; "                           // varying from [0, termsize) in x and y
"   vec2 conPos = floor(rawCoord); "                               // console integer position
"   vec2 pixPos = fract(rawCoord); "                               // pixel offset within console position
"   pixPos = vec2(pixPos.x/fontsize.x,pixPos.y/fontsize.y); "      // Correct pixel offset for font tex location

"   vec2 address = vec2((conPos.x/termsize.x)*termscale.x,(conPos.y/termsize.y)*termscale.y); "
"	address=address+vec2(0.001, 0.001); "
"   float inchar = texture2D(term, address).r*256.0; "         // character
"   vec4 tcharfcol = texture2D(termfcol, address); "           // front color
"   vec4 tcharbcol = texture2D(termbcol, address); "           // back color

"   vec4 tchar = vec4(floor(inchar),floor(inchar/fontsize.x), 0.0, 0.0); "  // 1D index to 2D index map for character

"   gl_FragColor = texture2D(font, vec2((tchar.x/fontsize.x),(tchar.y/fontsize.y))+pixPos.xy); "   // magic func: finds pixel value in font file

"   gl_FragColor=gl_FragColor.a*tcharfcol+(1.0-gl_FragColor.a)*tcharbcol; "      // Coloring stage
"} "
;

inline bool _CheckGL_Error(const char* GLcall, const char* file, const int line)
{
    GLenum errCode;
    if((errCode = glGetError())!=GL_NO_ERROR)
    {
		printf("OPENGL ERROR #%i: in file %s on line %i\n",errCode,file, line);
        printf("OPENGL Call: %s\n",GLcall);
        return false;
    }
    return true;
}

// called before creating window
void TCOD_opengl_init_attributes() {
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,8);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32 );
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
}

// console size (power of 2 and cells)
static int POTconwidth, POTconheight, conwidth, conheight;
// programs and shaders handles
static GLuint conProgram, conVertShader, conFragShader;
// font texture handle
static GLuint font_tex;
// console data
static GLuint Tex[ConsoleDataEnumSize];
static unsigned char *data[ConsoleDataEnumSize];
static bool dirty[ConsoleDataEnumSize];

// extension function pointers
static PFNGLCREATESHADEROBJECTARBPROC glCreateShaderObjectARB=0;
static PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameterivARB=0;
static PFNGLSHADERSOURCEARBPROC glShaderSourceARB=0;
static PFNGLCOMPILESHADERARBPROC glCompileShaderARB=0;
static PFNGLGETINFOLOGARBPROC glGetInfoLogARB=0;
static PFNGLCREATEPROGRAMOBJECTARBPROC glCreateProgramObjectARB=0;
static PFNGLATTACHOBJECTARBPROC glAttachObjectARB=0;
static PFNGLLINKPROGRAMARBPROC glLinkProgramARB=0;
static PFNGLUSEPROGRAMOBJECTARBPROC glUseProgramObjectARB=0;
static PFNGLUNIFORM2FARBPROC glUniform2fARB=0;
static PFNGLGETUNIFORMLOCATIONARBPROC glGetUniformLocationARB=0;
static PFNGLACTIVETEXTUREPROC glActiveTexture=0;
static PFNGLUNIFORM1IARBPROC glUniform1iARB=0;
                                        
// call after creating window
bool TCOD_opengl_init_state(int conw, int conh, void *font) {
	const char *glexts=(const char *)glGetString(GL_EXTENSIONS);
	SDL_Surface *font_surf=(SDL_Surface *)font;

	// check opengl extensions
	if (!glexts || !strstr(glexts,"GL_ARB_shader_objects")) return false;

	// set extensions functions pointers
   	glCreateShaderObjectARB=(PFNGLCREATESHADEROBJECTARBPROC)wglGetProcAddress("glCreateShaderObjectARB");
	glGetObjectParameterivARB=(PFNGLGETOBJECTPARAMETERIVARBPROC)wglGetProcAddress("glGetObjectParameterivARB");
	glShaderSourceARB=(PFNGLSHADERSOURCEARBPROC)wglGetProcAddress("glShaderSourceARB");
	glCompileShaderARB=(PFNGLCOMPILESHADERARBPROC)wglGetProcAddress("glCompileShaderARB");
	glGetInfoLogARB=(PFNGLGETINFOLOGARBPROC)wglGetProcAddress("glGetInfoLogARB");
	glCreateProgramObjectARB=(PFNGLCREATEPROGRAMOBJECTARBPROC)wglGetProcAddress("glCreateProgramObjectARB");
	glAttachObjectARB=(PFNGLATTACHOBJECTARBPROC)wglGetProcAddress("glAttachObjectARB");
	glLinkProgramARB=(PFNGLLINKPROGRAMARBPROC)wglGetProcAddress("glLinkProgramARB");
	glUseProgramObjectARB=(PFNGLUSEPROGRAMOBJECTARBPROC)wglGetProcAddress("glUseProgramObjectARB");
	glUniform2fARB=(PFNGLUNIFORM2FARBPROC)wglGetProcAddress("glUniform2fARB");
	glGetUniformLocationARB=(PFNGLGETUNIFORMLOCATIONARBPROC)wglGetProcAddress("glGetUniformLocationARB");
	glActiveTexture=(PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");
	glUniform1iARB=(PFNGLUNIFORM1IARBPROC)wglGetProcAddress("glUniform1iARB");

	// set opengl state
	glEnable(GL_TEXTURE_2D);
	glClearColor(1.0f, 1.0f, 0.0f, 0.0f);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glClear( GL_COLOR_BUFFER_BIT );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho(0, conw, 0, conh, -1.0f, 1.0f);
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
#ifdef TCOD_WINDOWS
	if ( ! ((TCOD_console_data_t *)TCOD_root)->fullscreen ) {
		// turn vsync off in windowed mode
		typedef bool (APIENTRY *PFNWGLSWAPINTERVALFARPROC)(int);
		PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = 0;

		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress("wglSwapIntervalEXT");

		if (wglSwapIntervalEXT) wglSwapIntervalEXT(0);
	}
#endif

	// compute pot size
	conwidth=conw;
	conheight=conh;
	POTconwidth=POTconheight=1;
	while ( POTconwidth < conw ) POTconwidth *= 2;
	while ( POTconheight < conh ) POTconheight *= 2;

	// convert font for opengl
	Uint32 rmask, gmask, bmask, amask;
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

	SDL_SetColorKey(font_surf, SDL_SRCCOLORKEY, SDL_MapRGB(font_surf->format, 0, 0, 0));
	temp_alpha = SDL_DisplayFormatAlpha(font_surf);
	SDL_SetAlpha(temp_alpha, 0, SDL_ALPHA_TRANSPARENT);

	temp = SDL_CreateRGBSurface(SDL_SWSURFACE, font_surf->w, font_surf->h, 32, bmask, gmask, rmask, amask); //BGRA

	SDL_BlitSurface(temp_alpha, NULL, temp, NULL);
	SDL_FreeSurface(temp_alpha);

	CHECKGL(glGenTextures(1, &font_tex));
	CHECKGL(glBindTexture(GL_TEXTURE_2D, font_tex));
	SDL_LockSurface(temp);

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

	CHECKGL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, temp->w, temp->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, temp->pixels));
	SDL_UnlockSurface(temp);
	SDL_FreeSurface(temp);
	return true;
}

static GLuint loadShader(const char *txt, GLuint type) {
#ifndef GL_ARB_shader_objects
	return 0;
#else
	int success;
	int infologLength = 0;
	int charsWritten = 0;
    char *infoLog;
	GLuint v = glCreateShaderObjectARB(type);
	glShaderSourceARB(v, 1, &txt, 0);
	glCompileShaderARB(v);

	glGetObjectParameterivARB(v, GL_COMPILE_STATUS, &success);
	if(success!=GL_TRUE)
	{
	    // something went wrong
		glGetObjectParameterivARB(v, GL_INFO_LOG_LENGTH,&infologLength);
		if(infologLength>0)
		{
			infoLog = (char *)malloc(infologLength);
			glGetInfoLogARB(v, infologLength, &charsWritten, infoLog);
			printf("GLSL ERROR : %s\n",infoLog);
			free(infoLog);
		}
		return 0;
	}

	return v;
#endif
}

static bool loadProgram(const char *vertShaderCode, const char *fragShaderCode,
	GLuint *vertShader, GLuint *fragShader, GLuint *prog) {
#ifndef GL_ARB_shader_objects
	return false;
#else
	// Create and load Program and Shaders
	int success;
	*prog = DBGCHECKGL(glCreateProgramObjectARB());

	*vertShader = loadShader(vertShaderCode, GL_VERTEX_SHADER);
	if ( *vertShader == 0 ) return false;
	glAttachObjectARB(*prog, *vertShader);

	*fragShader = loadShader(fragShaderCode, GL_FRAGMENT_SHADER);
	if ( *fragShader == 0 ) return false;
	glAttachObjectARB(*prog, *fragShader);

	glLinkProgramARB(*prog);

	glGetObjectParameterivARB(*prog, GL_LINK_STATUS, &success);
	if(success!=GL_TRUE)
	{
		// something went wrong
		int infologLength = 0;
		int charsWritten = 0;
		char *infoLog;
		glGetObjectParameterivARB(*prog, GL_INFO_LOG_LENGTH,&infologLength);
		if (infologLength > 0)
	    {
	        infoLog = (char *)malloc(infologLength);
	        glGetInfoLogARB(*prog, infologLength, &charsWritten, infoLog);
			printf("OPENGL ERROR: Program link Error");
			printf("%s\n",infoLog);
	        free(infoLog);
	    }
		return false;
	}
	return true;
#endif
}

bool TCOD_opengl_init_shaders() {
#ifndef GL_ARB_shader_objects
	return false;
#else
	int i;
	if (! loadProgram(TCOD_con_vertex_shader, TCOD_con_pixel_shader, &conVertShader, &conFragShader, &conProgram ) ) return false;
	// Host side data init
	for(i = 0; i< ConsoleDataEnumSize; i++)
	{
		data[i] = (unsigned char *)calloc(conwidth*conheight,ConsoleDataAlignment[i]);
		dirty[i]=true;
	}
	// Initialize ForeCol to 255, 255, 255, 255
	TCOD_color_t *fCol = (TCOD_color_t *)data[ForeCol];
	for( i = 0; i < conwidth*conheight; i++)
	{
	    fCol[i].r=255;
	    fCol[i].g=255;
	    fCol[i].b=255;
	}

    // Generate Textures
	glGenTextures(3, Tex);

	// Character Texture
	CHECKGL(glBindTexture(GL_TEXTURE_2D, Tex[Character]));

    CHECKGL(glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST ));
    CHECKGL(glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST ));

    CHECKGL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, POTconwidth, POTconheight, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, 0));


    // ForeCol Texture
	CHECKGL(glBindTexture(GL_TEXTURE_2D, Tex[ForeCol]));

    CHECKGL(glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST ));
    CHECKGL(glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST ));

	CHECKGL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, POTconwidth, POTconheight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0));

    // BackCol Texture
	CHECKGL(glBindTexture(GL_TEXTURE_2D, Tex[BackCol]));

    CHECKGL(glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST ));
    CHECKGL(glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST ));

	CHECKGL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, POTconwidth, POTconheight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0));

	CHECKGL(glBindTexture(GL_TEXTURE_2D, 0));

	return true;
#endif
}

static bool updateTex(ConsoleDataEnum dataType) {
	DBGCHECKGL(glBindTexture(GL_TEXTURE_2D, Tex[dataType]));

	GLenum Type=0;
	switch(ConsoleDataAlignment[dataType])
	{
	case 1:
		Type = GL_RED;
		break;
	//case 2:
	//	Type = GL_RG;
	//	break;
	case 3:
		Type = GL_RGB;
		break;
	case 4:
		Type = GL_RGBA;
	}
    //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	DBGCHECKGL(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, conwidth, conheight, Type, GL_UNSIGNED_BYTE, data[dataType]));

	DBGCHECKGL(glBindTexture(GL_TEXTURE_2D,0));
	return true;
}

static void updateChar(ConsoleDataEnum dataType, int BufferPos, unsigned char *c, int length, int offset) {
	int i;
	dirty[dataType] = true;		// Set dirty so Texture gets updated next frame

	for(i = 0; i<length; i++) {
		data[dataType][BufferPos*ConsoleDataAlignment[dataType] + i + offset] = c[i];
	}

}

void TCOD_opengl_putchar_ex(int x, int y, unsigned char c, TCOD_color_t fore, TCOD_color_t back) {
	int loc = x+y*conwidth;

	updateChar(Character, loc, &c, ConsoleDataAlignment[Character], 0);
	updateChar(ForeCol, loc, &fore.r, ConsoleDataAlignment[ForeCol], 0);
	updateChar(BackCol, loc, &back.r, ConsoleDataAlignment[BackCol], 0);

}

bool TCOD_opengl_render( int oldFade, bool *ascii_updated, char_t *console_buffer, char_t *prev_console_buffer) {
#ifdef GL_ARB_shader_objects
	int x,y,i;
	int fade = (int)TCOD_console_get_fade();
	bool track_changes=(oldFade == fade && prev_console_buffer);
	char_t *c=&console_buffer[0];
	char_t *oc=&prev_console_buffer[0];
	// update opengl data
	// TODO use function pointers so that libtcod's putchar directly updates opengl data
	for (y=0;y<conheight;y++) {
		for (x=0; x<conwidth; x++) {
			bool changed=true;
			if ( c->cf == -1 ) c->cf = ascii_to_tcod[c->c];
			if ( track_changes ) {
				changed=false;
				if ( c->dirt || ascii_updated[ c->c ] || c->back.r != oc->back.r || c->back.g != oc->back.g
					|| c->back.b != oc->back.b || c->fore.r != oc->fore.r
					|| c->fore.g != oc->fore.g || c->fore.b != oc->fore.b
					|| c->c != oc->c || c->cf != oc->cf) {
					changed=true;
				}
			}
			c->dirt=0;
			if ( changed ) {
				TCOD_opengl_putchar_ex(x,y,c->cf,c->fore,c->back);
			}
			c++;oc++;
		}
	}

	// check if any of the textures have changed since they were last uploaded
	for( i = 0; i< ConsoleDataEnumSize; i++)
	{
		if(dirty[i])
		{
			updateTex((ConsoleDataEnum)i);
			dirty[i] = false;
		}
	}

	// actual rendering
    DBGCHECKGL(glUseProgramObjectARB(conProgram));

	// Technically all these glUniform calls can be moved to SFConsole() when the shader is loaded
	// None of these change
	// The Textures still need to bind to the same # Activetexture throughout though
    DBGCHECKGL(glUniform2fARB(glGetUniformLocationARB(conProgram,"termsize"), (float) conwidth, (float) conheight));
	DBGCHECKGL(glUniform2fARB(glGetUniformLocationARB(conProgram,"POTtermsize"), (float) POTconwidth, (float) POTconheight));
    DBGCHECKGL(glUniform2fARB(glGetUniformLocationARB(conProgram,"fontsize"), fontNbCharHoriz, fontNbCharVertic));

    DBGCHECKGL(glActiveTexture(GL_TEXTURE0));
    DBGCHECKGL(glBindTexture(GL_TEXTURE_2D, font_tex));
    DBGCHECKGL(glUniform1iARB(glGetUniformLocationARB(conProgram,"font"),0));

    DBGCHECKGL(glActiveTexture(GL_TEXTURE1));
    DBGCHECKGL(glBindTexture(GL_TEXTURE_2D, Tex[Character]));
    DBGCHECKGL(glUniform1iARB(glGetUniformLocationARB(conProgram,"term"),1));

    DBGCHECKGL(glActiveTexture(GL_TEXTURE2));
    DBGCHECKGL(glBindTexture(GL_TEXTURE_2D, Tex[ForeCol]));
    DBGCHECKGL(glUniform1iARB(glGetUniformLocationARB(conProgram,"termfcol"),2));

    DBGCHECKGL(glActiveTexture(GL_TEXTURE3));
    DBGCHECKGL(glBindTexture(GL_TEXTURE_2D, Tex[BackCol]));
    DBGCHECKGL(glUniform1iARB(glGetUniformLocationARB(conProgram,"termbcol"),3));

//    DBGCHECKGL(shader->Validate());

	DBGCHECKGL(glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-1.0f,-1.0f,0.0f);
        glTexCoord2f(1.0f, 1.0f);
		glVertex3f(1.0f,-1.0f,0.0f);
        glTexCoord2f(1.0f, 0.0f);
		glVertex3f(1.0f,1.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-1.0f,1.0f,0.0f);
	glEnd());

    DBGCHECKGL(glBindTexture(GL_TEXTURE_2D, 0));

    DBGCHECKGL(glUseProgramObjectARB(0));
#endif
	return true;
}

void TCOD_opengl_swap() {
	SDL_GL_SwapBuffers();
}


#endif

