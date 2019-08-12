#include <glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <math.h>
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <string.h>
#include <mmsystem.h>
#include <iostream.h>
#include <fstream.h>
#include "afont.h"

//Class inclusion
#include "variables.h"
#include "o_list.h"
#include "s_o_list.h"
#include "c_list.h"
#include "s_c_list.h"
#include "object.h"
extern "C++"{
#include "coms.h"
}

bool shoot = false;

Single_Object_List Object_List(" sdfsdf ");

class hand{
	public:
	
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

	bool reload;

	HANDLE inHandle;
	char* GunPtr;
	unsigned long waveSize, action;		


	hand(){
		handrad=6.0; 
		holding = false;
		reload = true;
		sound_init();
	}

	void display(){

		//Set collision variables
		finger_collision = false;
		palm_collision = false;
		xPosMov = true;
		xNegMov = true;
		yPosMov = true; 
		yNegMov = true;
		zPosMov = true; 
		zNegMov = true; 

		//Poll serial port for latest values of glove
		gloveData = GloveGetDataPtr(); //poll glove and fill array with current values
		little_finger = (float)gloveData[4];
		ring_finger = (float)gloveData[3];
		middle_finger = (float)gloveData[2]-30;
		index_finger = (float)gloveData[1];
		thumb_joint = (float)gloveData[0];

		xRaw = (float)gloveData[5];
		zRaw = (float)gloveData[6];
		
		track_finger_sensors(); //Update latest position of finger tips (sensors)

		xRot = ((float)gloveData[5]-130)/255*180;
		zRot = (((float)gloveData[6])-110)/255*120;

		// Check the Object List and see if we've hit anything.
		Object_List.n = Object_List.head->next;

		while ( Object_List.n->next != 0){ /// while there are objects, scan through them 
			Object_List.n = Object_List.n->next;

			Object_List.n->obj->C_Points->n = Object_List.n->obj->C_Points->head->next;

			fore_collision = false;
			aft_collision = false;
					
			while ( Object_List.n->obj->C_Points->n->next != NULL){ // while there are more contact points scan through them.
					
					Object_List.n->obj->C_Points->n = Object_List.n->obj->C_Points->n->next;

					c_sh_x = Object_List.n->obj->C_Points->n->x; //to hold x value of contact sphere etc
					c_sh_y = Object_List.n->obj->C_Points->n->y;
					c_sh_z = Object_List.n->obj->C_Points->n->z;
					c_rad = Object_List.n->obj->C_Points->n->radius;

					for (int sensor = 0;sensor <8;sensor+=2){
						
						finger_collision  = check_for_collision(c_sh_x,c_sh_y,c_sh_z,s[sensor][0],s[sensor][1],s[sensor][2],c_rad,0.75);
						if(finger_collision){
							fore_collision = true;
							Object_List.n->obj->selected = ++Current_Selected_Object;
				
						}

						palm_collision  = check_for_collision(c_sh_x,c_sh_y,c_sh_z,s[sensor+1][0],s[sensor+1][1],s[sensor+1][2],c_rad,0.75);
						if(palm_collision){
							aft_collision = true;
						}
						
						if (aft_collision && fore_collision){
							
							if(index_finger < 90 || middle_finger < 90 || ring_finger < 90 || little_finger < 90 ){
								Object_List.n->obj->held = true;
							}
					
							else
							Object_List.n->obj->held = false;
	
						}
						else 
							Object_List.n->obj->held = false;

				
					}

					if (aft_collision || fore_collision){

						if(Object_List.n->obj->anchored){
							
							if (Object_List.n->obj->xPos > xTrans)
								xPosMov = false;
							else 
								xNegMov = false;

							if (Object_List.n->obj->yPos > yTrans)
								yPosMov = false; 
							else 
								yNegMov = false;

							if (Object_List.n->obj->zPos > zTrans)
								zPosMov = false;
							else 
								zNegMov = false;

							
						}
					}
					
				/*	if(Object_List.n->obj->object == CRITTER && Object_List.n->obj->held){
						xTrans = Object_List.n->obj->xPos+13;
						centrex = xTrans;
						eyex =xTrans+30;
						yTrans = Object_List.n->obj->yPos+1;
						centrey = yTrans;
						eyey =xTrans+20;
						zTrans = Object_List.n->obj->zPos;
						centrez = zTrans;
						eyez = zTrans;
						glTranslatef(xTrans,yTrans,zTrans);
						glRotatef(90, 0.0, 1.0, 0.0);
						glTranslatef(-xTrans,-yTrans,-zTrans);
					} 	*/
				}
			}
		
	

		gesture_check();	//check for gesture 

		//Material and light settings for hand
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseMaterial);
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

		// palm  ...
		glPushMatrix();	

		glTranslatef(xTrans, yTrans, zTrans);

		//Spin hand around to face into screen
		glRotatef(90, 0.0, 1.0, 0.0);
		glRotatef(xRot, 1.0, 0.0, 0.0); 

		glColor3f(0.7996, 0.6549, 0.45); //Flesh tones
		glRotatef (zRot, 0.0, 0.0, 1.0);
		
		glPushMatrix();
			glScalef (3.0, 1.0, 5.5);
			glutWireCube (1.0);
		glPopMatrix();

		glBegin(GL_LINES);
			glVertex3f(-4.0, 0.5, 2.0);
			glVertex3f(-1.5, 0.5,2.75);
			glVertex3f(-4.0,-0.5, 2.0);
			glVertex3f(-1.5,-0.5,2.75);
			glVertex3f(-4.0,-0.5,-2.0);
			glVertex3f(-1.5,-0.5,-2.75);
			glVertex3f(-4.0, 0.5,-2.0);
			glVertex3f(-1.5, 0.5,-2.75);
		glEnd();

		glBegin(GL_LINE_LOOP);
			glVertex3f(-4.0, 0.5, 2.0);
			glVertex3f(-4.0,-0.5, 2.0);
			glVertex3f(-4.0,-0.5,-2.0);
			glVertex3f(-4.0, 0.5,-2.0);
		glEnd();

		//Thumb 
		glPushMatrix();
			glTranslatef (-1.5, 0, -2.75);
			glRotatef (-thumb_joint/255*40+70, 0.0, 1.0, 0.0);
			glTranslatef (1.5, 0, 2.75);
			glTranslatef (-0.5, 0,-2.25); 
			glPushMatrix();
				glScalef (2.0, 1.0, 1.0);
				glutWireCube (1.0);
			glPopMatrix();
		
			glTranslatef (0.5, 0,2.25); 
			
			glPushMatrix();

			glTranslatef (0.5, 0, -1.75);
			glRotatef (-thumb_joint/255*30, 0.0, 1.0, 0.0);
			
			glTranslatef (-0.5, 0, 1.75);
			glTranslatef (1.5, 0, -2.25);
		
			glPushMatrix();
				glScalef (2.0, 1.0, 1.0);
				glutSolidCube (1.0);
			glPopMatrix();
		
			glPopMatrix();
		
		glPopMatrix();
		
		glPushMatrix();

			/*Sensor Sphere
			glPushMatrix();
				glTranslatef (1.75, 0.0, 0.0);
				glutWireSphere(handrad, 10.0, 10.0);
			glPopMatrix();
			*/

			// index finger ...

			glPushMatrix();
				glTranslatef (2.0, 0.0, -2.25);
				glRotatef ((GLfloat)((-index_finger/255)*90), 0.0, 0.0, 1.0);
				glTranslatef (0.5, 0.0, 0.0);

				glPushMatrix();
					glScalef (2.0, 1.0, 1.0);
					glColor3f(0.8196, 0.6549, 0.498);
					glutSolidCube (1.0);		// First Section
				glPopMatrix();

				glTranslatef (1.5, 0.0, 0.0);
				glRotatef ((GLfloat)((-index_finger/255)*90), 0.0, 0.0, 1.0);
				glTranslatef (0.5, 0.0, 0.0);

				glPushMatrix();
					glScalef (2.0, 1.0, 1.0);
					glColor3f(1.0, 0.0, 0.0);
					glutSolidCube (1.0);		// Middle section
				glPopMatrix();

				glTranslatef (1.5, 0.0, 0.0);
				glRotatef ((GLfloat)((-index_finger/255)*90), 0.0, 0.0, 1.0);
				glTranslatef (0.25, 0.0, 0.0);

				glPushMatrix();
					glColor3f(0.0, 0.0, 1.0);
					glutSolidSphere(.75, 5, 5);	// Touch sensor
					glScalef (1.5, 1.0, 1.0);	// Finger tip
					//glutWireCube (1.0);	
				glPopMatrix();

			glPopMatrix();

			// middle finger ...

			glPushMatrix();
				glTranslatef (2.25, 0.0, -0.75);
				glRotatef ((GLfloat)((-middle_finger/255)*90), 0.0, 0.0, 1.0);
				glTranslatef (0.5, 0.0, 0.0);
				
				glPushMatrix();
					glScalef (2.5, 1.0, 1.0);
					glColor3f(0.8196, 0.6549, 0.498);
					glutSolidCube (1.0);		//First section
				glPopMatrix();
	
				glTranslatef (1.75, 0.0, 0.0);
				glRotatef ((GLfloat)((-middle_finger/255)*90), 0.0, 0.0, 1.0);
				glTranslatef (0.5, 0.0, 0.0);
				
				glPushMatrix();
					glScalef (2.0, 1.0, 1.0);
					glColor3f(1.0, 0.0, 0.0);	//Second section
					glutSolidCube (1.0);
				glPopMatrix();

				glTranslatef (1.5, 0.0, 0.0);
				glRotatef ((GLfloat)((-middle_finger/255)*90), 0.0, 0.0, 1.0);
				glTranslatef (0.25, 0.0, 0.0);

				glPushMatrix();
					glColor3f(0.0, 0.0, 1.0);
					glutSolidSphere(.75, 5, 5);	// Touch sensor
					glScalef (1.5, 1.0, 1.0);
					//glutWireCube (1.0);			//Finger Tip
				glPopMatrix();

			glPopMatrix();

			// ring finger ...

			glPushMatrix();
				glTranslatef (2.0, 0.0, 0.75);
				glRotatef ((GLfloat)((-ring_finger/255)*90), 0.0, 0.0, 1.0);
				glTranslatef (0.5, 0.0, 0.0);
	
				glPushMatrix();
					glScalef (2.0, 1.0, 1.0);
					glColor3f(0.8196, 0.6549, 0.498);
					glutSolidCube (1.0);		//First Section
				glPopMatrix();

				glTranslatef (1.5, 0.0, 0.0);
				glRotatef ((GLfloat)((-ring_finger/255)*90), 0.0, 0.0, 1.0);
				glTranslatef (0.5, 0.0, 0.0);

				glPushMatrix();
					glScalef (2.0, 1.0, 1.0);
					glColor3f(1.0, 0.0, 0.0);
					glutSolidCube (1.0);		//Second Section
				glPopMatrix();

				glTranslatef (1.5, 0.0, 0.0);
				glRotatef ((GLfloat)((-ring_finger/255)*90), 0.0, 0.0, 1.0);
				glTranslatef (0.25, 0.0, 0.0);

				glPushMatrix();
					glColor3f(0.0, 0.0, 1.0);
					glutSolidSphere(.75, 5, 5);	// Touch sensor
					glScalef (1.5, 1.0, 1.0);
					//glutWireCube (1.0);			//Finger Tip
				glPopMatrix();

			glPopMatrix();

			// little finger ...
	
			glPushMatrix();
				glTranslatef (1.75, 0.0, 2.25);
				glRotatef ((GLfloat)((-little_finger/255)*90), 0.0, 0.0, 1.0);
				glTranslatef (0.5, 0.0, 0.0);
				
