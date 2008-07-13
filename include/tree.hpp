#ifndef _TCOD_TREE_HPP
#define _TCOD_TREE_HPP

class TCODLIB_API TCODTree {
public :
	TCODTree *next;
	TCODTree *father;
	TCODTree *sons;

	TCODTree() : next(NULL),father(NULL),sons(NULL){}
	void addSon(TCODTree *data) {
		data->father=this;
		TCODTree *lastson = sons;
		while ( lastson && lastson->next ) lastson=lastson->next;
		if ( lastson ) {
			lastson->next=data;
		} else {
			sons=data;
		}
	}

};

#endif
