/*
* libtcod 1.6.0
* Copyright (c) 2008,2009,2010,2012,2013 Jice & Mingos
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
#ifndef _TCOD_HEIGHTMAP_HPP
#define _TCOD_HEIGHTMAP_HPP

/**
 @PageName heightmap
 @PageCategory Roguelike toolkits
 @PageTitle Heightmap toolkit
 @PageDesc This toolkit allows to create a 2D grid of float values using various algorithms.

The code using the heightmap toolkit can be automatically generated with the heightmap tool (hmtool) included in the libtcod package.
 */
class TCODLIB_API TCODHeightMap {
public :
	int w,h;
	float *values;

	/**
	@PageName heightmap_init
	@PageFather heightmap
	@PageTitle Creating a heightmap
	@FuncTitle Creating an empty map
	@FuncDesc As with other modules, you have to create a heightmap object first :
		Note that whereas most other modules use opaque structs, the TCOD_heightmap_t fields can be freely accessed. Thus, the TCOD_heightmap_new function returns a TCOD_heightmap_t pointer, not a TCOD_heightmap_t. The w and h fields should not be modified after the heightmap creation. The newly created heightmap is filled with 0.0 values.
	@Cpp TCODHeightMap::TCODHeightMap(int w, int h)
	@C
		typedef struct {
			int w,h;
			float *values;
		} TCOD_heightmap_t;
		TCOD_heightmap_t *TCOD_heightmap_new(int w,int h)
	@Py heightmap_new(w,h)
	@C# TCODHeightMap::TCODHeightMap(int w, int h)
	@Param w,h	The width and height of the heightmap.
	@CppEx TCODHeightMap myMap(50,50);
	@CEx TCOD_heightmap_t *my_map=TCOD_heightmap_new(50,50);
	@PyEx
		map=libtcod.heightmap_new(50,50)
		print map.w, map.h
	*/
	TCODHeightMap(int w, int h);

	/**
	@PageName heightmap_init
	@FuncTitle Destroying a heightmap
	@FuncDesc To release the resources used by a heightmap, destroy it with :
	@Cpp TCODHeightMap::~TCODHeightMap()
	@C void TCOD_heightmap_delete(TCOD_heightmap_t *hm)
	@Py heightmap_delete(hm)
	@C# void TCODHeightMap::Dispose()
	@Param hm	In the C version, the address of the heightmap struct returned by the creation function.
	*/
	virtual ~TCODHeightMap();

	/**
	@PageName heightmap_base
	@PageFather heightmap
	@PageTitle Basic operations
	@PageDesc Those are simple operations applied either on a single map cell or on every map cell.
	@FuncTitle Setting a cell value
	@FuncDesc Once the heightmap has been created, you can do some basic operations on the values inside it.
		You can set a single value :
	@Cpp void TCODHeightMap::setValue(int x, int y, float v)
	@C void TCOD_heightmap_set_value(TCOD_heightmap_t *hm, int x, int y, float value)
	@Py heightmap_set_value(hm, x, y, value)
	@C# void TCODHeightMap::setValue(int x, int y, float v)
	@Param hm	In the C version, the address of the heightmap struct returned by the creation function.
	@Param x,y	Coordinates of the cells to modify inside the map.
		0 <= x < map width
		0 <= y < map height
	@Param value	The new value of the map cell.
	*/
	inline void setValue(int x, int y, float v) {
		values[x+y*w]=v;
	}

	/**
	@PageName heightmap_base
	@FuncTitle Adding a float value to all cells
	@Cpp void TCODHeightMap::add(float value)
	@C void TCOD_heightmap_add(TCOD_heightmap_t *hm, float value)
	@Py heightmap_add(hm, value)
	@C# void TCODHeightMap::add(float value)
	@Param hm	In the C version, the address of the heightmap struct returned by the creation function.
	@Param value	Value to add to every cell.
	*/
	void add(float f);