				glPushMatrix();
					glScalef (1.5, 1.0, 1.0);
					glColor3f(1.0, 1.0, 0.0);
					glutSolidCube (1.0);		//First Section
				glPopMatrix();

				glTranslatef (1.25, 0.0, 0.0);
				glRotatef ((GLfloat)((-little_finger/255)*90), 0.0, 0.0, 1.0);
				glTranslatef (0.25, 0.0, 0.0);
		
				glPushMatrix();
					glScalef (1.5, 1.0, 1.0);
					glColor3f(1.0, 0.0, 0.0);
					glutSolidCube (1.0);		//Second Section
				glPopMatrix();

				glTranslatef (1.125, 0.0, 0.0);
				glRotatef ((GLfloat)((-little_finger/255)*90), 0.0, 0.0, 1.0);
				glTranslatef (0.25, 0.0, 0.0);

				glPushMatrix();
					glColor3f(0.0, 0.0, 1.0);
					glutSolidSphere(.75, 5, 5);	// Touch sensor
					glScalef (1.25, 1.0, 1.0);
					//glutWireCube (1.0);			//Finger Tip
				glPopMatrix();

			glPopMatrix();// end of little finger

		glPopMatrix(); // end of fingers.

	glPopMatrix(); // end of hand.

	}

 	int gesture_check()
		{

		if (shoot){
		PlaySound((LPCSTR)GunPtr, NULL, SND_MEMORY | SND_ASYNC);
		load_object( BULLET , xTrans , yTrans, zTrans , 1 );
		printf(" bang !! ");
		shoot = false;
		}
		
		xRaw = (float)gloveData[5];
		zRaw = (float)gloveData[6];

		zStep = 0;
		xStep = 0;

		if ( zRaw > 130 )
			zStep = ((zRaw - 130) / 125) * 5;

		if ( zRaw < 90 )
			zStep = ((zRaw - 90) / 90) * 5;

		if ( xRaw > 150 )
			xStep = ((xRaw - 150) / 105) * 5;

		if ( xRaw < 110 )  
			xStep = ((xRaw - 110) / 110) * 5;

		if (!menu){
		
			if(index_finger < 150 ){
				if(middle_finger < 150){
					if(ring_finger > 150){
						if(little_finger > 150){	
							if ( zRaw < 90 ){
								if(yNegMov){
									if(!camera_mode){
									yTrans += zStep;
									centrey += zStep;
									}
									eyey += zStep;
								}
							}
					
							if ( zRaw > 130 ){
								if(yPosMov){
									if(!camera_mode){
									yTrans += zStep;
									centrey += zStep;
									}
									eyey += zStep;
								}
							}
						}
					}
				}
					
				if(middle_finger > 150){	
					if(ring_finger > 150){
						if(little_finger > 150){
							if ( zRaw < 90 ){
								if(zNegMov){
									if(!camera_mode){
									zTrans += zStep;
									centrez += zStep;
									}
									eyez += zStep;
								}
							}
							if ( zRaw > 130 ){
								if(zPosMov){
									if(!camera_mode){
									zTrans += zStep;
									centrez += zStep;
									}
									eyez += zStep;
								}
							}
							if ( xRaw < 110 ){
								if(xNegMov){
									if(!camera_mode){
									xTrans += xStep;
									centrex += xStep;
									}
									eyex += xStep;
								}
							}
					
							if ( xRaw > 150 ){
								if(xPosMov){
									if(!camera_mode){
									xTrans += xStep;
									centrex += xStep;
									}
									eyex += xStep;
								}
							}
						}
					}
				}
			}
		}

		//Action on make a fist
		if(index_finger > 200 ){
			if(middle_finger > 150){
				if(ring_finger > 200){
					if(little_finger > 200){
						if (menu && !fist_made){
						// call function to set text.
						calc_menu_choice();
						set_text();
						fist_made = true;
						}
					}
				}
			}
		}
	 	

		//Action on make a flat hand
		if(index_finger < 200 ){
			if(middle_finger < 150){
				if(ring_finger < 200){
					if(little_finger < 200){
						if (fist_made){
							fist_made = false;}
						reload = true;
						
					}
				}
			}
		}
	 	

		if(index_finger < 150 ){
			if(middle_finger > 200){
				if(ring_finger > 200){
					if(little_finger < 150){
						set_text();
						menu=true;
					}
				}
			}
		}
	 	
		if(gloveData[5] > 200){
			if(gloveData[0] > 200){
				xTrans -= 0.25;
				centrex -=0.25;
				eyex -=0.25;
			}
		}		

		if(index_finger < 150 ){
			if(middle_finger > 150){
				if(ring_finger > 150){
					if(little_finger > 150){	
						if(gloveData[0] > 200){
							if (reload && gun_mode){
							PlaySound((LPCSTR)GunPtr, NULL, SND_MEMORY | SND_ASYNC);
							load_object( BULLET , s[0][0] , s[0][1], s[0][2], 1 );
							reload = false;
							}
						}
					}
				}
			}	
		}


	
		return 0;
	}

	void track_finger_sensors(){
	
		// read flex of all fingers.
		index_angle = (-index_finger/255)*90;
		middle_angle = (-middle_finger/255)*90;
		ring_angle = (-ring_finger/255)*90;
		little_angle = (-little_finger/255)*90;

		//////////////////////////////////////
		/// Calculate j0 for all fingers.  ///
		//////////////////////////////////////
		
		j0_index[0] = 0;
		j0_index[1] = 0;
		j0_index[2] = zTrans;
		
		j0_middle[0] = 0;
		j0_middle[1] = 0;
		j0_middle[2] = zTrans;

		j0_ring[0] = 0;
		j0_ring[1] = 0;
		j0_ring[2] = zTrans;

		j0_little[0] = 0;
		j0_little[1] = 0;
		j0_little[2] = zTrans;

		///////////////////////////////////////////
		/// Next calculate j1 for all fingers.  ///
		///////////////////////////////////////////
		
		// angle palm is rotated.
		palm_angle = 90 - zRot;// downward z-rotation neg.

		// new position in y adn z direction.
		temp_y = 1.5 * cos(palm_angle*PI/180);
		temp_z = 1.5 * sin(palm_angle*PI/180);

		// index finger
		j1_index[0] = j0_index[0];
		j1_index[1] = j0_index[1] + temp_y;
		j1_index[2] = j0_index[2] - temp_z;

		// middle finger
		j1_middle[0] = j0_middle[0];
		j1_middle[1] = j0_middle[1] + temp_y;
		j1_middle[2] = j0_middle[2] - temp_z;
		
		// ring finger
		j1_ring[0] = j0_ring[0];
		j1_ring[1] = j0_ring[1] + temp_y;
		j1_ring[2] = j0_ring[2] - temp_z;
		
		//little finger
		j1_little[0] = j0_little[0];
		j1_little[1] = j0_little[1] + temp_y;
		j1_little[2] = j0_little[2] - temp_z;
		
		///////////////////////////////////////////
		/// Next calculate j2 for all fingers.  ///
		///////////////////////////////////////////

		// index finger.
		total_angle = 90 - (zRot + index_angle);

		temp_y = 2 * cos(total_angle*PI/180);
		temp_z = 2 * sin(total_angle*PI/180);

		j2_index[0] = j1_index[0];
		j2_index[1] = j1_index[1] + temp_y;
		j2_index[2] = j1_index[2] - temp_z;
		
		// middle finger.
		total_angle = 90 - (zRot + middle_angle);

		temp_y = 2 * cos(total_angle*PI/180);
		temp_z = 2 * sin(total_angle*PI/180);

		j2_middle[0] = j1_middle[0];
		j2_middle[1] = j1_middle[1] + temp_y;
		j2_middle[2] = j1_middle[2] - temp_z;

		// ring finger.
		total_angle = 90 - (zRot + ring_angle);

		temp_y = 2.5 * cos(total_angle*PI/180);
		temp_z = 2.5 * sin(total_angle*PI/180);

		j2_ring[0] = j1_ring[0];
		j2_ring[1] = j1_ring[1] + temp_y;
		j2_ring[2] = j1_ring[2] - temp_z;

		// little finger.
		total_angle = 90 - (zRot + little_angle);

		temp_y = 1.5 * cos(total_angle*PI/180);
		temp_z = 1.5 * sin(total_angle*PI/180);

		j2_little[0] = j1_little[0];
		j2_little[1] = j1_little[1] + temp_y;
		j2_little[2] = j1_little[2] - temp_z;

		///////////////////////////////////////////
		/// Next calculate j3 for all fingers.  ///
		///////////////////////////////////////////

		// index finger.
		total_angle = 90 - (zRot + 2*(index_angle));

		temp_y = 2 * cos(total_angle*PI/180);
		temp_z = 2 * sin(total_angle*PI/180);

		j3_index[0] = j2_index[0];
		j3_index[1] = j2_index[1] + temp_y;
		j3_index[2] = j2_index[2] - temp_z;

		// middle finger.
		total_angle = 90 - (zRot + 2*(middle_angle));

		temp_y = 2 * cos(total_angle*PI/180);
		temp_z = 2 * sin(total_angle*PI/180);

		j3_middle[0] = j2_middle[0];
		j3_middle[1] = j2_middle[1] + temp_y;
		j3_middle[2] = j2_middle[2] - temp_z;

		// ring finger.
		total_angle = 90 - (zRot + 2*(ring_angle));

		temp_y = 2 * cos(total_angle*PI/180);
		temp_z = 2 * sin(total_angle*PI/180);

		j3_ring[0] = j2_ring[0];
		j3_ring[1] = j2_ring[1] + temp_y;
		j3_ring[2] = j2_ring[2] - temp_z;

		// little finger.
		total_angle = 90 - (zRot + 2*(little_angle));

		temp_y = 1.5 * cos(total_angle*PI/180);
		temp_z = 1.5 * sin(total_angle*PI/180);

		j3_little[0] = j2_little[0];
		j3_little[1] = j2_little[1] + temp_y;
		j3_little[2] = j2_little[2] - temp_z;


		///////////////////////////////////////////
		/// Next calculate j4 for all fingers.  ///
		///////////////////////////////////////////
		
		// index finger.
		total_angle = 90 - (zRot + 3*(index_angle));

		temp_y = 0.75 * cos(total_angle*PI/180);
		temp_z = 0.75 * sin(total_angle*PI/180);

		j4_index[0] = j3_index[0] ;
		j4_index[1] = j3_index[1] + temp_y;
		j4_index[2] = j3_index[2] - temp_z;

		// middle finger.
		total_angle = 90 - (zRot + 3*(middle_angle));

		temp_y = 0.75 * cos(total_angle*PI/180);
		temp_z = 0.75 * sin(total_angle*PI/180);

		j4_middle[0] = j3_middle[0];
		j4_middle[1] = j3_middle[1] + temp_y;
		j4_middle[2] = j3_middle[2] - temp_z;

		// ring finger.
		total_angle = 90 - (zRot + 3*(ring_angle));

		temp_y = 0.75 * cos(total_angle*PI/180);
		temp_z = 0.75 * sin(total_angle*PI/180);

		j4_ring[0] = j3_ring[0];
		j4_ring[1] = j3_ring[1] + temp_y;
		j4_ring[2] = j3_ring[2] - temp_z;

		// little finger.
		total_angle = 90 - (zRot + 3*(little_angle));

		temp_y = 0.625 * cos(total_angle*PI/180);
		temp_z = 0.625 * sin(total_angle*PI/180);

		j4_little[0] = j3_little[0] ;
		j4_little[1] = j3_little[1] + temp_y;
		j4_little[2] = j3_little[2] - temp_z;

		///////////////////////////////////////////////////////
		/// Next calculate actual position of touch sensors ///
		///////////////////////////////////////////////////////

		// Rotation around the X axis...

		//index_finger

		temp_x = -2.25 * cos(xRot*PI/180);  //compsensate for x value after x_rot 
		temp_y = -2.25 * sin(xRot*PI/180);  //compsensate for y value after x rot 

		s[1][0] = s[0][0] = xTrans + temp_x;  
		s[1][1] = s[0][1] = yTrans - temp_y;
		s[1][2] = zTrans;
		
		temp_x = j4_index[1] * sin(xRot*PI/180);   
		temp_y = j4_index[1] * cos(xRot*PI/180);

		s[0][0] += temp_x;
		s[0][1] += temp_y;
		s[0][2] = j4_index[2];
		
		//middle_finger

		temp_x = -0.75 * cos(xRot*PI/180);
		temp_y = -0.75 * sin(xRot*PI/180);

		s[3][0] = s[2][0] = xTrans + temp_x;
		s[3][1] = s[2][1] = yTrans - temp_y;
		s[3][2] = zTrans;
		
		temp_x = j4_middle[1] * sin(xRot*PI/180);
		temp_y = j4_middle[1] * cos(xRot*PI/180);

		s[2][0] += temp_x;
		s[2][1] += temp_y;
		s[2][2] = j4_middle[2];

		//ring_finger

		temp_x = 0.75 * cos(xRot*PI/180);
		temp_y = 0.75 * sin(xRot*PI/180);

		s[5][0] = s[4][0] = xTrans + temp_x;
		s[5][1] = s[4][1] = yTrans - temp_y;
		s[5][2] = zTrans;
		
		temp_x = j4_ring[1] * sin(xRot*PI/180);
		temp_y = j4_ring[1] * cos(xRot*PI/180);

		s[4][0] += temp_x;
		s[4][1] += temp_y;
		s[4][2] = j4_ring[2];

		//little_finger

		temp_x = 2.25 * cos(xRot*PI/180);
		temp_y = 2.25 * sin(xRot*PI/180);

		s[7][0] = s[6][0] = xTrans + temp_x;
		s[7][1] = s[6][1] = yTrans - temp_y;
		s[7][2] = zTrans;
		
		temp_x = j4_little[1] * sin(xRot*PI/180);
		temp_y = j4_little[1] * cos(xRot*PI/180);

		s[6][0] += temp_x;
		s[6][1] += temp_y;
		s[6][2] = j4_little[2];

		return;
	}

	void sound_init(){

	TCHAR WaveName[] = "GUN.WAV";

	/* Open the WAVE file */
	inHandle = CreateFile("GUN.WAV", GENERIC_READ,FILE_SHARE_READ, 0, OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN, 0);
	/* Get the size of the file */
	waveSize = GetFileSize(inHandle, 0);
	/* Allocate some memory to load the file */
	GunPtr = (char *)VirtualAlloc(0, waveSize, MEM_COMMIT,PAGE_READWRITE);
	/* Read in WAVE file */
	ReadFile(inHandle, GunPtr, waveSize, &action, 0);
	/* Free the memory */
	VirtualFree(GunPtr, waveSize, MEM_FREE);
	/* Close the WAVE file */
	CloseHandle(inHandle);
	
	return;
	}


};


