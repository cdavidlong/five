#include "o_list.h"

Object_List::Object_List(){
		n = new list_item(true,false,NULL,NULL);
		head = new list_item(true,false,NULL,n);
		size = 0;
		}

// destructer
Object_List::~Object_List(){}//delete head;delete n;}

// virtual functions ...
virtual void Object_List::add(Object* key);	 
virtual void Object_List::remove(Object* key);


