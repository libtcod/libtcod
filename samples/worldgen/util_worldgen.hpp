/*
* Copyright (c) 2009 Jice
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

// size of the heightmap
#define HM_WIDTH 400
#define HM_HEIGHT 400

// biome and climate list. based on Whittaker Biome Diagram
enum EClimate {
    ARTIC_ALPINE,
    COLD,
    TEMPERATE,
    WARM,
    TROPICAL,
    NB_CLIMATES
};

// grassland : might be either grassland, shrubland or woodland depending on the vegetation level
// savanna : might be either savanna or thorn forest depending on the vegetation level
enum EBiome {TUNDRA,
    COLD_DESERT, GRASSLAND, BOREAL_FOREST,
    TEMPERATE_FOREST, TROPICAL_MONTANE_FOREST,
    HOT_DESERT, SAVANNA, TROPICAL_DRY_FOREST, TROPICAL_EVERGREEN_FOREST,
    THORN_FOREST,
    NB_BIOMES
    };

class WorldGenerator {
public :
	// altitude->color map
	TCODColor mapGradient[256];
	// world height map (0.0 - 1.0)
	TCODHeightMap *hm;
	// height map without erosion
	TCODHeightMap *hm2;
	// complete world map (not shaded)
	TCODImage *worldmap;
	// temperature map (in °C)
	TCODHeightMap *temperature;
	// precipitation map (0.0 - 1.0)
	TCODHeightMap *precipitation;
	// biome map
	EBiome *biomeMap;

	void generate(TCODRandom *wRng);

	// getters
	int getWidth() const;
	int getHeight() const;
	float getAltitude(int x, int y) const; // heightmap. between 0 and 1
	float getInterpolatedAltitude(float x, float y) const;
	float getSandHeight() const;
	bool isOnSea(float x, float y) const;
	float getCloudThickness(float x, float y) const;
	void getInterpolatedNormal(float x, float y, float n[3]) const;
	TCODColor getInterpolatedColor(float worldX,float worldY);
	float getInterpolatedIntensity(float worldX, float worldY);

	// update
	void updateClouds(float elapsedTime);
	void computeSunLight(float lightDir[3]);
	
	// data
	float getRealAltitude(float x, float y) const; // altitude in meters
	float getPrecipitations(float x, float y) const; // in centimeter/m²/year
	float getTemperature(float x, float y) const; // in °C
	EBiome getBiome(float x, float y) const;

	// map generators
	void saveBiomeMap(const char *filename = NULL);
	void saveAltitudeMap(const char *filename = NULL);
	void saveTemperatureMap(const char *filename = NULL);
	void savePrecipitationMap(const char *filename = NULL);
protected :
    friend class RiverPathCbk;
	TCODNoise *noise;
	// cloud thickness
	float clouds[HM_WIDTH][HM_HEIGHT];
	float cloudDx; // horizontal offset for smooth scrolling
	float cloudTotalDx;
	// world light intensity map (shadow map)
	float *worldint;
	typedef struct {
		float slope;
		// number of cells flowing into this cell
		uint32 area;
		// direction of lowest neighbour
		uint8 flowDir;
		// inverse flow direction
		uint8 upDir;
		uint8 inFlags; // incoming flows
		uint8 riverId;
		int riverLength;
	} map_data_t;
	map_data_t * mapData;
	typedef struct {
	    TCODList<int> coords;
	    TCODList<int> strength;
	} river_t;
	TCODList<river_t *> rivers;
	TCODRandom *wgRng;

	void addHill(int nbHill, float baseRadius, float radiusVar, float height);
	void buildBaseMap();
	void erodeMap();
	void smoothMap();
	// compute the ground color from the heightmap
	TCODColor getMapColor(float h);
	// get sun light intensity on a point of the map
	float getMapIntensity(float worldX,float worldY, float lightDir[3]);
	TCODColor getInterpolatedColor(TCODImage *img,float x,float y);
	float getInterpolatedFloat(float *arr,float x,float y, int width, int height);
	void generateRivers();
	void smoothPrecipitations() ;
	int getRiverStrength(int riverId);
	void setLandMass(float percent, float waterLevel);
	void computeTemperaturesAndBiomes();
	TCODColor getBiomeColor(EBiome biome,int x,int y);
	void computePrecipitations();
	void computeColors();
	void drawCoasts(TCODImage *img);
	EClimate getClimateFromTemp(float temp);
};
