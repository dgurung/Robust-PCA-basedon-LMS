/***************************************************************************
                                    SamplingBasedOnOctree.h
                             -------------------
    update               : 2012-12-11
    Author               : Deepak Gurung
    email                : dpaq.grg@gmail.com

 ***************************************************************************/



#ifndef OCTREEBASEDSAMPLING_H
#define OCTREEBASEDSAMPLING_H

#include <vector>
using namespace std;

#include <Eigen/Core>

using namespace Eigen;


#include "useful.h"

//void Octree


//--------------------------------------------------------------------------------------------------------------------------
//  Input arguments:    sample_table    --> holds pair of cdf with corresponding point(s).
//                      no_of_nodes     --> No. of "selected nodes"
//
//  Output arguments:   selected_node               <-- array of location(index) of "selected node"        array size = no_of_nodes
//                      points_of_selected_node     <-- array of random point of selected node      array size = no_of_nodes
//--------------------------------------------------------------------------------------------------------------------------

void PointsSampling(vector < pair <unsigned int, vector<Vector3d> > > & sample_table,
                              const unsigned int          no_of_nodes,
                              unsigned int*         selected_node,
                              vector <Vector3d> &   points_of_selected_node);


void PCbasedOnOctree(vector< pair <unsigned int, vector<Vector3d> > > & sample_table,
                     vector<Vector3d> &feature,
                     unsigned int no_of_nodes,
                     vector<Vector3d> &Q,
                     double rmin = 100000000,
                     int T = 5000);




#endif // OCTREEBASEDSAMPLING_H
