/***************************************************************************
                                 scene.cpp
                             -------------------
    update               : 2009-10-09
    copyright            : (C) 2008-2009 by Yohan Fougerolle
    email                : yohan.fougerolle@u-bourgogne.fr

    Update               : Deepak Gurung
    email                : dpaq.grg@gmail.com
 ***************************************************************************/

// this program is just a bunch of functions I often need, and students too

// important : quick and very dirty implementation

#include <iostream>
#include "scene.h"
#include "Mesh.h"
#include "constante.h"
#include "useful.h"
#include "NeighborMesh.h"
using namespace std;

// variable in scene.cpp

extern Vector3d Camera_Position,Camera_Target,rotation_axis,translations;
extern scene My_Scene;
extern NeighborMesh globalmesh;
extern int id_globalmesh;
extern double znear, zfar;

// window variables
//to do things well, these should adjust to the object to render, but they dont...

int                     window_width = 640;
int                     window_height = 480;
int                     window_number = 0;
double                  view_angle = 45;
Vector3d                translations(0,0,0);
Vector3d                rotations(0,0,0);
Vector2i                previous_mouse_position(0,0);
Vector3d                previous_trackball_position(0,0,0);
Vector3d                rotation_axis(0,0,0);
double                  rotation_angle(0);
float                   trackball_transform[4][4] = {{1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1}};

//-------------
//functions for openGl display
//-------------

void scene::Create_Lighting(Vector3d Pmin, Vector3d Pmax,Vector3d CamPosition, Vector3d CamTarget){

     //function to play around with open GL lights

    glEnable(GL_COLOR_MATERIAL);

    //roughly speaking : color of reflected light
    GLfloat specular[]={1.0, 1.0, 1.0, 1.0};
    glMaterialfv(GL_FRONT,GL_SPECULAR,specular);

    //intensity of the shining...
    GLfloat shine[]={100};
	glMaterialfv(GL_FRONT,GL_SHININESS,shine);

    //ambient and diffuse light
	GLfloat ambient[]={0.0, 0.0, 0.0, 1.0};
	GLfloat diffuse[]={0.4, 0.4, 0.4, 1.0};
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

    //set spot light cone, direction, angle, etc
	GLfloat position[]={CamPosition[0],CamPosition[1],CamPosition[2] + 10 ,1};
	Vector3d Dir = -(CamPosition -CamTarget).normalized();
	GLfloat spot_direction[] = {Dir[0],Dir[1],Dir[2]};
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 360.0);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 2.0);

	//activate the first light
	glEnable(GL_LIGHT0);

	//-- if you want to add more lights, up to 8 lights are possible
	//-- insert similar code here
}

//---------------
//3D axis display
//---------------
int scene::Draw_Axis(double size)	{

	int id_list = glGenLists(1);
	glNewList(id_list,GL_COMPILE_AND_EXECUTE);

	glMatrixMode(GL_MODELVIEW);

	glPushMatrix();

		GLfloat ratio=size/8.0;

		glColor3f(1.0,0.0,0.0);

		glLineWidth(2.0);
		glBegin(GL_LINES);
			glVertex3f(0.0,0.0,0.0);
			glVertex3f(size,0.0,0.0);
		glEnd();
		glLineWidth(1.0);

		glPushMatrix();
			glTranslatef(size,0.0,0.0);
			glPushMatrix();
				glRotatef(90.0,0.0,1.0f,0.0f);
				glutSolidCone(1*ratio,3*ratio,10,10);
			glPopMatrix();
		glTranslatef(size/1.5,-1,0.0);

		Print3DMessage(0,0,"x");

		glPopMatrix();

		glColor3f(0.0,1.0,0.0);

		glLineWidth(2.0);
		glBegin(GL_LINES);
			glVertex3f(0.0,0.0,0.0);
			glVertex3f(0.0,size,0.0);
		glEnd();
		glLineWidth(1.0);

		glPushMatrix();
			glTranslatef(0.0,size,0.0);
			glPushMatrix();
				glRotatef(90.0 , -1.0 , 0.0f , 0.0f );
				glutSolidCone(1*ratio,3*ratio,10,10);
			glPopMatrix();
			glTranslatef(-1,size/1.5,0.0);

		Print3DMessage(0,0,"y");
        glPopMatrix();

		glColor3f(0.0f,0.0f,1.0f);

		glLineWidth(2.0);
		glBegin(GL_LINES);
			glVertex3f(0.0,0.0,0.0);
			glVertex3f(0.0,0.0,size);
		glEnd();
		glLineWidth(1.0);

		glPushMatrix();
			glTranslatef(0.0,0.0,size);
			glPushMatrix();
				glRotatef(90.0,0.0,0.0f,1.0f);
				glutSolidCone(1*ratio,3*ratio,10,10);
			glPopMatrix();
			glTranslatef(-1,-1,size/1.5);
			Print3DMessage(0,0,"z");
		glPopMatrix();
	glPopMatrix();

	glEndList();
	return id_list;
}

