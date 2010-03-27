%module libtcod
#pragma SWIG nowarn=503
%{
#include "libtcod.hpp"
%}

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
%rename(Char) TCODK_CHA;

%include typemaps.i
%rename(Equal) operator ==;
%rename(NotEqual) operator !=;
%rename(Multiply) operator *;
%rename(Plus) operator +;
%rename(Minus) operator -;

// console.hpp
%ignore TCOD_colctrl_t;
%ignore TCODConsole::setColorControl(TCOD_colctrl_t con, const TCODColor &fore, const TCODColor &back);
%ignore TCODConsole::TCODConsole(TCOD_console_t n);

// color.hpp
%ignore TCODColor::TCODColor(const TCOD_color_t & c);
%ignore TCODColor::genMap(TCODColor *map, int nbKey, TCODColor const *keyColor, int const *keyIndex);
%ignore TCODColor::colors;

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
%apply int *OUTPUT { int *xCur, int *yCur};
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
float TCODNoise::getPerlin(float *f) const;
float TCODNoise::getFbmPerlin(float *f, float octaves) const;
float TCODNoise::getTurbulencePerlin(float *f, float octaves) const;
float TCODNoise::getSimplex(float *f) const;
float TCODNoise::getFbmSimplex(float *f, float octaves) const;
float TCODNoise::getTurbulenceSimplex(float *f, float octaves) const;
float TCODNoise::getWavelet(float *f) const;
float TCODNoise::getFbmWavelet(float *f, float octaves) const;
float TCODNoise::getTurbulenceWavelet(float *f, float octaves) const;
#endif // SWIGCSHARP

// path.hpp
%ignore TCOD_path_func(int xFrom, int yFrom, int xTo, int yTo, void *data);
%apply int *OUTPUT { int *x, int *y };

// fov_types.h
%rename(TCODFOVTypes) TCOD_fov_algorithm_t;
%ignore TCODMap::data;

// namegen.hpp
%ignore TCODNamegen::getSets();

// heightmap.hpp
%ignore TCODHeightMap::values;
%apply float *OUTPUT { float *min, float *max };
void TCODHeightMap::getMinMax(float *min, float *max) const;

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

%include "bresenham.hpp"
%include "bsp.hpp"
%include "color.hpp"
%include "console.hpp"
%include "console_types.h"
%include "fov.hpp"
%include "fov_types.h"
%include "heightmap.hpp"
%include "image.hpp"
%include "mersenne.hpp"
%include "mersenne_types.h"
%include "mouse.hpp"
%include "mouse_types.h"
%include "namegen.hpp"
%include "noise.hpp"
%include "path.hpp"
%include "sys.hpp"

// File parser, container, compression skipped due to higher level languages having better tools.
