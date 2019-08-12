#ifndef _cube_h_
#define _cube_h_

#include "object.h" 

class cube:public Object{
	//Low and behold its about time, the CUBE CLASS is upon us!!!! Yeeehar!
	public:
	
	float zRot;
	float width;
	float colour[3];
	bool collision;
	Single_Contact_List* cb_points;

	//Temp variables for scanning through link list
	float c_sh_x, c_sh_y, c_sh_z;
	float c_rad;

		
	cube(float x, float y, float z, float w);
		
	void display_c_points();

	void display();
};

#endif 