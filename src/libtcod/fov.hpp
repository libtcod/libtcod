/* BSD 3-Clause License
 *
 * Copyright © 2008-2023, Jice and the libtcod contributors.
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
// clang-format off
#pragma once
#ifndef TCOD_FOV_HPP_
#define TCOD_FOV_HPP_

#include <utility>

#include "fov.h"

class TCODPath;
/**
 @PageName fov
 @PageCategory Roguelike toolkits
 @PageTitle Field of view
 @PageDesc This toolkit allows one to easily calculate the potential visible set of map cells from the player position.
A cell is potentially visible if the line of sight from the player to the cell in unobstructed.
 */

class TCODFOV_PUBLIC TCODMap {
	public :
		/**
		@PageName fov_init
		@PageFather fov
		@PageTitle Building the map
		@FuncTitle Creating the map object
		@FuncDesc First, you have to allocate a map of the same size as your dungeon.
		@Cpp TCODMap::TCODMap (int width, int height)
		@C TCODFOV_Map* TCODFOV_map_new (int width, int height)
		@Py map_new (width, height)
		@C# TCODMap::TCODMap(int width, int height)
		@Param width, height	The size of the map (in map cells).
		*/
		TCODMap(int width, int height);

    TCODMap(const TCODMap&) = delete;
    TCODMap& operator=(const TCODMap&) = delete;
    TCODMap(TCODMap&& rhs) noexcept { std::swap(data, rhs.data); };
    TCODMap& operator=(TCODMap&& rhs) noexcept {
      std::swap(data, rhs.data);
      return *this;
    };

		/**
		@PageName fov_init
		@PageFather fov
		@FuncTitle Defining the cell properties
		@FuncDesc Then, build your dungeon by defining which cells let the light pass (by default, all cells block the light) and which cells are walkable (by default, all cells are not-walkable).
		@Cpp void TCODMap::setProperties (int x, int y, bool isTransparent, bool isWalkable)
		@C void TCODFOV_map_set_properties (TCODFOV_Map* map, int x, int y, bool is_transparent, bool is_walkable)
		@Py map_set_properties (map, x, y, is_transparent, is_walkable)
		@C# void TCODMap::setProperties (int x, int y, bool isTransparent, bool isWalkable)
		@Param map	In the C version, the map handler returned by the TCODFOV_map_new function.
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
		@C void TCODFOV_map_clear (TCODFOV_Map* map, bool transparent, bool walkable)
		@Py map_clear (map, transparent = False, walkable = False)
		@C#
			void TCODMap::clear()
			void TCODMap::clear(bool transparent)
			void TCODMap::clear(bool transparent, bool walkable)
		@Param map	In the C version, the map handler returned by the TCODFOV_map_new function.
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
		@C void TCODFOV_map_copy (TCODFOV_Map* source, TCODFOV_Map* dest)
		@Py map_copy (source, dest)
		@C# void TCODMap::copy (TCODMap source)
		@Param source	The map containing the source data.
		@Param dest	In C and Python version, the map where data is copied.
		@CppEx
			TCODMap * map = new TCODMap(50,50); // allocate the map
			map->setProperties(10,10,true,true); // set a cell as 'empty'
			TCODMap * map2 = new TCODMap(10,10); // allocate another map
			map2->copy(map); // copy map data into map2, reallocating it to 50x50
		@CEx
			TCODFOV_Map* map = TCODFOV_map_new(50,50);
			TCODFOV_Map* map2 = TCODFOV_map_new(10,10);
			TCODFOV_map_set_properties(map,10,10,true,true);
			TCODFOV_map_copy(map,map2);
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
			<div class="code"><pre>typedef enum { TCODFOV_BASIC,
               TCODFOV_DIAMOND,
               TCODFOV_SHADOW,
               TCODFOV_PERMISSIVE_0,TCODFOV_PERMISSIVE_1,TCODFOV_PERMISSIVE_2,TCODFOV_PERMISSIVE_3,
               TCODFOV_PERMISSIVE_4,TCODFOV_PERMISSIVE_5,TCODFOV_PERMISSIVE_6,TCODFOV_PERMISSIVE_7,TCODFOV_PERMISSIVE_8,
               TCODFOV_RESTRICTIVE,
               NB_FOV_ALGORITHMS } TCODFOV_fov_algorithm_t;
            </pre></div>
			* TCODFOV_BASIC : classic libtcod fov algorithm (ray casted from the player to all the cells on the submap perimeter)
			* TCODFOV_DIAMOND : based on <a href="http://www.geocities.com/temerra/los_rays.html">this</a> algorithm
			* TCODFOV_SHADOW : based on <a href="http://roguebasin.roguelikedevelopment.org/index.php?title=FOV_using_recursive_shadowcasting">this</a> algorithm
			* TCODFOV_PERMISSIVE_x : based on <a href="http://roguebasin.roguelikedevelopment.org/index.php?title=Precise_Permissive_Field_of_View">this</a> algorithm
			Permissive has a variable permissiveness parameter. You can either use the constants TCODFOV_PERMISSIVE_x, x between 0 (the less permissive) and 8 (the more permissive), or using the macro FOV_PERMISSIVE(x).
			* TCODFOV_RESTRICTIVE : Mingos' Restrictive Precise Angle Shadowcasting (MRPAS). Original implementation <a href="http://umbrarumregnum.110mb.com/download/mrpas">here</a>. Comparison of the algorithms :
			Check <a href="http://roguecentral.org/libtcod/fov/fov.pdf">this</a>.
		@Cpp void TCODMap::computeFov(int playerX,int playerY, int maxRadius=0,bool light_walls = true, TCODFOV_fov_algorithm_t algo = TCODFOV_BASIC)
		@C void TCODFOV_map_compute_fov(TCODFOV_Map* map, int player_x, int player_y, int max_radius, bool light_walls, TCODFOV_fov_algorithm_t algo)
		@Py map_compute_fov(map, player_x, player_y, max_radius=0, light_walls=True, algo=TCODFOV_BASIC )
		@C#
			void TCODMap::computeFov(int playerX, int playerY)
			void TCODMap::computeFov(int playerX, int playerY, int maxRadius)
			void TCODMap::computeFov(int playerX, int playerY, int maxRadius,bool light_walls)
			void TCODMap::computeFov(int playerX, int playerY, int maxRadius,bool light_walls, TCODFOVTypes algo)
		@Param map	In the C version, the map handler returned by the TCODFOV_map_new function.
		@Param player_x,player_y	Position of the player in the map.
			0 <= player_x < map width.
			0 <= player_y < map height.
		@Param maxRadius	If > 0, the fov is only computed up to maxRadius cells away from the player. Else, the range is unlimited.
		@Param light_walls	Whether the wall cells near ground cells in fov must be in fov too.
		@Param algo	FOV algorithm to use.
		@CppEx
			TCODMap *map = new TCODMap(50,50); // allocate the map
			map->setProperties(10,10,true,true); // set a cell as 'empty'
			map->computeFov(10,10); // calculate fov from the cell 10x10 (basic raycasting, unlimited range, walls lighting on)
		@CEx
			TCODFOV_Map* map = TCODFOV_map_new(50,50);
			TCODFOV_map_set_properties(map,10,10,true,true);
			TCODFOV_map_compute_fov(map,10,10,0,true,TCODFOV_SHADOW); // using shadow casting
		@PyEx
			map = libtcod.map_new(50,50)
			libtcod.map_set_properties(map,10,10,True,True)
			libtcod.map_compute_fov(map,10,10,0,True,libtcod.FOV_PERMISSIVE(2))
		*/
		void computeFov(int playerX,int playerY, int maxRadius = 0,bool light_walls = true, TCODFOV_fov_algorithm_t algo = TCODFOV_BASIC);