hand glove;

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

		
	cube(float x, float y, float z, float w){
		xPos=x; yPos=y; zPos=z;
		zRot = 0;
		size = w;
		object = CUBE;
		width = w;
		anchored=true;
		colour[0] = 1.0;
		colour[1] = 1.0;
		colour[2] = 0.0;
		held = false;
		
		cb_points = new Single_Contact_List("Cube");
		
		cb_points->add(x,y,z,w/2);
		cb_points->add(x+w/4, y+w/4, z+w/4, w/4);
		cb_points->add(x+w/4, y+w/4, z-w/4, w/4);
		cb_points->add(x+w/4, y-w/4, z+w/4, w/4);
		cb_points->add(x+w/4, y-w/4, z-w/4, w/4);
		cb_points->add(x-w/4, y+w/4, z+w/4, w/4);
		cb_points->add(x-w/4, y+w/4, z-w/4, w/4);
		cb_points->add(x-w/4, y-w/4, z+w/4, w/4);
		cb_points->add(x-w/4, y-w/4, z-w/4, w/4);
		
		C_Points = cb_points; 
	
	}
	
	void display_c_points(){
		C_Points->n = C_Points->head->next;

		while ( C_Points->n->next != 0){ // while there are contact points scan them.
			C_Points->n = C_Points->n->next;
						
			glPushMatrix();
				glColor3f(0,1,0);
				glTranslatef(C_Points->n->x,C_Points->n->y,C_Points->n->z);
				glutSolidSphere(C_Points->n->radius, 10, 10);	
			glPopMatrix();
		}
	}

 	void display(){

		if(selected == Current_Selected_Object){
		
			if(Remove_Selected_Object){
				remove = true;
				Remove_Selected_Object = false;
			}

			if(Hide_Selected_Object){
				visible = false;
				Hide_Selected_Object = false;
			}

			if(Show_Selected_Object){
				visible = true;
				Show_Selected_Object = false;
			}

		}

		if (!visible)
			return;
		
		glPushMatrix();
			int sensor;
			float x_total=0, y_total=0, z_total=0;	//This is for totaling the movement of the contact spheres to apply at the end of loop
			static Single_Contact_List* C = new Single_Contact_List("Temp");
			C->n = C_Points->head->next;

			static Single_Object_List* T = new Single_Object_List("Temp");
			T->n = Object_List.n;

			while ( C->n->next != 0){ // while there are contact points scan them.
			
				C->n = C->n->next;
			
				for (sensor = 0;sensor <8;sensor+=2){
				
					collision  = check_for_collision(C->n->x,C->n->y,C->n->z,glove.s[sensor][0],glove.s[sensor][1],glove.s[sensor][2],C->n->radius,0.75);
					
					if(!anchored && collision && !held){

						if ( glove.s[sensor][0] > xPos){
							x_total-=0.5;	
						}

						if ( glove.s[sensor][0] < xPos){
							x_total+=0.5;
						}

						if ( glove.s[sensor][1] > yPos){
							y_total-=0.5;		
						}
						if ( glove.s[sensor][1] < yPos){
							y_total+=0.5;		
						}
					
						if ( glove.s[sensor][2] > zPos){
							z_total-=0.5;		
						}
					
						if ( glove.s[sensor][2] < zPos){
							z_total+=0.5;		
						}
						
					}
					
					if(!anchored && held){
						float t_x;
						float t_y;

						t_y = width/2 * cos(glove.xRot*PI/180);
						t_x = width/2 * sin(glove.xRot*PI/180);

						x_total = glove.xTrans - xPos - t_x;
						y_total = glove.yTrans - yPos - t_y;
						z_total = glove.zTrans - zPos;

						zRot = -glove.zRot;


					}

				collision = false;
				}//end of for

				// now check for other object in the world .... !!!!!
				
				// Check the Object List and see if we've hit anything.

				Object_List.n = Object_List.head->next;

				while ( Object_List.n->next != 0){ /// while there are objects, scan through them 
					Object_List.n = Object_List.n->next;

					Object_List.n->obj->C_Points->n = Object_List.n->obj->C_Points->head->next;
							
					while ( Object_List.n->obj->C_Points->n->next != NULL){ // while there are more contact points scan through them.
						
						Object_List.n->obj->C_Points->n = Object_List.n->obj->C_Points->n->next;

						c_sh_x = Object_List.n->obj->C_Points->n->x; //to hold x value of contact sphere etc
						c_sh_y = Object_List.n->obj->C_Points->n->y;
						c_sh_z = Object_List.n->obj->C_Points->n->z;
						c_rad = Object_List.n->obj->C_Points->n->radius;

						collision  = check_for_collision(c_sh_x,c_sh_y,c_sh_z,C->n->x,C->n->y,C->n->z,c_rad,C->n->radius);
						
						if ( Object_List.n->obj->C_Points->name != C_Points->name ){
							
							if(collision && !anchored){

								if ( c_sh_x > xPos){
									x_total-=0.5;	
								}

								if ( c_sh_x < xPos){
									x_total+=0.5;
								}

								if ( c_sh_y > yPos){
									y_total-=0.5;		
								}
								if ( c_sh_y < yPos){
									y_total+=0.5;		
								}
							
								if ( c_sh_z > zPos){
									z_total-=0.5;		
								}
							
								if ( c_sh_z < zPos){
									z_total+=0.5;		
								}
							}
							collision= false;
						}
						
					}
					
				}

  
			}// end of while

			if(!held)
				zRot = 0;
					

			C_Points->n = C_Points->head->next;

			while ( C_Points->n->next != 0){ // while there are contact points scan them.
			
				C_Points->n = C_Points->n->next;
				C_Points->n->x+=x_total;
				C_Points->n->y+=y_total;
				C_Points->n->z+=z_total;
			}			

			xPos += x_total;
			yPos += y_total;
			zPos += z_total;

			glColor3fv(colour);
			glTranslatef(xPos, yPos, zPos);
			glRotatef(zRot,0,0,1);
			glTranslatef(-xPos, -yPos, -zPos);
		
			glTranslatef(xPos, yPos, zPos);
			
			if (show_spheres)
				glutWireCube(width);	
			else
				glutSolidCube(width);
			
		glPopMatrix();

		Object_List.n = T->n;
		
	}
};


class sphere:public Object{
	//Low and behold its about time, the SPHERE CLASS is upon us!!!! Yeeehar!
	public:
	
	float radius;
	float colour[3];
	bool anchored;
	bool collision;
	Single_Contact_List* sp_points;
	int yRot;

	//Temp variables for scanning through link list
	float c_sh_x, c_sh_y, c_sh_z;
	float c_rad;

 	sphere(float x,float y,float z,float r){
		xPos=x; yPos=y; zPos=z;
		anchored = false;
		held = false;
		colour[0] = 1.0;
		colour[1] = 1.0;
		colour[2] = 1.0;
		object = SPHERE;
		size = r;
		radius = r;
		yRot = 0;

		sp_points = new Single_Contact_List("Sphere");
		sp_points->add(x,y,z,r);

		C_Points = sp_points; 
	}
	

