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

#ifndef _TCOD_FOV_HPP
#define _TCOD_FOV_HPP

#include "fov_types.h"

class TCODPath;

/**
 @PageName fov
 @PageCategory Roguelike toolkits
 @PageTitle Field of view
 @PageDesc This toolkit allows to easily calculate the potential visible set of map cells from the player position.
A cell is potentially visible if the line of sight from the player to the cell in unobstructed.
 */

class TCODLIB_API TCODMap {
	public :
		/**
		@PageName fov_init
		@PageFather fov
		@PageTitle Building the map
		@FuncTitle Creating the map object
		@FuncDesc First, you have to allocate a map of the same size as your dungeon.
		@Cpp TCODMap::TCODMap (int width, int height)
		@C TCOD_map_t TCOD_map_new (int width, int height)
		@Py map_new (width, height)
		@C# TCODMap::TCODMap(int width, int height)
		@Param width, height	The size of the map (in map cells).
		*/		
		TCODMap(int width, int height);
		
		/**
		@PageName fov_init
		@PageFather fov
		@FuncTitle Defining the cell properties
		@FuncDesc Then, build your dungeon by defining which cells let the light pass (by default, all cells block the light) and which cells are walkable (by default, all cells are not-walkable).
		@Cpp void TCODMap::setProperties (int x, int y, bool isTransparent, bool isWalkable)
		@C void TCOD_map_set_properties (TCOD_map_t map, int x, int y, bool is_transparent, bool is_walkable)
		@Py map_set_properties (map, x, y, is_transparent, is_walkable)
		@C# void TCODMap::setProperties (int x, int y, bool isTransparent, bool isWalkable)
		@Param map	In the C version, the map handler returned by the TCOD_map_new function.
		@Param x, y	Coordinate of the cell that we want to update.
		@Param isTransparent	If true, this cell will let the light pass else it will block the light.
		@Param isWalkable	If true, creatures can walk true this cell (it is not a wall).
		*/		
		void setProperties(int x,int y, bool isTransparent, bool isWalkable);
		
		/**
		@PageName fov_init
		@PageFather fov
		@FuncTitle Clearing the map
		@FuncDesc You can clear an existing map (setting all cells to the chosen walkable/transparent values) with:
		@Cpp void TCODMap::clear (bool transparent = false, bool walkable = false)
		@C void TCOD_map_clear (TCOD_map_t map, bool transparent, bool walkable)
		@Py map_clear (map, transparent = False, walkable = False)
		@C#
			void TCODMap::clear()
			void TCODMap::clear(bool transparent)
			void TCODMap::clear(bool transparent, bool walkable)
		@Param map	In the C version, the map handler returned by the TCOD_map_new function.
		@Param walkable	Whether the cells should be walkable.
		@Param transparent	Whether the cells should be transparent.
		*/		
		void clear(bool transparent=false, bool walkable=false);
		
		/**
		@PageName fov_init
		@PageFather fov
		@FuncTitle Copying a map
		@FuncDesc You can copy an existing map into another. You have to allocate the destination map first.
		@Cpp void TCODMap::copy (const TCODMap * source)
		@C void TCOD_map_copy (TCOD_map_t source, TCOD_map_t dest)
		@Py map_copy (source, dest)
		@C# void TCODMap::copy (TCODMap source)
		@Param source	The map containing the source data.
		@Param dest	In C and python version, the map where data is copied.
		@CppEx 
			TCODMap * map = new TCODMap(50,50); // allocate the map
			map->setProperties(10,10,true,true); // set a cell as 'empty'
			TCODMap * map2 = new TCODMap(10,10); // allocate another map
			map2->copy(map); // copy map data into map2, reallocating it to 50x50
		@CEx 
			TCOD_map_t map = TCOD_map_new(50,50);
			TCOD_map_t map2 = TCOD_map_new(10,10);
			TCOD_map_set_properties(map,10,10,true,true);
			TCOD_map_copy(map,map2);
		@PyEx 
			map = libtcod.map_new(50,50)
			map2 = libtcod.map_new(10,10)
			libtcod.map_set_properties(map,10,10,True,True)
			libtcod.map_copy(map,map2)
		*/		
		void copy (const TCODMap *source);

