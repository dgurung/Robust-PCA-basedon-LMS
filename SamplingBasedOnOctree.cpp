/***************************************************************************
                                 SamplingBasedOnOctree.cpp
                             -------------------
    update               : 2013-01-01
    copyright            : (C) 2013 by Deepak Gurung
    email                : dpaq.grg@gmail.com
 ***************************************************************************/


#include <fstream>
#include <iostream>
#include <time.h>
#include <stdio.h>


#include <Eigen/Core>
#include <Eigen/Geometry>


#include <vector>
#include <set>
#include <iterator>

#include "SamplingBasedOnOctree.h"
#include "PCA.h"


//--------------------------------------------------------------------------------------------------------------------------
//  PointsSampling
//  Input arguments:    sample_table    --> holds pair of cdf with corresponding point(s).
//                      no_of_nodes     --> No. of "selected nodes"
//
//  Output arguments:   selected_node               <-- array of location of "selected node"        array size = no_of_nodes
//                      points_of_selected_node     <-- array of random point of selected node      array size = no_of_nodes
//--------------------------------------------------------------------------------------------------------------------------

void PointsSampling(vector < pair <unsigned int, vector<Vector3d> > > & sample_table,
                              unsigned int no_of_nodes,
                              unsigned int *selected_node,
                              vector <Vector3d> & points_of_selected_node)
{

    //-----------------------------------------------
    // Objective OPS-1:  select a certain number of nodes from the sample table randomly
    //-----------------------------------------------
    vector < pair <unsigned int, vector<Vector3d> > >::iterator  iter_sample;
    iter_sample = sample_table.end();
    --iter_sample;                                  // end() takes iterator right after the last element of map
    unsigned int cumulative_density = iter_sample->first;

    //    // No of nodes.......used later for PCA calculation
    //    unsigned int no_of_nodes = 4;

    // generate no_nodes random numbers between [0 CDF]
    // and perform binary search to locate the CDF generate randomly in the vector table

   // unsigned int *key         = new unsigned int [no_of_nodes];           // K : randomly generated between [0 CDF]


    int t;
    for (unsigned int i = 0; i < no_of_nodes; i++ )
    {        
        t = (int)((double)rand() / ((double)RAND_MAX + 1) * cumulative_density); //76;

        selected_node[i] = binarySearch(sample_table, t);

    }


    //-----------------------------------------------
    // OBJECTIVE OPS-2:  PICK a point, at random, from the selected boundary cell in the sample table.
    //-----------------------------------------------

    //cout << "\nFrom sample table: " << endl;

    //iterator to scan across the table.
    vector< pair <unsigned int, vector<Vector3d> > >::iterator  it_table;

    // iterator to scan across points in a given node.
    vector <Vector3d>::iterator   it_table_point ;

    //    // An array to store the randomly picked points from selected boundary cell
    //    vector <Vector3d>  points_of_selected_node;

    unsigned int pick_up_position;
    for (unsigned int i = 0; i < no_of_nodes; i++)
    {

        it_table = sample_table.begin() + selected_node[i];
        //cout<< "Node: "<< selected_node[i]<<endl;
        //cout<<"sel2"<<selected_node[i]<<endl;
        //  cout << "CDF: "<< it_table->first << endl;

        // random point in this boundary cell
        unsigned int sizeofnode         = (it_table->second).size();
        pick_up_position = (int)((double)rand() / ((double)RAND_MAX + 1) * sizeofnode);     // range of random number: [0 maxsizeofnode]

     //   cout << "P: "<<pick_up_position << "\t";
        // position the iterator to pickupposition
        it_table_point = (it_table->second).begin() + pick_up_position;

        // Store the point in a pre-built array
        points_of_selected_node.push_back(*it_table_point);

        //cout << "size of cell: " << sizeofnode << endl;
        //cout << "Random point of selected cell: " << *it_table_point << endl;
    }
    // cout << "Size of point_selected_cell: " << points_of_selected_node.size() << endl;

}




void PCbasedOnOctree(vector< pair <unsigned int, vector<Vector3d> > > & sample_table,
                     vector<Vector3d> &feature,
                     unsigned int no_of_nodes,
                     vector<Vector3d> &Q,
                     double rmin,
                     int T)
{
    // array to hold selected node location
    unsigned int *selected_node = new unsigned int [no_of_nodes];      // [0 Max_no_of_leaf_nodes]

    // array to hold randomly picked point of selected node location
    vector <Vector3d> points_of_selected_node;

    //------------------------------
    //      Looping starts after variable declaration
    //------------------------------

    // Variables used inside loop

    // Use this matrix later to hold selected point as a matrix
    MatrixXd MatNx3;
    // Instantiate an object to compute PCA
    PCA <MatrixXd> pca1;
    // Residual container
    set <double> residual;
    // iterator to access median of residual
    set <double>::iterator residual_iter = residual.begin();

    // Median residual
    double rhalf ;

    for(unsigned int i = 0; i < T; i++ )
    {
        //cout<<"Loop: " << i << endl;
        // output: array node locations, and array of corresponding points

        PointsSampling(sample_table, no_of_nodes, selected_node, points_of_selected_node);


        //---- END OF --- Octree based point sampling-

        //---- PCA  of K points (C_temp)----
        // PCA input:  MatrixXd Points

        // Convert vector <Vector3d> points into MatrixXd
        MatNx3 =  VectVect3d2Mat(points_of_selected_node);
        pca1.pca_mat = MatNx3;        
        pca1.compute(); //PCA Computation

        //------------------------------- END OF ------------  PCA -----------------------------
        //------------------------------------  Residuals of features of each cell and its projection on 1st principal axis---------------------

        // clear all residuals entires
        residual.clear();
        residual_iter = residual.begin();

        for(unsigned int j =0; j < feature.size(); j++)
        {
            // Store all the residuals of feature
            residual.insert(    pca1.residuals_other(feature[j])    );            
        }

        residual_iter = residual.begin();
        advance (residual_iter, int(feature.size()/2));

        rhalf = *residual_iter;

        // If rhalf is the minimum among all then store principal axis, mean and the selected points

        if (rhalf <= rmin)
        {
            rmin = rhalf;

            // Clear Previous data and copy new cell
            Q.clear();
            copyVecFast(Q, points_of_selected_node);

            // Store the 3 principal axis and selected points
         }

        // Clear the previous k points.
        points_of_selected_node.clear();
    }


}