		/**
		@PageName fov_get
		@PageFather fov
		@PageTitle Reading fov information
		@FuncTitle Checking if a cell is in fov
		@FuncDesc Once your computed the field of view, you can know if a cell is visible with :
		@Cpp bool TCODMap::isInFov(int x, int y) const
		@C bool TCODFOV_map_is_in_fov(TCODFOV_Map* map, int x, int y)
		@Py map_is_in_fov(map, x, y)
		@C# bool TCODMap::isInFov(int x, int y)
		@Param map	In the C version, the map handler returned by the TCODFOV_map_new function.
		@Param x,y	Coordinates of the cell we want to check.
			0 <= x < map width.
			0 <= y < map height.
		@CppEx
			TCODMap *map = new TCODMap(50,50); // allocate the map
			map->setProperties(10,10,true,true); // set a cell as 'empty'
			map->computeFov(10,10); // calculate fov from the cell 10x10
			bool visible=map->isInFov(10,10); // is the cell 10x10 visible ?
		@CEx
			TCODFOV_Map* map = TCODFOV_map_new(50,50);
			TCODFOV_map_set_properties(map,10,10,true,true);
			TCODFOV_map_compute_fov(map,10,10);
			bool visible = TCODFOV_map_is_in_fov(map,10,10);
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
   		@FuncDesc You can also retrieve transparent/walkable information with :
		@Cpp
			bool TCODMap::isTransparent(int x, int y) const
			bool TCODMap::isWalkable(int x, int y) const
		@C
			bool TCODFOV_map_is_transparent(TCODFOV_Map* map, int x, int y)
			bool TCODFOV_map_is_walkable(TCODFOV_Map* map, int x, int y)
		@Py
			map_is_transparent(map, x, y)
			map_is_walkable(map, x, y)
		@C#
			bool TCODMap::isTransparent(int x, int y)
			bool TCODMap::isWalkable(int x, int y)
		@Param map	In the C version, the map handler returned by the TCODFOV_map_new function.
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
			int TCODFOV_map_get_width(TCODFOV_Map* map)
			int TCODFOV_map_get_height(TCODFOV_Map* map)
		@Py
			map_get_width(map)
			map_get_height(map)
		@C#
			int TCODMap::getWidth()
			int TCODMap::getHeight()
		@Param map	In the C version, the map handler returned by the TCODFOV_map_new function.
		*/
   		int getWidth() const;
		int getHeight() const;

		virtual ~TCODMap();
		void setInFov(int x,int y, bool fov);
		int getNbCells() const;
//	protected :
		TCODFOV_Map* data;
};

#endif // TCOD_FOV_HPP_
