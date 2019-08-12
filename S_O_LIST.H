#ifndef _s_o_list_
#define _s_o_list_

#include "o_list.h"
#include <string.h>

class Single_Object_List:public Object_List{
	
	public:

	Single_Object_List(char* n);		// default constructor
	Single_Object_List(Single_Object_List& s);	// copy constructor
	
		~Single_Object_List();					// destructor

		Single_Object_List& operator=(Single_Object_List& s);	// over-rides = operator
		
	//vitual functions ...
	// Add .. add objects to linked list.
		void add(Object* key);

    // remove .... removes objects from list.
	void remove(Object* ob1);

	// overloads (char*) casting operator ... 
	operator char*();

	char* name; // name of list
	// stores name passed in on contruction.
	void init(char* n);
	
	struct list_item* t;
	struct list_item* b;

};

#endif