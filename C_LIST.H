#ifndef _c_list_
#define _c_list_

class Contact_List{

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
	Contact_List();

	// destructer
	~Contact_List();
	// virtual functions ...
	virtual void add(float,float,float,float) = 0;	 

};
#endif