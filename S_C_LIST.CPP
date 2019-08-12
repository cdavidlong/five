#include <string.h>
#include "s_c_list.h"

Single_Contact_List::Single_Contact_List(char* n="Default"){init(n);}				// default constructor
Single_Contact_List::Single_Contact_List(Single_Contact_List& s){init((char*)s);}	// copy constructor
Single_Contact_List::~Single_Contact_List(){delete name;}							// destructor

Single_Contact_List& Single_Contact_List::operator=(Single_Contact_List& s){			// over-rides = operator
			init((char*)s); return * this; 
		}
	
void Single_Contact_List::add(float _x,float _y,float _z,float _r){
			t = head->next;
			while (t->next != 0)t=t->next;
			b = new contact_point(_x,_y,_z,_r,0);
			t->next = b;
			size++;
		}
	// overloads (char*) casting operator ... 
Single_Contact_List::operator char*() {return name;}

// stores name passed in on contruction.
void Single_Contact_List::init(char* n){
	name = new char[strlen(n)+1];strcpy(name,n);
}

