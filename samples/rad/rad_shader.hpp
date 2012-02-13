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

class Shader {
public :
	virtual ~Shader() {}
	virtual void init(TCODMap *map);
	// add a new light. return its ID
	virtual int addLight(int x, int y, int radius, const TCODColor &col);
	// update the light with given ID
	virtual void updateLight(int id, int x, int y, int radius, const TCODColor &col);
	virtual void compute() = 0;
	// get the computed light color
	virtual const TCODColor &getLightColor(int x, int y);
protected :
	struct Light {
		int x,y,radius;
		TCODColor col;
	};
	TCODList<Light> lights;
	TCODColor *lightmap;
	TCODMap *map;
};

class StandardShader : public Shader {
public :
	void compute();
};

class PhotonShader : public Shader {
public :
	PhotonShader(float reflectivity, float selfIllumination, int nbPass);
	void init(TCODMap *map);
	void compute();
	int addLight(int x, int y, int radius, const TCODColor &col);
protected :
	// maximum radius of a light in the map
	int maxRadius;
	float reflectivity;
	float selfIllumination;
	int nbPass;
	// array of MAP_WIDTH*MAP_HEIGHT*maxDiameter*maxDiamter form factor coefficients (0-255)
	// maxDiameter = 2*maxRadius+1
	float *ff;
	// array of MAP_WIDTH*MAP_HEIGHT containing the sum of all affected cells form factors for each cell in the map
	float *ffSum;
	// convenient way to store the list of cells with non null incoming light
	struct Coord {
		uint16 x;
		uint16 y;
		Coord() {}
		Coord(int x, int y) : x(x),y(y){}
	};
	TCODList<Coord> lightsCoord;
	// color not limited to 0-255 range
	struct FColor {
		float r,g,b;
	}; 
	struct CellData {
		// amount of light emitted by this cell for this pass
		FColor emission;
		// amount of light incoming from other cells for this pass (including self through self illumination)
		FColor incoming;
		// total amount of light on the cell (used to shade the map)
		FColor outgoing;
	};
	// array of MAP_WIDTH*MAP_HEIGHT CellData
	CellData *data;
	
	void propagate();
	void computeFormFactor(int x, int y);
	void computeLightContribution(int lx, int ly, int lradius, const FColor &lcol, float reflectivity);
		
};
