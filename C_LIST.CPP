#include <windows.h>
#include "c_list.h"

Contact_List::Contact_List(){
		n = new contact_point(0,0,0,0,NULL);
		head = new contact_point(0,0,0,0,n);
		size = 0;
		}

	// destructer
Contact_List::~Contact_List(){delete head; delete n;}

// virtual functions ...
virtual void Contact_List::add(float,float,float,float);	 

