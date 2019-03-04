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
#ifndef _TCOD_PATH_HPP
#define _TCOD_PATH_HPP

#include "fov.hpp"
#include "path.h"

class TCODLIB_API ITCODPathCallback {
public :
	virtual ~ITCODPathCallback() {}
	virtual float getWalkCost( int xFrom, int yFrom, int xTo, int yTo, void *userData ) const = 0;
};

/**
 @PageName path
 @PageTitle Path finding
 @PageCategory Roguelike toolkits
 @PageDesc This toolkit allows one to easily calculate the optimal path between two points in your dungeon by using either the <a href="http://en.wikipedia.org/wiki/A*">A* algorithm</a> or <a href="http://en.wikipedia.org/wiki/Dijkstra%27s_algorithm">Dijkstra's algorithm</a>.
Please note that the paths generated with the two algorithms may differ slightly. Due to how they're implemented, A* will usually prefer diagonal moves over orthogonal, while Dijkstra will have the opposite preference. In other words, paths from point X to point Y will look like this:
<div class="code"><pre>
Dijkstra:      A*:
..........   ..........
.X........   .X*.......
..*.......   ...**.....
...*......   .....**...
....****Y.   .......*Y.
..........   ..........
</pre></div>
 */
class TCODLIB_API TCODPath {
public :
	/**
	@PageName path_init
	@PageFather path
	@PageTitle Creating a path
	@FuncTitle Allocating a pathfinder from a map
	@FuncDesc First, you have to allocate a path using a map from <a href="fov.html">the Field of view module</a>.
	@Cpp
		TCODPath::TCODPath(const TCODMap *map, float diagonalCost=1.41f)
		TCODDijkstra::TCODDijkstra(const TCODMap *map, float diagonalCost=1.41f)
	@C
		TCOD_path_t TCOD_path_new_using_map(TCOD_map_t map, float diagonalCost)
		TCOD_dijkstra_t TCOD_dijkstra_new(TCOD_map_t map, float diagonalCost)
	@Py
		path_new_using_map(map, diagonalCost=1.41)
		dijkstra_new(map, diagonalCost=1.41)
	@C#
		TCODPath(TCODMap map, float diagonalCost)
		TCODPath(TCODMap map)
		TCODDijkstra(TCODMap map, float diagonalCost)
		TCODDijkstra(TCODMap map)
	@Param map	The map. The path finder will use the 'walkable' property of the cells to find a path.
	@Param diagonalCost	Cost of a diagonal movement compared to an horizontal or vertical movement. On a standard cartesian map, it should be sqrt(2) (1.41f).
		It you want the same cost for all movements, use 1.0f.
		If you don't want the path finder to use diagonal movements, use 0.0f.
	@CppEx
		// A* :
		TCODMap *myMap = new TCODMap(50,50);
		TCODPath *path = new TCODPath(myMap); // allocate the path
		// Dijkstra:
		TCODMap *myMap = new TCODMap(50,50);
		TCODDijkstra *dijkstra = new TCODDijkstra(myMap); // allocate the path
	@CEx
		// A* :
		TCOD_map_t my_map=TCOD_map_new(50,50,true);
		TCOD_path_t path = TCOD_path_new_using_map(my_map,1.41f);
		// Dijkstra :
		TCOD_map_t my_map=TCOD_map_new(50,50,true);
		TCOD_dijkstra_t dijk = TCOD_dijkstra_new(my_map,1.41f);
	@PyEx
		# A* :
		my_map=libtcod.map_new(50,50,True)
		path = libtcod.path_new_using_map(my_map)
		# Dijkstra
		my_map=libtcod.map_new(50,50,True)
		dijk = libtcod.dijkstra_new(my_map)
	*/
	TCODPath(const TCODMap *map, float diagonalCost=1.41f);
	/**
	@PageName path_init
	@FuncTitle Allocating a pathfinder using a callback
	@FuncDesc Since the walkable status of a cell may depend on a lot of parameters (the creature type, the weather, the terrain type...), you can also create a path by providing a function rather than relying on a TCODMap.
	@Cpp
		// Callback :
		class ITCODPathCallback {
			public: virtual float getWalkCost( int xFrom, int yFrom, int xTo, int yTo, void *userData ) const = 0;
		};
		// A* constructor:
		TCODPath::TCODPath(int width, int height, const ITCODPathCallback *callback, void *userData, float diagonalCost=1.41f)
		// Dijkstra constructor
		TCODDijkstra::TCODDijkstra(int width, int height, const ITCODPathCallback *callback, void *userData, float diagonalCost=1.41f)
	@C
		typedef float (*TCOD_path_func_t)( int xFrom, int yFrom, int xTo, int yTo, void *user_data )
		TCOD_path_t TCOD_path_new_using_function(int width, int height, TCOD_path_func_t callback, void *user_data, float diagonalCost)
		TCOD_dijkstra_t TCOD_dijkstra_new_using_function(int width, int height, TCOD_path_func_t callback, void *user_data, float diagonalCost)
	@Py
		def path_func(xFrom,yFrom,xTo,yTo,userData) : ...
		path_new_using_function(width, height, path_func, user_data=0, diagonalCost=1.41)
		dijkstra_new_using_function(width, height, path_func, user_data=0, diagonalCost=1.41)
	@C#
		TCODPath(int width, int height, ITCODPathCallback listener, float diagonalCost)
		TCODPath(int width, int height, ITCODPathCallback listener)
		TCODDijkstra(int width, int height, ITCODPathCallback listener, float diagonalCost)
		TCODDijkstra(int width, int height, ITCODPathCallback listener)
	@Param width,height	The size of the map (in map cells).
	@Param callback	A custom function that must return the walk cost from coordinates xFrom,yFrom to coordinates xTo,yTo.
		The cost must be > 0.0f if the cell xTo,yTo is walkable.
		It must be equal to 0.0f if it's not.
		You must not take additional cost due to diagonal movements into account as it's already done by the pathfinder.
	@Param userData	Custom data that will be passed to the function.
	@Param diagonalCost	Cost of a diagonal movement compared to an horizontal or vertical movement. On a standard cartesian map, it should be sqrt(2) (1.41f).
		It you want the same cost for all movements, use 1.0f.
		If you don't want the path finder to use diagonal movements, use 0.0f.
	@CppEx
		class MyCallback : public ITCODPathCallback {
		public :
			float getWalkCost(int xFrom, int yFrom, int xTo, int yTo, void *userData ) const { ... }
		};
		TCODPath *path = new TCODPath(50,50,new MyCallback(),NULL); // allocate the path
		TCODDijkstra *dijkstra = new TCODDijkstra(50,50,new MyCallback(),NULL); // allocate Dijkstra
	@CEx
		float my_func(int xFrom, int yFrom, int xTo, int yTo, void *user_data) { ... }
		TCOD_path_t path = TCOD_path_new_using_function(50,50,my_func,NULL,1.41f);
		TCOD_dijkstra_t dijkstra = TCOD_dijkstra_new_using_function(50,50,my_func,NULL,1.41f);
	@PyEx
		def my_func(xFrom, yFrom, xTo, yTo, user_data) :
			# return a float cost for this movement
			return 1.0
		path = libtcod.path_new_using_function(50,50,my_func)
		dijkstra = libtcod.dijkstra_new_using_function(50,50,my_func)
	*/
	TCODPath(int width, int height, const ITCODPathCallback *listener, void *userData, float diagonalCost=1.41f);