	void display_c_points(){
		
		C_Points->n = C_Points->head->next;

		while ( C_Points->n->next != 0){ // while there are contact points scan them.
		
			glPushMatrix();
				C_Points->n = C_Points->n->next;
		
				glColor3f(0,1,0);
				glTranslatef(C_Points->n->x,C_Points->n->y,C_Points->n->z);
				glutSolidSphere(C_Points->n->radius, 10, 10);	
			glPopMatrix();
		}
	}
	
	
void display(){

//	quad1 = gluNewQuadric();
//	gluQuadricTexture(quad2, GL_TRUE);

	yRot += 1;
	if(selected == Current_Selected_Object){
	
		if(Remove_Selected_Object){
			remove = true;
			Remove_Selected_Object = false;
		}

		if(Hide_Selected_Object){
			visible = false;
			Hide_Selected_Object = false;
		}

		if(Show_Selected_Object){
			visible = true;
			Show_Selected_Object = false;
		}

	}

			if (!visible)
			return;
		
		glPushMatrix();
			int sensor;
			float x_total=0, y_total=0, z_total=0;	//This is for totaling the movement of the contact spheres to apply at the end of loop
			static Single_Contact_List* C  = new Single_Contact_List("Temp");

			C->n = C_Points->head->next;

			static Single_Object_List* T = new Single_Object_List("Temp");
			T->n = Object_List.n;

			while ( C->n->next != 0){ // while there are contact points scan them.
			
				C->n = C->n->next;
			
				for (sensor = 0;sensor <8;sensor+=2){
				
					collision  = check_for_collision(C->n->x,C->n->y,C->n->z,glove.s[sensor][0],glove.s[sensor][1],glove.s[sensor][2],C->n->radius,0.75);
					
					if(!anchored && collision &&!held){
					
						if ( glove.s[sensor][0] > xPos){
							x_total-=0.5;	
						}

						if ( glove.s[sensor][0] < xPos){
							x_total+=0.5;
						}

						if ( glove.s[sensor][1] > yPos){
							y_total-=0.5;		
						}
						if ( glove.s[sensor][1] < yPos){
							y_total+=0.5;		
						}
					
						if ( glove.s[sensor][2] > zPos){
							z_total-=0.5;		
						}
					
						if ( glove.s[sensor][2] < zPos){
							z_total+=0.5;		
						}
						
					}
					
					if(!anchored && held){
					float t_x;
					float t_y;

					t_y = radius * cos(glove.xRot*PI/180);
					t_x = radius * sin(glove.xRot*PI/180);

					x_total = glove.xTrans - xPos - t_x;
					y_total = glove.yTrans - yPos - t_y;
					z_total = glove.zTrans - zPos;

					}
					
					collision = false;
				}//end of for

				// now check for other object in the world .... !!!!!
				
				// Check the Object List and see if we've hit anything.

				Object_List.n = Object_List.head->next;

				while ( Object_List.n->next != 0){ /// while there are objects, scan through them 
					Object_List.n = Object_List.n->next;

					Object_List.n->obj->C_Points->n = Object_List.n->obj->C_Points->head->next;
							
					while ( Object_List.n->obj->C_Points->n->next != NULL){ // while there are more contact points scan through them.
						
						Object_List.n->obj->C_Points->n = Object_List.n->obj->C_Points->n->next;

						c_sh_x = Object_List.n->obj->C_Points->n->x; //to hold x value of contact sphere etc
						c_sh_y = Object_List.n->obj->C_Points->n->y;
						c_sh_z = Object_List.n->obj->C_Points->n->z;
						c_rad = Object_List.n->obj->C_Points->n->radius;

						collision  = check_for_collision(c_sh_x,c_sh_y,c_sh_z,C->n->x,C->n->y,C->n->z,c_rad,C->n->radius);
						
						if ( Object_List.n->obj->C_Points->name != C_Points->name ){
							
							if(collision && !anchored){

								if ( c_sh_x > xPos){
									x_total-=0.5;	
								}

								if ( c_sh_x < xPos){
									x_total+=0.5;
								}

								if ( c_sh_y > yPos){
									y_total-=0.5;		
								}
								if ( c_sh_y < yPos){
									y_total+=0.5;		
								}
							
								if ( c_sh_z > zPos){
									z_total-=0.5;		
								}
							
								if ( c_sh_z < zPos){
									z_total+=0.5;		
								}
							}
							collision= false;
						}
						
					}
					
				}
  
			}// end of while

		xPos += x_total;
		yPos += y_total;
		zPos += z_total;

		if ( yPos - radius <=  0){
			yPos -= y_total;
			y_total = 0;
		}


			C_Points->n = C_Points->head->next;

			while ( C_Points->n->next != 0){ // while there are contact points scan them.
			
				C_Points->n = C_Points->n->next;
				C_Points->n->x+=x_total;
				C_Points->n->y+=y_total;
				C_Points->n->z+=z_total;
			}			
					
		glColor3f(1,1,1);

		glEnable(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, e_texture);

		glColor3fv(colour);
		glTranslatef(xPos, yPos, zPos);
		glRotatef(90.0, 1.0, 0.0, 0.0);
		glRotatef(yRot, 0.0, 0.0, 1.0);
		if(show_spheres)glutWireSphere(radius, 10, 10);
		else gluSphere(quad1, radius, 16, 16);
		glRotatef(-90.0, 1.0, 0.0, 0.0);

		glDisable(GL_TEXTURE_2D);
		glPopMatrix();
		Object_List.n = T->n;

	}
	

};


class bullet:public Object{
	//Low and behold its about time, the SPHERE CLASS is upon us!!!! Yeeehar!
	public:
	
	float radius;
	float colour[3];
	Single_Contact_List* sp_points;
	float speed;

 	bullet(float x,float y,float z,float r){
		xPos=x; yPos=y; zPos=z;
		colour[0] = 1.0;
		colour[1] = 1.0;
		colour[2] = 1.0;
		object = BULLET;
		size = r;
		radius = r;
		speed = 10;

		sp_points = new Single_Contact_List("Sphere");
		sp_points->add(x,y,z,r);

		C_Points = sp_points; 
	}
	

	void display_c_points(){
		
		C_Points->n = C_Points->head->next;

		while ( C_Points->n->next != 0){ // while there are contact points scan them.
		
			glPushMatrix();
				C_Points->n = C_Points->n->next;
		
				glColor3f(0,1,0);
				glTranslatef(C_Points->n->x,C_Points->n->y,C_Points->n->z);
				glutSolidSphere(C_Points->n->radius, 10, 10);	
			glPopMatrix();
		}
	}
	
	
	void display(){


		if(selected == Current_Selected_Object){
		
			if(Remove_Selected_Object){
				remove = true;
				Remove_Selected_Object = false;
			}
		}

		glPushMatrix();
	
		zPos -= speed;

		C_Points->n = C_Points->head->next;

			while ( C_Points->n->next != 0){ // while there are contact points scan them.
			
				C_Points->n = C_Points->n->next;
				C_Points->n->z-=speed;
			}			
					
		glColor3fv(colour);
		glTranslatef(xPos, yPos, zPos);
		glutSolidSphere(radius, 5, 5);
		
		glPopMatrix();

	}
	
};




  
class key:public Object{
	//Low and behold its about time, the KEY CLASS is upon us!!!! Yeeehar!
	public:
	
	float c_keyrot;
	float colour[3];
	bool collision;
	Single_Contact_List* ky_points;
	int width;

	//Temp variables for scanning through link list
	float c_sh_x, c_sh_y, c_sh_z;
	float c_rad;

	key(float x,float y,float z,float w){
		xPos=x; yPos=y; zPos=z;
		object = PIANO_KEY;
		size = w;

		anchored = false;

		if ( w == 1 ){ 
		colour[0] = 1.0;
		colour[1] = 1.0;
		colour[2] = 1.0;
		width = 2;
		}

		else {
		colour[0] = 0.3;
		colour[1] = 0.3;
		colour[2] = 0.3;
		width = 1;	
		}
		

		c_keyrot = 0;
		
		ky_points = new Single_Contact_List("Key");
		ky_points->add(x,y,z+4,0.5);
		ky_points->add(x,y,z+3,0.5);
		ky_points->add(x,y,z+2,0.5);
		ky_points->add(x,y,z+1,0.5);
		
		C_Points = ky_points; 

	}
	

	void display_c_points(){
		
		C_Points->n = C_Points->head->next;

		while ( C_Points->n->next != 0){ // while there are contact points scan them.
		
			glPushMatrix();
				C_Points->n = C_Points->n->next;
		
				glColor3f(0,1,0);
				glTranslatef(C_Points->n->x,C_Points->n->y,C_Points->n->z);
				glutSolidSphere(C_Points->n->radius, 10, 10);	
			glPopMatrix();
		}
	}

	void display(){

		glPushMatrix();

			c_keyrot=0;

			int sensor;
			float x_total=0, y_total=0, z_total=0;	//This is for totaling the movement of the contact spheres to apply at the end of loop
			static Single_Contact_List* C = new Single_Contact_List("Temp");
			C->n = C_Points->head->next;

			static Single_Object_List* T = new Single_Object_List("Temp");
			T->n = Object_List.n;

			while ( C->n->next != 0){ // while there are contact points scan them.
			
				C->n = C->n->next;
			
				for (sensor = 0;sensor <8;sensor+=2){
				
					collision  = check_for_collision(C->n->x,C->n->y,C->n->z,glove.s[sensor][0],glove.s[sensor][1],glove.s[sensor][2],C->n->radius,0.75);
					
					if(collision){
			
						if ( glove.s[sensor][1] < yPos){
							if(c_keyrot <15)
								c_keyrot+=5;
						//	yContact+=0.69724;

						}
						if ( glove.s[sensor][1] > yPos){
							if(c_keyrot !=0)
								c_keyrot-=5;
						//	yContact-=0.69724;
						}

					}

					collision = false;
				}//end of for

				// now check for other object in the world .... !!!!!
				
				// Check the Object List and see if we've hit anything.

				Object_List.n = Object_List.head->next;

				while ( Object_List.n->next != 0){ /// while there are objects, scan through them 
					Object_List.n = Object_List.n->next;

					Object_List.n->obj->C_Points->n = Object_List.n->obj->C_Points->head->next;
							
					while ( Object_List.n->obj->C_Points->n->next != NULL){ // while there are more contact points scan through them.
						
						Object_List.n->obj->C_Points->n = Object_List.n->obj->C_Points->n->next;

						c_sh_x = Object_List.n->obj->C_Points->n->x; //to hold x value of contact sphere etc
						c_sh_y = Object_List.n->obj->C_Points->n->y;
						c_sh_z = Object_List.n->obj->C_Points->n->z;
						c_rad = Object_List.n->obj->C_Points->n->radius;

						collision  = check_for_collision(c_sh_x,c_sh_y,c_sh_z,C->n->x,C->n->y,C->n->z,c_rad,C->n->radius);
						
						if ( Object_List.n->obj->C_Points->name != C_Points->name ){
							
							if(collision){
			
								if ( glove.s[sensor][1] < yPos){
									if(c_keyrot <15)
										c_keyrot+=5;
								//	yContact+=0.69724;

								}
								if ( glove.s[sensor][1] > yPos){
									if(c_keyrot !=0)
										c_keyrot-=5;
								//	yContact-=0.69724;
								}
							}
							collision= false;
						}						
					}
				}
			}// end of while

		glColor3fv(colour);
		glPushMatrix();
			glTranslatef(xPos, yPos, zPos);
			glRotatef(90, 0.0, 1.0, 0.0);
			
			glTranslatef (4.0, 0.5, 0.0);
			glRotatef (c_keyrot, 0.0, 0.0, 1.0);
			glTranslatef (-4.0, -0.5, 0.0);
			
			glPushMatrix();
				glColor3fv(colour);
				glScalef (8.0, 1.0, width);
				if(show_spheres)glutWireCube(1.0);		
				
				else glutSolidCube(1.0);	// Key
			glPopMatrix();
		
		glPopMatrix();	//End of switch

	
	glPopMatrix();
	Object_List.n = T->n;
	}



};

 
class domino:public Object{
	//Low and behold its about time, the Domino CLASS is upon us!!!! Yeeehar!
	public:
	
	float c_domrot;
	float y_total, z_total;	//for calculating rotation of c_pointson 
	float colour[3];
	bool collision;
	bool tipped;
	Single_Contact_List* dm_points;
	
	//Temp variables for scanning through link list
	float c_sh_x, c_sh_y, c_sh_z;
	float c_rad;

	domino(float x,float y,float z){
		xPos=x; yPos=y; zPos=z;
		object = DOMINO;
		show_spheres=true;
		anchored = false;
		collision = false;
		tipped = false;
		
		dm_points = new Single_Contact_List("Domino");
		
		dm_points->add(x+1, y+4, z, 0.5);
		dm_points->add(x  , y+4, z, .75);
		dm_points->add(x-1, y+4, z, 0.5);
		
		C_Points = dm_points; 
	}
	

	void display_c_points(){
			
		C_Points->n = C_Points->head->next;

		while ( C_Points->n->next != 0){ // while there are contact points scan them.
		
			glPushMatrix();
				C_Points->n = C_Points->n->next;
				glColor3f(0,1,0);
				glTranslatef(C_Points->n->x,C_Points->n->y,C_Points->n->z);
				glutSolidSphere(C_Points->n->radius, 10, 10);	
			glPopMatrix();
		}
	}

