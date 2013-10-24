/***************************************************************************
                                 PCA_utilities.cpp
                             -------------------
    update               : 2013-01-01
    copyright            : (C) 2013 by Deepak Gurung
    email                : dpaq.grg@gmail.com
 ***************************************************************************/



#include <iostream>
#include <Eigen/Dense>
#include <Eigen/Core>
#include <PCA.h>
#include <vector>
#include <map>

#include "PCA.h"
#include "Robust_PCA.h"

using namespace Eigen;
using namespace std;




//------------------------ FUNCTION -----------------------------------------------

MatrixXd VectVect3d2Mat(vector <Vector3d> & Vect3)
{
    unsigned int N = Vect3.size();

    // Mapping stl::vector to Eigen::Matrix
    Map< Matrix<Vector3d, Dynamic, Dynamic, RowMajor> >  temp (Vect3.data(), 3, N);

    // Allocate Matrix MatNx3
    MatrixXd MatNx3(Vect3.size(),3);

    // copy all the points into MatNx3
    // coping 1 point at a time

    for(int i = 0; i < Vect3.size(); i++)
    {
        MatNx3.row(i) = temp(i);
        //MatNx3 = Vect3[i];
    }

    return MatNx3;
}



//MatrixXd Vect3d2Mat(Vector3d & Vect)
//{
//    MatrixXd MatNx3(Vect.size(),3);

//    for(int i = 0; i < Vect.size(); i++)
//    {
//        //MatNx3.row(i) =  globalme ;
//    }

//    return MatNx3;

//}

