#include <GL/glut.h>
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

extern "C++" {
#include "variables.h"
#include "afont.h"
}

bool PRINT = false;

char * PWavHead = "bounce.wav";


class Contact_List{

	// friend functions. 
	friend void List(Contact_List&);

	public:
	
	struct contact_point{

		contact_point(){x=y=z=0;radius=1;next = 0;};
		contact_point(float _x,float _y,float _z,float _r, struct contact_point* _next){
			x = _x; y = _y; z = _z;
			radius = _r;
			next = _next;
		};

		struct contact_point* next;
		float radius;	// radius of contact sphere
		float x,y,z;	// x y z location of sphere

	};
	
	struct contact_point *n; // initial node.
	struct contact_point *head; // head of list
	int size;// size of list

	// default constructer.
	Contact_List(){
		n = new contact_point(0,0,0,0,NULL);
		head = new contact_point(0,0,0,0,n);
		size = 0;
		}

	// destructer
	~Contact_List(){delete head;delete n;}

	// virtual functions ...
	virtual void add(float,float,float,float) = 0;	 
//	virtual void remove(TYPE key) = 0;


};

//template <class TYPE> 
//void List(Contact_List<TYPE> &l)
void List(Contact_List &l)
{
	l.n = l.head->next;

	while ( l.n->next != 0){ // while there are more keys print them out.
	l.n = l.n->next;
	printf("\nContact points : %f,%f,%f ",l.n->x,l.n->y,l.n->z);
//	l.n->obj->display();
	}
	return;
}

/////////////////////////
// Single_Contact_List Linked List. //
/////////////////////////

//template <class T>
class Single_Contact_List:public Contact_List{//<T>{
	
	public:

		Single_Contact_List(char* n="Default"){init(n);} // default constructor
		Single_Contact_List(Single_Contact_List& s){init((char*)s);}  // copy constructor
	
		~Single_Contact_List(){delete name,t,b;} // destructor

		Single_Contact_List& operator=(Single_Contact_List& s){  // over-rides = operator
			init((char*)s); return * this; 
		}
	
	//vitual functions ...
	// Add .. add key to linked list.
		void add(float _x,float _y,float _z,float _r){
			t = head->next;
			while (t->next != 0)t=t->next;
			b = new contact_point(_x,_y,_z,_r,0);
			t->next = b;
			size++;
		}

/*    // remove .... removes key from list.
	void remove(T key){
		t = head->next;
		if (t->key == key){
			head->next = t->next;
			size--;}

		else{
		while (t->next!= NULL){
		b = t->next;
		if ( b->key == key ){t->next = b->next;size--;}
		if ( t->next != NULL)t = t->next;}
		}
	}
*/
	// overloads (char*) casting operator ... 
	operator char*() {return name;}

	char* name; // name of list

	// stores name passed in on contruction.
	void init(char* n){
	name = new char[strlen(n)+1];strcpy(name,n);
	}

	struct contact_point* t;
	struct contact_point* b;

};


class Object{

public:

	float xPos,yPos,zPos;
	bool anchored;
	Single_Contact_List* C_Points;

	Object(){ xPos = 0; yPos = 0; zPos = 0; anchored = false;}

	virtual void display() = 0;

};



// Base Class of Linked List.
// Abstract class ..... has virtual funtions -> cannot create instance of it.

//template <class TYPE> // template class -> can create linked list of various types.
class Object_List{

	// friend functions. 
	friend void List(Object_List&);

	public:
		
	struct list_item{		 

		bool active;    // status of object
		bool moving;	// other flags for direction ?? 

		Object* obj;   // object.
		struct list_item* next;
		list_item(){
			active  = true;
			moving = false;
			obj = NULL;
			next = NULL;
		};
		
		list_item(bool a,bool b,Object* ob1,struct list_item* s){
			active = a;
			moving = b;
			obj = ob1;
			next = s;
		};

	};

	struct list_item *n; // initial node.
	struct list_item *head; // head of list
	int size;// size of list

	// default constructer.
	Object_List(){
		n = new list_item(true,false,NULL,NULL);
		head = new list_item(true,false,NULL,n);
		size = 0;
		}

	// destructer
	~Object_List(){delete head;delete n;}

	// virtual functions ...
	virtual void add(Object* key) = 0;	 
//	virtual void remove(TYPE key) = 0;


};

