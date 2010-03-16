%module libtcod
#pragma SWIG nowarn=503
%{
#include "libtcod.hpp"
%}

%include "color.hpp"
%include "console_types.h"
%include "console.hpp"
