%module libtcod
#pragma SWIG nowarn=503
%{
#include "libtcod.hpp"
#include "../swig/BackgroundHelperFunctions.hpp"

#include <vector>
class TCODNameGenerator
{
	public:
		static void parse (const char * filename, TCODRandom * random = NULL)
		{
			TCOD_namegen_parse (filename, random ? random->data : NULL);
		}

		static char * generate (char * name, bool allocate = false)
		{
			return TCOD_namegen_generate (name, allocate);
		}

		static char * generateCustom (char * name, char * rule, bool allocate = false)
		{
			return TCOD_namegen_generate_custom (name, rule, allocate);
		}

		static std::vector<char*> getSets(void)
		{
			std::vector<char*> returnList;
			TCOD_list_t setList = TCOD_namegen_get_sets();
			for(int i = 0 ; i < TCOD_list_size(setList) ; ++i)
			{
			 	returnList.push_back((char*)TCOD_list_get(setList, i));
			}
			return returnList;
		}
		
		static void destroy (void)
		{
			TCOD_namegen_destroy ();
		}
};

TCOD_bkgnd_flag_t TCODBackgroundAlphaMask(TCOD_bkgnd_flag_t background)
{
	return (TCOD_bkgnd_flag_t)(background & 0xff);
}
%}

class TCODLIB_API TCODNameGenerator {
	public:
		static void parse (const char * filename, TCODRandom * random = NULL);
		static char * generate (char * name, bool allocate = false);
		static char * generateCustom (char * name, char * rule, bool allocate = false);
		static std::vector<char*> getSets (void);
		static void destroy (void);
};

%include std_vector.i
namespace std {
%template(StringVector) vector<char*>;
};

#define NO_UNICODE
typedef unsigned char uint8;
typedef unsigned int uint32;

#if SWIGCSHARP
%typemap(cstype) void * "System.Runtime.InteropServices.HandleRef" 
%typemap(csin) void * "$csinput" 
#endif // SWIGCSHARP

%rename(TCODKey) TCOD_key_t;
%rename(TCODKeyCode) TCOD_keycode_t;
%rename(TCODBackgroundFlag) TCOD_bkgnd_flag_t;
%rename(TCODRendererType) TCOD_renderer_t;

%rename(TCODSpecialCharacter) TCOD_chars_t;
%rename(TCODFontFlags) TCOD_font_flags_t;
%rename(TCODKeyStatus) TCOD_key_status_t;

%rename(MersenneTwister) TCOD_RNG_MT;
%rename(ComplementaryMultiplyWithCarry) TCOD_RNG_CMWC;

%rename(BasicFov) FOV_BASIC;
%rename(DiamondFov) FOV_DIAMOND;
%rename(ShadowFov) FOV_SHADOW;
%rename(Permissive0Fov) FOV_PERMISSIVE_0;
%rename(Permissive1Fov) FOV_PERMISSIVE_1;
%rename(Permissive2Fov) FOV_PERMISSIVE_2;
%rename(Permissive3Fov) FOV_PERMISSIVE_3;
%rename(Permissive4Fov) FOV_PERMISSIVE_4;
%rename(Permissive5Fov) FOV_PERMISSIVE_5;
%rename(Permissive6Fov) FOV_PERMISSIVE_6;
%rename(Permissive7Fov) FOV_PERMISSIVE_7;
%rename(Permissive8Fov) FOV_PERMISSIVE_8;
%rename(RestrictiveFov) FOV_RESTRICTIVE;

%rename(TCODAlignment) TCOD_alignment_t;
%rename(LeftAlignment) TCOD_LEFT;
%rename(RightAlignment) TCOD_RIGHT;
%rename(CenterAlignment) TCOD_CENTER;

%ignore NB_FOV_ALGORITHMS;

%include typemaps.i
%rename(Equal) operator ==;
%rename(NotEqual) operator !=;
%rename(Multiply) operator *;
%rename(Plus) operator +;
%rename(Minus) operator -;

