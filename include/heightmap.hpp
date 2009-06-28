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
#ifndef _TCOD_HEIGHTMAP_HPP
#define _TCOD_HEIGHTMAP_HPP

class TCODLIB_API TCODHeightMap {
public :
	int w,h;
	float *values;
	
	TCODHeightMap(int w, int h);
	virtual ~TCODHeightMap();
	inline float getValue(int x, int y) const {
		return values[x+y*w];
	}
	inline void setValue(int x, int y, float v) {
		values[x+y*w]=v;
	}
	float getInterpolatedValue(float x, float y) const;
	float getSlope(int x, int y) const; // returns the slope in radian between 0 and PI/2
	void getNormal(float x, float y,float n[3], float waterLevel=0.0f) const; // returns the surface normal or (0,0,1) if beyond water level. 
	int countCells(float min,float max) const;
	bool hasLandOnBorder(float waterLevel) const;
	void getMinMax(float *min, float *max) const;

	void copy(const TCODHeightMap *source);
	void add(float f);
	void scale(float f);
	void clamp(float min, float max);
	void clear(); // resets all values to 0.0
	void normalize(float newMin=0.0f, float newMax=1.0f); // scales the values to the range [newMin;newMax]
	void lerp(const TCODHeightMap *a, const TCODHeightMap *b,float coef);
	void add(const TCODHeightMap *a, const TCODHeightMap *b);
	void multiply(const TCODHeightMap *a, const TCODHeightMap *b);

	void addHill(float x, float y, float radius, float height); // adds a hill (half sphere) at given position
	void digHill(float hx, float hy, float hradius, float height);
	void digBezier(int px[4], int py[4], float startRadius, float startDepth, float endRadius, float endDepth);
	void rainErosion(int nbDrops,float erosionCoef,float sedimentationCoef,TCODRandom *rnd);
//	void heatErosion(int nbPass,float minSlope,float erosionCoef,float sedimentationCoef,TCODRandom *rnd);
	void kernelTransform(int kernelSize, int *dx, int *dy, float *weight, float minLevel,float maxLevel);
	void addVoronoi(int nbPoints, int nbCoef, float *coef,TCODRandom *rnd);
//	void midPointDeplacement(TCODRandom *rnd);
	void addFbm(TCODNoise *noise,float mulx, float muly, float addx, float addy, float octaves, float delta, float scale); 
	void scaleFbm(TCODNoise *noise,float mulx, float muly, float addx, float addy, float octaves, float delta, float scale); 
	void islandify(float seaLevel,TCODRandom *rnd); // lowers the terrain near the heightmap borders
	// TODO : checks island connectivity with floodfill
private :
//	void setMPDHeight(TCODRandom *rnd,int x,int y, float z, float offset);
//	void setMDPHeightSquare(TCODRandom *rnd,int x, int y, int initsz, int sz,float offset);
};

#endif
