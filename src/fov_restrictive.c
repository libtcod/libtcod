/*
* libtcod 1.4.2
* Copyright (c) 2008,2009 Jice
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * The name of Jice may not be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY Jice ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL Jice BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
* Mingos' Restrictive Precise Angle Shadowcasting (MRPAS)
* This file was written by Dominik "Mingos" Marczuk.
* Original implementation: http://umbrarumregnum.110mb.com/downloads/MRPAS.zip
*/

#include <stdlib.h> // for NULL in VS
#include "libtcod.h"
#include "libtcod_int.h"

void TCOD_map_compute_fov_restrictive_shadowcasting_quadrant (map_t *m, int player_x, int player_y, int max_radius, bool light_walls, int maxObstacles, int dx, int dy) {
    static double *startAngle=NULL, *endAngle=NULL;
    static int angleArraySize=0;

    if ( angleArraySize > 0 && angleArraySize < maxObstacles ) {
        free(startAngle);
        startAngle=NULL;
    }
    if ( startAngle == NULL ) {
        angleArraySize = maxObstacles;
        startAngle = (double *)malloc(sizeof(double) * 2 * maxObstacles);
        endAngle = &startAngle[maxObstacles];
    }
    //octant: vertical edge
    {
        int iteration = 1; //iteration of the algo for this octant
        bool done = false;
        int totalObstacles = 0;
        int obstaclesInLastLine = 0;
		double minAngle = 0.0f;
		int x,y;

        //do while there are unblocked slopes left and the algo is within the map's boundaries
        //scan progressive lines/columns from the PC outwards
        y = player_y+dy; //the outer slope's coordinates (first processed line)
        if (y < 0 || y >= m->height) done = true;
		while(!done) {
            //process cells in the line
			double slopesPerCell = 1.0f/(double)(iteration+1);
			double halfSlopes = slopesPerCell*0.5f;
			int processedCell = (int)(minAngle / slopesPerCell);
            int minx = MAX(0,player_x-iteration), maxx = MIN(m->width-1,player_x+iteration);
            done = true;
            for (x = player_x + (processedCell * dx); x >= minx && x <= maxx; x+=dx) {
                int c = x + (y * m->width);
                //calculate slopes per cell
                bool visible = true;
                double startSlope = (double)processedCell*slopesPerCell;
                double centreSlope = startSlope+halfSlopes;
                double endSlope = startSlope+slopesPerCell;
                if (obstaclesInLastLine > 0 && m->cells[c].fov == 0) {
                    int idx = 0;
                    while(visible && idx < obstaclesInLastLine) {
                        if (m->cells[c].transparent == true) {
                            if (centreSlope > startAngle[idx] && centreSlope < endAngle[idx])
                                visible = false;
                            }
                        else {
                            if (startSlope >= startAngle[idx] && endSlope <= endAngle[idx])
                                visible = false;
                        }
                        if (visible && (m->cells[c-(m->width*dy)].fov == 0 || !m->cells[c-(m->width*dy)].transparent) && (x-dx >= 0 && x-dx < m->width && (m->cells[c-(m->width*dy)-dx].fov == 0 || !m->cells[c-(m->width*dy)-dx].transparent))) visible = false;
                        idx++;
                    }
                }
                if (visible) {
                    m->cells[c].fov = 1;
                    done = false;
                    //if the cell is opaque, block the adjacent slopes
                    if (!m->cells[c].transparent) {
                        if (minAngle >= startSlope) minAngle = endSlope;
                        else {
                        	startAngle[totalObstacles] = startSlope;
                        	endAngle[totalObstacles++] = endSlope;
                        }
                        if (!light_walls) m->cells[c].fov = 0;
                    }
                }
                processedCell++;
            }
            if (iteration == max_radius) done = true;
            iteration++;
            obstaclesInLastLine = totalObstacles;
            y += dy;
            if (y < 0 || y >= m->height) done = true;
			if ( minAngle == 1.0f ) done=true;
        }
    }
    //octant: horizontal edge
    {
        int iteration = 1; //iteration of the algo for this octant
        bool done = false;
        int totalObstacles = 0;
        int obstaclesInLastLine = 0;
		double minAngle = 0.0f;
		int x,y;

        //do while there are unblocked slopes left and the algo is within the map's boundaries
        //scan progressive lines/columns from the PC outwards
        x = player_x+dx; //the outer slope's coordinates (first processed line)
        if (x < 0 || x >= m->width) done = true;
		while(!done) {
            //process cells in the line
			double slopesPerCell = 1.0f/(double)(iteration+1);
			double halfSlopes = slopesPerCell*0.5f;
			int processedCell = (int)(minAngle / slopesPerCell);
            int miny = MAX(0,player_y-iteration), maxy = MIN(m->height-1,player_y+iteration);
            done = true;
            for (y = player_y + (processedCell * dy); y >= miny && y <= maxy; y+=dy) {
                int c = x + (y * m->width);
                //calculate slopes per cell
                bool visible = true;
                double startSlope = (double)processedCell*slopesPerCell;
                double centreSlope = startSlope+halfSlopes;
                double endSlope = startSlope+slopesPerCell;
                if (obstaclesInLastLine > 0 && m->cells[c].fov == 0) {
                    int idx = 0;
                    while(visible && idx < obstaclesInLastLine) {
                        if (m->cells[c].transparent == true) {
                            if (centreSlope > startAngle[idx] && centreSlope < endAngle[idx])
                                visible = false;
                            }
                        else {
                            if (startSlope >= startAngle[idx] && endSlope <= endAngle[idx])
                                visible = false;
                        }
                        if (visible && (m->cells[c-dx].fov == 0 || !m->cells[c-dx].transparent) && (y-dy >= 0 && y-dy < m->height && (m->cells[c-(m->width*dy)-dx].fov == 0 || !m->cells[c-(m->width*dy)-dx].transparent))) visible = false;
                        idx++;
                    }
                }
                if (visible) {
                    m->cells[c].fov = 1;
                    done = false;
                    //if the cell is opaque, block the adjacent slopes
                    if (!m->cells[c].transparent) {
                        if (minAngle >= startSlope) minAngle = endSlope;
                        else {
                        	startAngle[totalObstacles] = startSlope;
                        	endAngle[totalObstacles++] = endSlope;
                        }
                        if (!light_walls) m->cells[c].fov = 0;
                    }
                }
                processedCell++;
            }
            if (iteration == max_radius) done = true;
            iteration++;
            obstaclesInLastLine = totalObstacles;
            x += dx;
            if (x < 0 || x >= m->width) done = true;
			if ( minAngle == 1.0f ) done=true;
        }
    }
}

