/***************************************************************************
                                    Mesh.h
                             -------------------
    update               : 2002-10-11
    copyright            : (C) 2002 by Michaël ROY
    email                : michaelroy@users.sourceforge.net

    Edit: Deepak Gurung and Yohan Fougerolle
    Warning : handle only triangular faces!

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _NEIGHBORMESH_
#define _NEIGHBORMESH_

#include <Eigen/Core>
#include <Eigen/Geometry>
using namespace Eigen;

#include <cassert>
#include <cstring>
#include <vector>
#include <set>
#include <map>
using namespace std;

#include "Mesh.h"
#include "PCA.h"

class NeighborMesh : public Mesh
{
    public:

    //constructor and destructor
    NeighborMesh();
    virtual ~NeighborMesh();

    //attributes

    vector < set<int> > P2P_Neigh;
    vector < set<int> > P2F_Neigh;
    vector < set<int> > F2F_Neigh;

    vector<double> Labels;
    map < pair <int,int>,  set<int> > Edges;

    inline map < pair <int,int>,  set<int> > :: iterator Get_Edge(int i1, int i2)
    {
        pair <int, int> mypair;
        if (i1<i2) {mypair.first = i1; mypair.second = i2; return Edges.find(mypair);}
        else {mypair.first = i2; mypair.second = i1; return Edges.find(mypair);}
    }

    //construction of the previous attributes once the file is loaded

    bool Build_P2P_Neigh();
    bool Build_P2F_Neigh();
    bool Build_F2F_Neigh();
    bool Build_Edges();
    void BuildDistanceLabels(int A);

    //rendering functions for verification

    void DrawP2P_Neigh( int i );
    void DrawP2F_Neigh( int i );
    void DrawF2F_Neigh( int i );
    void DrawEdge( int i);
    void DrawEdge( map< pair<int,int>, set<int> > :: iterator it);
    void DrawBoudaryEdges();
    vector<int> ShortestPath(int, int, bool buildlabels=false);
    void SetColorsFromLabels();
    void SetColorsFromKean(double n = 5);

    //compute extended neighborhoods

    set<int> GetP2P_Neigh( int, int );
    set<int> GetF2F_Neigh( int, int );

    //drawing functions

    void DrawPoints ( set <int> );
    void DrawFaces  ( set <int> );

    void  IllustratePointNeighbourhoodComputation(int,int);
    void  IllustrateFaceNeighbourhoodComputation(int,int);
    void  IllustrateEdges( int n);
    void  IllustrateP2P_Neigh( int n);

    void  IllustrateP2F_Neigh( int n );
    void  IllustrateF2F_Neigh( int n );
    void  IllustrateShortestPaths (int ngeod, int startpointindex);

    int IsObtuse( int face_index);


    // Drawing for Octree
    //void DrawNode();

    void Draw_PCA( PCA <MatrixXd> &, int = 1);
    void Draw_major_region(vector <unsigned int> &, int = 1);

};

#endif // _NEIGHBORMESH_
