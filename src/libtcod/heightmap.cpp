/* BSD 3-Clause License
 *
 * Copyright © 2008-2019, Jice and the libtcod contributors.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include "heightmap.hpp"

#include <math.h>

#include <cstring>

TCODHeightMap::TCODHeightMap(int width, int height)
: w(width), h(height)
{
  values = new float[w * h];
  memset(values, 0, sizeof(float) * w * h);
}
TCODHeightMap::~TCODHeightMap() {
	delete [] values;
}

void TCODHeightMap::clear() {
	TCOD_heightmap_t hm={w,h,values};
	TCOD_heightmap_clear(&hm);
}
void TCODHeightMap::normalize(float newMin, float newMax) {
	TCOD_heightmap_t hm={w,h,values};
	TCOD_heightmap_normalize(&hm,newMin,newMax);
}

void TCODHeightMap::getMinMax(float *min, float *max) const {
	TCOD_heightmap_t hm={w,h,values};
	TCOD_heightmap_get_minmax(&hm,min,max);
}

void TCODHeightMap::addHill(float hx, float hy, float hradius, float height) {
	TCOD_heightmap_t hm={w,h,values};
	TCOD_heightmap_add_hill(&hm,hx,hy,hradius,height);
}

void TCODHeightMap::digHill(float hx, float hy, float hradius, float height) {
	TCOD_heightmap_t hm={w,h,values};
	TCOD_heightmap_dig_hill(&hm,hx,hy,hradius,height);
}

void TCODHeightMap::copy(const TCODHeightMap *source) {
	TCOD_heightmap_t hm_source={source->w,source->h,source->values};
	TCOD_heightmap_t hm_dest={w,h,values};
	TCOD_heightmap_copy(&hm_source,&hm_dest);
}

void TCODHeightMap::addFbm(TCODNoise *noise, float mulx, float muly, float addx, float addy, float octaves, float delta, float scale) {
	TCOD_heightmap_t hm={w,h,values};
	TCOD_heightmap_add_fbm(&hm,noise->data,mulx,muly,addx,addy,octaves,delta,scale);
}
void TCODHeightMap::scaleFbm(TCODNoise *noise, float mulx, float muly, float addx, float addy, float octaves, float delta, float scale) {
	TCOD_heightmap_t hm={w,h,values};
	TCOD_heightmap_scale_fbm(&hm,noise->data,mulx,muly,addx,addy,octaves,delta,scale);
}

float TCODHeightMap::getInterpolatedValue(float x, float y) const {
	TCOD_heightmap_t hm={w,h,values};
	return TCOD_heightmap_get_interpolated_value(&hm,x,y);
}

void TCODHeightMap::getNormal(float x, float y,float n[3], float waterHeight) const {
	TCOD_heightmap_t hm={w,h,values};
	return TCOD_heightmap_get_normal(&hm,x,y,n,waterHeight);
}

void TCODHeightMap::digBezier(int px[4], int py[4], float startRadius, float startDepth, float endRadius, float endDepth) {
	TCOD_heightmap_t hm={w,h,values};
	TCOD_heightmap_dig_bezier(&hm,px,py,startRadius,startDepth,endRadius,endDepth);
}

bool TCODHeightMap::hasLandOnBorder(float seaLevel) const {
	TCOD_heightmap_t hm={w,h,values};
	return TCOD_heightmap_has_land_on_border(&hm,seaLevel) != 0;
}

void TCODHeightMap::islandify(float seaLevel,TCODRandom *rnd) {
	TCOD_heightmap_t hm={w,h,values};
	return TCOD_heightmap_islandify(&hm,seaLevel,rnd->data);
}

void TCODHeightMap::add(float f) {
	TCOD_heightmap_t hm={w,h,values};
	TCOD_heightmap_add(&hm,f);
}

int TCODHeightMap::countCells(float min,float max) const {
	TCOD_heightmap_t hm={w,h,values};
	return TCOD_heightmap_count_cells(&hm,min,max);
}

void TCODHeightMap::scale(float f) {
	TCOD_heightmap_t hm={w,h,values};
	TCOD_heightmap_scale(&hm,f);
}

void TCODHeightMap::clamp(float min, float max) {
	TCOD_heightmap_t hm={w,h,values};
	TCOD_heightmap_clamp(&hm,min,max);
}

void TCODHeightMap::lerp(const TCODHeightMap *a, const TCODHeightMap *b,float coef) {
	TCOD_heightmap_t hm1={a->w,a->h,a->values};
	TCOD_heightmap_t hm2={b->w,b->h,b->values};
	TCOD_heightmap_t hmres={w,h,values};
	TCOD_heightmap_lerp_hm(&hm1,&hm2,&hmres,coef);
}

void TCODHeightMap::add(const TCODHeightMap *a, const TCODHeightMap *b) {
	TCOD_heightmap_t hm1={a->w,a->h,a->values};
	TCOD_heightmap_t hm2={b->w,b->h,b->values};
	TCOD_heightmap_t hmres={w,h,values};
	TCOD_heightmap_add_hm(&hm1,&hm2,&hmres);
}

void TCODHeightMap::multiply(const TCODHeightMap *a, const TCODHeightMap *b) {
	TCOD_heightmap_t hm1={a->w,a->h,a->values};
	TCOD_heightmap_t hm2={b->w,b->h,b->values};
	TCOD_heightmap_t hmres={w,h,values};
	TCOD_heightmap_multiply_hm(&hm1,&hm2,&hmres);
}

float TCODHeightMap::getSlope(int x, int y) const {
	TCOD_heightmap_t hm={w,h,values};
	return TCOD_heightmap_get_slope(&hm,x,y);
}

void TCODHeightMap::rainErosion(int nbDrops, float erosionCoef,float agregationCoef, TCODRandom *rnd) {
	TCOD_heightmap_t hm={w,h,values};
	TCOD_heightmap_rain_erosion(&hm, nbDrops, erosionCoef, agregationCoef, rnd->data);
}

void TCODHeightMap::kernelTransform(int kernelSize, const int *dx, const int *dy, const float *weight, float minLevel,float maxLevel) {
	TCOD_heightmap_t hm={w,h,values};
	TCOD_heightmap_kernel_transform(&hm, kernelSize,dx,dy,weight,minLevel, maxLevel);
}

void TCODHeightMap::addVoronoi(int nbPoints, int nbCoef, const float *coef,TCODRandom *rnd) {
	TCOD_heightmap_t hm={w,h,values};
	TCOD_heightmap_add_voronoi(&hm, nbPoints, nbCoef, coef, rnd->data);
}

#if 0
void TCODHeightMap::heatErosion(int nbPass,float minSlope,float erosionCoef,float agregationCoef,TCODRandom *rnd) {
	TCOD_heightmap_t hm={w,h,values};
	TCOD_heightmap_heat_erosion(&hm, nbPass, minSlope, erosionCoef, agregationCoef, rnd->data);
}
#endif

void TCODHeightMap::midPointDisplacement(TCODRandom *rnd, float roughness) {
	TCOD_heightmap_t hm={w,h,values};
	if ( ! rnd ) rnd = TCODRandom::getInstance();
	TCOD_heightmap_mid_point_displacement(&hm, rnd->data, roughness);
}