		/**
		@PageName fov_compute
		@PageTitle Computing the field of view
		@PageFather fov
		@FuncDesc Once your map is allocated and empty cells have been defined, you can calculate the field of view with :
			<div class="code"><pre>typedef enum { FOV_BASIC, 
               FOV_DIAMOND, 
               FOV_SHADOW, 
               FOV_PERMISSIVE_0,FOV_PERMISSIVE_1,FOV_PERMISSIVE_2,FOV_PERMISSIVE_3,
               FOV_PERMISSIVE_4,FOV_PERMISSIVE_5,FOV_PERMISSIVE_6,FOV_PERMISSIVE_7,FOV_PERMISSIVE_8, 
               FOV_RESTRICTIVE,
               NB_FOV_ALGORITHMS } TCOD_fov_algorithm_t;
            </pre></div>
			* FOV_BASIC : classic libtcod fov algorithm (ray casted from the player to all the cells on the submap perimeter)
			* FOV_DIAMOND : based on <a href="http://www.geocities.com/temerra/los_rays.html">this</a> algorithm
			* FOV_SHADOW : based on <a href="http://roguebasin.roguelikedevelopment.org/index.php?title=FOV_using_recursive_shadowcasting">this</a> algorithm
			* FOV_PERMISSIVE_x : based on <a href="http://roguebasin.roguelikedevelopment.org/index.php?title=Precise_Permissive_Field_of_View">this</a> algorithm
			Permissive has a variable permissiveness parameter. You can either use the constants FOV_PERMISSIVE_x, x between 0 (the less permissive) and 8 (the more permissive), or using the macro FOV_PERMISSIVE(x). 
			* FOV_RESTRICTIVE : Mingos' Restrictive Precise Angle Shadowcasting (MRPAS). Original implementation <a href="http://umbrarumregnum.110mb.com/download/mrpas">here</a>. Comparison of the algorithms :
			Check <a href="http://roguecentral.org/libtcod/fov/fov.pdf">this</a>.
		@Cpp void TCODMap::computeFov(int playerX,int playerY, int maxRadius=0,bool light_walls = true, TCOD_fov_algorithm_t algo = FOV_BASIC)
		@C void TCOD_map_compute_fov(TCOD_map_t map, int player_x, int player_y, int max_radius, bool light_walls, TCOD_fov_algorithm_t algo)
		@Py map_compute_fov(map, player_x, player_y, max_radius=0, light_walls=True, algo=FOV_BASIC )
		@C# 
			void TCODMap::computeFov(int playerX, int playerY)
			void TCODMap::computeFov(int playerX, int playerY, int maxRadius)
			void TCODMap::computeFov(int playerX, int playerY, int maxRadius,bool light_walls)
			void TCODMap::computeFov(int playerX, int playerY, int maxRadius,bool light_walls, TCODFOVTypes algo)
		@Param map	In the C version, the map handler returned by the TCOD_map_new function.
		@Param player_x,player_y	Position of the player in the map.
			0 <= player_x < map width.
			0 <= player_y < map height.
		@Param maxRadius	If > 0, the fov is only computed up to maxRadius cells away from the player. Else, the range is unlimited.
		@Param light_walls	Wether the wall cells near ground cells in fov must be in fov too.
		@Param algo	FOV algorithm to use.
		@CppEx 
			TCODMap *map = new TCODMap(50,50); // allocate the map
			map->setProperties(10,10,true,true); // set a cell as 'empty'
			map->computeFov(10,10); // calculate fov from the cell 10x10 (basic raycasting, unlimited range, walls lighting on)
		@CEx 
			TCOD_map_t map = TCOD_map_new(50,50);
			TCOD_map_set_properties(map,10,10,true,true);
			TCOD_map_compute_fov(map,10,10,0,true,FOV_SHADOW); // using shadow casting
		@PyEx 
			map = libtcod.map_new(50,50)
			libtcod.map_set_properties(map,10,10,True,True)
			libtcod.map_compute_fov(map,10,10,0,True,libtcod.FOV_PERMISSIVE(2)) 
		*/
		void computeFov(int playerX,int playerY, int maxRadius = 0,bool light_walls = true, TCOD_fov_algorithm_t algo = FOV_BASIC);