// console.hpp
%varargs (1, char * = NULL) TCODConsole::printFrame;
%ignore TCOD_colctrl_t;
%ignore TCODConsole::setColorControl(TCOD_colctrl_t con, const TCODColor &fore, const TCODColor &back);
%ignore TCODConsole::TCODConsole(TCOD_console_t n);
%rename (setCharBackground) setBack;
%rename (setCharForeground) setFore;
%rename (getCharBackground) getBack;
%rename (getCharForeground) getFore;
%ignore TCODConsole::printLeft(int x, int y, TCOD_bkgnd_flag_t flag, const char *fmt, ...); 
%ignore TCODConsole::printRight(int x, int y, TCOD_bkgnd_flag_t flag, const char *fmt, ...); 
%ignore TCODConsole::printCenter(int x, int y, TCOD_bkgnd_flag_t flag, const char *fmt, ...); 
%ignore TCODConsole::printLeftRect(int x, int y, int w, int h, TCOD_bkgnd_flag_t flag, const char *fmt, ...); 
%ignore TCODConsole::printRightRect(int x, int y, int w, int h, TCOD_bkgnd_flag_t flag, const char *fmt, ...); 
%ignore TCODConsole::printCenterRect(int x, int y, int w, int h, TCOD_bkgnd_flag_t flag, const char *fmt, ...);
%ignore TCODConsole::getHeightLeftRect(int x, int y, int w, int h, const char *fmt, ...); 
%ignore TCODConsole::getHeightRightRect(int x, int y, int w, int h, const char *fmt, ...); 
%ignore TCODConsole::getHeightCenterRect(int x, int y, int w, int h, const char *fmt, ...); 
TCOD_bkgnd_flag_t TCODBackgroundAlphaMask(TCOD_bkgnd_flag_t background);

// color.hpp
%ignore TCODColor::TCODColor(const TCOD_color_t & c);
%ignore TCODColor::genMap(TCODColor *map, int nbKey, TCODColor const *keyColor, int const *keyIndex);
%ignore TCODColor::colors;
%ignore operator *(float value, const TCODColor &c);

%apply float *OUTPUT { float *h, float *s, float *v};
void TCODColor::getHSV(float *h, float *s, float *v) const;

// sys.hpp
%ignore TCODSystem::createDirectory(const char *path);
%ignore TCODSystem::deleteFile(const char *path);
%ignore TCODSystem::deleteDirectory(const char *path);
%ignore TCODSystem::isDirectory(const char *path);
%ignore TCODSystem::getDirectoryContent(const char *path, const char *pattern);
%ignore TCODSystem::setClipboard(const char *value);
%ignore TCODSystem::getClipboard();
%ignore TCODSystem::getNumCores();
%ignore TCODSystem::newThread(int (*func)(void *), void *data);
%ignore TCODSystem::deleteThread(TCOD_thread_t th);
%ignore TCODSystem::waitThread(TCOD_thread_t th);
%ignore TCODSystem::newMutex();
%ignore TCODSystem::mutexIn(TCOD_mutex_t mut);
%ignore TCODSystem::mutexOut(TCOD_mutex_t mut);
%ignore TCODSystem::deleteMutex(TCOD_mutex_t mut);
%ignore TCODSystem::newSemaphore(int initVal);
%ignore TCODSystem::lockSemaphore(TCOD_semaphore_t sem);
%ignore TCODSystem::unlockSemaphore(TCOD_semaphore_t sem);
%ignore TCODSystem::deleteSemaphore( TCOD_semaphore_t sem);
%ignore TCODSystem::newCondition();
%ignore TCODSystem::signalCondition(TCOD_cond_t sem);
%ignore TCODSystem::broadcastCondition(TCOD_cond_t sem);
%ignore TCODSystem::waitCondition(TCOD_cond_t sem, TCOD_mutex_t mut);
%ignore TCODSystem::deleteCondition( TCOD_cond_t sem);
%ignore TCODSystem::registerSDLRenderer(ITCODSDLRenderer *renderer);
%ignore TCODSystem::updateChar(int asciiCode, int fontx, int fonty,const TCODImage *img,int x,int y);
%ignore ITCODSDLRenderer;
%apply int *OUTPUT {int *w, int *h};
void TCODSystem::getCurrentResolution(int *w, int *h);
void TCODSystem::getCharSize(int *w, int *h);

// bresenham.hpp
%apply int *INOUT { int *xCur, int *yCur};
bool TCODLine::step(int *xCur, int *yCur);

// image.hpp
%ignore TCODImage::TCODImage(TCOD_image_t img);

// mouse_types.h
%rename(TCODMouseData) TCOD_mouse_t;

// mersenne_types.h
%rename(TCODRandomType) TCOD_random_algo_t;