	/**
	@PageName heightmap_base
	@FuncTitle Multiplying all values by a float
	@Cpp void TCODHeightMap::scale(float value)
	@C void TCOD_heightmap_scale(TCOD_heightmap_t *hm, float value)
	@Py heightmap_scale(hm, value)
	@C# void TCODHeightMap::scale(float value)
	@Param hm	In the C version, the address of the heightmap struct returned by the creation function.
	@Param value	Every cell's value is multiplied by this value.
	*/
	void scale(float f);

	/**
	@PageName heightmap_base
	@FuncTitle Resetting all values to 0.0
	@Cpp void TCODHeightMap::clear()
	@C void TCOD_heightmap_clear(TCOD_heightmap_t *hm)
	@Py heightmap_clear(hm)
	@C# void TCODHeightMap::clear()
	@Param hm	In the C version, the address of the heightmap struct returned by the creation function.
	*/
	void clear(); // resets all values to 0.0

	/**
	@PageName heightmap_base
	@FuncTitle Clamping all values
	@Cpp void TCODHeightMap::clamp(float min, float max)
	@C void TCOD_heightmap_clamp(TCOD_heightmap_t *hm, float min, float max)
	@Py heightmap_clamp(hm, mi, ma)
	@C# void TCODHeightMap::clamp(float min, float max)
	@Param hm	In the C version, the address of the heightmap struct returned by the creation function.
	@Param min,max	Every cell value is clamped between min and max.
		min < max
	*/
	void clamp(float min, float max);

	/**
	@PageName heightmap_base
	@FuncTitle Copying values from another heightmap
	@Cpp void TCODHeightMap::copy(const TCODHeightMap *source)
	@C void TCOD_heightmap_copy(const TCOD_heightmap_t *source,TCOD_heightmap_t *dest)
	@Py heightmap_copy(source,dest)
	@C# void TCODHeightMap::copy(TCODHeightMap source)
	@Param source	Each cell value from the source heightmap is copied in the destination (this for C++) heightmap.
		The source and destination heightmap must have the same width and height.
	@Param dest	In the C and python versions, the address of the destination heightmap.
	*/
	void copy(const TCODHeightMap *source);

	/**
	@PageName heightmap_base
	@FuncTitle Normalizing values
	@Cpp void TCODHeightMap::normalize(float min=0.0f, float max=1.0f)
	@C void TCOD_heightmap_normalize(TCOD_heightmap_t *hm, float min, float max)
	@Py heightmap_normalize(hm, mi=0.0, ma=1.0)
	@C#
		void TCODHeightMap::normalize()
		void TCODHeightMap::normalize(float min)
		void TCODHeightMap::normalize(float min, float max)
	@Param hm	In the C version, the address of the heightmap struct returned by the creation function.
	@Param min,max	The whole heightmap is translated and scaled so that the lowest cell value becomes min and the highest cell value becomes max
		min < max
	*/
	void normalize(float newMin=0.0f, float newMax=1.0f); // scales the values to the range [newMin;newMax]

	/**
	@PageName heightmap_base
	@FuncTitle Doing a lerp operation between two heightmaps
	@Cpp void TCODHeightMap::lerp(const TCODHeightMap *a, const TCODHeightMap *b,float coef)
	@C void TCOD_heightmap_lerp_hm(const TCOD_heightmap_t *a, const TCOD_heightmap_t *b, TCOD_heightmap_t *res, float coef)
	@Py heightmap_lerp_hm(a, b, res, coef)
	@C# void TCODHeightMap::lerp(TCODHeightMap a, TCODHeightMap b, float coef)
	@Param a	First heightmap in the lerp operation.
	@Param b	Second heightmap in the lerp operation.
	@Param coef	lerp coefficient.
		For each cell in the destination map (this for C++), value = a.value + (b.value - a.value) * coef
	@Param res	In the C and python versions, the address of the destination heightmap.
	*/
	void lerp(const TCODHeightMap *a, const TCODHeightMap *b,float coef);