	void display(){

		glPushMatrix();
					
			c_domrot = 0;
			int sensor;
			float x_total=0, y_total=0, z_total=0;	//This is for totaling the movement of the contact spheres to apply at the end of loop
			static Single_Contact_List* C = new Single_Contact_List("Temp");
			C->n = C_Points->head->next;

			static Single_Object_List* T = new Single_Object_List("Temp");
			T->n = Object_List.n;

			while ( C->n->next != 0){ // while there are contact points scan them.
			
				C->n = C->n->next;
			
				for (sensor = 0;sensor <8;sensor+=2){
					
					if(!tipped){
						collision  = check_for_collision(C->n->x,C->n->y,C->n->z,glove.s[sensor][0],glove.s[sensor][1],glove.s[sensor][2],C->n->radius,0.75);
						if(collision){
							tipped=true;
						}
					}
				}//end of for
				
				// Check the Object List and see if anything has hit us.
				Object_List.n = Object_List.head->next;

				while ( Object_List.n->next != 0){ /// while there are objects, scan through them 
					Object_List.n = Object_List.n->next;

					Object_List.n->obj->C_Points->n = Object_List.n->obj->C_Points->head->next;
							
					while ( Object_List.n->obj->C_Points->n->next != NULL){ // while there are more contact points scan through them.
						
						Object_List.n->obj->C_Points->n = Object_List.n->obj->C_Points->n->next;

						c_sh_x = Object_List.n->obj->C_Points->n->x; //to hold x value of contact sphere etc
						c_sh_y = Object_List.n->obj->C_Points->n->y;
						c_sh_z = Object_List.n->obj->C_Points->n->z;
						c_rad = Object_List.n->obj->C_Points->n->radius;

						collision  = check_for_collision(c_sh_x,c_sh_y,c_sh_z,C->n->x,C->n->y,C->n->z,c_rad,C->n->radius);
						
						if ( Object_List.n->obj->C_Points->name != C_Points->name ){
							
							if(collision)tipped=true;

						}
						
					}
					
				}
  
			}// end of while
				

		if((tipped && c_domrot < 90)){
				c_domrot+=5;
				}


		y_total = 8.0 * cos(c_domrot *PI/180);
		z_total = 8.0 * sin(c_domrot *PI/180);

		C_Points->n = C_Points->head->next;

		while ( C_Points->n->next != 0){ // while there are contact points scan them.
		
			C_Points->n = C_Points->n->next;
			C_Points->n->y = y_total;
			C_Points->n->z = -z_total;
		}			

	
		glColor3fv(colour);
		glPushMatrix();
			glTranslatef(xPos, yPos, zPos);
			glRotatef(90, 0.0, 0.0, 1.0);
			
			glTranslatef (-4.0, 0.0, 0.0);
				glRotatef (c_domrot, 0.0, 1.0, 0.0);
			glTranslatef (+4.0, 0.0, 0.0);
			
			glPushMatrix();
				glColor3f(1,0,0);
				glScalef (8.0, 3.0, 1);
				if(show_spheres)glutWireCube(1.0);		
				
				else glutSolidCube(1.0);	// Domino
			glPopMatrix();
		
		glPopMatrix();	//End of domino

	
	glPopMatrix();
	Object_List.n = T->n;

	
	}

};


class falling_sphere:public Object{
	//Low and behold its about time, the SPHERE CLASS is upon us!!!! Yeeehar!
	public:
	
	float radius;
	float colour[3];
	bool anchored;
	bool collision;
	bool just_released;
	bool moving;
	float time;
	float xz_time;
	float x_speed;
	float y_speed;
	float z_speed;
	float distance;
	float y_start_time;
	float xz_start_time;
	float initial_y_position;
	float initial_x_position;
	float initial_z_position;
	float current_speed;
	Single_Contact_List* sp_points;
	HANDLE inHandle;
	char* BouncePtr;
	unsigned long waveSize, action;		


	//Temp variables for scanning through link list
	float c_sh_x, c_sh_y, c_sh_z;
	float c_rad;


	falling_sphere(float x,float y,float z,float r){
		xPos=x; yPos=y; zPos=z;
		anchored = false;
		held = false;
		just_released = true;
		moving = true;
		colour[0] = 1.0;
		colour[1] = 0.0;
		colour[2] = 0.0;
		object = BOUNCING_SPHERE;
		size = r;
		radius = r;

		sp_points = new Single_Contact_List("Falling Sphere");
		sp_points->add(x,y,z,r);
		sound_init();
		C_Points = sp_points; 
	}

	void display_c_points(){
		
		C_Points->n = C_Points->head->next;

		while ( C_Points->n->next != 0){ // while there are contact points scan them.
		
			glPushMatrix();
				C_Points->n = C_Points->n->next;
		
				glColor3f(0,1,0);
				glTranslatef(C_Points->n->x,C_Points->n->y,C_Points->n->z);
				glutSolidSphere(C_Points->n->radius, 10, 10);	
			glPopMatrix();
		}
	}

	
	void display(){
		if(selected == Current_Selected_Object){
		
			if(Remove_Selected_Object){
				remove = true;
				Remove_Selected_Object = false;
			}

			if(Hide_Selected_Object){
				visible = false;
				Hide_Selected_Object = false;
			} 

			if(Show_Selected_Object){
				visible = true;
				Show_Selected_Object = false;
			}

		}

		if (!visible)
			return;

	
		glPushMatrix();
			int sensor;
			float x_total=0, y_total=0, z_total=0;	//This is for totaling the movement of the contact spheres to apply at the end of loop
			static Single_Contact_List* C = new Single_Contact_List("Temp");
			C->n = C_Points->head->next;

			static Single_Object_List* T = new Single_Object_List("Temp");
			T->n = Object_List.n;

			while ( C->n->next != 0){ // while there are contact points scan them.
			
				C->n = C->n->next;
			
				for (sensor = 0;sensor <8;sensor+=2){
				
					collision  = check_for_collision(C->n->x,C->n->y,C->n->z,glove.s[sensor][0],glove.s[sensor][1],glove.s[sensor][2],C->n->radius,0.75);
					
					if(!anchored && collision && !held){
					
						if ( glove.s[sensor][0] > xPos){
							x_total-=0.5;	
						}

						if ( glove.s[sensor][0] < xPos){
							x_total+=0.5;
						}

						if ( glove.s[sensor][1] > yPos){
							y_total-=0.5;		
						}
						if ( glove.s[sensor][1] < yPos){
							y_total+=0.5;		
						}
					
						if ( glove.s[sensor][2] > zPos){
							z_total-=0.5;		
						}
					
						if ( glove.s[sensor][2] < zPos){
							z_total+=0.5;		
						}
						
					}
					
					if(!anchored && held){
					float t_x;
					float t_y;

					t_y = radius * cos(glove.xRot*PI/180);
					t_x = radius * sin(glove.xRot*PI/180);

					x_total = glove.xTrans - xPos - t_x;
					y_total = glove.yTrans - yPos - t_y;
					z_total = glove.zTrans - zPos;

					just_released = true;
					moving = true;

					}
					
					collision = false;
				}//end of for

				// now check for other object in the world .... !!!!!
				
				// Check the Object List and see if we've hit anything.

				Object_List.n = Object_List.head->next;

				while ( Object_List.n->next != 0){ /// while there are objects, scan through them 
					Object_List.n = Object_List.n->next;

					Object_List.n->obj->C_Points->n = Object_List.n->obj->C_Points->head->next;
							
					while ( Object_List.n->obj->C_Points->n->next != NULL){ // while there are more contact points scan through them.
						
						Object_List.n->obj->C_Points->n = Object_List.n->obj->C_Points->n->next;

						c_sh_x = Object_List.n->obj->C_Points->n->x; //to hold x value of contact sphere etc
						c_sh_y = Object_List.n->obj->C_Points->n->y;
						c_sh_z = Object_List.n->obj->C_Points->n->z;
						c_rad = Object_List.n->obj->C_Points->n->radius;

						collision  = check_for_collision(c_sh_x,c_sh_y,c_sh_z,C->n->x,C->n->y,C->n->z,c_rad,C->n->radius);
						
						if ( Object_List.n->obj->C_Points->name != C_Points->name ){
							
							if(collision && !anchored ){

								if ( c_sh_x > xPos){
									x_total-=0.5;	
								}

								if ( c_sh_x < xPos){
									x_total+=0.5;
								}

								if ( c_sh_y > yPos){
									y_total-=0.5;		
								}
								if ( c_sh_y < yPos){
									y_total+=0.5;		
								}
							
								if ( c_sh_z > zPos){
									z_total-=0.5;		
								}
							
								if ( c_sh_z < zPos){
									z_total+=0.5;		
								}
							}
							collision= false;
						}
						
					}
					
				}
  
			}// end of while

			if (!held && just_released){
				release();
			}

			if(!held && moving){// calulate how much should have fallen

					if ((yPos < radius) && (current_speed < 0) ){

						y_start_time = Time();
						y_speed = current_speed;// = 50;
						y_speed = -0.7 * y_speed;
						current_speed = y_speed;
		//				x_speed = 0.7 * x_speed;
		//				z_speed = 0.7 * z_speed;
						initial_y_position = radius;
						if (y_speed < 1)moving = false;
						PlaySound((LPCSTR)BouncePtr, NULL, SND_MEMORY | SND_ASYNC);
					}
					
					time = Time() - y_start_time;
					xz_time = Time() - xz_start_time;
					distance = y_speed*time + (0.5)*Gravity*time*time;
					current_speed = y_speed + Gravity*time;
					y_total += distance + initial_y_position - yPos;
	//				x_total += x_speed*xz_time + initial_x_position - xPos;
	//				z_total += z_speed*xz_time + initial_z_position - zPos;
					
			}


			C_Points->n = C_Points->head->next;

			while ( C_Points->n->next != 0){ // while there are contact points scan them.
			
				C_Points->n = C_Points->n->next;
				C_Points->n->x+=x_total;
				C_Points->n->y+=y_total;
				C_Points->n->z+=z_total;
			}			

			xPos += x_total;
			yPos += y_total;
			zPos += z_total;

		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseMaterial);
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);


		glEnable(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, texture);

		glColor3fv(white);
		glTranslatef(xPos, yPos, zPos);
		glRotatef(90.0, 1.0, 0.0, 0.0);
		if(show_spheres)glutWireSphere(radius, 10, 10);
		else gluSphere(quad1, radius, 16, 16);
		glRotatef(-90.0, 1.0, 0.0, 0.0);

		glDisable(GL_TEXTURE_2D);
		
		glPopMatrix();
		Object_List.n = T->n;
	
	 
	}
	
	void hold(){

		just_released = true;
		printf(" Ball held \n");
	}

	void release(){

		just_released = false;
		y_speed = current_speed = 0;
		x_speed = glove.xStep * 5;
		z_speed = glove.zStep * 5;
		y_start_time = Time();
		xz_start_time = Time();
		initial_y_position = yPos;
		initial_x_position = xPos;
		initial_z_position = zPos;

	}
	
	void sound_init(){

	TCHAR WaveName[] = "BOUNCE.WAV";

	/* Open the WAVE file */
	inHandle = CreateFile("BOUNCE.WAV", GENERIC_READ,FILE_SHARE_READ, 0, OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN, 0);
	/* Get the size of the file */
	waveSize = GetFileSize(inHandle, 0);
	/* Allocate some memory to load the file */
	BouncePtr = (char *)VirtualAlloc(0, waveSize, MEM_COMMIT,PAGE_READWRITE);
	/* Read in WAVE file */
	ReadFile(inHandle, BouncePtr, waveSize, &action, 0);
	/* Free the memory */
	VirtualFree(BouncePtr, waveSize, MEM_FREE);
	/* Close the WAVE file */
	CloseHandle(inHandle);
	
	return;
	}

};

