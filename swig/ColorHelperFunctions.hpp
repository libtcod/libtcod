#ifndef _COLORHELPERFUNCTIONS_H
#define _COLORHELPERFUNCTIONS_H

#include "console_types.h"

class TCODColorHelpers
{
	public:
		static TCOD_bkgnd_flag_t CreateAddAlphaBackground(float alpha)
		{
    		return (TCOD_bkgnd_flag_t)(TCOD_BKGND_ADDA|(((uint8)(alpha*255))<<8));
		}
	
		static TCOD_bkgnd_flag_t CreateAlphaBackground(float alpha)
		{
    		return (TCOD_bkgnd_flag_t)(TCOD_BKGND_ALPH|(((uint8)(alpha*255))<<8));
		}
};

#endif //_COLORHELPERFUNCTIONS_H