	/**
	@PageName heightmap_base
	@FuncTitle Adding two heightmaps
	@Cpp void TCODHeightMap::add(const TCODHeightMap *a, const TCODHeightMap *b)
	@C void TCOD_heightmap_add_hm(const TCOD_heightmap_t *a, const TCOD_heightmap_t *b, TCOD_heightmap_t *res)
	@Py heightmap_add_hm(a, b, res)
	@C# void TCODHeightMap::add(TCODHeightMap a, TCODHeightMap b)
	@Param a	First heightmap.
	@Param b	Second heightmap. For each cell in the destination map (this for C++), value = a.value + b.value
	@Param res	In the C and python versions, the address of the destination heightmap.
	*/
	void add(const TCODHeightMap *a, const TCODHeightMap *b);

	/**
	@PageName heightmap_base
	@FuncTitle Multiplying two heightmaps
	@Cpp void TCODHeightMap::multiply(const TCODHeightMap *a, const TCODHeightMap *b)
	@C void TCOD_heightmap_multiply_hm(const TCOD_heightmap_t *a, const TCOD_heightmap_t *b, TCOD_heightmap_t *res)
	@Py heightmap_multiply_hm(a, b, res)
	@C# void TCODHeightMap::multiply(TCODHeightMap a, TCODHeightMap b)
	@Param a	First heightmap.
	@Param b	Second heightmap. For each cell in the destination map (this for C++), value = a.value * b.value
	@Param res	In the C and python versions, the address of the destination heightmap.
	*/
	void multiply(const TCODHeightMap *a, const TCODHeightMap *b);

	/**
	@PageName heightmap_modify
	@PageFather heightmap
	@PageTitle Modifying the heightmap
	@PageDesc Those are advanced operations involving several or all map cells.
	@FuncTitle Add hills
	@FuncDesc This function adds a hill (a half spheroid) at given position.
	@Cpp void TCODHeightMap::addHill(float x, float y, float radius, float height)
	@C void TCOD_heightmap_add_hill(TCOD_heightmap_t *hm, float x, float y, float radius, float height)
	@Py heightmap_add_hill(hm, x, y, radius, height)
	@C# void TCODHeightMap::addHill(float x, float y, float radius, float height)
	@Param hm	In the C version, the address of the heightmap struct returned by the creation function.
	@Param x,y	Coordinates of the center of the hill.
		0 <= x < map width
		0 <= y < map height
	@Param radius	The hill radius.
	@Param height	The hill height. If height == radius or -radius, the hill is a half-sphere.
	*/
	void addHill(float x, float y, float radius, float height); // adds a hill (half sphere) at given position

	/**
	@PageName heightmap_modify
	@FuncTitle Dig hills
	@FuncDesc This function takes the highest value (if height > 0) or the lowest (if height < 0) between the map and the hill.
		It's main goal is to carve things in maps (like rivers) by digging hills along a curve.
	@Cpp void TCODHeightMap::digHill(float hx, float hy, float hradius, float height)
	@C void TCOD_heightmap_dig_hill(TCOD_heightmap_t *hm, float x, float y, float radius, float height)
	@Py heightmap_dig_hill(hm, x, y, radius, height)
	@C# void TCODHeightMap::digHill(float hx, float hy, float hradius, float height)
	@Param hm	In the C version, the address of the heightmap struct returned by the creation function.
	@Param x,y	Coordinates of the center of the hill.
		0 <= x < map width
		0 <= y < map height
	@Param radius	The hill radius.
	@Param height	The hill height. Can be < 0 or > 0
	*/
	void digHill(float hx, float hy, float hradius, float height);

