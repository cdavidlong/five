#ifndef _object_h_
#define _object_h_

#include "s_c_list.h"

class Object{

public:

	float xPos,yPos,zPos;
	float object , size;
	bool anchored;
	bool held;	
	bool visible;
	bool remove;
	int selected;


	Single_Contact_List* C_Points;

	Object();

	virtual void display() = 0;
	virtual void display_c_points() = 0;

};

#endif