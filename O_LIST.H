#ifndef _o_list_h_
#define _o_list_h_

#include "object.h"
#include <windows.h>

class Object_List{

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
	Object_List();
	~Object_List();
	// virtual functions ...
	virtual void add(Object* key) = 0;	 
	virtual void remove(Object* key) = 0;

};
#endif