//--
//constructor for scene
//--

scene::scene(){

	rotate=0;
	Axis=0;

	for(int i=0;i<4;i++)
		for(int j=0;j<4;j++)
			Current_Matrix[i][j]=0;

	Current_Matrix[0][0]=Current_Matrix[1][1]=Current_Matrix[2][2]=Current_Matrix[3][3]=1; // current matrix transform for trackball

    // useless I guess now
	Vector3d COL_BLUE(0,0,1);
	Vector3d COL_CYAN(0,1,1);
	Vector3d COL_GREEN(0,1,0);
	Vector3d COL_YELLOW(1,1,0);
	Vector3d COL_RED(1,0,0);

	Color_Steps.push_back(COL_BLUE);
	Color_Steps.push_back(COL_CYAN);
	Color_Steps.push_back(COL_GREEN);
	Color_Steps.push_back(COL_YELLOW);
	Color_Steps.push_back(COL_RED);
}

scene ::~scene(){

}

//useless color palette creation (used for writing papers)

void scene::Create_Palette(){

Palette.push_back(Vector3d(255,128,128));
Palette.push_back(Vector3d(128,0,64));
Palette.push_back(Vector3d(255,128,0));
Palette.push_back(Vector3d(0,64,128));
Palette.push_back(Vector3d(0,0,255));
Palette.push_back(Vector3d(128,255,128));
Palette.push_back(Vector3d(0,128,255));
Palette.push_back(Vector3d(0,64,128));
Palette.push_back(Vector3d(255,0,255));
Palette.push_back(Vector3d(128,255,0));
Palette.push_back(Vector3d(255,128,64));
Palette.push_back(Vector3d(0,128,128));
Palette.push_back(Vector3d(128,128,0));
Palette.push_back(Vector3d(64,0,64));
Palette.push_back(Vector3d(128,0,0));
Palette.push_back(Vector3d(0,128,0));
Palette.push_back(Vector3d(255,255,0));
Palette.push_back(Vector3d(0,255,64));
Palette.push_back(Vector3d(128,128,192));
Palette.push_back(Vector3d(128,64,64));

for(int i=0;i<Palette.size(); i++) Palette[i]/=255.0;


}


//---
//display a color bar on the left part of the window, can be useful to see intensities etc
//---

void scene::Draw_Color_Bar(int size_x, int size_y,int x_init,int y_init){

	// store draw mode (light and polygon mode)

	GLint polygon_draw_mode[2];
	glGetIntegerv(GL_POLYGON_MODE,polygon_draw_mode);

	vector<Vector3d>::iterator it(Color_Steps.begin());
	Vector3d current,prev;

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT,viewport);

	glDisable ( GL_LIGHTING );
	glPolygonMode(GL_FRONT,GL_FILL);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0, viewport[2], 0, viewport[3]);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glTranslatef(x_init, y_init,0);
		double coef_x(size_x),coef_y(size_y/double(Color_Steps.size()-1) );

		for(int i=0;i<Color_Steps.size()-1;i++){
			prev=*it;
			it++;
			current=*it;

			glBegin(GL_QUADS);

			glNormal3d(0,0,1.0);
			glColor3d(prev[0], prev[1], prev[2]);
			glVertex3d(size_x,(i)*coef_y,0);

			glNormal3d(0,0,1.0);
			glColor3d(current[0],current[1],current[2]);
			glVertex3d(size_x,(i+1)*coef_y,0);

			glNormal3d(0,0,1.0);
			glColor3d(current[0],current[1],current[2]);
			glVertex3d(0,(i+1)*coef_y,0);

			glNormal3d(0,0,1.0);
			glColor3d(prev[0], prev[1], prev[2]);
			glVertex3d(0,(i)*coef_y,0);


			glEnd();


			double t0=(double)(i)/((double)Color_Steps.size()-1);
			double t1=(double)(i+1)/((double)Color_Steps.size()-1);

			glBegin(GL_QUADS);

			glNormal3d(0,0,1.0);
			glColor3d(t0,t0,t0);
			glVertex3d(2*size_x+10,i*coef_y,0);

			glNormal3d(0,0,1.0);
			glColor3d(t1,t1,t1);
			glVertex3d(2*size_x+10,(i+1)*coef_y,0);

			glNormal3d(0,0,1.0);
			glColor3d(t1,t1,t1);
			glVertex3d(size_x+10,(i+1)*coef_y,0);

			glNormal3d(0,0,1.0);
			glColor3d(t0,t0,t0);
			glVertex3d(size_x+10,i*coef_y,0);


			glEnd();
		}

		glPopMatrix();
  		glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	//old polygon drawing mode restoration
	glPolygonMode(GL_FRONT,polygon_draw_mode[0]|polygon_draw_mode[1]);
}

