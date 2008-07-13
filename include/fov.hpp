#ifndef _TCOD_FOV_HPP
#define _TCOD_FOV_HPP

class TCODLIB_API TCODMap {
	public :
		TCODMap(int width, int height);
		void clear();
		void setProperties(int x,int y, bool isTransparent, bool isWalkable);
		virtual ~TCODMap();
		void computeFov(int playerX,int playerY, int maxRadius=0);
		bool isInFov(int x,int y) const;
		bool isTransparent(int x, int y) const;
		bool isWalkable(int x, int y) const;
	protected :
		TCOD_map_t data;
};

#endif
