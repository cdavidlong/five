#ifndef _hand_h_
#define _hand_h_

#include "s_o_list.h"

class hand{
	public:
	
	Single_Object_List* object_list;
	//Hand rotation and transformation values
	GLfloat xRot, xRaw, xTrans;
	GLfloat yTrans;
	GLfloat zRot, zRaw, zTrans;
	
	float xStep ,zStep; // speed at which hand moves...
	
	float handrad; 

	float index_finger;		//Finger flexure values
	float middle_finger;
	float ring_finger;
	float little_finger;
	float thumb_joint;

	//Temp variables for scanning through link list
	float c_sh_x, c_sh_y, c_sh_z;
	float c_rad;
	
	//Collision detection
	bool xPosMov;
	bool xNegMov;
	bool yPosMov; 
	bool yNegMov;
	bool zPosMov; 
	bool zNegMov; 
	bool finger_collision;
	bool palm_collision;

	bool fore_collision;
	bool aft_collision;

	bool holding;
	bool fist_made;

	// for location of touch sensors...
	int x,y,z;
	
	float index_angle;
	float middle_angle;
	float ring_angle;
	float little_angle;

	float j0_index[3];
	float j0_middle[3];
	float j0_ring[3];
	float j0_little[3];

	float j1_index[3];
	float j1_middle[3];
	float j1_ring[3];
	float j1_little[3];

	float j2_index[3];
	float j2_middle[3];
	float j2_ring[3];
	float j2_little[3];

	float j3_index[3];
	float j3_middle[3];
	float j3_ring[3];
	float j3_little[3];

	float j4_index[3];
	float j4_middle[3];
	float j4_ring[3];
	float j4_little[3];

	float s[8][3];
	
	// total angle palm is rotated.
	float palm_angle;
	
	// calculate angle of rotation =total finger rotation + palm rotation
	float total_angle;

	// new position in y and z direction.
	float temp_x, temp_y, temp_z;

	hand(Single_Object_List*);
	void display();
 	int gesture_check();
	void track_finger_sensors();
	
};

#endif