	/**
	@PageName heightmap_modify
	@FuncTitle Simulate rain erosion
	@FuncDesc This function simulates the effect of rain drops on the terrain, resulting in erosion patterns.
	@Cpp void TCODHeightMap::rainErosion(int nbDrops,float erosionCoef,float sedimentationCoef,TCODRandom *rnd)
	@C void TCOD_heightmap_rain_erosion(TCOD_heightmap_t *hm, int nbDrops,float erosionCoef,float sedimentationCoef,TCOD_random_t rnd)
	@Py heightmap_rain_erosion(hm, nbDrops,erosionCoef,sedimentationCoef,rnd=0)
	@C# void TCODHeightMap::rainErosion(int nbDrops, float erosionCoef, float sedimentationCoef, TCODRandom rnd)
	@Param hm	In the C version, the address of the heightmap struct returned by the creation function.
	@Param nbDrops	Number of rain drops to simulate. Should be at least width * height.
	@Param erosionCoef	Amount of ground eroded on the drop's path.
	@Param sedimentationCoef	Amount of ground deposited when the drops stops to flow
	@Param rnd	RNG to use, NULL for default generator.
	*/
	void rainErosion(int nbDrops,float erosionCoef,float sedimentationCoef,TCODRandom *rnd);

	/**
	@PageName heightmap_modify
	@FuncTitle Do a generic transformation
	@FuncDesc This function allows you to apply a generic transformation on the map, so that each resulting cell value is the weighted sum of several neighbour cells. This can be used to smooth/sharpen the map. See examples below for a simple horizontal smoothing kernel : replace value(x,y) with 0.33*value(x-1,y) + 0.33*value(x,y) + 0.33*value(x+1,y).To do this, you need a kernel of size 3 (the sum involves 3 surrounding cells). The dx,dy array will contain :
	dx=-1,dy = 0 for cell x-1,y
	dx=1,dy=0 for cell x+1,y
	dx=0,dy=0 for current cell (x,y)
	The weight array will contain 0.33 for each cell.
	@Cpp void TCODHeightMap::kernelTransform(int kernelSize, int *dx, int *dy, float *weight, float minLevel,float maxLevel)
	@C void TCOD_heightmap_kernel_transform(TCOD_heightmap_t *hm, int kernelsize, int *dx, int *dy, float *weight, float minLevel,float maxLevel)
	@Py heightmap_kernel_transform(hm, kernelsize, dx, dy, weight, minLevel,maxLevel)
	@C# void TCODHeightMap::kernelTransform(int kernelSize, int[] dx, int[] dy, float[] weight, float minLevel, float maxLevel)
	@Param hm	In the C version, the address of the heightmap struct returned by the creation function.
		kernelSize	Number of neighbour cells involved.
	@Param dx,dy	Array of kernelSize cells coordinates. The coordinates are relative to the current cell (0,0) is current cell, (-1,0) is west cell, (0,-1) is north cell, (1,0) is east cell, (0,1) is south cell, ...
	@Param weight	Array of kernelSize cells weight. The value of each neighbour cell is scaled by its corresponding weight
	@Param minLevel	The transformation is only applied to cells which value is >= minLevel.
	@Param maxLevel	The transformation is only applied to cells which value is <= maxLevel.
	@CEx
		int dx [] = {-1,1,0};
		int dy[] = {0,0,0};
		float weight[] = {0.33f,0.33f,0.33f};
		TCOD_heightMap_kernel_transform(heightmap,3,dx,dy,weight,0.0f,1.0f);
	@CppEx
		int dx [] = {-1,1,0};
		int dy[] = {0,0,0};
		float weight[] = {0.33f,0.33f,0.33f};
		heightmap->kernelTransform(heightmap,3,dx,dy,weight,0.0f,1.0f);
	*/
	void kernelTransform(int kernelSize, const int *dx, const int *dy, const float *weight, float minLevel,float maxLevel);