		/**
		@PageName fov_get
		@PageFather fov
		@PageTitle Reading fov information
		@FuncTitle Checking if a cell is in fov
		@FuncDesc Once your computed the field of view, you can know if a cell is visible with :
		@Cpp bool TCODMap::isInFov(int x, int y) const
		@C bool TCOD_map_is_in_fov(TCOD_map_t map, int x, int y)
		@Py map_is_in_fov(map, x, y)
		@C# bool TCODMap::isInFov(int x, int y)
		@Param map	In the C version, the map handler returned by the TCOD_map_new function.
		@Param x,y	Coordinates of the cell we want to check.
			0 <= x < map width.
			0 <= y < map height.
		@CppEx 
			TCODMap *map = new TCODMap(50,50); // allocate the map
			map->setProperties(10,10,true,true); // set a cell as 'empty'
			map->computeFov(10,10); // calculate fov from the cell 10x10
			bool visible=map->isInFov(10,10); // is the cell 10x10 visible ?      
		@CEx 
			TCOD_map_t map = TCOD_map_new(50,50);
			TCOD_map_set_properties(map,10,10,true,true);
			TCOD_map_compute_fov(map,10,10);
			bool visible = TCOD_map_is_in_fov(map,10,10);
		@PyEx 
			map = libtcod.map_new(50,50)
			libtcod.map_set_properties(map,10,10,True,True)
			libtcod.map_compute_fov(map,10,10)
			visible = libtcod.map_is_in_fov(map,10,10)
		*/		
   		bool isInFov(int x,int y) const;
   		/**
   		@PageName fov_get
   		@FuncTitle Checking a cell transparency/walkability
   		@FuncDesc You can also retrieve transparent/walkable informations with :
		@Cpp 
			bool TCODMap::isTransparent(int x, int y) const
			bool TCODMap::isWalkable(int x, int y) const
		@C 
			bool TCOD_map_is_transparent(TCOD_map_t map, int x, int y)
			bool TCOD_map_is_walkable(TCOD_map_t map, int x, int y)
		@Py 
			map_is_transparent(map, x, y)
			map_is_walkable(map, x, y)
		@C# 
			bool TCODMap::isTransparent(int x, int y)
			bool TCODMap::isWalkable(int x, int y)
		@Param map	In the C version, the map handler returned by the TCOD_map_new function.
		@Param x,y	Coordinates of the cell we want to check.
			0 <= x < map width.
			0 <= y < map height.
		*/   		
		bool isTransparent(int x, int y) const;
		bool isWalkable(int x, int y) const;

   		/**
   		@PageName fov_get
   		@FuncTitle Getting the map size
   		@FuncDesc You can retrieve the map size with :
		@Cpp 
			int TCODMap::getWidth() const
			int TCODMap::getHeight() const
		@C 
			int TCOD_map_get_width(TCOD_map_t map)
			int TCOD_map_get_height(TCOD_map_t map)
		@Py 
			map_get_width(map)
			map_get_height(map)
		@C#
			int TCODMap::getWidth()
			int TCODMap::getHeight()
		@Param map	In the C version, the map handler returned by the TCOD_map_new function.
		*/   		
   		int getWidth() const;
		int getHeight() const;

		virtual ~TCODMap();
		void setInFov(int x,int y, bool fov);
		int getNbCells() const;
		friend class TCODLIB_API TCODPath;
		friend class TCODLIB_API TCODDijkstra;
//	protected :
		TCOD_map_t data;
};

#endif