	/**
	@PageName path_init
	@FuncTitle Destroying a path
	@FuncDesc To release the resources used by a path, destroy it with :
	@Cpp
		TCODPath::~TCODPath()
		TCODDijkstra::~TCODDijkstra()
	@C
		void TCOD_path_delete(TCOD_path_t path)
		void TCOD_dijkstra_delete(TCOD_dijkstra_t dijkstra)
	@Py
		path_delete(path)
		dijkstra_delete(dijkstra)
	@C#
		void TCODPath::Dispose()
		void TCODDijkstra::Dispose()
	@Param path	In the C version, the path handler returned by one of the TCOD_path_new_* function.
	@Param dijkstra	In the C version, the path handler returned by one of the TCOD_dijkstra_new* function.
	@CppEx
		TCODPath *path = new TCODPath(myMap); // allocate the path
		// use the path...
		delete path; // destroy the path

		TCODDijkstra *dijkstra = new TCODDijkstra(myMap); // allocate the path
		// use the path...
		delete dijkstra; // destroy the path
	@CEx
		TCOD_path_t path = TCOD_path_new_using_map(my_map);
		// use the path ...
		TCOD_path_delete(path);

		TCOD_dijkstra_t dijkstra = TCOD_dijkstra_new(my_map);
		// use the path ...
		TCOD_dijkstra_delete(dijkstra);
	@PyEx
		path = libtcod.path_new_using_map(my_map)
		# use the path ...
		libtcod.path_delete(path)

		dijkstra = libtcod.dijkstra_new(my_map)
		# use the path ...
		libtcod.dijkstra_delete(dijkstra)
	*/
	virtual ~TCODPath();

