%module libtcod
#pragma SWIG nowarn=503
%{
#include "libtcod.hpp"
%}

#define NO_UNICODE
typedef unsigned char uint8;
typedef unsigned int uint32;

%rename(TCODKey) TCOD_key_t;
%rename(TCODKeyCode) TCOD_keycode_t;
%rename(TCODBackgroundFlag) TCOD_bkgnd_flag_t;
%rename(TCODRendererType) TCOD_renderer_t;

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
%ignore TCODLineListener;
%ignore TCODLine::line(int xFrom, int yFrom, int xTo, int yTo, TCODLineListener *listener);
%apply int *OUTPUT { int *xCur, int *yCur};
bool TCODLine::step(int *xCur, int *yCur);

// image.hpp
%ignore TCODImage::TCODImage(TCOD_image_t img);

// mouse_types.h
%rename(TCODMouseData) TCOD_mouse_t;

// mersenne_types.h
%rename(TCODRandomType) TCOD_random_algo_t;

// noise.h
%apply float *OUTPUT { float *f};
float TCODNoise::getPerlin(float *f) const;
float TCODNoise::getFbmPerlin(float *f, float octaves) const;
float TCODNoise::getTurbulencePerlin(float *f, float octaves) const;
float TCODNoise::getSimplex(float *f) const;
float TCODNoise::getFbmSimplex(float *f, float octaves) const;
float TCODNoise::getTurbulenceSimplex(float *f, float octaves) const;
float TCODNoise::getWavelet(float *f) const;
float TCODNoise::getFbmWavelet(float *f, float octaves) const;
float TCODNoise::getTurbulenceWavelet(float *f, float octaves) const;

// fov_types.h
%rename(TCODFOVTypes) TCOD_fov_algorithm_t;
%ignore TCODMap::data;

// namegen.hpp
%ignore TCODNamegen::getSets();

%include "color.hpp"
%include "console_types.h"
%include "console.hpp"
%include "sys.hpp"
%include "bresenham.hpp"
%include "image.hpp"
%include "mouse_types.h"
%include "mouse.hpp"
%include "mersenne.hpp"
%include "mersenne_types.h"
%include "noise.hpp"
%include "fov.hpp"
%include "fov_types.h"
%include "namegen.hpp"