void List(Object_List &l)

{
	l.n = l.head->next;

	while ( l.n->next != 0){ // while there are more keys print them out.
	l.n = l.n->next;
	l.n->obj->display();
	}
	return;
}

/////////////////////////////////////
// Single_Object_List Linked List. //
/////////////////////////////////////

class Single_Object_List:public Object_List{//<T>{
	
	public:

		Single_Object_List(char* n="Default"){init(n);} // default constructor
		Single_Object_List(Single_Object_List& s){init((char*)s);}  // copy constructor
	
		~Single_Object_List(){delete name,t,b;} // destructor

		Single_Object_List& operator=(Single_Object_List& s){  // over-rides = operator
			init((char*)s); return * this; 
		}
	
	//vitual functions ...
	// Add .. add key to linked list.
		void add(Object* key){
			t = head->next;
			while (t->next != 0)t=t->next;
			b = new list_item(true,false,key,0);
			t->next = b;
			size++;
		}

/*    // remove .... removes key from list.
	void remove(T key){
		t = head->next;
		if (t->key == key){
			head->next = t->next;
			size--;}

		else{
		while (t->next!= NULL){
		b = t->next;
		if ( b->key == key ){t->next = b->next;size--;}
		if ( t->next != NULL)t = t->next;}
		}
	}
*/
	// overloads (char*) casting operator ... 
	operator char*() {return name;}

	char* name; // name of list
	// stores name passed in on contruction.
	void init(char* n){
	name = new char[strlen(n)+1];strcpy(name,n);
	}

	struct list_item* t;
	struct list_item* b;

};


Single_Object_List Object_List("My Single_Object_List List");


class hand:public Object{
	public:
	
	//Hand rotation and transformation values
	GLfloat xRot, xRaw, xTrans;
	GLfloat yTrans;
	GLfloat zRot, zRaw, zTrans;
	bool collision;

	float handrad; 

	float index_finger;		//Finger flexure values
	float middle_finger;
	float ring_finger;
	float little_finger;
	float thumb_joint;

	//Temp variables for scanning through link list
	float ob_x, ob_y, ob_z;
	float c_sh_x, c_sh_y, c_sh_z;
	float c_rad;
	
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

	hand(){
		handrad=6.0; 
	}

	void display(){

		/// Check the Object List and see if we've hit anything.

			Object_List.n = Object_List.head->next;

			while ( Object_List.n->next != 0){ /// while there are objects, scan through them 
				Object_List.n = Object_List.n->next;

				Object_List.n->obj->C_Points->n = Object_List.n->obj->C_Points->head->next;
						
			while ( Object_List.n->obj->C_Points->n->next != NULL){ // while there are more keys print them out.
					
					Object_List.n->obj->C_Points->n = Object_List.n->obj->C_Points->n->next;

					c_sh_x = Object_List.n->obj->C_Points->n->x; //to hold x value of contact sphere etc
					c_sh_y = Object_List.n->obj->C_Points->n->y;
					c_sh_z = Object_List.n->obj->C_Points->n->z;
					c_rad = Object_List.n->obj->C_Points->n->radius;

		//			printf("Contact points are: %f,%f,%f \n",Object_List.n->points->n->x,Object_List.n->points->n->y,Object_List.n->points->n->z);
	
					for (int sensor = 0;sensor <8;sensor++){
						
						if (PRINT){
							printf("\nHand values ,%f,%f,%f",s[0][0],s[0][1],s[0][2]);
							printf("\nBall values ,%f,%f,%f,%f\n" ,c_sh_x, c_sh_y, c_sh_z, c_rad);
							
							PRINT = false;
						}
						collision  = check_for_collision(c_sh_x,c_sh_y,c_sh_z,s[sensor][0],s[sensor][1],s[sensor][2],c_rad,0.75);
						if(collision)
							printf("OOoooppss I have collided with a %s \n",Object_List.n->obj->C_Points->name);
						collision = false;
					}

				}
			}

			


		
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseMaterial);
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
		
		// palm  ...
		glPushMatrix();	

			glTranslatef(xTrans, yTrans, zTrans);
		
			glRotatef(90, 0.0, 1.0, 0.0);
			glRotatef(xRot, 1.0, 0.0, 0.0); 