	/**
	@PageName heightmap_modify
	@FuncTitle Add a Voronoi diagram
	@FuncDesc This function adds values from a Voronoi diagram to the map.
	@Cpp void TCODHeightMap::addVoronoi(int nbPoints, int nbCoef, float *coef,TCODRandom *rnd)
	@C void TCOD_heightmap_add_voronoi(TCOD_heightmap_t *hm, int nbPoints, int nbCoef, float *coef,TCOD_random_t rnd)
	@Py heightmap_add_voronoi(hm, nbPoints, nbCoef, coef,rnd=0)
	@C# void TCODHeightMap::addVoronoi(int nbPoints, int nbCoef, float[] coef, TCODRandom rnd)
	@Param hm	In the C version, the address of the heightmap struct returned by the creation function.
	@Param nbPoints	Number of Voronoi sites.
	@Param nbCoef	The diagram value is calculated from the nbCoef closest sites.
	@Param coef	The distance to each site is scaled by the corresponding coef.
		Closest site : coef[0], second closest site : coef[1], ...
	@Param rnd	RNG to use, NULL for default generator.
	*/
	void addVoronoi(int nbPoints, int nbCoef, const float *coef,TCODRandom *rnd);

	/**
	@PageName heightmap_modify
	@FuncTitle Add a fbm
		This function adds values from a simplex fbm function to the map.
	@Cpp void TCODHeightMap::addFbm(TCODNoise *noise,float mulx, float muly, float addx, float addy, float octaves, float delta, float scale)
	@C void TCOD_heightmap_add_fbm(TCOD_heightmap_t *hm, TCOD_noise_t noise,float mulx, float muly, float addx, float addy, float octaves, float delta, float scale)
	@Py heightmap_add_fbm(hm, noise,mulx, muly, addx, addy, octaves, delta, scale)
	@C# void TCODHeightMap::addFbm(TCODNoise noise, float mulx, float muly, float addx, float addy, float octaves, float delta, float scale)
	@Param hm	In the C version, the address of the heightmap struct returned by the creation function.
	@Param noise	The 2D noise to use.
	@Param mulx, muly / addx, addy	The noise coordinate for map cell (x,y) are (x + addx)*mulx / width , (y + addy)*muly / height.
		Those values allow you to scale and translate the noise function over the heightmap.
	@Param octaves	Number of octaves in the fbm sum.
	@Param delta / scale	The value added to the heightmap is delta + noise * scale.
	@Param noise is between -1.0 and 1.0
	*/
	void addFbm(TCODNoise *noise,float mulx, float muly, float addx, float addy, float octaves, float delta, float scale);

	/**
	@PageName heightmap_modify
	@FuncTitle Scale with a fbm
	@FuncDesc This function works exactly as the previous one, but it multiplies the resulting value instead of adding it to the heightmap.
	@Cpp void TCODHeightMap::scaleFbm(TCODNoise *noise,float mulx, float muly, float addx, float addy, float octaves, float delta, float scale)
	@C void TCOD_heightmap_scale_fbm(TCOD_heightmap_t *hm, TCOD_noise_t noise,float mulx, float muly, float addx, float addy, float octaves, float delta, float scale)
	@Py heightmap_scale_fbm(hm, noise,mulx, muly, addx, addy, octaves, delta, scale)
	@C# void TCODHeightMap::scaleFbm(TCODNoise noise, float mulx, float muly, float addx, float addy, float octaves, float delta, float scale)
	*/
	void scaleFbm(TCODNoise *noise,float mulx, float muly, float addx, float addy, float octaves, float delta, float scale);