//explanatory huh?
void Print3DMessage( int x, int y, const string message ){

	glDisable ( GL_LIGHTING );
	glMatrixMode(GL_MODELVIEW);
	glRasterPos2f ( x, y );
	for( int i=0; i<(int)message.size(); i++ )	glutBitmapCharacter( GLUT_BITMAP_9_BY_15, message[i] );
}

//same in 2D for printing message in opengl : tedious and unavoidable
void PrintMessage( int x, int y, const string message ){

	GLboolean lights_on[1];
	glGetBooleanv(GL_LIGHTING,lights_on);

	glDisable ( GL_LIGHTING );
	glPushMatrix();

	glMatrixMode(GL_PROJECTION);
    glPushMatrix();//C++ implementation of octree
		glLoadIdentity();
		gluOrtho2D(0, window_width, 0, window_height);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glColor3f(0.0, 0.0, 0.0);
		glRasterPos2f ( x, y );
		for( int i=0; i<(int)message.size(); i++ )
		{
			glutBitmapCharacter( GLUT_BITMAP_9_BY_15, message[i] );
		}
        glPopMatrix();
  		glMatrixMode(GL_PROJECTION);

	glPopMatrix();
	if(lights_on) glEnable( GL_LIGHTING );
}

//--------------------------------
//
// display function : if something has to be drawn, it's here
//
//--------------------------------

void display(void){

	GLboolean lights_on[1];
	glGetBooleanv(GL_LIGHTING,lights_on);

	GLint polygon_draw_mode[2];
	glGetIntegerv(GL_POLYGON_MODE,polygon_draw_mode);

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT,viewport);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	///////
	//
	//	Windows informations display (color bar, referential,...)
	//
	//////

	glDisable(GL_LIGHTING);

    //render 3D ref at the bottom left corner of the Opengl window

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

		//Trackball Transformation
		glRotatef( rotation_angle, rotation_axis[0], rotation_axis[1], rotation_axis[2] );
		glMultMatrixf((GLfloat *)trackball_transform);
		glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *)trackball_transform);

		//Color Bar Draw
		My_Scene.Draw_Color_Bar(30,300,30,170);

		//translation in windows coordinates
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
			glLoadIdentity();

			//local 3d projection to draw 3D referential(size=30 pixels)
			glOrtho(0,viewport[2],0,viewport[3],0,max(viewport[2],viewport[3]));
			//to avoid clip planes... quick and dirty-> this could be adjusted automatically
			glTranslatef(50,90,-50);

			glPushMatrix();

				glMatrixMode(GL_MODELVIEW);
				//disable lights before drawing 3d axis
                glDisable(GL_LIGHTING);
				glCallList(My_Scene.Axis_Id_List);

				glMatrixMode(GL_PROJECTION);

			glPopMatrix();

		glPopMatrix();

	///////
	//
	//	Display mesh stats
	//
	//////

	globalmesh.Print_Stats();


	///////
	//
	//	render mesh
	//
	//////


	glPolygonMode(GL_FRONT,polygon_draw_mode[0]|polygon_draw_mode[1] ); //set drawing mode

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	Vector3d center(0,0,0);
	glTranslatef(translations[0],translations[1],translations[2]);

	glMultMatrixf((GLfloat *)trackball_transform);
	if(My_Scene.lights_on) glEnable(GL_LIGHTING);
	else glDisable(GL_LIGHTING);
	glCallList(id_globalmesh); // call the display list created in the main function

//illustrate results for a point



	rotation_axis = Vector3d(0,0,0);
	rotation_angle = 0;
	glutSwapBuffers(); // forces refresh
}

//----------------------
//
// reshape function: important this is where the camera and the windowing are set
//
//---------------------


void reshape(int w,int h){

	extern scene My_Scene;
	glPushMatrix();
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(
					45,		    // 45 deg is ok
					(float)w/h, // to preserve window ratio when rescaling the window
					znear,      // computed in main function
					zfar        // same
					);

    // wonderful gluLookAt function
	gluLookAt(
	Camera_Position[0], Camera_Position[1], Camera_Position[2], // from main.cpp
	Camera_Target[0], Camera_Target[1], Camera_Target[2],       // same
	0,1,0);		//Up Vector, do not change this

	glMatrixMode(GL_MODELVIEW);
	glutPostRedisplay();
}