			glColor3f(0.7996, 0.6549, 0.45); 
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
						glutWireCube (1.0);	
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
						glutWireCube (1.0);			//Finger Tip
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
						glutWireCube (1.0);			//Finger Tip
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
						glutWireCube (1.0);			//Finger Tip
					glPopMatrix();

				glPopMatrix();// end of little finger

			glPopMatrix(); // end of fingers.

		glPopMatrix(); // end of hand.

	}

	int gesture_check()
		{
		
		xRaw = (float)gloveData[5];
		zRaw = (float)gloveData[6];
		
		if(index_finger < 90 ){
			
			if(middle_finger < 150){
				
				if(ring_finger > 200){
				
					if(little_finger > 200){	
						
						if ( zRaw < 90 ){
							if(yNegMov){
								yTrans -= 1;
								centrey -=1;
								eyey -=1;
							}
							//collision = check_for_collision();
							
							return 1;
						}
				
						if ( zRaw > 130 ){
							if(yPosMov){
								yTrans += 1;
								centrey +=1;
								eyey +=1;
							}
							//collision = check_for_collision();
							return 1;
						}
											
						
					}
				}
			}
				
			if(middle_finger > 200){	
			
				if(ring_finger > 200){
				
					if(little_finger > 200){
									
						if ( zRaw < 90 ){
							if(zNegMov){
								zTrans -= 1;
								centrez -=1;
								eyez -=1;
							}
							return 1;
						}
						if ( zRaw > 130 ){
							if(zPosMov){
								zTrans += 1;
								centrez +=1;
								eyez +=1;
							}
							return 1;
						}
						if ( xRaw < 100 ){
							if(xNegMov){
								xTrans -= 1;
								centrex -=1;
								eyex -=1;
							}
							//collision = check_for_collision();
							return 1;
						}
				
						if ( xRaw > 160 ){
							if(xPosMov){
								xTrans += 1;
								centrex +=1;
								eyex +=1;
							}
							return 1;
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

		s[1][0] =	s[0][0] = xTrans + temp_x;  
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
		
		temp_x = j4_index[1] * sin(xRot*PI/180);
		temp_y = j4_index[1] * cos(xRot*PI/180);

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

};

hand glove;

class sphere:public Object{
	//Low and behold its about time, the SPHERE CLASS is upon us!!!! Yeeehar!
	public:
	
	float radius;
	float colour[3];
	bool anchored;
	bool collision;

			
	sphere(){
		xPos=5; yPos=2; zPos=-10;
		anchored=false;
		colour[0] = 0.0;
		colour[1] = 0.0;
		colour[2] = 1.0;
		radius = 4;
	}
	
	//sphere(float x,float y,float z, Single_Contact_List* c_list){
	sphere(float x,float y,float z, Single_Contact_List* c_list){
		xPos=x; yPos=y; zPos=z;
		anchored=false;
		colour[0] = 0.0;
		colour[1] = 0.0;
		colour[2] = 1.0;
		radius = 4;
		C_Points = c_list; 
		
	}
	
	void display(){
		
		glPushMatrix();

//		int sensor;
/*
		for (sensor = 0;sensor <8;sensor++){
			
			collision  = check_for_collision(xPos,yPos,zPos,glove.s[sensor][0],glove.s[sensor][1],glove.s[sensor][2],radius,0.75);
			
			sndPlaySound ((char *)PWavHead, SND_MEMORY | SND_LOOP | SND_ASYNC); 

			if(!anchored && collision){

				if ( glove.s[0][0] > xPos)
					xPos-=0.5;	

				if ( glove.s[0][0] < xPos)
					xPos+=0.5;

				if ( glove.s[0][1] > yPos)
					yPos-=0.5;		
			
				if ( glove.s[0][1] < yPos)
					yPos+=0.5;		
			
				if ( glove.s[0][2] > zPos)
					zPos-=0.5;		
			
				if ( glove.s[0][2] < zPos)
					zPos+=0.5;		
			}

			collision = false;
			
		}
*/
	glColor3fv(colour);
	glTranslatef(xPos, yPos, zPos);
	glutSolidSphere(radius, 10, 10);	
	glPopMatrix();
	}
	

};

/*
class cube:public Object{
	//Low and behold its about time, the CUBE CLASS is upon us!!!! Yeeehar!
	public:
	
	float xPos;
	float yPos;
	float zPos;
	float width;
	float colour[3];
	bool anchored;
	bool collision;
	
	Single_Contact_List* c_list

	cube(){
		xPos=-5; yPos=2; zPos=-10;
		anchored=false;
		colour[0] = 1.0;
		colour[1] = 0.0;
		colour[2] = 1.0;
		width = 4;
	
	}

	cube(){
		xPos=-5; yPos=2; zPos=-10;
		anchored=false;
		colour[0] = 1.0;
		colour[1] = 0.0;
		colour[2] = 1.0;
		width = 4;
	
	}
	
	void display(){
		
		glPushMatrix();
		collision  = check_for_collision(xPos, yPos, zPos, s[0][0], s[0][1], s[0][2], width/2, 0.75);
		
		if(collision){	
			colour[0] = 0.0;
			colour[1] = 1.0;
			colour[2] = 0.0;
		}

		else {
			colour[0] = 1.0;
			colour[1] = 0.0;
			colour[2] = 1.0;
		}

		if(!anchored && collision){
			
			if ( s[0][0] > xPos)
				xPos-=0.5;	

			if ( s[0][0] < xPos)
				xPos+=0.5;

			if ( s[0][1] > yPos)
				yPos-=0.5;		
		
			if ( s[0][1] < yPos)
				yPos+=0.5;		
		
			if ( s[0][2] > zPos)
				zPos-=0.5;		
		
			if ( s[0][2] < zPos)
				zPos+=0.5;		
		
		}

		
	glColor3fv(colour);
	glTranslatef(xPos, yPos, zPos);
	glutSolidCube(width);	
	glPopMatrix();
	}
};

  
class key:public Object{
	//Low and behold its about time, the KEY CLASS is upon us!!!! Yeeehar!
	public:
	
	float xPos;
	float yPos;
	float zPos;
	float xContact;
	float yContact;
	float zContact;

	float c_keyrot;
	float radius;
	float colour[3];
	bool anchored;
	bool collision;

	key(){
		xPos=0; yPos=2; zPos=-5;
		xContact=xPos; yContact=yPos; zContact=zPos+4;
		c_keyrot=0;
		anchored=false;
		colour[0] = 1.0;
		colour[1] = 0.0;
		colour[2] = 1.0;
		radius = 0.5;
	}
	
	key(float x,float y,float z){
		xPos=x; yPos=y; zPos=z;
		xContact=xPos; yContact=yPos; zContact=zPos+4;

		anchored=false;
		colour[0] = 1.0;
		colour[1] = 0.0;
		colour[2] = 1.0;
		radius = 0.5;
	}
	
	
	void display(){
		
		glPushMatrix();
		collision  = check_for_collision(xContact, yContact, zContact, s[0][0], s[0][1], s[0][2], radius, 0.75);
		
			colour[0] = 1.0;
			colour[1] = 1.0;
			colour[2] = 1.0;
		
		if(!anchored && collision){
			
			if ( s[0][1] < yPos){
				if(c_keyrot <15)
					c_keyrot+=5;
				yContact+=0.69724;
			
			}
			if ( s[0][1] > yPos){
				if(c_keyrot !=0)
					c_keyrot-=5;
				yContact-=0.69724;
			}
		}
		
		glColor3fv(colour);
		glPushMatrix();
			glTranslatef(xPos, yPos, zPos);
			glRotatef(90, 0.0, 1.0, 0.0);
			
			glTranslatef (4.0, 0.5, 0.0);
			glRotatef (c_keyrot, 0.0, 0.0, 1.0);
			glTranslatef (-4.0, -0.5, 0.0);
			
			glPushMatrix();
				glColor3fv(colour);
				glTranslatef (-3.5, 0.0, 0.0);
				glutWireSphere(0.5, 10, 10);	// Switch contact point
				glTranslatef (3.5, 0.0, 0.0);
				glScalef (8.0, 1.0, 1.0);
				glutSolidCube (1.0);		// Switch
			glPopMatrix();
		
		glPopMatrix();	//End of switch
	
	glPopMatrix();
	}
};


class falling_sphere:public Object{
	//Low and behold its about time, the SPHERE CLASS is upon us!!!! Yeeehar!
	public:
	
	float xPos;
	float yPos;
	float zPos;
	float radius;
	float colour[3];
	bool anchored;
	bool collision;
	bool held;
	float time;
	float speed;
	float distance;
	float start_time;
	float initial_position;
	float current_speed;

	falling_sphere(){
		xPos=0; yPos=0; zPos=0;
		anchored=false;
		held = true;
		colour[0] = 1.0;
		colour[1] = 0.0;
		colour[2] = 1.0;
		radius = 4;
	}
	
	falling_sphere(float x,float y,float z){
		xPos=x; yPos=y; zPos=z;
		anchored=false;
		held = true;
		colour[0] = 1.0;
		colour[1] = 0.0;
		colour[2] = 1.0;
		radius = 4;
	}

	void display(){
		
		glPushMatrix();
		collision  = check_for_collision(xPos, yPos, zPos, s[0][0], s[0][1], s[0][2], radius, 0.75);
		
		if(collision){	
			colour[0] = 0.0;
			colour[1] = 1.0;
			colour[2] = 0.0;
		}

		else {
			colour[0] = 1.0;
			colour[1] = 0.0;
			colour[2] = 0.0;
		}

		if(!anchored && collision){
			
			xPos = 1 * xPos - s[0][0] / radius + 0.75;	
			yPos = 1 * yPos - s[0][1] / radius + 0.75;
			zPos = 10 * zPos - s[0][2] / radius + 0.75;
/*
			if ( s[0][0] < xPos)
				xPos+=0.5;

			if ( s[0][1] > yPos)
				yPos-=0.5;		
		
			if ( s[0][1] < yPos)
				yPos+=0.5;		
		
			if ( s[0][2] > zPos)
				zPos-=0.5;		
		
			if ( s[0][2] < zPos)
				zPos+=0.5;		

  // 
/
		}


		if(!held){// calulate how much should have fallen


			if ((yPos < radius) && (current_speed < 0)){

				start_time = Time();
				speed = current_speed;
				speed = -0.5 * speed;
				current_speed = speed;
				initial_position = radius;
			}
			
			time = Time() - start_time;
			distance = speed*time + (0.5)*Gravity*time*time;
			current_speed = speed + Gravity*time;
			yPos = distance + initial_position;
			printf(" Speed : %f \n" , current_speed);
		}
						
	glColor3fv(colour);
	glTranslatef(xPos, yPos, zPos);
	glutSolidSphere(radius, 10, 10);	

	glPopMatrix();
	}
	
	void hold(){

		held = true;
		printf(" Ball held \n");
	}

	void release(){

		printf(" Ball released \n");
		held = false;
		speed = current_speed = 50;
		start_time = Time();
		initial_position = yPos;

	}

};

*/



//Object* s[1] = new sphere(0,10,0);
//Object* k1 = new key(1,0,0);
//Object* k2 = new key(2.4,0,0);
//Object* ball = new falling_sphere(0,6,0);
//Object* glove = new hand();

//Single_Contact_List sp1_points(" Sphere 1 ");
//Single_Contact_List sp2_points(" Sphere 2 ");
//Single_Contact_List sp3_points(" Sphere 3 ");

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
 
void print_text(){
	
	GLfloat red[3] = { 1.0, 0.0, 0.0 };
    glShadeModel (GL_FLAT);
	glColor3fv(red);

	glRasterPos3i(glove.xTrans, glove.yTrans+10, glove.zTrans-10);
	printString("THE VIRTUAL MENU");
    glRasterPos3i(glove.xTrans, glove.yTrans+7, glove.zTrans-10);
	printString("FLY BY MODE");
    glRasterPos3i( glove.xTrans, glove.yTrans+5,  glove.zTrans-10);
	printString("WALK THROUGH MODE");
    
	glShadeModel (GL_SMOOTH);
}

void init(void) 
{

	static Single_Contact_List sp1_points(" Sphere 1 ");
	static Single_Contact_List sp2_points(" Sphere 2 ");
	static Single_Contact_List sp3_points(" Sphere 3 ");

	sp1_points.add(-10,0,0,4);
	sp2_points.add(10 ,0,0,4);
	sp3_points.add(0,0,-10,4);

	Object* sp1 = new sphere(-10,0,0,&sp1_points);
	Object* sp2 = new sphere(10,0,0,&sp2_points);
	Object* sp3 = new sphere(0,0,-10,&sp3_points);

	Object_List.add(sp1);
	Object_List.add(sp2);
	Object_List.add(sp3);

	glClearColor (0.0, 0.0, 0.0, 0.0);
	
	//Text stuff
	glShadeModel (GL_FLAT);
    makeRasterFont();

	glShadeModel (GL_SMOOTH);
	
	//Ambient lighting 
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient); 
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse); 
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular); 
	
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING); //Tell openGL to prepare to calculate lighting equations
	glEnable(GL_LIGHT0);   //Switch on the defined light
	glEnable(GL_BLEND);
		
	// Enable standard hidden surface removal
	glDepthFunc(GL_LEQUAL);
	
	glEnable(GL_DEPTH_TEST);

	// initialise glove ...

	if (!GloveInit("COM1")) {
		printf("\nUnable to initialise glove on COM1 \n");

		if (!GloveInit("COM2")) {
			printf("Unable to initialise glove on COM2 \n");
			return;
		}
	}

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
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseMaterial);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

//	List(Object_List);

	Object_List.n = Object_List.head->next;

	while ( Object_List.n->next != 0){ // while there are more keys print them out.
	Object_List.n = Object_List.n->next;
	Object_List.n->obj->display();
	}

/*	s_points.n = s_points.head->next;

	while ( s_points.n->next != 0){ // while there are more keys print them out.
	s_points.n = s_points.n->next;
	printf("\nContact points : %f,%f,%f ",s_points.n->x,s_points.n->y,s_points.n->z);
	}
*/
	
//	List(s_points);
	
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
		
	//Draw hand		
	//display_hand();
	
	//draw fixed world ...
	fixed_world();
	
	//draw menu...
	if (menu){
		print_text();
	}


	glutSwapBuffers();
	
}