// noise.h
#if SWIGCSHARP
%include "arrays_csharp.i"
%apply float INPUT[] { float *f };
#else
%include <typemaps.i>
%apply (float *INOUT) {(double* f)};
%apply (float *INOUT, float IN) {(double* f, float octaves)};
#endif
%rename(TCODNoiseType) TCOD_noise_type_t;
%rename(NoiseDefault) TCOD_NOISE_DEFAULT;
%rename(NoiseSimplex) TCOD_NOISE_SIMPLEX;
%rename(NoisePerlin) TCOD_NOISE_PERLIN;
%rename(NoiseWavelet) TCOD_NOISE_WAVEVLET;
float TCODNoise::get(float *f, TCOD_noise_type_t typ=TCOD_NOISE_DEFAULT);
float TCODNoise::getFbm(float *f, float octaves, TCOD_noise_type_t typ=TCOD_NOISE_DEFAULT);
float TCODNoise::getTurbulence(float *f, float octaves, TCOD_noise_type_t typ=TCOD_NOISE_DEFAULT);

// path.hpp
// Swig is too stupid to handle an INOUT and OUTPUT %apply with the same name. So reproduce the entire class...sigh
class TCODLIB_API TCODPath {
public :
	TCODPath(const TCODMap *map, float diagonalCost=1.41f);
	TCODPath(int width, int height, const ITCODPathCallback *listener, void *userData, float diagonalCost=1.41f);
	virtual ~TCODPath();

	bool compute(int ox, int oy, int dx, int dy);
	%apply int *INOUT { int *x, int *y };
	bool walk(int *x, int *y, bool recalculateWhenNeeded);
	bool isEmpty() const;
	int size() const;
	%apply int *OUTPUT { int *x, int *y };
	void get(int index, int *x, int *y) const;
	%apply int *OUTPUT { int *x, int *y };
	void getOrigin(int *x,int *y) const;
	%apply int *OUTPUT { int *x, int *y };
	void getDestination(int *x,int *y) const;
};

//Dijkstra kit
class TCODLIB_API TCODDijkstra {
    public:
        TCODDijkstra (TCODMap *map, float diagonalCost=1.41f);
        TCODDijkstra (int width, int height, const ITCODPathCallback *listener, void *userData, float diagonalCost=1.41f);
        ~TCODDijkstra (void);
        void compute (int rootX, int rootY);
        float getDistance (int x, int y);
        bool setPath (int toX, int toY);
		%apply int *INOUT { int *x, int *y };
        bool walk (int *x, int *y);
		bool isEmpty() const;
		int size() const;
		%apply int *OUTPUT { int *x, int *y };
		void get(int index, int *x, int *y) const;
};

// fov_types.h
%rename(TCODFOVTypes) TCOD_fov_algorithm_t;
%ignore TCODMap::data;

// heightmap.hpp
%ignore TCODHeightMap::values;
%apply float *OUTPUT { float *min, float *max };
void TCODHeightMap::getMinMax(float *min, float *max) const;

// mouse.hpp
%rename(moveMouse) move;

#if SWIGCSHARP
%include "arrays_csharp.i"
%apply float INPUT[] { float n[3], const float *weight, const float *coef };
%apply int INPUT[] { int px[4], int py[4], const int *dx, const int *dy };
TCODHeightMap::getNormal(float x, float y,float n[3], float waterLevel=0.0f) const;
TCODHeightMap::digBezier(int px[4], int py[4], float startRadius, float startDepth, float endRadius, float endDepth);
TCODHeightMap::kernelTransform(int kernelSize, const int *dx, const int *dy, const float *weight, float minLevel,float maxLevel);
TCODHeightMap::addVoronoi(int nbPoints, int nbCoef, const float *coef,TCODRandom *rnd);
#endif // SWIGCSHARP

// bsp.hpp
%module(directors="1") directors
%{
#include "bsp.hpp"
#include "path.hpp"
#include "bresenham.hpp"
%}

%feature("director") ITCODBspCallback;
%feature("director") ITCODPathCallback;
%feature("director") TCODLineListener;

// File parser, container, compression skipped due to higher level languages having better tools.
%include "bresenham.hpp"
%include "bsp.hpp"
%include "console.hpp"
%include "fov.hpp"
%include "fov_types.h"
%include "heightmap.hpp"
%include "image.hpp"
%include "mersenne.hpp"
%include "mersenne_types.h"
%include "mouse.hpp"
%include "noise.hpp"
%include "path.hpp"
%include "sys.hpp"
%include "txtfield.hpp"
%include "BackgroundHelperFunctions.hpp"

