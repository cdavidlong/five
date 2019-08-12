#include <GL/glut.h>

#include "cube.h"
#include "variables.h"
#include "s_o_list.h"
#include "o_list.h"


cube::cube(float x, float y, float z, float w){
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
	
void cube::display_c_points(){
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

void cube::display(){

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