class critter:public Object{
	//Low and behold its about time, the CRITTER CLASS is upon us!!!! Yeeehar!
	public:
	int SEGMENTS;
	GLfloat speed; 

	GLfloat segment_roll[8];
	GLfloat segment_separation[8];
	GLfloat segment_pitch[8];
	GLfloat segment_yaw[8];
	GLfloat segment_size[8];
	GLfloat wing_angle;
	int will_frame;
	float wingsize;
	
	float w_xRot;
	float w_yRot;
	float w_zRot;
	
	//Wills flight path variables
	float start_time;
	float current_time;
	float delay;

	float random_x; 
	float random_y;
	float random_z;

	float radius;

	//Temp variables for scanning through link list
	float c_sh_x, c_sh_y, c_sh_z;
	float c_rad;

	bool collision;

	HANDLE inHandle;
	char* GunPtr;
	unsigned long waveSize, action;	


	Single_Contact_List* cr_points;

	critter(float x,float y,float z, int s){
		xPos = x; yPos = y; zPos = z;
		size = s;
		SEGMENTS = 8;
		xPos=x; yPos=y; zPos=z;
		anchored = false;
		held = false;
		object = CRITTER;
		speed = 5.0f;
		wing_angle =0.0;
		will_frame = 0;
		wingsize = 3.0;
		start_time = Time();
		random_x = .05; 
		random_y = .05; 
		random_z = .05; 
		delay = 1;
		radius = 8;
		
		for (int i=0; i<SEGMENTS; i++){
			segment_roll[i] = 0.0;
			segment_separation[i]=0.75;
			segment_pitch[i]=0.0;
			segment_yaw[i]=0.0;
			segment_size[i]= 1.5 - ((i / (SEGMENTS + 0.1)) / 2.0);
		}

		sound_init();

		cr_points = new Single_Contact_List("Critter");
		cr_points->add(x,y,z,8);
		cr_points->add(x+10,y,z,3);
		C_Points = cr_points; 
	}


	void wiggle_will(float position){

		for (int i=0; i<SEGMENTS; i++){
			//Generate Sine wave
			float adj = sin ( (position/50.0 + ((float)i/(float)SEGMENTS)) * PI ) * 2.0;
			segment_pitch[i] = adj;
			segment_separation[i] = (segment_size[i]/1.5) + fabs(adj/4.0);
			wing_angle = adj * 25.0;
		}
	}

	void display_c_points(){
		
		C_Points->n = C_Points->head->next;

		while ( C_Points->n->next != 0){ // while there are contact points scan them.
		
			glPushMatrix();
				C_Points->n = C_Points->n->next;
		
				glColor3f(0,1,0);
				glTranslatef(C_Points->n->x,C_Points->n->y,C_Points->n->z);
				glutSolidSphere(C_Points->n->radius, 10, 10);	
			glPopMatrix();
		}
	}

	void fly_will(){	

		current_time = Time();
		
		while( current_time > ( start_time + delay) ){

			start_time = Time();
			delay = rand() % 20;
			random_x = rand() % 1000 - 500;
			random_x = random_x / 2000;
			random_y = rand() % 1000 - 500;
			random_y = random_y / 2000;
			random_z = rand() % 1000 - 500;
			random_z = random_z / 2000;
		}


		xPos += random_x;
		yPos += random_y;
		zPos += random_z;

/*		// limit movement outside the world 
		if (xPos > 200 || xPos < -200){
			xPos -= random_x;
			random_x = 0;
		}		

		if (yPos > 100 || yPos < 0){
			yPos -= random_y;
			random_y = 0;
		}		

		if (zPos > 100 || zPos < -100){
			zPos -= random_z;
			random_z = 0;
		}		
*/
		C_Points->n = C_Points->head->next;

		while ( C_Points->n->next != 0){ // while there are contact points scan them.
		
			C_Points->n = C_Points->n->next;
			C_Points->n->x+=random_x;
			C_Points->n->y+=random_y;
			C_Points->n->z+=random_z;
		}			

	}
		
	float calc_rotation(float x, float z){
		float theta = tanh(z/x);
		theta = theta * 180/PI;
		return theta;
	}

	void display(){
	
		quad2 = gluNewQuadric();
		quad3 = gluNewQuadric();
		wiggle_will(will_frame+=5);
		fly_will();

		if(selected == Current_Selected_Object){
		
			if(Remove_Selected_Object){
				remove = true;
				Remove_Selected_Object = false;
			}

			if(Hide_Selected_Object){
				visible = false;
				Hide_Selected_Object = false;
			}

			if(Show_Selected_Object){
				visible = true;
				Show_Selected_Object = false;
			}

		}

		if (!visible)
		return;

		int sensor;
		float x_total=0, y_total=0, z_total=0;	//This is for totaling the movement of the contact spheres to apply at the end of loop
		static Single_Contact_List* C  = new Single_Contact_List("Temp");

		C->n = C_Points->head->next;

		static Single_Object_List* T = new Single_Object_List("Temp");
		T->n = Object_List.n;

		while ( C->n->next != 0){ // while there are contact points scan them.
		
			C->n = C->n->next;
		
			for (sensor = 0;sensor <8;sensor+=2){
			
				collision  = check_for_collision(C->n->x,C->n->y,C->n->z,glove.s[sensor][0],glove.s[sensor][1],glove.s[sensor][2],C->n->radius,0.75);

				if(!anchored && collision &&!held){
				
					if ( glove.s[sensor][0] > xPos){
						x_total-=0.5;	
					}

					if ( glove.s[sensor][0] < xPos){
						x_total+=0.5;
					}

					if ( glove.s[sensor][1] > yPos){
						y_total-=0.5;		
					}
					if ( glove.s[sensor][1] < yPos){
						y_total+=0.5;		
					}
				
					if ( glove.s[sensor][2] > zPos){
						z_total-=0.5;		
					}
				
					if ( glove.s[sensor][2] < zPos){
						z_total+=0.5;		
					}
					
				}
				
				if(!anchored && held){
				float t_x;
				float t_y;

				t_y = radius * cos(glove.xRot*PI/180);
				t_x = radius * sin(glove.xRot*PI/180);

				x_total = glove.xTrans - xPos - t_x;
				y_total = glove.yTrans - yPos - t_y;
				z_total = glove.zTrans - zPos;

				}
				
				collision = false;
			}//end of for

			// now check for other object in the world .... !!!!!
			
			// Check the Object List and see if we've hit anything.

			Object_List.n = Object_List.head->next;

			while ( Object_List.n->next != 0){ /// while there are objects, scan through them 
				Object_List.n = Object_List.n->next;

				Object_List.n->obj->C_Points->n = Object_List.n->obj->C_Points->head->next;
						
				while ( Object_List.n->obj->C_Points->n->next != NULL){ // while there are more contact points scan through them.
					
					Object_List.n->obj->C_Points->n = Object_List.n->obj->C_Points->n->next;

					c_sh_x = Object_List.n->obj->C_Points->n->x; //to hold x value of contact sphere etc
					c_sh_y = Object_List.n->obj->C_Points->n->y;
					c_sh_z = Object_List.n->obj->C_Points->n->z;
					c_rad = Object_List.n->obj->C_Points->n->radius;

					collision  = check_for_collision(c_sh_x,c_sh_y,c_sh_z,C->n->x,C->n->y,C->n->z,c_rad,C->n->radius);
					
					if ( Object_List.n->obj->C_Points->name != C_Points->name ){

					if(Object_List.n->obj->object == BULLET && collision)
						PlaySound((LPCSTR)GunPtr, NULL, SND_MEMORY | SND_ASYNC);
						
						if(collision && !anchored){

							if ( c_sh_x > xPos){
								x_total-=0.5;	
							}

							if ( c_sh_x < xPos){
								x_total+=0.5;
							}

							if ( c_sh_y > yPos){
								y_total-=0.5;		
							}
							if ( c_sh_y < yPos){
								y_total+=0.5;		
							}
						
							if ( c_sh_z > zPos){
								z_total-=0.5;		
							}
						
							if ( c_sh_z < zPos){
								z_total+=0.5;		
							}
						}
						collision= false;
					}
					
				}
				
			}

		}// end of while

		xPos += x_total;
		yPos += y_total;
		zPos += z_total;

		C_Points->n = C_Points->head->next;

		while ( C_Points->n->next != 0){ // while there are contact points scan them.
		
			C_Points->n = C_Points->n->next;
			C_Points->n->x+=x_total;
			C_Points->n->y+=y_total;
			C_Points->n->z+=z_total;
		}			
					

		printf(" I'm here %f, %f, %f \n",xPos,yPos,zPos);

		glPushMatrix();

			glTranslatef(xPos,yPos,zPos);
			glRotatef((calc_rotation(random_x,random_z)),0.0, 1.0, 0.0);
			glTranslatef(-xPos,-yPos,-zPos);

			glPushMatrix();
				glTranslatef(xPos, yPos, zPos);	// move to next segment
				//Draw quadric spheres
				for (int i=0; i<SEGMENTS; i++){
					glTranslatef(segment_separation[i], 0.0, 0.0);	// move to next segment
					glRotatef(segment_pitch[i], 0.0, 0.0, 1.0);		// pitch this angle
					glColor3fv(green);
					gluSphere(quad2, segment_size[i], 8, 8);
	
					if (i==0) {
						// Draw Eyes...
						glPushMatrix();
							glColor3fv(white);
							glRotatef(30, 0.0, 1.0, -0.5);
							glTranslatef(-segment_size[i] + 0.1, 0.0, 0.0);
							gluSphere(quad2, .4, 5, 5);
							glTranslatef(-0.3, 0.0, 0.0);
							glColor3fv(black);
							gluSphere(quad2, .25, 5, 5);
						glPopMatrix();
					
						//Right eye
						glPushMatrix();	   
							glColor3fv(white);
							glRotatef(30, 0.0, -1.0, -0.5);
							glTranslatef(-segment_size[i] + 0.1, 0.0, 0.0);
							gluSphere(quad2, .4, 5, 5);
							glTranslatef(-0.3, 0.0, 0.0);
							glColor3fv(black);
							gluSphere(quad2, .25, 5, 5);
						glPopMatrix();

						//Draw in beak.
						glPushMatrix();
							glRotatef(30, 0.0, 0.0, 1.0);
							glTranslatef(-segment_size[i] + 0.86 - 0.5, -0.1, 0.0);
							glRotatef(90, 1.0, 0.0, 0.0);
							glColor3fv(yellow);
							glutSolidTorus(0.44, 0.44, 40, 5);
						glPopMatrix();
						}

					if (i==3){
						
						gluQuadricOrientation(quad3, GLU_INSIDE); 
						gluQuadricDrawStyle(quad3, GLU_FILL);
						glPushMatrix();
							glRotatef(90 + wing_angle, 1.0 , 0.0, 0.0 );	// flap the wing
							// now move it to the size of the segment
							glTranslatef(0.0, segment_size[i] + wingsize - 0.2 , 0.0 );      
							glColor3fv(magenta);
							gluDisk( quad3, wingsize, 5, 16, 1);		// draw wing
						glPopMatrix();

						// do it again for the other wing...
						glPushMatrix();
							glRotatef(90 - wing_angle, 1.0 , 0.0, 0.0 );	// flap the wing
							// now move it to the size of the segment
							glTranslatef(0.0, - segment_size[i] - wingsize + 0.2 , 0.0 );      
							glColor3fv(magenta);
							gluDisk( quad3, wingsize, 5, 16, 1);		// draw first bit of wing
						glPopMatrix();
					}
					
				}


				gluDeleteQuadric(quad2);
				gluDeleteQuadric(quad3);
				glPopMatrix();
			glPopMatrix();
			Object_List.n = T->n;
		}