%rename(EventKeyPress) TCOD_EVENT_KEY_PRESS;
%rename(EventKeyRelease) TCOD_EVENT_KEY_RELEASE;
%rename(EventKey) TCOD_EVENT_KEY;
%rename(EventMouseMove) TCOD_EVENT_MOUSE_MOVE;
%rename(EventMousePress) TCOD_EVENT_MOUSE_PRESS;
%rename(EventMouseRelease) TCOD_EVENT_MOUSE_RELEASE;
%rename(EventMouse) TCOD_EVENT_MOUSE;
%rename(EventAny) TCOD_EVENT_ANY;
%rename(TCODEvent) TCOD_event_t;

// Since selective rename/ignore on "namespaced" structs appears to be broken for C style typedef structs
// To work around this global renames, but only after everything else. This should be the last stuff in file.
%rename(LeftButton) lbutton;
%rename(MiddleButton) mbutton;
%rename(RightButton) rbutton;
%rename(LeftButtonPressed) lbutton_pressed;
%rename(MiddleButtonPressed) mbutton_pressed;
%rename(RightButtonPressed) rbutton_pressed;
%rename(WheelUp) wheel_up;
%rename(WheelDown) wheel_down;

%rename(PixelX) x;
%rename(PixelY) y;
%rename(PixelVelocityX) dx;
%rename(PixelVelocityY) dy;
%rename(CellX) cx;
%rename(CellY) cy;
%rename(CellVelocityX) dcx;
%rename(CellVelocityY) dcy;

%include "mouse_types.h"

%rename(KeyCode) vk;
%rename(Character) c;
%rename(Pressed) pressed;
%rename(LeftAlt) lalt;
%rename(LeftControl) lctrl;
%rename(RightAlt) ralt;
%rename(RightControl) rctrl;
%rename(Shift) shift;

%rename(Default) TCOD_BKGND_DEFAULT;
%rename(None) TCOD_BKGND_NONE;
%rename(Set) TCOD_BKGND_SET;
%rename(Multiply) TCOD_BKGND_MULTIPLY;
%rename(Lighten) TCOD_BKGND_LIGHTEN;
%rename(Darken) TCOD_BKGND_DARKEN;
%rename(Screen) TCOD_BKGND_SCREEN;
%rename(ColorDodge) TCOD_BKGND_COLOR_DODGE;
%rename(ColorBurn) TCOD_BKGND_COLOR_BURN;
%rename(Add) TCOD_BKGND_ADD;
%rename(AddAlpha) TCOD_BKGND_ADDA;
%rename(Burn) TCOD_BKGND_BURN;
%rename(Overlay) TCOD_BKGND_OVERLAY;
%rename(Alpha) TCOD_BKGND_ALPH;

%rename(GLSL) TCOD_RENDERER_GLSL;
%rename(OpenGL) TCOD_RENDERER_OPENGL;
%rename(SDL) TCOD_RENDERER_SDL;
%ignore TCOD_NB_RENDERERS;

