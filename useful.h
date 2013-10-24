/***************************************************************************
                                  useful.h
                             -------------------
    update               : 2009-10-09
    copyright            : (C) 2008-2009 by Yohan Fougerolle
    email                : yohan.fougerolle@u-bourgogne.fr

    Edited: 2012-12-31   : Deepak Gurung
 ***************************************************************************/


#ifndef _USEFUL_
#define _USEFUL_

#include <cstring>
#include <vector>
#include <fstream>
#include <iostream>
#include <map>

using namespace std;

#include "Mesh.h"
#include <ctime>
#include "constante.h"

#include <Eigen/Core>
using namespace Eigen;

    // convertion RGB <--> value between [0,1]
	Vector3d DoubleToColor( double d );
	double ColorToDouble(Vector3d RGB);
	Vector3d DoubleToColorDiscrete( const double d, double n =20);

    //OpenGL text display
	void PrintMessage(int,int, const string);

    //mesh perturbation if you want to play a little bit
    void AddGaussianNoise(vector<Vector3d> & vertices, vector<Vector3d> normals, double sigma,double mu=0);

    //read and write 3D files
    bool ReadIv( Mesh& mesh, const string& file_name );
    bool WriteIv( const Mesh& mesh, const string& file_name, const bool& vrml1 );

    // Read mesh from OFF file
    bool ReadOff(Mesh&, const string& file_name);

    //miscellanous
    void ScreenShot(string File); // from opengl buffer to infamous .tga export


    // binary search
    unsigned int binarySearch(vector<pair<unsigned int, vector<Vector3d> > > &array,                              
                              unsigned int key);

    bool BIsearchVec(vector<unsigned int> &,
                     unsigned int key);

    // Copy vector to vector of same size
    void copyVecFast(vector<Vector3d> &new_,
                     vector<Vector3d> original);

    // vector comparision
    bool equals(const Vector3d &v,
                const Vector3d &w);
    void vector2Map (const vector<Vector3d> &points, multimap<unsigned int, Vector3d> &map_points);


#endif