void idle(void)
{
	gloveData = GloveGetDataPtr(); //poll glove and fill array with current values
	
	glove.little_finger = (float)gloveData[4];
	glove.ring_finger = (float)gloveData[3];
	glove.middle_finger = (float)gloveData[2]-30;
	glove.index_finger = (float)gloveData[1];
	glove.thumb_joint = (float)gloveData[0];

	glove.xRaw = (float)gloveData[5];
	glove.zRaw = (float)gloveData[6];

	//Check for gestures...
	if(glove.gesture_check())
	{
		 display();
	}

	glove.track_finger_sensors();
	
	glove.xRot = ((float)gloveData[5]-130)/255*180;
	glove.zRot = (((float)gloveData[6])-110)/255*120;
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
   glLightfv(GL_LIGHT0, GL_POSITION, light_position0); 
	   
}


void keyboard (unsigned char key, int x, int y)
{
   switch (key) {
    
    //X, Y, Z, transformation of hand
	case 'a':
		if(xPosMov){
			glove.xTrans += 2;
			centrex +=2;
			eyex +=2;
		}
	break;


	case 'p':
		PRINT = true;

	break;

	case 'A':
		if(xNegMov){
			glove.xTrans -= 2;
			centrex -=2;
			eyex -=2;
		}
	break;

	case 's':
		if(yPosMov){
			glove.yTrans += 2;
			centrey +=2;
			eyey +=2;
		}
	break;

	case 'S':
		if(yNegMov){
			if(glove.yTrans >2){
				glove.yTrans -= 2;
				centrey -=2;
				eyey -=2;
			}
		}
	break;

	case 'd':
		if(zPosMov){
			glove.zTrans += 2;
			centrez +=2;
			eyez +=2;
		}
	break;

	case 'D':
		if(zNegMov){
			glove.zTrans -= 2;
			centrez -=2;
			eyez -=2;
		}
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
	
	//Camera movement - centre
	case 'f':
		centrex=centrex + 1.0;
		break;
	
	case 'F':
		centrex=centrex - 1.0;
		break;
	
	case 'g':
		centrey=centrey + 1.0;
		break;
	
	case 'G':
		centrey=centrey - 1.0;
		break;
	
	case 'h':
		centrez=centrez + 1.0;
		break;
	
	case 'H':
		centrez=centrez - 1.0;
		break;

	case 'z':
		menu=true;
		break;

	case 'Z':
		menu=false;
		break;
	
	//case 'x':
	
	//break;
	
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
/*
	case '1':
		ball.time += .5;
		printf(" time = %f \n",ball.time);
	break;

	case '2':
		ball.time -= .5;
		printf(" time = %f \n",ball.time);
	break;

	case '3':
		ball.release();
	break;

	case '4':
		ball.hold();
	break;

*/
	case 27:
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

 