%rename(NoKey) TCODK_NONE;
%rename(Escape) TCODK_ESCAPE;
%rename(Backspace) TCODK_BACKSPACE;
%rename(Tab) TCODK_TAB;
%rename(Enter) TCODK_ENTER;
%rename(Shift) TCODK_SHIFT;
%rename(Control) TCODK_CONTROL;
%rename(Alt) TCODK_ALT;
%rename(Pause) TCODK_PAUSE;
%rename(Capslock) TCODK_CAPSLOCK;
%rename(Pageup) TCODK_PAGEUP;
%rename(Pagedown) TCODK_PAGEDOWN;
%rename(End) TCODK_END;
%rename(Home) TCODK_HOME;
%rename(Up) TCODK_UP;
%rename(Left) TCODK_LEFT;
%rename(Right) TCODK_RIGHT;
%rename(Down) TCODK_DOWN;
%rename(Printscreen) TCODK_PRINTSCREEN;
%rename(Insert) TCODK_INSERT;
%rename(Delete) TCODK_DELETE;
%rename(Lwin) TCODK_LWIN;
%rename(Rwin) TCODK_RWIN;
%rename(Apps) TCODK_APPS;
%rename(Zero) TCODK_0;
%rename(One) TCODK_1;
%rename(Two) TCODK_2;
%rename(Three) TCODK_3;
%rename(Four) TCODK_4;
%rename(Five) TCODK_5;
%rename(Six) TCODK_6;
%rename(Seven) TCODK_7;
%rename(Eight) TCODK_8;
%rename(Nine) TCODK_9;
%rename(KeypadZero) TCODK_KP0;
%rename(KeypadOne) TCODK_KP1;
%rename(KeypadTwo) TCODK_KP2;
%rename(KeypadThree) TCODK_KP3;
%rename(KeypadFour) TCODK_KP4;
%rename(KeypadFive) TCODK_KP5;
%rename(KeypadSix) TCODK_KP6;
%rename(KeypadSeven) TCODK_KP7;
%rename(KeypadEight) TCODK_KP8;
%rename(KeypadNine) TCODK_KP9;
%rename(KeypadAdd) TCODK_KPADD;
%rename(KeypadSubtract) TCODK_KPSUB;
%rename(KeypadDivide) TCODK_KPDIV;
%rename(KeypadMultiply) TCODK_KPMUL;
%rename(KeypadDecimal) TCODK_KPDEC;
%rename(KeypadEnter) TCODK_KPENTER;
%rename(F1) TCODK_F1;
%rename(F2) TCODK_F2;
%rename(F3) TCODK_F3;
%rename(F4) TCODK_F4;
%rename(F5) TCODK_F5;
%rename(F6) TCODK_F6;
%rename(F7) TCODK_F7;
%rename(F8) TCODK_F8;
%rename(F9) TCODK_F9;
%rename(F10) TCODK_F10;
%rename(F11) TCODK_F11;
%rename(F12) TCODK_F12;
%rename(Numlock) TCODK_NUMLOCK;
%rename(Scrolllock) TCODK_SCROLLLOCK;
%rename(Space) TCODK_SPACE;
%rename(Char) TCODK_CHAR;

%rename(LayoutAsciiInColumn) TCOD_FONT_LAYOUT_ASCII_INCOL;
%rename(LayoutAsciiInRow) TCOD_FONT_LAYOUT_ASCII_INROW;
%rename(Greyscale) TCOD_FONT_TYPE_GREYSCALE;
%rename(Grayscale) TCOD_FONT_TYPE_GRAYSCALE;
%rename(LayoutTCOD) TCOD_FONT_LAYOUT_TCOD;

%rename(KeyPressed) TCOD_KEY_PRESSED;
%rename(KeyReleased) TCOD_KEY_RELEASED;

%rename(HorzLine) TCOD_CHAR_HLINE;
%rename(VertLine) TCOD_CHAR_VLINE;
%rename(NE) TCOD_CHAR_NE;
%rename(NW) TCOD_CHAR_NW;
%rename(SE) TCOD_CHAR_SE;
%rename(SW) TCOD_CHAR_SW;
%rename(TeeWest) TCOD_CHAR_TEEW;
%rename(TeeEast) TCOD_CHAR_TEEE;
%rename(TeeNorth) TCOD_CHAR_TEEN;
%rename(TeeSouth) TCOD_CHAR_TEES;
%rename(Cross) TCOD_CHAR_CROSS;
%rename(DoubleHorzLine) TCOD_CHAR_DHLINE;
%rename(DoubleVertLine) TCOD_CHAR_DVLINE;
%rename(DoubleNE) TCOD_CHAR_DNE;
%rename(DoubleNW) TCOD_CHAR_DNW;
%rename(DoubleSE) TCOD_CHAR_DSE;
%rename(DoubleSW) TCOD_CHAR_DSW;
%rename(DoubleTeeWest) TCOD_CHAR_DTEEW;
%rename(DoubleTeeEast) TCOD_CHAR_DTEEE;
%rename(DoubleTeeNorth) TCOD_CHAR_DTEEN;
%rename(DoubleTeeSouth) TCOD_CHAR_DTEES;
%rename(DoubleCross) TCOD_CHAR_DCROSS;
%rename(Block1) TCOD_CHAR_BLOCK1;
%rename(Block2) TCOD_CHAR_BLOCK2;
%rename(Block3) TCOD_CHAR_BLOCK3;
%rename(ArrowNorth) TCOD_CHAR_ARROW_N;
%rename(ArrowSouth) TCOD_CHAR_ARROW_S;
%rename(ArrowEast) TCOD_CHAR_ARROW_E;
%rename(ArrowWest) TCOD_CHAR_ARROW_W;
%rename(ArrowNorthNoTail) TCOD_CHAR_ARROW2_N;
%rename(ArrowSouthNoTail) TCOD_CHAR_ARROW2_S;
%rename(ArrowEastNoTail) TCOD_CHAR_ARROW2_E;
%rename(ArrowWestNoTail) TCOD_CHAR_ARROW2_W;
%rename(DoubleArrowHorz) TCOD_CHAR_DARROW_H;
%rename(DoubleArrowVert) TCOD_CHAR_DARROW_V;
%rename(SubpixelNorthWest) TCOD_CHAR_SUBP_NW;
%rename(SubpixelNorthEast) TCOD_CHAR_SUBP_NE;
%rename(SubpixelNorth) TCOD_CHAR_SUBP_N;
%rename(SubpixelSoutheast) TCOD_CHAR_SUBP_SE;
%rename(SubpixelDiagonal) TCOD_CHAR_SUBP_DIAG;
%rename(SubpixelEast) TCOD_CHAR_SUBP_E;
%rename(SubpixelSouthwest) TCOD_CHAR_SUBP_SW;