	void sound_init(){

	TCHAR WaveName[] = "SCREAM.WAV";

	/* Open the WAVE file */
	inHandle = CreateFile("SCREAM.WAV", GENERIC_READ,FILE_SHARE_READ, 0, OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN, 0);
	/* Get the size of the file */
	waveSize = GetFileSize(inHandle, 0);
	/* Allocate some memory to load the file */
	GunPtr = (char *)VirtualAlloc(0, waveSize, MEM_COMMIT,PAGE_READWRITE);
	/* Read in WAVE file */
	ReadFile(inHandle, GunPtr, waveSize, &action, 0);
	/* Free the memory */
	VirtualFree(GunPtr, waveSize, MEM_FREE);
	/* Close the WAVE file */
	CloseHandle(inHandle);
	
	return;
	}



};
	
void makeRasterFont(void)
{
   GLuint i, j;
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

   fontOffset = glGenLists (128);
   for (i = 0,j = 'A'; i < 26; i++,j++) {
      glNewList(fontOffset + j, GL_COMPILE);
      glBitmap(8, 13, 0.0, 2.0, 10.0, 0.0, letters[i]);
      glEndList();
   }
   glNewList(fontOffset + ' ', GL_COMPILE);
   glBitmap(8, 13, 0.0, 2.0, 10.0, 0.0, space);
   glEndList();
  
}


void printString(char *s)
{
   glPushAttrib (GL_LIST_BIT);
   glListBase(fontOffset);
   glCallLists(strlen(s), GL_UNSIGNED_BYTE, (GLubyte *) s);
   glPopAttrib ();
}

void menu_check(){

	count = glove.zStep + 1;

	if ( count > 4 )
		count = 4;

	if (count < 0 )
		count = 0;

	if(count==0){
		for(j = 0;j<15;j++)butcolour[j]=1;
		butcolour[1]=0; 
		butcolour[2]=0; 
		choice=0;
	}

	if(count==1){
		for(j = 0;j<15;j++)butcolour[j]=1;
		butcolour[4]=0; 
		butcolour[5]=0; 
		choice=1;
	}

	if(count==2){
		for(j = 0;j<15;j++)butcolour[j]=1;
		butcolour[7]=0; 
		butcolour[8]=0; 
		choice=2;
	}

	if(count==3){
		for(j = 0;j<15;j++)butcolour[j]=1;
		butcolour[10]=0; 
		butcolour[11]=0; 
		choice=3;
	}

	if(count==4){
		for(j = 0;j<15;j++)butcolour[j]=1;
		butcolour[13]=0; 
		butcolour[14]=0; 
		choice=4;
	}
}

void print_menu(int num_boxes){
	
	GLfloat colour[10][3];
    
	for(int i=0;i<num_boxes;i++){
		colour[i][0]=butcolour[i*3]; 
		colour[i][1]=butcolour[i*3+1]; 
		colour[i][2]=butcolour[i*3+2];
	}

	float m1x1_init=.5;
	float m1y1_init= 0;
	float m1x2_init= 2.5;
	float m1y2_init= .45;
	
	glMatrixMode( GL_PROJECTION );

	glPushMatrix();
		glLoadIdentity();
		gluOrtho2D( 0.0, 10.0, 0.0, 10.0 );

		glMatrixMode( GL_MODELVIEW );
		glPushMatrix();
			glLoadIdentity();
							
			for(i=0;i<num_boxes;i++){
				glColor3fv (colour[i]);
				glRectf (m1x1_init, m1y1_init + .5, m1x2_init, m1y2_init+.75);
				m1y1_init +=.75;
				m1y2_init +=.75;
			}
				
			glColor3f(0.1953125, 0, 1.0);
			glRectf (m1x1_init, m1y1_init + .5, m1x2_init, m1y2_init+.75);
			m1y1_init +=.75;
			m1y2_init +=.75;
			
			m1x1_init=.55;
			m1y1_init= 0.05;
			m1x2_init= 2.45;
			m1y2_init= .4;
			
			for(i=0;i<6;i++){
					
				glColor3f (0.7734375, 0.76171875, 0.7734375);
				glRectf (m1x1_init, m1y1_init + .5, m1x2_init, m1y2_init+.75);
				m1y1_init +=.75;
				m1y2_init +=.75;
			}

			glColor3f(0.1953125, 0, 1.0);
			glRasterPos2f(0.65,4.5);
			printString(current_text[0]);

			glColor3f(0, 0, 0);
			glRasterPos2f(0.65,3.75);
			printString(current_text[1]);
			
			glRasterPos2f(0.65,3.0);
			printString(current_text[2]);
			
			glRasterPos2f(0.65,2.25);
			printString(current_text[3]);
		
			glRasterPos2f(0.65,1.5);
			printString(current_text[4]);
		
			glRasterPos2f(0.65,0.75);
			printString(current_text[5]);
		
		glPopMatrix();
			
		glMatrixMode( GL_PROJECTION );
		
	glPopMatrix();
	
	glMatrixMode( GL_MODELVIEW );

}

void init(void) 
{
	init_glove();
	setup_texture_mapping();	
	init_menu();
	setup_lighting();
}

void display_camera_values()
{
	//Camera Coordinates
	printf("\n--------------------\n");
    printf(" Camera Coordinates:\n");
    printf(" eyeX = %f \n", eyex);
    printf(" eyeY = %f \n", eyey);
    printf(" eyeZ = %f \n", eyez);
	printf("\n");
	printf(" centreX = %f \n", centrex);
    printf(" centreY = %f \n", centrey);
    printf(" centreZ = %f \n", centrez);
    
	return;
}

void draw_floor(){

   glPushMatrix();
	   glScalef (3.0, 1.0, 2.5);
	   glColor3f(1.0, 1.0, 1.0);

	   glBegin(GL_LINE_LOOP);	//Outline of floor
			glVertex3f(-100.0, 0.0, 100.0);
			glVertex3f(-100.0, 0.0,-100.0);
			glVertex3f( 100.0, 0.0,-100.0);
			glVertex3f( 100.0, 0.0, 100.0);
	   glEnd();
   
   
	   glBegin(GL_LINES);		//perspective lines of floor
		   for (int i=-100; i < 100; i+=5){	
				glVertex3f(i,  0.0,   100.0);
				glVertex3f(i,  0.0,  -100.0);
		   }

		   for (i=-100; i < 100; i+=5){	
	 			glVertex3f(-100,  0.0, i);
				glVertex3f(100,  0.0, i);
		   }
	   glEnd();
   glPopMatrix();
}

void fixed_world()
{
	
	Object_List.n = Object_List.head->next;

	while ( Object_List.n->next != 0){ // while there are more keys print them out.
	Object_List.n = Object_List.n->next;
	
	Object_List.n->obj->display();
	
	if ( show_spheres )	Object_List.n->obj->display_c_points();
		if(Object_List.n->obj->remove)
			Object_List.remove(Object_List.n->obj);
	}

	// draw hand ...
	glove.display();
	



}

bool check_for_collision(float ob1xPos, float ob1yPos, float ob1zPos, float ob2xPos, float ob2yPos, float ob2zPos, float ob1Rad, float ob2Rad)
{
	bool xycollision = false, zycollision = false;
	float length;

	length = distance(ob1xPos, ob1yPos, ob2xPos, ob2yPos);
	
	if (length < ob1Rad+ ob2Rad){
		xycollision = true;
	}

	length = distance(ob1yPos, ob1zPos, ob2yPos, ob2zPos);
	
	if (length < ob1Rad+ ob2Rad){
		zycollision = true;
	}

	if ( xycollision && zycollision ) 
		return true;
	else
		return false;
	
}

void display(void)
{

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 	glLoadIdentity();
    
	gluLookAt (eyex, eyey, eyez, centrex, centrey ,centrez , 0.0, 1.0, 0.0);
	
    // draw floor ...
   	draw_floor();
		
	//draw fixed world ...
	fixed_world();

	//draw menu...
	if (menu){
		menu_check();
		print_menu(5);

	}

	glutSwapBuffers();
	
}


void idle(void)
{
	display(); 	
}

void reshape(int w, int h)
{
	glViewport (0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 1.0, -100.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position); 
}

void keyboard (unsigned char key, int x, int y)
{
   switch (key) {
    
    //X, Y, Z, transformation of hand
	case 'a':
			glove.xTrans += 2;
			centrex +=2;
			eyex +=2;
			shoot = true;
	break;

	case 'A':
			glove.xTrans -= 2;
			centrex -=2;
			eyex -=2;
	break;

	case 's':
			glove.yTrans += 2;
			centrey +=2;
			eyey +=2;
	break;

	case 'S':
			if(glove.yTrans >2){
				glove.yTrans -= 2;
				centrey -=2;
				eyey -=2;
		}
	break;

	case 'd':
			glove.zTrans += 2;
			centrez +=2;
			eyez +=2;
	break;

	case 'D':
			glove.zTrans -= 2;
			centrez -=2;
			eyez -=2;
	break;

	//Camera movement - Eye
	case 'r':
		eyex=eyex + 1.0;
		break;
	
	case 'R':
		eyex=eyex - 1.0;
		break;
	
	case 't':
		eyey=eyey + 1.0;
		break;
	
	case 'T':
		eyey=eyey - 1.0;
		break;
	
	case 'y':
		eyez=eyez + 1.0;
		break;
	
	case 'Y':
		eyez=eyez - 1.0;
		break;
	
	case 'c':
		eyex = 0; 
		eyey = 20; 
		eyez = 30; 
		centrex = 0;
		centrey = 0;
		centrez = 0;
		glove.xTrans = 0; 
		glove.yTrans = 0; 
		glove.zTrans = 0; 		
	break;


	
	case 27:
	//	glutLeaveGameMode();
		exit(0);
	break;

	default:
	break;

   }

}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	//Set depth recognition, and enable hidden surface recognition
	glutInitDisplayMode (GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
	
	//Define window parameteres

	// 640x480, 16bit pixel depth, 60Hz refresh rate
	//glutGameModeString( "800x600:16@85" );

	// start fullscreen game mode
	//glutEnterGameMode();

	glutInitWindowSize (800,600); 
	glutInitWindowPosition (100, 100);
	glutCreateWindow (argv[0]);   

	//HARDCORE!!!!! of program
	init (); 
	glutDisplayFunc(display); 
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
	return 0;

}

 

float distance(float x1, float y1, float x2, float y2)
{
	float sum ,sqroot;
	sum = (x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2);
	sqroot =  sqrt(sum);
	return sqroot;
}

float Time() // returns the current system time in seconds.
{

	int Secs1,Secs2,Mins1,Mins2;
	float time;
	struct _timeb tstruct;
	float milli;
	char ctime[8];

	_strtime( ctime );
	_ftime(&tstruct);

	Secs1 = ctime[7]-48;Secs2 = ctime[6]-48;
	Mins1 = ctime[4]-48;Mins2 = ctime[3]-48;
	milli = tstruct.millitm;
	
	time = Secs1 + 10*Secs2 + 60*Mins1 + 600*Mins2 + milli/1000;
	
	return time;
 
}

void load_object(float object, float x , float y, float z, float size)
{

	if ( object == PIANO_KEY){
		Object* ky = new key(x,y,z,size);
		Object_List.add(ky);
	}

	if ( object == SPHERE){
		Object* sph = new sphere(x,y,z,size);
		Object_List.add(sph);
	}

	if ( object == CUBE){
		Object* cb = new cube(x,y,z,size);
		Object_List.add(cb);
	}

	if ( object == BOUNCING_SPHERE){
		Object* f_sph = new falling_sphere(x,y,z,size);
		Object_List.add(f_sph);
	}

	if ( object == CRITTER){
		Object* will = new critter(x,y,z,size );
		Object_List.add(will);
	}

	if ( object == BULLET){
		Object* silver = new bullet(x,y,z,size );
		Object_List.add(silver);
	}
	
	if ( object == DOMINO){
		Object* dm = new domino(x, y, z);
		Object_List.add(dm);
	}
	

	return;

}

 
void load_from_file(int world)
{
	
	clear_current_world();
	float object,x,y,z,size;

	ifstream is;

	if ( world == 1 ) is.open("World1.dat",ios::nocreate );
	else if ( world == 2 ) is.open("World2.dat",ios::nocreate );
	else if ( world == 3 ) is.open("World3.dat",ios::nocreate );
	else is.open("World4.dat",ios::nocreate );
	
	while(!is.eof()){
		is >> object >> x >> y >> z >> size;
		load_object(object,x,y,z,size);
	}

	is.close();
	return;

}