	/**
	@PageName path_compute
	@PageFather path
	@PageTitle Computing the path
	@FuncTitle Computing an A* path
	@FuncDesc Once you created a TCODPath object, you can compute the path between two points:
	@Cpp bool TCODPath::compute(int ox, int oy, int dx, int dy)
	@C bool TCOD_path_compute(TCOD_path_t path, int ox,int oy, int dx, int dy)
	@Py path_compute(path, ox, oy, dx, dy)
	@C#	void TCODPath::compute(int ox, int oy, int dx, int dy)
	@Param path	In the C version, the path handler returned by a creation function.
	@Param ox,oy	Coordinates of the origin of the path.
	@Param dx,dy	Coordinates of the destination of the path.
		Both points should be inside the map, and at a walkable position. The function returns false if there is no possible path.
	@CppEx
		TCODMap *myMap = new TCODMap(50,50);
		TCODPath *path = new TCODPath(myMap); // allocate the path
		path->compute(5,5,25,25); // calculate path from 5,5 to 25,25
	@CEx
		TCOD_map_t my_map=TCOD_map_new(50,50);
		TCOD_path_t path = TCOD_path_new_using_map(my_map);
		TCOD_path_compute(path,5,5,25,25);
	@PyEx
		my_map=libtcod.map_new(50,50)
		path = libtcod.path_new_using_map(my_map)
		libtcod.path_compute(path,5,5,25,25)
	*/
	bool compute(int ox, int oy, int dx, int dy);

	/**
	@PageName path_compute
	@FuncTitle Reversing a path
	@FuncDesc Once you computed a path, you can exchange origin and destination :
	@Cpp
		void TCODPath::reverse()
		void TCODDijkstra::reverse()
	@C
		void TCOD_path_reverse(TCOD_path_t path)
		void TCOD_dijkstra_reverse(TCOD_dijkstra_t dijkstra)
	@Py
		path_reverse(path)
		dijkstra_reverse(dijkstra)
	@C#
		void TCODPath::reverse()
		void TCODDijkstra::reverse()
	@Param path	In the C version, the path handler returned by a creation function.
	@CppEx
		TCODMap *myMap = new TCODMap(50,50);
		TCODPath *path = new TCODPath(myMap); // allocate the path
		path->compute(5,5,25,25); // calculate path from 5,5 to 25,25
		path->reverse(); // now the path goes from 25,25 to 5,5
	@CEx
		TCOD_map_t my_map=TCOD_map_new(50,50);
		TCOD_path_t path = TCOD_path_new_using_map(my_map);
		TCOD_path_compute(path,5,5,25,25); // calculate path from 5,5 to 25,25
		TCOD_path_reverse(path); // now the path goes from 25,25 to 5,5
	@PyEx
		my_map=libtcod.map_new(50,50)
		path = libtcod.path_new_using_map(my_map)
		libtcod.path_compute(path,5,5,25,25) # calculate path from 5,5 to 25,25
		libtcod.path_reverse(path) # now the path goes from 25,25 to 5,5
	*/
	void reverse();


