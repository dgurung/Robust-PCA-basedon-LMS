/***************************************************************************
                                 Robust_PCA.cpp
                             -------------------
    update               : 2013-01-01
    copyright            : (C) 2013 by Deepak Gurung
    email                : dpaq.grg@gmail.com
 ***************************************************************************/



#include <iostream>

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/Dense>

#include <vector>
#include <map>

#include <cmath>

#include <time.h>

#include <algorithm>

#include "Mesh.h"
#include "PCA.h"
#include "Robust_PCA.h"
#include "useful.h"

RobustPCA :: RobustPCA(){}
RobustPCA :: ~RobustPCA(){}

double RobustPCA::AnlgeDifference( RobustPCA &P )
{
    double angle = acos( (this->pca_best.p1).dot(P.pca_best.p1)) * 180.0/PI;
    return angle;
}

void RobustPCA :: RPCA_compute(vector <Vector3d> &Q,
                               NeighborMesh &globalmesh,
                               double rmax,
                               double lambda,
                               unsigned int MAX_ITERS)
{
    //--------------------------------------------------------------------------------------------------------------
    //
    //                      Algorithm1: Robust Principal Axis
    //
    //--------------------------------------------------------------------------------------------------------------


    // in: pca_best with 4 points
    // out: pca_best with best PA

    // compute rmax using Q
    MatrixXd MatQNx3 = VectVect3d2Mat(Q);
    pca_best.pca_mat = MatQNx3;
    pca_best.compute();

    double rt = 0;
    double rnew;


    for(unsigned int i = 0; i < Q.size(); i++)
    {
        rnew = pca_best.residuals_other(Q[i]);
        if (rt < rnew)
            rt = rnew;
    }

    rmax =lambda * rt;

    //--------------- rmax is obtained at this point

    //------------------------- Looping starts here -----------------------------

    double residual_crem;

    // FILL Q_major by the 4 points obtained from LMS algorithm 1.

    for (unsigned int i = 0; i < Q.size(); i++ )
    {
        for(unsigned int j = 0; j < globalmesh.vertices.size(); j++)
        {

            if( equals(Q[i], globalmesh.vertices[j]) )
            {
                Q_major.push_back(j);       // Push index of globalmesh.
            }
        }
    }

    // At the same time fill CRem by the remaining points

    int flag = 0;

    // The main idea is whichever point is in Q_major is not in CRem
    // within the program, size of previous CRem keeps on decreasing and at the end of loop it holds point of minor region
    // At the same time size of Q_major keeps on increasing and at the end of loop it holds points of major region
    // Copy points from previous CRem into Q_major that satisfies residual threshold condition (r < rmax)

    // Compute PCA based on Q_major

    // First, create vector of Vector3d
    vector< Vector3d > vec3_vec;

    unsigned int ITERATION = 0;

    int count_m;

    int FLAG_BREAK = 0;

    while( ITERATION++ < MAX_ITERS && !FLAG_BREAK )
    {
        multimap <double, unsigned int> CRem_vectors;

        // Data type conversion for PCA computation
        vec3_vec.clear();
        for(unsigned int i = 0; i < Q_major.size(); i ++)
        {
            vec3_vec.push_back( globalmesh.vertices[ Q_major[i] ] );
        }

        pca_best.pca_mat =  VectVect3d2Mat(vec3_vec);

        // Second, Compute PCA usingMatrix pca_mat
        pca_best.compute();


        //Compute residuals for Crem points and
        // If residuals is less than rmax then store the point in Q

        // sort Q_major for binary search
        std::sort(Q_major.begin(),Q_major.end(),std::less<unsigned int>());

        for(unsigned int i=0; i < globalmesh.vertices.size(); i++)
        {
            // Query if i is in Q_major

//            for(unsigned int j = 0; j < Q_major.size(); j++ )
//            {
//                if (Q_major[j] == i )       // search in mesh
//                { flag = 1;
//                    cout << "loo" << Q_major[j] << endl;
//                }
//            }

            if(BIsearchVec(Q_major, i))      //use sorted Q_major
               {flag = 1;
               //cout << "bi" << i << endl;
            }

            if (flag == 0 )
            {
                residual_crem = pca_best.residuals_other(globalmesh.vertices[i]);
                CRem_vectors.insert(pair<double, unsigned int> (residual_crem, i) );
            }
            flag = 0;
        }

        // At this point we have: Crem - Q

        // Take m least residual points

        count_m = 0;

        std::multimap<double, unsigned int>::iterator It_crem_res = CRem_vectors.begin();

        for (It_crem_res =  CRem_vectors.begin(); It_crem_res != CRem_vectors.end(); It_crem_res++)
        {

            Q_major.push_back((*It_crem_res).second);
            count_m++;
            if(count_m ==  30) //50 axle
            {
                if((*It_crem_res).first >= rmax)
                {
                    FLAG_BREAK = 1;
                }
                break;
            }
        }
        //cout<< rmax <<" : "<< (*It_crem_res).first<<endl;
       // cout << "iteration:" << ITERATION << endl;
       // cout << "Major Region Size: " << Q_major.size() << endl;

        // Delete Crem
        CRem_vectors.erase(CRem_vectors.begin(), CRem_vectors.end());
    }
    cout << "rmax: " << rmax << endl;



}




// maintaing two container simultaneously
// Copy from one container CRem_previous to Q_Major with memory handling

// UPDATE MAJOR REGION Q and Principal Axis accordingly.

//    while( ITERATION++ < MAX_ITERS )
//    {

//        // Data type conversion for PCA computation
//        vec3_vec.clear();
//        for(unsigned int i = 0; i < Q_major.size(); i ++)
//        {
//            vec3_vec.push_back( globalmesh.globalmesh.vertices[ Q_major[i] ] );
//        }

//        pca_best.pca_mat =  VectVect3d2Mat(vec3_vec);

//        // Second, Compute PCA usingMatrix pca_mat
//        pca_best.compute();

//        //cout << "OK P1: " << pca_best.p1 <<  endl;

//        //Compute residuals for Crem points and
//        // If residuals is less than rmax then store the point in Q

//        std::vector<unsigned int>::iterator Iter_to_crem;
//        Iter_to_crem = CRem_previous.begin();

//        //for (unsigned int i = 0; i < CRem_previous.size(); i++)
//        unsigned int i = 0;

//        count_m = 0;

//       while(Iter_to_crem != CRem_previous.end())
//        {

//            // find residual of point with index in CRem_previous
//            residual_crem = pca_best.residuals_other( globalmesh.globalmesh.vertices[ CRem_previous[i] ] );

//            // CHECK is the point can be included in Q_major
//            if (residual_crem <= rmax)
//            {
//                // This means yes point should be included in Q_major
//                // Add the point into Q_major
//                Q_major.push_back( CRem_previous[i] );

//                // Remove the point from CRem_previous // technique to remove: remove by value
//                CRem_previous.erase(std::remove(CRem_previous.begin(), CRem_previous.end(), CRem_previous[i]), CRem_previous.end());
//                // The above line removes point and decreases size of CRem_previous by 1.
//                ++count_m;
//            }
//            else
//            {
//                // This means point is not included in Q_major for current time
//                // Go to another pointer
//                ++i;
//            }

//            // Go to next point which must be checked whether or not to include in Q_major
//            Iter_to_crem = CRem_previous.begin() + i;
//        }
//        cout << "Q count : " << count_m << endl;
//    }

//    cout << "Q size " << Q_major.size() << endl;
//    cout << "Crem " << CRem_previous.size() << endl;