void save_to_file(int world)
{

	ofstream os;

	if ( world == 1 ) os.open("World1.dat",ios::nocreate );
	else if ( world == 2 ) os.open("World2.dat",ios::nocreate );
	else if ( world == 3 ) os.open("World3.dat",ios::nocreate );
	else os.open("World4.dat",ios::nocreate );
	
	Object_List.n = Object_List.head->next;

	while ( Object_List.n->next != 0){ // while there are more objects print them out.
	Object_List.n = Object_List.n->next;
	
	os << Object_List.n->obj->object <<" ";	
	os << Object_List.n->obj->xPos <<" ";	
	os << Object_List.n->obj->yPos <<" ";	
	os << Object_List.n->obj->zPos <<" ";	
	os << Object_List.n->obj->size <<" " << endl;	
	}

	os.close();
	return;

}

void rain()
{
	int object,x,y,z,size;
	
	object = 3;
	size = 1;

	for ( int i = 0 ; i < 40 ; i++ ){
	
	x = rand()%500 - 250;
	y = rand()%500;
	z = rand()%500 - 250;

	load_object(object,x,y,z,size);
	}

	return;
}

void set_text()
{

	for ( int i = 0; i < 6 ; i++ ){

	strcpy(current_text[i],all_text[i+ 6 * menu_choice]);
		
	}

}

void calc_menu_choice()
{


	if (menu_choice == 0){//main menu
		if(choice==4)menu_choice=1;// environment
		else if(choice==3){menu_choice=3;}//objects
		else if(choice==2){menu_choice=2;}//viewing mode
		else if(choice==1){menu=false;menu_choice=0;}//exit menu
		else if(choice==0)exit(0);// exit program
		return;
	}
	
	if (menu_choice == 1){//environment
		if(choice==2)menu_choice=0;// back
		else if(choice == 4){menu_choice = 5;}//load 
		else if(choice == 3){menu_choice = 6;}//save
		return;
	}

	if (menu_choice == 2){// viewing mode
		if(choice==0){menu_choice=0;}// back

		else if(choice==4){ // camera mode 
			menu=false;
			menu_choice=0;
			camera_mode = true;
			gun_mode=false;
		}		

		else if(choice==3){ // hand mode
			menu=false;
			menu_choice=0;
			camera_mode = false;
			gun_mode=false;
		}		

		else if(choice==2){ // centre camera
			eyex = glove.xTrans; 
			eyey = glove.yTrans + 20; 
			eyez = glove.zTrans + 30; 
			centrex = glove.xTrans;
			centrey = glove.yTrans;
			centrez = glove.zTrans; 		
		}		

		else if(choice==1){ // gun mode
			menu=false;
			menu_choice=0;

			camera_mode = false;
			if(gun_mode)gun_mode=false;
			else gun_mode=true;	
			
			glove.reload = true;
		}		

	return;
	}


	if (menu_choice == 3){//objects
		if(choice==1)menu_choice=1;
		else if(choice==4){menu_choice = 4;}// selected 
		else if(choice==3){menu_choice = 7;}// new 
		else if(choice==2){// contact points
			menu=false;
			menu_choice=0;
			if(show_spheres)show_spheres=false;
			else show_spheres=true;
		}		

	return;
	}

	if (menu_choice == 4){//selected
		if(choice==1){menu_choice=3;}// back 

		else if(choice==4){// hide
			menu=false;
			menu_choice=0;
			Hide_Selected_Object = true;

		}		

		else if(choice==3){//show
			menu=false;
			menu_choice=0;
			Show_Selected_Object = true;

		}		

		else if(choice==2){//remove
			menu=false;
			menu_choice=0;
			Remove_Selected_Object = true;
		}		

	return;
	}

	if (menu_choice == 5){//load 
		if(choice==0)menu_choice=1;// back

		else if(choice==4){// world 1
			menu=false;
			menu_choice=0;
			load_from_file(1);
		}		
	
		else if(choice==3){// world 2
			menu=false;
			menu_choice=0;
			load_from_file(2);
		}		
		
		else if(choice==2){// world 3
			menu=false;
			menu_choice=0;
			load_from_file(3);
		}		

		else if(choice==1){// world 4
			menu=false;
			menu_choice=0;
			load_from_file(4);
		}		

	return;
	}

	if (menu_choice == 6){//save
		if(choice==0)menu_choice=1;// back

		else if(choice==4){// world 1
			menu=false;
			choice=1;menu_choice=0;
			save_to_file(1);

		}		
	
		else if(choice==3){// world 2
			menu=false;
			choice=1;menu_choice=0;
			save_to_file(2);

		}		
		
		else if(choice==2){// world 3
			menu=false;
			choice=1;menu_choice=0;
			save_to_file(3);

		}		
		
		else if(choice==1){// world 3
			menu=false;
			choice=1;menu_choice=0;
			save_to_file(4);

		}		
	
	return;
	}

	if (menu_choice == 7){//new object 
		if(choice==0)menu_choice=3;

		else if(choice==4){
			menu=false;
			choice=1;menu_choice=0;
			load_object(BOUNCING_SPHERE,glove.xTrans,glove.yTrans,glove.zTrans - 7.1,7);
		}		
	
		else if(choice==3){
			menu=false;
			choice=1;menu_choice=0;
			load_object(SPHERE,glove.xTrans,glove.yTrans,glove.zTrans - 5.1,5);
		}		
		
		else if(choice==2){
			menu=false;
			choice=1;menu_choice=0;
			load_object(CUBE,glove.xTrans,glove.yTrans,glove.zTrans - 5,9);
		}		

		else if(choice==1){
			menu=false;
			choice=1;menu_choice=0;
			load_object(CRITTER,glove.xTrans,glove.yTrans,glove.zTrans - 5.1,5);
		}		

	
	return;
	}


	if (menu_choice == 9){//ambient
		
		if(choice==4){
			if (ambient_strength < 1.0){ ambient_strength += 0.1;}
			setup_lighting();
			
		}		
	
		else if(choice==3){
			if (ambient_strength > 0.0){ ambient_strength -= 0.1;}
			setup_lighting();
		}		
		
		else if(choice==2){
			menu_choice=3;
		}		
	
	return;
	}

	if (menu_choice == 10){//diffuse
		
		if(choice==4){
			if (diffuse_strength< 1.0){diffuse_strength += 0.1;}
			setup_lighting();
		}		
	
		else if(choice==3){
			if (diffuse_strength > 0.0){diffuse_strength -= 0.1;}
			setup_lighting();
		}		
		
		else if(choice==2){
			menu_choice=3;
		}		
	
	return;
	}

	if (menu_choice == 11){//specular

		
		if(choice==4){
			if (specular_strength< 1.0){specular_strength+=0.1;}
			setup_lighting();
		}		
	
		else if(choice==3){
			if (specular_strength > 0.0){specular_strength-=0.1;}
			setup_lighting();
		}		
		
		else if(choice==2){
			menu_choice=3;
		}		
	
	return;
	}

}


void setup_lighting()
{

	glDisable(GL_LIGHTING);

	GLfloat light_ambient[] = { ambient_strength,ambient_strength,ambient_strength, 1.0 };
	GLfloat light_diffuse[] = { diffuse_strength,diffuse_strength,diffuse_strength, 1.0 };
	GLfloat light_specular[] = { specular_strength,specular_strength,specular_strength, 1.0 };

	//Ambient lighting 
//	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient); //Position of light
//	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse); //Position of light
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular); //Position of light
	glLightfv(GL_LIGHT1, GL_POSITION, light0_position); //Position of light
	
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient); //Position of light
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse); //Position of light
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular); //Position of light
	glLightfv(GL_LIGHT0, GL_POSITION, light1_position); //Position of light
	
	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	
	glEnable(GL_LIGHTING); //Tell openGL to prepare to calculate lighting equations
	
	glEnable(GL_LIGHT0);   //Switch on the defined lights
	glEnable(GL_LIGHT1);   
	// Enable standard hidden surface removal
	glDepthFunc(GL_LEQUAL);
	
	glEnable(GL_DEPTH_TEST);

//	reshape(width,heigth);

	return;

}

void init_menu()
{
	//Initialise menu
	//Menu
	for(j = 0;j<15;j++)butcolour[j]=1;

	butcolour[1]=0;
	butcolour[2]=0;

	//Text stuff
    makeRasterFont();

	return;

}

void setup_texture_mapping()
{
	
	FILE *texf;
	char line[256];
	int xres, yres, ret, i;
	GLubyte *texdata;

	// Initialise the texture engine.  First create a
	// new texture entry for us to use:
	glGenTextures(1, &texture);

	// Now read in a .PPM format file containing the 
	// texture data 

	if((texf = fopen("b3ball256.ppm", "rb")) == (FILE *)NULL)
		fprintf(stderr, "Error opening file bball256.ppm\n");
	fgets(line, 256, texf);
	ret = sscanf(line, "P6 %d %d 255", &xres, &yres);
	texdata = (GLubyte *)malloc(xres*yres*3);
	
	for(i=0;i<xres*yres*3;i++)
		texdata[i] = (GLubyte)getc(texf);
	fclose(texf);

	// Finally load the texture into VRAM for use in the main
	// OpenGL display loop:
    glBindTexture(GL_TEXTURE_2D, texture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, xres, yres, 0, GL_RGB, GL_UNSIGNED_BYTE, texdata);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


	if((texf = fopen("dcu256.ppm", "rb")) == (FILE *)NULL)
		fprintf(stderr, "Error opening file bball256.ppm\n");
	fgets(line, 256, texf);
	ret = sscanf(line, "P6 %d %d 255", &xres, &yres);
	texdata = (GLubyte *)malloc(xres*yres*3);
	
	for(i=0;i<xres*yres*3;i++)
		texdata[i] = (GLubyte)getc(texf);
	fclose(texf);

	// Finally load the texture into VRAM for use in the main
	// OpenGL display loop:
    glBindTexture(GL_TEXTURE_2D, e_texture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, xres, yres, 0, GL_RGB, GL_UNSIGNED_BYTE, texdata);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


	// Set the texture mode so that both texturing and lighting
	// information are used simultaneously
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

	// Create the Quadric for use later on in the scene generation.
	// & enable texturing for quadrics
	quad1 = gluNewQuadric();
	gluQuadricTexture(quad1, GL_TRUE);

	// This ensures that colours are blended across the surface of
	// the polygon.
	glShadeModel(GL_SMOOTH);
	glClearColor (0.0, 0.0, 0.0, 0.0);
	return;
}

void clear_current_world() // remove all objects from the current linked list of objects...
{
	Object_List.n = Object_List.head->next;

	while ( Object_List.n->next != 0){ // while there are more keys print them out.
	Object_List.n = Object_List.n->next;
	Object_List.remove(Object_List.n->obj);
	}

}

void init_glove()
{
	// initialise glove ...
	if (!GloveInit("COM1")) {
		printf("\nUnable to initialise glove on COM1 \n");

		if (!GloveInit("COM2")) {
			printf("Unable to initialise glove on COM2 \n");
			return;
		}
	}

}