	/**
	@PageName path_read
	@PageTitle Reading path information
	@PageFather path
	@PageDescDesc Once the path has been computed, you can get information about it using of one those functions.
	@FuncTitle Getting the path origin and destination
	@FuncDesc
		You can read the current origin and destination cells with getOrigin/getDestination.
		Note that when you walk the path, the origin changes at each step.
	@Cpp
		void TCODPath::getOrigin(int *x,int *y) const
		void TCODPath::getDestination(int *x,int *y) const
	@C
		void TCOD_path_get_origin(TCOD_path_t path, int *x, int *y)
		void TCOD_path_get_destination(TCOD_path_t path, int *x, int *y)
	@Py
		path_get_origin(path) # returns x,y
		path_get_destination(path) # returns x,y
	@C#
		void TCODPath::getOrigin(out int x, out int y)
		void TCODPath::getDestination(out int x, out int y)
	@Param path	In the C version, the path handler returned by a creation function.
	@Param x,y	The function returns the cell coordinates in these variables
	*/
	void getOrigin(int *x,int *y) const;
	void getDestination(int *x,int *y) const;

	/**
	@PageName path_read
	@FuncTitle Getting the path length
	@FuncDesc You can get the number of steps needed to reach destination :
	@Cpp
		int TCODPath::size() const
		int TCODDijkstra::size() const
	@C
		int TCOD_path_size(TCOD_path_t path)
		int TCOD_dijkstra_size(TCOD_dijkstra_t dijkstra)
	@Py
		path_size(path)
		dijkstra_size(dijkstra)
	@C#
		int TCODPath::size()
		int TCODDijkstra::size()
	@Param path, dijkstra	In the C version, the path handler returned by a creation function.
	*/
	int size() const;

	/**
	@PageName path_read
	@FuncTitle Read the path cells' coordinates
	@FuncDesc You can get the coordinates of each point along the path :
	@Cpp
		void TCODPath::get(int index, int *x, int *y) const
		void TCODDijkstra::get(int index, int *x, int *y) const
	@C
		void TCOD_path_get(TCOD_path_t path, int index, int *x, int *y)
		void TCOD_dijkstra_get(TCOD_dijkstra_t dijkstra, int index, int *x, int *y)
	@Py
		path_get(path, index) # returns x,y
		dijkstra_get(dijkstra, index) # returns x,y
	@C#
		int TCODPath::size()
		int TCODDijkstra::size()
	@Param path, dijkstra	In the C version, the path handler returned by a creation function.
	@Param index	Step number.
		0 <= index < path size
	@Param x,y	Address of the variables receiving the coordinates of the point.
	@CppEx
		for (int i=0; i < path->size(); i++ ) {
			int x,y;
			path->get(i,&x,&y);
			printf ("Astar coord : %d %d\n", x,y );
		}
		for (int i=0; i < dijkstra->size(); i++ ) {
			int x,y;
			dijkstra->get(i,&x,&y);
			printf ("Dijkstra coord : %d %d\n", x,y );
		}
	@CEx
		int i;
		for (i=0; i < TCOD_path_size(path); i++ ) {
			int x,y;
			TCOD_path_get(path,i,&x,&y);
			printf ("Astar coord : %d %d\n", x,y );
		}
		for (i=0; i < TCOD_dijkstra_size(dijkstra); i++ ) {
			int x,y;
			TCOD_dijkstra_get(dijkstra,i,&x,&y);
			printf ("Dijsktra coord : %d %d\n", x,y );
		}
	@PyEx
		for i in range (libtcod.path_size(path)) :
			x,y=libtcod.path_get(path,i)
			print 'Astar coord : ',x,y
		for i in range (libtcod.dijkstra_size(dijkstra)) :
			x,y=libtcod.dijkstra_get(dijkstra,i)
			print 'Dijkstra coord : ',x,y
	*/
	void get(int index, int *x, int *y) const;

