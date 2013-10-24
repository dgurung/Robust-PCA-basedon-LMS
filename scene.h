/***************************************************************************
                                 scene.h
                             -------------------
    update               : 2009-10-09
    copyright            : (C) 2008-2009 by Yohan Fougerolle
    email                : yohan.fougerolle@u-bourgogne.fr
 ***************************************************************************/

//*********************************************************************/
//
//					Scene.h : you should not have to work on that file
//                  unless you want to implement your own rendering functions for the scene
//
//*********************************************************************/

#ifndef _VIEWER_
#define _VIEWER_

#include <GL/glut.h>
#include <vector>
using namespace std;

#include <Eigen/Core>
using namespace Eigen;


//one day i'll implement this in a clean way

class scene{
public:

    //render the 2D color bar in the OGL window
	void Draw_Color_Bar(int size_x,int size_y,int x_init, int y_init);

	//Init lights before rendering
	void Create_Lighting(Vector3d Pmin, Vector3d Pmax, Vector3d Cam_pos, Vector3d Cam_targ);

	//constructor and destructor
	scene();
	virtual ~scene();
	//render the 3D referential in the OGL window
	int Draw_Axis(double size);		//dessine un repere

	//variable to store the state of the rendering

	/*
	Remark : this is a BAD strategy to store such variable!
	Indeed, OpenGL is a state machine, and every single state can be retrieved.

	In other words, most of the following variable are simply redundant, but they reduce the amount of
	pure OpenGL instructions used

	*/

	int lights_on;
	int oldx,oldy;
	float Current_Matrix[4][4];// = {{1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1}};

	int rotate,translate;					// flag de rotation
	int Axis;
	int Axis_Id_List;

	vector<Vector3d> Color_Steps; // for the color bar

	Vector3d Object_Move; //displacement when arrows are pressed from keyboard

	Vector3d Vmax,Vmin;
	vector<Vector3d> Palette; // useless now, can be used if you want to use your own color coding
	void Create_Palette();
};

// Functions related to OpenGl display and event handle

void display (void);    // rendering function
void reshape(int ,int); // self explanatory I guess

//on the art of displaying 2D text in a 2D or 3D world
void PrintMessage( int x, int y, const string message );
void Print3DMessage( int x, int y, const string message );

void Mouse(int, int , int ,int); //function called when you click

void Motion(int,int);   // function called when you move the mouse

void Keyboard(unsigned char , int , int); // when a key is pressed
void Special(int , int , int );           // when a special key is pressed

void Create_Lighting(); // Create and Init lights. Up to 8 lights available

Vector3d TrackballMapping( int x, int y );  // I'll let you figure this out ;)
void Init();    //quick and dirty initialization
#endif