	/**
	@PageName heightmap_modify
	@FuncTitle Dig along a Bezier curve
	@FuncDesc This function carve a path along a cubic Bezier curve using the digHill function.
		Could be used for roads/rivers/...
		Both radius and depth can vary linearly along the path.
	@Cpp void TCODHeightMap::digBezier(int px[4], int py[4], float startRadius, float startDepth, float endRadius, float endDepth)
	@C void TCOD_heightmap_dig_bezier(TCOD_heightmap_t *hm, int px[4], int py[4], float startRadius, float startDepth, float endRadius, float endDepth)
	@Py heightmap_dig_bezier(hm, px, py,  startRadius,  startDepth, endRadius, endDepth)
	@C# void TCODHeightMap::digBezier(int[] px, int[] py, float startRadius, float startDepth, float endRadius, float endDepth)
	@Param hm	In the C version, the address of the heightmap struct returned by the creation function.
	@Param px,py	The coordinates of the 4 Bezier control points.
	@Param startRadius	The path radius in map cells at point P0. Might be < 1.0
	@Param startDepth	The path depth at point P0.
	@Param endRadius	The path radius in map cells at point P3. Might be < 1.0
	@Param endDepth	The path depth at point P3.
	*/
	void digBezier(int px[4], int py[4], float startRadius, float startDepth, float endRadius, float endDepth);

	/**
	@PageName heightmap_read
	@PageFather heightmap
	@PageTitle Reading data from the heightmap
	@PageDesc Those functions return raw or computed information about the heightmap.
	@FuncTitle Get the value of a cell
	@FuncDesc This function returns the height value of a map cell.
	@Cpp float TCODHeightMap::getValue(int x, int y) const
	@C float TCOD_heightmap_get_value(const TCOD_heightmap_t *hm, int x, int y)
	@Py heightmap_get_value(hm,  x, y)
	@C# float TCODHeightMap::getValue(int x, int y)
	@Param hm	In the C version, the address of the heightmap struct returned by the creation function.
	@Param x,y	Coordinates of the map cell.
		0 <= x < map width
		0 <= y < map height
	*/
	inline float getValue(int x, int y) const {
		return values[x+y*w];
	}

	/**
	@PageName heightmap_read
	@FuncTitle  Interpolate the height
	@FuncDesc This function returns the interpolated height at non integer coordinates.
	@Cpp float TCODHeightMap::getInterpolatedValue(float x, float y) const
	@C float TCOD_heightmap_get_interpolated_value(const TCOD_heightmap_t *hm, float x, float y)
	@Py heightmap_get_interpolated_value(hm, x, y)
	@C# float TCODHeightMap::getInterpolatedValue(float x, float y)
	@Param hm	In the C version, the address of the heightmap struct returned by the creation function.
	@Param x,y	Coordinates of the map cell.
		0 <= x < map width
		0 <= y < map height
	*/
	float getInterpolatedValue(float x, float y) const;

	/**
	@PageName heightmap_read
	@FuncTitle Get the map slope
	@FuncDesc This function returns the slope between 0 and PI/2 at given coordinates.
	@Cpp float TCODHeightMap::getSlope(int x, int y) const
	@C float TCOD_heightmap_get_slope(const TCOD_heightmap_t *hm, int x, int y)
	@Py heightmap_get_slope(hm, x, y)
	@C# float TCODHeightMap::getSlope(int x, int y)
	@Param hm	In the C version, the address of the heightmap struct returned by the creation function.
	@Param x,y	Coordinates of the map cell.
		0 <= x < map width
		0 <= y < map height
	*/
	float getSlope(int x, int y) const; // returns the slope in radian between 0 and PI/2

	/**
	@PageName heightmap_read
	@FuncTitle Get the map normal
	@FuncDesc This function returns the map normal at given coordinates.
	@Cpp void TCODHeightMap::getNormal(float x, float y,float n[3], float waterLevel=0.0f) const
	@C void TCOD_heightmap_get_normal(const TCOD_heightmap_t *hm, float x, float y, float n[3], float waterLevel)
	@Py heightmap_get_normal(hm, x, y, waterLevel) # returns nx,ny,nz
	@C# void TCODHeightMap::getNormal(float x, float y, float[] n, float waterLevel)
	@Param hm	In the C version, the address of the heightmap struct returned by the creation function.
	@Param x,y	Coordinates of the map cell.
		0 <= x < map width
		0 <= y < map height
	@Param n	The function stores the normalized normal vector in this array.
	@Param waterLevel	The map height is clamped at waterLevel so that the sea is flat.
	*/
	void getNormal(float x, float y,float n[3], float waterLevel=0.0f) const; // returns the surface normal or (0,0,1) if beyond water level.