	/**
	@PageName path_read
	@FuncTitle Checking if the path is empty
	@FuncDesc If you want a creature to follow the path, a more convenient way is to walk the path :
		You know when you reached destination when the path is empty :
	@Cpp
		bool TCODPath::isEmpty() const
		bool TCODDijkstra::isEmpty() const
	@C
		bool TCOD_path_is_empty(TCOD_path_t path)
		bool TCOD_dijkstra_is_empty(TCOD_dijkstra_t dijkstra)
	@Py
		path_is_empty(path)
		dijkstra_is_empty(dijkstra)
	@C#
		bool TCODPath::isEmpty()
		bool TCODDijkstra::isEmpty()
	@Param path, dijkstra	In the C version, the path handler returned by a creation function.
	*/
	bool isEmpty() const;

	/**
	@PageName path_read
	@FuncTitle Walking the path
	@FuncDesc You can walk the path and go to the next step with :
		Note that walking the path consume one step (and decrease the path size by one). The function returns false if recalculateWhenNeeded is false and the next cell on the path is no longer walkable, or if recalculateWhenNeeded is true, the next cell on the path is no longer walkable and no other path has been found. Also note that recalculateWhenNeeded only applies to A*.
	@Cpp
		bool TCODPath::walk(int *x, int *y, bool recalculateWhenNeeded)
		bool TCODDijkstra::walk(int *x, int *y)
	@C
		bool TCOD_path_walk(TCOD_path_t path, int *x, int *y, bool recalculate_when_needed)
		bool TCOD_dijkstra_walk(TCOD_dijkstra_t dijkstra, int *x, int *y)
	@Py
		path_walk(TCOD_path_t path, recalculate_when_needed) # returns x,y or None,None if no path
		dijkstra_walk(TCOD_dijkstra_t dijkstra)
	@C#
		bool TCODPath::walk(ref int x, ref int y, bool recalculateWhenNeeded)
		bool TCODDijkstra::walk(ref int x, ref int y)
	@Param path, dijkstra	In the C version, the path handler returned by a creation function.
	@Param x,y	Address of the variables receiving the coordinates of the next point.
	@Param recalculateWhenNeeded	If the next point is no longer walkable (another creature may be in the way), recalculate a new path and walk it.
	@CppEx
		while (! path->isEmpty()) {
			int x,y;
			if (path->walk(&x,&y,true)) {
				printf ("Astar coord: %d %d\n",x,y );
			} else {
				printf ("I'm stuck!\n" );
				break;
			}
		}
		while (! dijkstra->isEmpty()) {
			int x,y;
			if (dijkstra->walk(&x,&y)) {
				printf ("Dijkstra coord: %d %d\n",x,y );
			} else {
				printf ("I'm stuck!\n" );
				break;
			}
		}
	@CEx
		while (! TCOD_path_is_empty(path)) {
			int x,y;
			if (TCOD_path_walk(path,&x,&y,true)) {
				printf ("Astar coord: %d %d\n",x,y );
			} else {
				printf ("I'm stuck!\n" );
				break;
			}
		}
		while (! TCOD_dijkstra_is_empty(dijkstra)) {
			int x,y;
			if (TCOD_dijkstra_walk(dijkstra,&x,&y)) {
				printf ("Dijkstra coord: %d %d\n",x,y );
			} else {
				printf ("I'm stuck!\n" );
				break;
			}
		}
	@PyEx
		while not libtcod.path_is_empty(path)) :
			x,y=libtcod.path_walk(path,True)
			if not x is None :
				print 'Astar coord: ',x,y
			else :
				print "I'm stuck!"
				break
		while not libtcod.dijkstra_is_empty(dijkstra)) :
			x,y=libtcod.dijkstra_walk(dijkstra,True)
			if not x is None :
				print 'Dijkstra coord: ',x,y
			else :
				print "I'm stuck!"
				break
	*/
	bool walk(int *x, int *y, bool recalculateWhenNeeded);

protected :
	friend float TCOD_path_func(int xFrom, int yFrom, int xTo,int yTo, void *data);
	TCOD_path_t data;
	struct WrapperData {
		void *userData;
		const ITCODPathCallback *listener;
	} cppData;
};

