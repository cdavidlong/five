#ifndef _v_h_    
#define _v_h_

#include <glut.h>
#include <windows.h>

#pragma once

//Colours
GLfloat black[] = {0.0, 0.0, 0.0};
GLfloat red[] = {1.0, 0.0, 0.0};
GLfloat yellow[] = {1.0, 1.0, 0.0};
GLfloat green[] = {0.0, 1.0, 0.0};
GLfloat white[] = {1.0, 1.0, 1.0};
GLfloat cyan[] = {0.0, 1.0, 1.0};
GLfloat magenta[] = {1.0, 0.0, 1.0};
GLfloat blue[] = {0.0, 0.0, 1.0};
GLfloat lightgreen[] = {1.0, 0.6, 0.6};

//Camera values...
static GLfloat eyex=0.0;
static GLfloat eyey=20.0;
static GLfloat eyez=30.0;

static GLfloat centrex=0.0;
static GLfloat centrey=0.0;
static GLfloat centrez=0.0;

//Lighting values...

float ambient_strength = 1.0;
float diffuse_strength = 1.0;
float specular_strength = 1.0;

static GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat mat_shininess[] = { 50.0 };
static GLfloat light0_position[] = { 1.0, 1.0, 1.0, 0.0 };
static GLfloat light1_position[] = { -4.0, 0.0, 1.0, 0.0 };
static GLfloat diffuseMaterial[4] = {0.5, 0.5, 0.5, 1.0};

//Menu variables
bool menu=false;
//Colour of border of buttons 
GLfloat butcolour[15];
int count=0;
int menu_choice=0;
int choice=0;
int j;

const double PI = 3.1415926535897932;
const double Gravity = -14.8; 
//Function prototypes

void makeRasterFont();
void print_text();
void printString(char *s);
void track_finger_sensors();
float distance(float x1, float y1, float x2, float y2);
bool check_for_collision(float,float,float,float,float,float,float,float);
float Time();
void load_from_file(int);
void load_object(float,float,float,float,float);
void save_to_file(int);
void rain();
void set_text();
void calc_menu_choice();
void init_will(void);
void walk_will(float position);
void will();
void setup_texture_mapping();	
void init_menu();
void setup_lighting();
void clear_current_world();
void init_glove();

#define SPHERE				1
#define CUBE				2
#define BOUNCING_SPHERE		3
#define PIANO_KEY			4
#define CRITTER				5
#define BULLET				6
#define DOMINO				7


bool show_spheres = false;

char current_text[6][20] = {" TEST "," TEST " ," TEST " ," TEST " ," TEST " ," TEST "};

char all_text[48][20] = {"MAIN MENU","ENVIRONMENT","OBJECTS","VIEWING MODE","EXIT MENU", "QUIT",
						"ENVIRONMENT",	"LOAD" , "SAVE","BACK" , " ", " ",
						"VIEWING MODE", "CAMERA", "HAND", "CENTRE","GUN","BACK",
						"OBJECTS", "SELECTED", "NEW", "CONTACT POINTS", "BACK", " ",
						"SELECTED", "HIDE", "SHOW", "REMOVE", "BACK", " ",
						"LOAD", "WORLD ONE", "WORLD TWO", "WORLD THREE", "WORLD FOUR" , "BACK",
						"SAVE", "WORLD ONE", "WORLD TWO", "WORLD THREE", "WORLD FOUR" , "BACK", 
						"NEW OBJECT", "BOUNCING BALL", "FLOATING SPHERE", "CUBE", "CRITTER", "BACK"};

				

bool Remove_Selected_Object = false;
bool Hide_Selected_Object = false;
bool Show_Selected_Object = false;
bool camera_mode = false;
bool gun_mode = false;

GLuint texture;
GLuint e_texture;

GLUquadric *quad1;
GLUquadric *quad2;
GLUquadric *quad3;

int Current_Selected_Object = 1;

//hand* glove;

//Single_Object_List *glove_o_list;// = new Single_Object_List("blah");
//o_list = new Single_Object_List("blah");
#endif
