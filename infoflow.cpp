/*********************************************************
				KAREEM's FINAL PROJECT STUFF
*********************************************************/

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#ifdef DARWIN
#include <GLUT/glut.h>
#include <OPENGL/gl.h>
#include <OPENGL/glext.h>
#endif

#ifdef LINUX
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glext.h>
#endif

#include "infoflow.h"


/*********************************************************
					CONSTANTS STUFF
*********************************************************/
#define HIGH_DENSITY_EMAIL 50.0
#define EDGE_WEIGHT_COLOR_EFFECT 0.1
#define STEP_SIZE 0.1
const double PI = 3.141592;
const float Rad2Deg = 180.0 / PI;
const float Deg2Rad = PI / 180.0;

int draw_count = 0;

void populate_map(void);
void print_comms(void);

//glut callbacks
void display(void);
void reshape( int w, int h );
void keyboard(unsigned char key, int x, int y );
//add funcs
void add_node(Node *theNode);
void add_children(Node *theNode, int level);

//drawing funcs
void draw_node(Node *theNode);
void draw_children(Node *theNode, int level);
void draw_string(string str);
void draw_path(Node *start, Node *end, vector3 color, int weight);


/*********************************************************
					CAMERA STUFF
*********************************************************/
vector3 eye;
vector3 look;
float theta;

void init_camera(void) {
	eye = vector3(0.0, 0.0 , 1.0 );
	look = vector3(0.0, 0.0, 0.0);
}

/*********************************************************
					LIGHTING STUFF
*********************************************************/
int light;
//globals for lighting - use a white light and apply materials
//light position
GLfloat light_pos[4] = {1.0, 1.0, 2.5, 1.0};
//light color (ambiant, diffuse and specular)
GLfloat light_amb[4] = {0.6, 0.6, 0.6, 1.0};
GLfloat light_diff[4] = {0.6, 0.6, 0.6, 1.0};
GLfloat light_spec[4] = {0.8, 0.8, 0.8, 1.0};

void init_lighting() {
  //turn on light0

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

  glEnable(GL_NORMALIZE);
  //set up the diffuse, ambient and specular components for the light
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diff);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_amb);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_spec);
  //specify our lighting model as 1 normal per face
  glShadeModel(GL_FLAT);

  glMatrixMode(GL_MODELVIEW);
  glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

}

/*********************************************************
					MATERIALS STUFF
*********************************************************/
typedef struct materialStruct {
  GLfloat ambient[4];
  GLfloat diffuse[4];
  GLfloat specular[4];
  GLfloat shininess[1];
} materialStruct;

void materials(materialStruct materials) {
  glMaterialfv(GL_FRONT, GL_AMBIENT, materials.ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, materials.diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, materials.specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, materials.shininess);
}

materialStruct GreenShiny = {
  {0.0, 0.3, 0.0, 1.0},
  {0.0, 0.9, 0.0, 1.0},
  {0.2, 1.0, 0.2, 1.0},
  {8.0}
};

/*********************************************************
				    STUFF
*********************************************************/
//map of email address and the nodes that they connect to
map<string, Node *> nodes;

vector<line *> paths2draw;
vector<Node *> nodes2draw;

//The parsed out file from the java process that parses an email folder
ifstream *commFile;

//global variables - necessary evil when using GLUT
//global width and height
int GW;
int GH;
string start_node;
int num_levels_out;

/*
Ok so store it in some data structure

write a function that translates emailid to position

go through draw nodes with lines point to position based on weight
*/
int main(int argc, char *argv[]){
	
	// some command line checking
	if(argc != 7) {
		printf("not enough arguments\n");
		
		printf("usage: ./infoflow -p <name of folder> -t <name of target node to start from> -l <num levels out>\n");
		printf("example: ./infoflow -p /Users/knassar/Desktop/maildir/lay-k -t sally.keepers@enron.com -l 3\n");
		return 0;
	}
	
	if( strcmp("-p", argv[1]) || strcmp("-t", argv[3]) ||strcmp("-l", argv[5]) ) {
		printf("usage: ./infoflow -p <name of folder> -t <name of target node to start from> -l <num levels out>\n");
		printf("example: ./infoflow -p /Users/knassar/Desktop/maildir/lay-k -t sally.keepers@enron.com -l 3\n");
		return 0;
	}
	
	num_levels_out = atoi(argv[6]);
	start_node = string(argv[4]);
	
	string forkthis =  "java -jar EmailParser.jar ";
	forkthis += string(argv[2]);
	
	//ok now fork off that java process to parse out those folders
	if (system(forkthis.c_str()) == -1) {
		printf("failed\n");
	}
	
	commFile = new ifstream("EnronEmailParse-output");

	populate_map();
	commFile->close();
	
	
	glutInit( &argc, argv );
	//intializations

	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(1000, 1000);
	glutInitWindowPosition(100, 100);
	glutCreateWindow( "Infoflow" );
    glClearColor(1.0, 1.0, 1.0, 1.0);


	//global variable intialization
	GW = GH = 1000;
	theta = 0;

	//register the callback functions
	glutDisplayFunc( display );	
	glutKeyboardFunc( keyboard );
	glutReshapeFunc( reshape );
			//glutMouseFunc( mouse );
			//glutMotionFunc( mouseMove );
			//glutIdleFunc(idle);

	glLoadIdentity();
	
	//features
	//enable GL features we want
    glEnable(GL_DEPTH_TEST);

	
	init_camera();
	//init_lighting();
	glutMainLoop();
	
	return 0;
}




