#include "s_o_list.h"


Single_Object_List::Single_Object_List(char* n="Default"){init(n);} // default constructor
Single_Object_List::Single_Object_List(Single_Object_List& s){init((char*)s);}  // copy constructor
	
Single_Object_List::~Single_Object_List(){delete name;}//,t,b;} // destructor

Single_Object_List& Single_Object_List::operator=(Single_Object_List& s){  // over-rides = operator
			init((char*)s); return * this; 
		}
	
	//vitual functions ...
	// Add .. add objects to linked list.
void Single_Object_List::add(Object* key){
	t = head->next;
	while (t->next != 0)t=t->next;
	b = new list_item(true,false,key,0);
	t->next = b;
	size++;
}

    // remove .... removes objects from list.
void Single_Object_List::remove(Object* ob1){
	t = head->next;
	if (t->obj == ob1){
		head->next = t->next;
		size--;}

	else{
	while (t->next!= NULL){
	b = t->next;
	if ( b->obj == ob1 ){t->next = b->next;size--;}
	if ( t->next != NULL)t = t->next;}
	}
}

	// overloads (char*) casting operator ... 
Single_Object_List::operator char*() {return name;}

void Single_Object_List::init(char* n){
	name = new char[strlen(n)+1];
	strcpy(name,n);
}