%rename(Smilie) TCOD_CHAR_SMILIE;
%rename(SmilieInv) TCOD_CHAR_SMILIE_INV;
%rename(Heart) TCOD_CHAR_HEART;
%rename(Diamond) TCOD_CHAR_DIAMOND;
%rename(Club) TCOD_CHAR_CLUB;
%rename(Spade) TCOD_CHAR_SPADE;
%rename(Bullet) TCOD_CHAR_BULLET;
%rename(BulletInv) TCOD_CHAR_BULLET_INV;
%rename(Male) TCOD_CHAR_MALE;
%rename(Female) TCOD_CHAR_FEMALE;
%rename(Note) TCOD_CHAR_NOTE;
%rename(NoteDouble) TCOD_CHAR_NOTE_DOUBLE;
%rename(Light) TCOD_CHAR_LIGHT;
%rename(ExclamationDouble) TCOD_CHAR_EXCLAM_DOUBLE;
%rename(Pilcrow) TCOD_CHAR_PILCROW;
%rename(Section) TCOD_CHAR_SECTION;
%rename(Pound) TCOD_CHAR_POUND;
%rename(Multiplication) TCOD_CHAR_MULTIPLICATION;
%rename(Function) TCOD_CHAR_FUNCTION;
%rename(Reserved) TCOD_CHAR_RESERVED;
%rename(Half) TCOD_CHAR_HALF;
%rename(OneQuarter) TCOD_CHAR_ONE_QUARTER;
%rename(Copyright) TCOD_CHAR_COPYRIGHT;
%rename(Cent) TCOD_CHAR_CENT;
%rename(Yen) TCOD_CHAR_YEN;
%rename(Currency) TCOD_CHAR_CURRENCY;
%rename(ThreeQuarters) TCOD_CHAR_THREE_QUARTERS;
%rename(Division) TCOD_CHAR_DIVISION;
%rename(Grade) TCOD_CHAR_GRADE;
%rename(Umlaut) TCOD_CHAR_UMLAUT;
%rename(Pow1) TCOD_CHAR_POW1;
%rename(Pow3) TCOD_CHAR_POW3;
%rename(Pow2) TCOD_CHAR_POW2;
%rename(BulletSquare) TCOD_CHAR_BULLET_SQUARE;

%ignore TCOD_CHAR_CHECKBOX_UNSET;
%ignore TCOD_CHAR_CHECKBOX_SET;
%ignore TCOD_CHAR_RADIO_UNSET;
%ignore TCOD_CHAR_RADIO_SET;

%include "console_types.h"

%rename (Red) r;
%rename (Green) g;
%rename (Blue) b;
%rename (Interpolate) lerp;

%include "color.hpp"

%rename (NoiseMaxOctaves) TCOD_NOISE_MAX_OCTAVES;
%rename (NoiseMaxDimensions) TCOD_NOISE_MAX_DIMENSIONS;
%rename (NoiseDefaultHurst) TCOD_NOISE_DEFAULT_HURST;
%rename (NoiseDefaultLacunarity) TCOD_NOISE_DEFAULT_LACUNARITY;

%include "noise_defaults.h"
