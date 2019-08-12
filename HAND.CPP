#include <math.h>
#include <GL/glut.h>
#include "hand.h"
#include <stdio.h>

extern "C++" {
#include "coms.h"
}

	hand::hand(Single_Object_List* ob1){
		object_list = ob1;
		handrad=6.0; 
		holding = false;
		
		// initialise glove ...
		if (!GloveInit("COM1")) {
			printf("\nUnable to initialise glove on COM1 \n");

			if (!GloveInit("COM2")) {
				printf("Unable to initialise glove on COM2 \n");
				return;
			}
		}

	}

	void hand::display(){

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
		object_list->n = object_list->head->next;

		while ( object_list->n->next != 0){ /// while there are objects, scan through them 
			object_list->n = object_list->n->next;

			object_list->n->obj->C_Points->n = object_list->n->obj->C_Points->head->next;

			fore_collision = false;
			aft_collision = false;
					
			while ( object_list->n->obj->C_Points->n->next != NULL){ // while there are more contact points scan through them.
					
					object_list->n->obj->C_Points->n = object_list->n->obj->C_Points->n->next;

					c_sh_x = object_list->n->obj->C_Points->n->x; //to hold x value of contact sphere etc
					c_sh_y = object_list->n->obj->C_Points->n->y;
					c_sh_z = object_list->n->obj->C_Points->n->z;
					c_rad = object_list->n->obj->C_Points->n->radius;

					for (int sensor = 0;sensor <8;sensor+=2){
						
						finger_collision  = check_for_collision(c_sh_x,c_sh_y,c_sh_z,s[sensor][0],s[sensor][1],s[sensor][2],c_rad,0.75);
						if(finger_collision){
							fore_collision = true;
							object_list->n->obj->selected = ++Current_Selected_Object;
				
						}

						palm_collision  = check_for_collision(c_sh_x,c_sh_y,c_sh_z,s[sensor+1][0],s[sensor+1][1],s[sensor+1][2],c_rad,0.75);
						if(palm_collision){
							aft_collision = true;
						}
						
						if (aft_collision && fore_collision){
							
							if(index_finger < 90 || middle_finger < 90 || ring_finger < 90 || little_finger < 90 ){
								object_list->n->obj->held = true;
							}
					
							else
							object_list->n->obj->held = false;
	
						}
						else 
							object_list->n->obj->held = false;

						if(object_list->n->obj->anchored){

							if (aft_collision || fore_collision){

								if (object_list->n->obj->xPos < xTrans)
									xPosMov = false;
								else 
									xNegMov = false;

								if (object_list->n->obj->yPos < yTrans)
									yPosMov = false; 
								else 
									yNegMov = false;

								if (object_list->n->obj->zPos < zTrans)
									zPosMov = false;
								else 
									zNegMov = false;

							}
						}
						finger_collision= false;
						palm_collision= false;
					}
					
				/*	if(object_list->n->obj->object == CRITTER && object_list->n->obj->held){
						xTrans = object_list->n->obj->xPos+13;
						centrex = xTrans;
						eyex =xTrans+30;
						yTrans = object_list->n->obj->yPos+1;
						centrey = yTrans;
						eyey =xTrans+20;
						zTrans = object_list->n->obj->zPos;
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

 	int hand::gesture_check()
		{
		
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
						fist_made = false;
						}
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
		
		return 0;
	}

	void hand::track_finger_sensors(){
	
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