	/**
	@PageName heightmap_read
	@FuncTitle Count the map cells inside a height range
	@FuncDesc This function returns the number of map cells which value is between min and max.
	@Cpp int TCODHeightMap::countCells(float min,float max) const
	@C int TCOD_heightmap_count_cells(const TCOD_heightmap_t *hm, float min, float max)
	@Py heightmap_count_cells(hm, min, max)
	@C# int TCODHeightMap::countCells(float min, float max)
	@Param hm	In the C version, the address of the heightmap struct returned by the creation function.
	@Param min,max	Only cells which value is >=min and <= max are counted.
	*/
	int countCells(float min,float max) const;

	/**
	@PageName heightmap_read
	@FuncTitle Check if the map is an island
	@FuncDesc This function checks if the cells on the map border are below a certain height.
	@Cpp bool TCODHeightMap::hasLandOnBorder(float waterLevel) const
	@C bool TCOD_heightmap_has_land_on_border(const TCOD_heightmap_t *hm, float waterLevel)
	@Py heightmap_has_land_on_border(hm, waterLevel)
	@C# bool TCODHeightMap::hasLandOnBorder(float waterLevel)
	@Param hm	In the C version, the address of the heightmap struct returned by the creation function.
	@Param waterLevel	Return true only if no border cell is > waterLevel.
	*/
	bool hasLandOnBorder(float waterLevel) const;

	/**
	@PageName heightmap_read
	@FuncTitle Get the map min and max values
	@FuncDesc This function calculates the min and max of all values inside the map.
	@Cpp void TCODHeightMap::getMinMax(float *min, float *max) const
	@C void TCOD_heightmap_get_minmax(const TCOD_heightmap_t *hm, float *min, float *max)
	@Py heightmap_get_minmax(hm) # returns min,max
	@C# void TCODHeightMap::getMinMax(out float min, out float max)
	@Param hm	In the C version, the address of the heightmap struct returned by the creation function.
	@Param min, max	The min and max values are returned in these variables.
	*/
	void getMinMax(float *min, float *max) const;

//	void heatErosion(int nbPass,float minSlope,float erosionCoef,float sedimentationCoef,TCODRandom *rnd);
	/**
	@PageName heightmap_modify
	@FuncTitle Generate a map with mid-point displacement
	@FuncDesc This algorithm generates a realistic fractal heightmap using the <a href="http://en.wikipedia.org/wiki/Diamond-square_algorithm">diamond-square</a> (or random midpoint displacement) algorithm.
		The roughness range should be comprised between 0.4 and 0.6. The image below show the same map with roughness varying from 0.4 to 0.6.
		<img src="midpoint.png" />
		It's also a good habit to normalize the map after using this algorithm to avoid unexpected heights.

	@Cpp void TCODHeightMap::midPointDisplacement(TCODRandom *rng=NULL,float roughness=0.45f)
	@C void TCOD_heightmap_mid_point_displacement(TCOD_heightmap_t *hm, TCOD_random_t rnd, float roughness)
	@Py heightmap_mid_point_displacement(hm, rng, roughness)
	@Param hm	In the C and python version, the adress of the heightmap struct returned by the creation function.
	@Param rng	Random number generation to use, or NULL/0 to use the default one.
	@Param roughness	Map roughness.
	*/
	void midPointDisplacement(TCODRandom *rnd = NULL, float roughness=0.45f);
	void islandify(float seaLevel,TCODRandom *rnd); // lowers the terrain near the heightmap borders
	// TODO : checks island connectivity with floodfill
private :
//	void setMPDHeight(TCODRandom *rnd,int x,int y, float z, float offset);
//	void setMDPHeightSquare(TCODRandom *rnd,int x, int y, int initsz, int sz,float offset);
};

#endif
