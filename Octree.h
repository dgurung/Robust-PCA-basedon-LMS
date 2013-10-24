/***************************************************************************
                                    PCA.h
                             -------------------
    update               : 2012-12-11
    Author               : Deepak Gurung
    email                : dpaq.grg@gmail.com

 ***************************************************************************/



#ifndef SAMPLE_TEST_H
#define SAMPLE_TEST_H

#include <Eigen/Core>
#include <Eigen/Geometry>

#include <vector>
#include <map>
#include <utility>

#include <GL/glut.h>

#include "NeighborMesh.h"
#include "useful.h"

using namespace Eigen;

//-----------------------------------------------------------------------------
//  Bounds class defines cubic bounding vloume (center, radius)
//-----------------------------------------------------------------------------
class Bounds{
public:
    Vector3d center;
    double radius;
};


//-----------------------------------------------------------------------------------------------
//                Octree class for query, build.
//-----------------------------------------------------------------------------------------------
class Octree
{
private:
    Octree              *_child[8];
    vector <Vector3d>   _points;
    Vector3d            _center;    // This is a feature point of a node.
    unsigned int _density;


public:
    Octree();
    ~Octree();

    virtual const bool build(multimap<unsigned int, Vector3d> &points,
                             const unsigned int count,
                             const unsigned int maximumdepth,
                             const Bounds &bounds,
                             const unsigned int currentdepth);

    static const Bounds calcCubicBounds(const vector <Vector3d> &points,
                                        const unsigned int count);

    // function with arguments to hold all points in indexed memory associated with cumulative density.
    bool traverse2Map_CdfnPointsnFeature(vector< pair <unsigned int, vector<Vector3d> > > & leafNodePoints,
                                         vector< Vector3d> & featurePoints);

    void DrawCubicCell(vector<Vector3d> & feature, Bounds *B);
};


#endif // SAMPLE_TEST_H



