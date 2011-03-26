
#include <vector>
#include <map>
#include <string>
#include <math.h>

#define MAXBUF 2000000
#define MAXNAME 200

using namespace std;



typedef struct vector2 {
  float x;
  float y;
} vector2;


typedef struct vector3 {
  float x;
  float y;
  float z;

  vector3(float xx, float yy, float zz) : x(xx), y(yy), z(zz){};
  vector3() : x(0.0), y(0.0), z(0.0) {};

  vector3 cross_product(vector3 rhs){
     float cross_mag_x = y * rhs.z - z*rhs.y;
	 float cross_mag_y = z * rhs.x - x*rhs.z;
	 float cross_mag_z = x * rhs.y - y *rhs.x;
	 return vector3(cross_mag_x, cross_mag_y, cross_mag_z);
  };

  void normalize(){
     float mag = sqrt( x*x + y*y + z*z);
     x /= mag;
	 y /= mag;
	 z /= mag;
  };
} vector3;

typedef struct line{
	vector3 start;
	vector3 end;
	
	line(vector3 s, vector3 e) : start(s), end(e) {};
	
}line;


class Node{
public:
   vector3 center;
   bool drawn;
   string name;
   int num_drawn;

   class Edge{
	public:
      int weight;
      Node *to;
      
	  Edge(): weight(0), to(NULL){};
	  Edge(int w, Node *t): weight(w), to(t){};

   };

   Node() : drawn(false), num_drawn(0){
      name[0] = '\0';
   }
   Node(string n) : drawn(false), name(n) , num_drawn(0){};


   map<string, Edge *> adj;
   

   
};





