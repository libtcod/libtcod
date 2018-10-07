/*
* Photon reactor
* Copyright (c) 2011 Jice
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * The names of Jice may not be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY JICE ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL JICE BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "libtcod.hpp"
#include "rad_shader.hpp"

void StandardShader::compute() {
	// turn off all lights
	memset(lightmap,0,sizeof(TCODColor)*map->getWidth()*map->getHeight());
	for ( Light *l=lights.begin(); l != lights.end(); l++) {
		// compute the potential visible set for this light
		int minx=l->x-l->radius;
		int miny=l->y-l->radius;
		int maxx=l->x+l->radius;
		int maxy=l->y+l->radius;
		minx=MAX(0,minx);
		miny=MAX(0,miny);
		maxx=MIN(maxx,map->getWidth()-1);
		maxy=MIN(maxy,map->getHeight()-1);
		float offset = 1.0f/(1.0f+(float)(l->radius*l->radius)/20);
		float factor = 1.0f/(1.0f-offset);
		// compute the light's fov
		TCODMap lmap(maxx-minx+1,maxy-miny+1);
		for (int x=minx; x <= maxx; x++) {
			for (int y=miny; y <= maxy; y++) {
				lmap.setProperties(x-minx,y-miny,map->isWalkable(x,y),map->isTransparent(x,y));
			}
		}
		lmap.computeFov(l->x-minx,l->y-miny,l->radius);
		// compute the light's contribution		
		//double invSquaredRadius=1.0 / (l->radius*l->radius);
		for (int x=minx; x <= maxx; x++) {
			for (int y=miny; y <= maxy; y++) {
				if ( lmap.isInFov(x-minx,y-miny)) {
					int squaredDist = (l->x-x)*(l->x-x)+(l->y-y)*(l->y-y);
					// basic
					//double coef = 1.0-squaredDist*invSquaredRadius;
					// invsqr1
					//double coef=(1.0f/(1.0f+(float)(squaredDist)));
					// invsqr2
					double coef=(1.0f/(1.0f+(float)(squaredDist)/20)- offset)*factor;
					TCODColor *col=&lightmap[x+y*map->getWidth()];
					*col = *col + l->col * coef;
				}					
			}
		}
	}
}