void TCOD_map_compute_fov_restrictive_shadowcasting(TCOD_map_t map, int player_x, int player_y, int max_radius, bool light_walls) {
    map_t *m = (map_t *)map;
    int c;
    int maxObstacles;
    //first, zero the FOV map
    for(c = m->nbcells - 1; c >= 0; c--) {
        m->cells[c].fov = 0;
    }

    //calculate an approximated (excessive, just in case) maximum number of obstacles per octant
    maxObstacles = m->nbcells / 7;

    //set PC's position as visible
    m->cells[player_x+(player_y*m->width)].fov = 1;

    //compute the 4 quadrants of the map
    TCOD_map_compute_fov_restrictive_shadowcasting_quadrant (m, player_x, player_y, max_radius, light_walls, maxObstacles, 1, 1);
    TCOD_map_compute_fov_restrictive_shadowcasting_quadrant (m, player_x, player_y, max_radius, light_walls, maxObstacles, 1, -1);
    TCOD_map_compute_fov_restrictive_shadowcasting_quadrant (m, player_x, player_y, max_radius, light_walls, maxObstacles, -1, 1);
    TCOD_map_compute_fov_restrictive_shadowcasting_quadrant (m, player_x, player_y, max_radius, light_walls, maxObstacles, -1, -1);
}