void Motion( int x ,int y){

	static Vector3d current_position;

	if(My_Scene.rotate)	{

	current_position = TrackballMapping( x, y );  // Map the mouse position to a logical

	//
	// Rotate about the axis that is perpendicular to the great circle connecting the mouse movements.
	//

	rotation_axis = previous_trackball_position .cross ( current_position );
	rotation_angle = 90.0 *( current_position - previous_trackball_position ).norm() * 1.5;
	previous_trackball_position = current_position;
	}

	if(My_Scene.translate){

		if( (y - My_Scene.oldy) > 0)
			translations[2] += My_Scene.Object_Move[2];
		else
			translations[2] -= My_Scene.Object_Move[2];

		My_Scene.oldx=x;
		My_Scene.oldy=y;
	}

	glutPostRedisplay();
}

void Mouse (int button ,int state, int x,int y){

	My_Scene.rotate=0;
	if (button==GLUT_LEFT_BUTTON) 	{
		if(state==GLUT_DOWN) {
			My_Scene.rotate=1;previous_trackball_position = TrackballMapping( x, y );
		}
		else My_Scene.rotate=0;
	}
	else
	if(button==GLUT_RIGHT_BUTTON)	{
		if(state==GLUT_DOWN){
			My_Scene.translate=1;
			My_Scene.oldx = x;
			My_Scene.oldy = y;
		}
		else{
			My_Scene.translate=0;
			My_Scene.oldx = x;
			My_Scene.oldy = y;
		}
	}
}


void Special(int key , int x ,int y){

	glMatrixMode(GL_PROJECTION);

	switch(key){
		case 100:translations[0] -= My_Scene.Object_Move[0];

			break;
		case 101:translations[1] += My_Scene.Object_Move[0];

			break;
		case 102:translations[0] += My_Scene.Object_Move[1];

			break;
		case 103:translations[1] -= My_Scene.Object_Move[1];

			break;
	}

	glutPostRedisplay();
	glMatrixMode(GL_MODELVIEW);
}


void Keyboard(unsigned char key, int x , int y){

double gap=10.0;

switch(key)	{

	case 'f' :{
				glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
			     }break;

	case 'l' :{
                glLineWidth(1.0);
				glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
				 }break;

	case 'i' :{
				if(My_Scene.lights_on==0)My_Scene.lights_on=1;
				else My_Scene.lights_on=0;
				}break;

	case'+' :{translations[2] += My_Scene.Object_Move[2];}break;
	case'-' :{translations[2] -= My_Scene.Object_Move[2];}break;


	}

glutPostRedisplay();
}


Vector3d TrackballMapping( int x, int y ){

	static const double PI_2 = 3.141592653589793238462643383/2.0;

	Vector3d v(0,0,0);
    static GLint params[4];

	glGetIntegerv(GL_VIEWPORT,params);

	v[0] = ( 2.0 * x - params[2] ) / params[2];
	v[1] = ( params[3] - 2.0 * y ) / params[3];

	double  d = v.norm();
	if( d > 1.0 ) d = 1.0;

	v[2] = cos((PI_2) * d);

	return v.normalized();
}



void Init(){

  glPolygonMode(GL_FRONT,GL_FILL);  // set the drawing mode to full rendering
  glEnable(GL_DEPTH_TEST);          //activate Z buffer (hide elements in the back)

  glEnable(GL_POLYGON_OFFSET_FILL); //useful if you want to superpose the rendering in full mode and in wireless mode
  glPolygonOffset(1.0,1.0);         //convenient settings for polygon offset, do not change this

  glEnable(GL_NORMALIZE);           // unit normals, in case you would forget to compute them
  glEnable(GL_COLOR_MATERIAL);      // now you can associate a color to a point...

  glClearColor(1.0f,1.0f,1.0f,0.0f);    //background color is white (better for screenshot when writing a paper)

  glEnable( GL_BLEND );                 //you can activate blending for better rendering...

  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // careful with those parameters, results depend on your graphic card
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

// activate / disable those and compare ;)

  glEnable( GL_POINT_SMOOTH );
  glHint( GL_POINT_SMOOTH, GL_NICEST );
/*
  glEnable( GL_LINE_SMOOTH );
  glHint( GL_LINE_SMOOTH, GL_NICEST );
*/
  glEnable( GL_POLYGON_SMOOTH );
  glHint( GL_POLYGON_SMOOTH, GL_NICEST );

}