/*********************************************************
					DISPLAY STUFF
*********************************************************/
void display() {
	
  draw_count++;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
	
	glPushMatrix();
	
	glRotatef(theta, 0.0, 1.0 , 0.0);
		gluLookAt(eye.x, eye.y , eye.z, //eye position
				  look.x, look.y, look.z, //point we are looking at
				  0.0, 1.0, 0.0);  //up vector

	    Node *curr = nodes[start_node];
		if(curr == NULL) {
			printf("Starting node does not exist\n");
			printf("please specify a node that exists with after -target\n");
			exit(1);
		}
		curr->center = vector3(0.0, 0.0, 0.0);
		
		
		draw_node(curr);
		draw_children(curr, 0);
		
		glColor3f(1.0, 1.0, 1.0);
	glPopMatrix();
	
	glutSwapBuffers();
}

void reshape( int w, int h ){
	GW = w;
	GH = h;
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    glOrtho( -20.0*(float)w/h, 20.0*(float)w/h, -20.0, 20.0, 1.0, 15.0);

   // gluPerspective(90.0, ((float)GW)/((float)GH) , 1.0, 150.0);
	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, w, h);
	
	glutPostRedisplay();
}

void draw_node(Node *theNode){
	if(draw_count <= theNode->num_drawn)
		return;
	vector3 pos = theNode->center;
	float num_emails = (theNode->adj).size();
	
	glPushMatrix();
	    //glLoadIdentity();
		glTranslatef(pos.x, pos.y, pos.z);
	
		glColor3f(1.0 , 0.9 * (1.0 - (num_emails/100.0)) , 0.9 * (1.0 - (num_emails/100.0)) );
		materials(GreenShiny);
		glutSolidSphere	(	0.2 , 100 , 100 );
	
		glPushMatrix();
			glTranslatef(-0.1, 0.0, 0.0);
			if(num_emails >= HIGH_DENSITY_EMAIL)
				draw_string(theNode->name);
	
			stringstream out;
			out << (theNode->adj).size();
			string sentout = "sent out emails to "; 
			sentout += out.str();
			sentout += " people";
	
			glTranslatef(0.0, -0.1, 0.0);
		    //draw_string(sentout);

		glPopMatrix();
	glPopMatrix();
	
	theNode->drawn = true;
	theNode->num_drawn++;
	glutSwapBuffers();
}

void draw_children(Node *parent, int level) {
	if(level >= 3)
		return;
	float radius = 7.0;
	double num_children = (parent->adj).size();
	double delta_theta = 360.0/num_children;
	double curr_theta = 0.0;
	Node *child;
	
	for(map<string, Node::Edge *>::iterator etr = (parent->adj).begin(); etr != (parent->adj).end(); etr++){
		Node::Edge *e = (*etr).second;
		
		child = e->to;
		
		if(! (child->drawn ) ) {
		   vector3 parentpos = parent->center;
		
		   double childx = parentpos.x + ( (radius * ( (25.0 - e->weight) / 25.0))* cos(curr_theta * Deg2Rad) );
		   double childy = parentpos.y - ( (radius * ( (25.0 - e->weight) / 25.0))* sin(curr_theta * Deg2Rad) );
		   child->center = vector3(childx, childy, parentpos.z);
	    }
		
		
		draw_node(child);
		draw_path(parent, child, vector3(1.0,1.0 - (EDGE_WEIGHT_COLOR_EFFECT * e->weight), 1.0 ), e->weight);
	
		
 	    curr_theta += delta_theta;
	
	}
	for(map<string, Node::Edge *>::iterator etr = (parent->adj).begin(); etr != (parent->adj).end(); etr++){
		Node::Edge *e = (*etr).second;
		
		child = e->to;
		draw_children(child , level + 1);
	
	}
	glutSwapBuffers();
	
}


