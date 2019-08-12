#ifndef _s_c_list_
#define _s_c_list_

#include "c_list.h"
#include <string.h>


class Single_Contact_List:public Contact_List{
	
	public:

		Single_Contact_List(char* n);				// default constructor
		Single_Contact_List(Single_Contact_List& s);	// copy constructor
	
		~Single_Contact_List();					// destructor

		Single_Contact_List& operator=(Single_Contact_List& s);			// over-rides = operator
			
	// Add .. add key to linked list.
	void add(float _x,float _y,float _z,float _r);

	// overloads (char*) casting operator ... 
	operator char*(); 

	char* name; // name of list

	// stores name passed in on contruction.
	void init(char* n);
	
	struct contact_point* t;
	struct contact_point* b;

};

#endif