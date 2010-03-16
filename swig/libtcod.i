%module libtcod
#pragma SWIG nowarn=503
%{
#include "libtcod.hpp"
%}

#define NO_UNICODE
typedef unsigned char uint8;

%include typemaps.i
%rename(Equal) operator ==;
%rename(NotEqual) operator !=;
%rename(Multiply) operator *;
%rename(Plus) operator +;
%rename(Minus) operator -;

%ignore TCODColor::genMap(TCODColor *map, int nbKey, TCODColor const *keyColor, int const *keyIndex);
%ignore TCODColor::colors;

%include "color.hpp"
%include "console_types.h"
%include "console.hpp"