void add_children(Node *parent, int level){
	nodes2draw.push_back(parent);
	if(level >= num_levels_out)
		return;
	float radius = 7.0;
	double num_children = (parent->adj).size();
	double delta_theta = 180.0/num_children;
	double curr_theta = 0.0;
	Node *child;
	
	for(map<string, Node::Edge *>::iterator etr = (parent->adj).begin(); etr != (parent->adj).end(); etr++){
		Node::Edge *e = (*etr).second;
		
		child = e->to;
		
		if(! (child->drawn ) ) {
			vector3 parentpos = parent->center;
			
			double childx = parentpos.x + (radius * cos(curr_theta * Deg2Rad) );
			double childy = parentpos.y - (radius * sin(curr_theta * Deg2Rad) );
			child->center = vector3(childx, childy, parentpos.z);
	    }
		
		//add node to the shapes list
		nodes2draw.push_back(child);
		paths2draw.push_back(new line(parent->center, child->center) );
		
		
 	    curr_theta += delta_theta;
		
	}
	for(map<string, Node::Edge *>::iterator etr = (parent->adj).begin(); etr != (parent->adj).end(); etr++){
		Node::Edge *e = (*etr).second;
		
		child = e->to;
		add_children(child , level + 1);
		
	}
	
}
 

void draw_string(string str){
	int size = str.length();
	glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
	glRasterPos2f(0, 0);
	
	
	for(int i = 0 ; i < size; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);
	}

}

void draw_path(Node *start, Node *end, vector3 color,int weight){
	vector3 s = start->center;
	vector3 e = end->center;
	
	glColor4f(color.x, color.y, color.z, 0.01f);
	glPushMatrix();
	
	glBegin(GL_LINES);
		glVertex3f(s.x, s.y, -1.0); // origin of the line
		glVertex3f(e.x, e.y, -1.0); // ending point of the line
	glEnd( );
	
	glPopMatrix();
}




/*********************************************************
					KEYBOARD STUFF
*********************************************************/
void keyboard(unsigned char key, int x, int y ){
  switch( key ) {
    case 'q': case 'Q' :
      exit( EXIT_SUCCESS );
      break;
    case 'h' : case 'H' :
      printf("hello!\n");
      break;

	case 'a':
	    eye.x -= STEP_SIZE;
		look.x -= STEP_SIZE;
		break;

	case 's':
		eye.y += STEP_SIZE;
		look.y += STEP_SIZE;
		break;

	case 'd':
		eye.x += STEP_SIZE;
		look.x += STEP_SIZE;
		break;

	case 'w':
		eye.y -= STEP_SIZE;
		look.y -= STEP_SIZE;
		break;
	  case '+':
		  eye.z -= 1.0;
		  look.z -= 1.0;
		  break;
	  case '-':
		  eye.z += 1.0;
		  look.z += 1.0;
		  break; 
	  case 'o':
		  theta -= 1.0;
		  break;
	  case 'p':
		  theta += 1.0;
		  break;
  }

	glutPostRedisplay();
}


/*********************************************************
			Populates hashmap from the commFile
*********************************************************/
void populate_map(void) {
	
	char line[MAXBUF];
	

	try{
		
	   while( commFile->getline(line, MAXBUF)){
		    char *from = strtok(line, "{}= ");
		    
		    #ifdef DEBUG
			printf("email from: %s\n", from);
			#endif
			
			string fromname = from;
			if( ! nodes[fromname] ){
				
				#ifdef DEBUG
				printf("not here already\n");
				#endif
				
				nodes[fromname] = new Node(fromname);
				(nodes[fromname])->drawn = false;
				
				#ifdef DEBUG
				if ( ! nodes[fromname] ) {
		            printf("still not here\n");
		        }
		        else {
					cout << "added " << nodes[fromname]->name << endl;
				}
				#endif
				
			}
			
			char *to;
			
			while(  (to = strtok(NULL, ",[] "))  ){

				string toname = to;
				
				if( ! (nodes[fromname]->adj)[toname] ){
					
					//not neighbors
					//is it in the nodes map already?
					if( !nodes[toname] ){
					   nodes[toname] = new Node(toname);
					   (nodes[toname])->drawn = false;
					   
					}
					
					(nodes[fromname]->adj)[toname] = new Node::Edge(1, nodes[toname]);
					
				}
				else {
					
					//they're already neighbors
					((nodes[fromname]->adj)[toname])->weight++;
				}
				
			}
			
			
	   }
	
	
    }

    catch (ifstream::failure e) {
	   printf("end of file\n");
    }

}

/*********************************************************
				Prints that hashmap 'nodes'
*********************************************************/
void print_comms(){
   for(map<string, Node *>::iterator itr = nodes.begin(); itr != nodes.end(); itr++){
	  Node *main = (*itr).second;
      cout << main->name << endl;
     
      for(map<string, Node::Edge *>::iterator etr = (main->adj).begin(); etr != (main->adj).end(); etr++){
		Node::Edge *e = (*etr).second;
	
		cout << " sent " << e->weight << " emails to " << e->to->name << endl;
	
	  }

	  if( (main->adj).size() == 0){
	     printf(" sent no emails\n");
	  }
     
     
	  printf("\n");
   }
}