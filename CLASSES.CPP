#include <iostream.h>
#include <string.h>

// Base Class of Linked List.
// Abstract class ..... has virtual funtions -> cannot create instance of it.


class Linked_List{

	// friend functions. 
	friend ostream &operator<<(ostream &,Linked_List&);
	friend void List(Linked_List&);

	public:
	// NODE of link list.
	struct node{		 
	int key;
	struct node* next;
	struct node* prev;
	node(){key=0;next=0;prev=0;};
	node(int k,struct node* n,struct node* p)
	{key=k;next=n;prev=p;};
	};	

	// default constructer.
	Linked_List(){
		n = new node(0,0,0);
		head = new node(0,n,0);
		size = 0;
		}

	// destructer
	~Linked_List(){delete head;delete n;}

	// virtual functions ...
	virtual void add(int key) = 0;	 
	virtual void remove(int key) = 0;

	//cast operator ... allows list to be cast to an int.
	operator int() {return size;}
	
	protected:
	struct node *head; // head of list
	int size;// size of list
	
	private:
	struct node *n; // initial node.

};

// friend function over-rides << opeator.
ostream &operator<<(ostream &output,Linked_List &l){

	// much the same as list ...
	l.n = l.head->next;
	output << " Linked list values : ";
	while ( l.n->next != NULL){
	l.n=l.n->next; cout<<l.n->key<<" ";
	}
	cout << ". \n";
	return output;
}

// friend function ... prints out list.

void List(Linked_List &l)
{
	l.n = l.head->next;
	cout << " Linked list values : ";
	while ( l.n->next != 0){ // while there are more keys print them out.
	l.n = l.n->next; cout<<l.n->key<<" ";
	}
	cout << ". \n";
	return;
}


/////////////////////////
// Single Linked List. //
/////////////////////////

class Single:public Linked_List{
	
	public:

		Single(char* n="Default"){init(n);} // default constructor
		Single(Single& s){init((char*)s);}  // copy constructor
	
		~Single(){delete name,t,b;} // destructor

		Single& operator=(Single& s){  // over-rides = operator
			init((char*)s); return * this; 
		}
	
	//vitual functions ...
	// Add .. add key to linked list.
		void add(int key){
			t = head->next;
			while (t->next != 0)t=t->next;
			b = new node(key,0,0);
			t->next = b;
			size++;
		}

    // remove .... removes key from list.
	void remove(int key){
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

	// overloads (char*) casting operator ... 
	operator char*() {return name;}

	protected:
	char* name; // name of list

	private:
	// stores name passed in on contruction.
	void init(char* n){
	name = new char[strlen(n)+1];strcpy(name,n);
	}

	struct node* t;
	struct node* b;

};
/*
////////////////////////////
// Double Linked List ... //
////////////////////////////

template <class int>
class Double:public Linked_List<T>{

	public:
		Double(char* n="Default"){init(n);} // default constructor
		Double(Double& d){ init((char*)d);} 
	
		~Double(){delete name,t,b;} 
	 
		Double& operator=(Double& d){init((char*)d); return * this;}
	
	void add(T key){
		t = head->next;
		while (t->next != 0)t=t->next;
		b = new node (key,0,t);
		t->next = b;
		size++;
	}

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
	
	operator char*() {return name;}

	protected:
	char* name;

	private:

	void init(char* n){
	name = new char[strlen(n)+1];strcpy(name,n);
	}

	struct node* t;
	struct node* b;

};

////////////////////////////
// Sorted Linked List ... //
////////////////////////////

template <class T>
class Sorted:public Linked_List<T>{

	public:
	Sorted(char* n="Default"){init(n);}
	Sorted(Sorted& s){ init((char*)s);}
	
	~Sorted(){delete name,t,b,y;}

	Sorted& operator=(Sorted& s){
		init((char*)s); return * this;
	}
	
	void add(T key){
		t = head->next;
		while (t->key<key){
			if ( t->next == NULL){
				y = new node (key,0,NULL);
				t->next = y;
				size++; return;
			}

			else {
				b = t;
				t = t->next;
			}
		}
		y = new node (key,t,0);
		b->next = y;

		size++;
	}

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

	operator char*() {return name;}

	protected:
	char* name;

	private:

	void init(char* n){
	name = new char[strlen(n)+1];strcpy(name,n);
	}

	struct node* t;
	struct node* b;
	struct node* y;

};


*/

void main(){

//Single s = new Single("My Single List");
Single s();
Single p("My Single List");

&s = &p;

s.add(1);
s.add(2);
s.add(123);


return;
}