//Dijkstra kit
class TCODLIB_API TCODDijkstra {
    public:
        TCODDijkstra (TCODMap *map, float diagonalCost=1.41f);
        TCODDijkstra (int width, int height, const ITCODPathCallback *listener, void *userData, float diagonalCost=1.41f);
        ~TCODDijkstra (void);
        /**
        @PageName path_compute
		@FuncTitle Computing a Dijkstra grid
		@FuncDesc In case of Dijkstra, this works in a slightly different way. In order to be able to compute a path, Dijkstra must first analyse the distances from the selected root (origin) node to all other nodes:
		@Cpp void TCODDijkstra::compute(int rootX, int rootY)
		@C void TCOD_dijkstra_compute(TCOD_dijkstra_t dijkstra, int root_x, int root_y)
		@Py dijkstra_compute(dijkstra, root_x, root_y)
		@C# void TCODDijkstra::compute(int rootX, int rootY)
		@Param dijkstra	In the C version, the path handler returned by a creation function.
		@Param root_x,root_y	Coordinates of the root node (origin) of the path.
			The coordinates should be inside the map, at a walkable position. Otherwise, the function's behaviour will be undefined.
        */
        void compute (int rootX, int rootY);

        /**
        @PageName path_compute
		@FuncTitle Computing a path from a Dijkstra grid
		@FuncDesc After the map is analysed and all the distances from the root node are known, an unlimited number of paths can be set, all originating at the root node, using:
			The path setting function will return true if there's a path from the root node to the destination node. Otherwise, it will return false.
		@Cpp bool TCODDijkstra::setPath(int toX, int toY)
		@C bool TCOD_dijkstra_path_set(TCOD_dijkstra_t dijkstra, int to_x, int to_y)
		@Py dijkstra_path_set(dijkstra, to_x, to_y)
		@C# bool TCODDijkstra::setPath(int toX, int toY)
		@Param dijkstra	In the C version, the path handler returned by a creation function.
		@Param to_x,to_y	Coordinates of the destination node of the path.
		@CppEx
			TCODMap *myMap = new TCODMap(50,50);
			TCODDijkstra *dijkstra = new TCODDijkstra(myMap); // allocate the path
			dijkstra->compute(25,25); // calculate distance from 25,25 to all other nodes
			dijkstra->setPath(5,5); // calculate a path to node 5,5
			dijkstra->setPath(45,45); //calculate another path from the same origin
		@CEx
			TCOD_map_t my_map=TCOD_map_new(50,50);
			TCOD_dijkstra_t dijkstra = TCOD_dijkstra_new(my_map);
			TCOD_dijkstra_compute(dijkstra,25,25);
			TCOD_dijkstra_path_set(dijkstra,5,5);
			TCOD_dijkstra_path_set(dijkstra,45,45);
		@PyEx
			my_map=libtcod.map_new(50,50)
			dijkstra = libtcod.dijkstra_new(my_map)
			libtcod.dijkstra_compute(dijkstra,25,25)
			libtcod.dijkstra_path_set(dijkstra,5,5)
			libtcod.dijkstra_path_set(dijkstra,45,45)
        */
        bool setPath (int toX, int toY);

        /**
        @PageName path_read
		@FuncTitle Getting the distance from a cell to the root node
		@FuncDesc You can get the distance of any set of coordinates from the root node:
			Note that if the coordinates x,y are outside of the map or are a non-walkable position, the function will return -1.0f. This functionality is only available for Dijkstra's algorithm.
		@Cpp float TCODDijkstra::getDistance(int x, int y)
		@C float TCOD_dijkstra_get_distance(TCOD_dijkstra_t dijkstra, int x, int y)
		@Py dijkstra_get_distance(dijkstra, x, y)
		@C# float TCODDijkstra::getDistance(int x, int y)
		@Param dijkstra	In the C version, the path handler returned by a creation function.
		@Param x,y	The coordinates whose distance from the root node are to be checked
        */
        float getDistance (int x, int y);
        bool walk (int *x, int *y);
		bool isEmpty() const;
		void reverse();
		int size() const;
		void get(int index, int *x, int *y) const;
    private:
        TCOD_dijkstra_t data;
        struct WrapperData {
            void *userData;
            const ITCODPathCallback *listener;
        } cppData;
};

#endif
