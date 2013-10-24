/***************************************************************************
                                    PCA.h
                             -------------------
    update               : 2012-12-11
    Author               : Deepak Gurung and Lee Yuan Hang
    email                : dpaq.grg@gmail.com

 ***************************************************************************/


#ifndef PCA_H
#define PCA_H

#include <iostream>

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/Dense>
#include "constante.h"
//#include "NeighborMesh.h"

#include <vector>
#include <cmath>

#include <GL/glut.h>

using namespace Eigen;

using namespace std;


// Convert vector <Vector3d> to MatrixXd
MatrixXd VectVect3d2Mat(vector <Vector3d> & Vect3);

MatrixXd Vect3d2Mat(Vector3d &Vect);

template <typename T>
class PCA
{

public:

    PCA();
    PCA(const T);
    VectorXd vec_mean; //column vector


    VectorXd p1,p2,p3;
    void compute();
    void residuals();
    T projection();
    double residuals_other(Vector3d &);
    T res_mat;
    T adjusted_mat;

    T pca_mat;      // Matrix onto which pca is performed

    void DrawPCA(vector <Vector3d> & );
    double PcaAngleDif(PCA <T> & );

private:

    T V_mat;

    T projected_mat;
    Vector3d projected_vec;
    T adjusted_mat2;
};

template <typename T>
PCA<T>::PCA()
{
  //Default constructor
};

template <typename T>
PCA<T>::PCA(const T mat)
{
    pca_mat =  mat;

};

template <typename T>
void PCA<T>::compute()
{

    vec_mean = pca_mat.colwise().mean();

    adjusted_mat = pca_mat.rowwise() - vec_mean.transpose();

    //N being the number of rows in other words, the number of points
    double N = adjusted_mat.rows();

    adjusted_mat2 = adjusted_mat;
    adjusted_mat2 = (adjusted_mat2.array()/sqrt(N-1)).matrix();

    JacobiSVD <T> svd(adjusted_mat2, ComputeThinU | ComputeThinV);


    V_mat = svd.matrixV();
    V_mat.col(2) = -V_mat.col(2); //a little weird perhaps insignificant but just to get the ame result with matlab

    T S_val = svd.singularValues(); //eigen values already sorted
    p1 = V_mat.col(0);
    p2 = V_mat.col(1);
    p3 = V_mat.col(2);



};


template <typename T>
T PCA<T>::projection()
{
//only can be called after svd is done
//projection on principal axis

    projected_mat = (adjusted_mat*p1)*p1.transpose();
    projected_mat = projected_mat.rowwise() + vec_mean.transpose();

    return projected_mat;
}


//correction:: pass by reference
template <typename T>
double PCA<T>::residuals_other(Vector3d & vec)
{
    Vector3d adjusted = vec - vec_mean;
    double res =0;
    double j = (p1.transpose()*adjusted);
    projected_vec = j*p1;
    projected_vec = projected_vec + vec_mean;
    res = (vec - projected_vec).norm();

    return res;
}



template <typename T>
void PCA <T>::residuals()
{
    res_mat = (pca_mat - projected_mat).rowwise().norm();
}


template <typename T>
double PCA <T>::PcaAngleDif(PCA <T> & pca1)
{
    double angle = acos( (this->p1).dot(pca1.p1)) * 180.0/PI;
    return angle;
}





//template <typename T>
//void PCA <T>::DrawPCA( vector<Vector3d> & Q)
//{

//    glDisable(GL_LIGHTING);

//    glPointSize(20.0);
//    glBegin(GL_POINTS);

//        glColor3f(0,1,0);

//        for(int i = 0; i < Q.size(); i++)
//        {
//            glVertex3d(Q[i][0], Q[i][1], Q[i][2]);
//            glColor3f(0,1,0);
//        }

//        glColor3f(0.3,0.3,0.3);
//        glVertex3d(vec_mean[0], vec_mean[1], vec_mean[2]);
//        glEnd();


//        int alpha = 5000;

//        glLineWidth(4);
//    glBegin(GL_LINES);
//    glColor3f(1,0,0);
//    glVertex3d(vec_mean[0], vec_mean[1], vec_mean[2]);
//    glVertex3d( (alpha * p1[0] + vec_mean[0]), (alpha * p1[1] +  vec_mean[1]),  (alpha * p1[2] + vec_mean[2]));
//    glEnd();

//    glBegin(GL_LINES);
//    glColor3f(0,1,0);
//    glVertex3d(vec_mean[0], vec_mean[1], vec_mean[2]);
//    glVertex3d( (alpha * p2[0] + vec_mean[0]), (alpha * p2[1] +  vec_mean[1]),  (alpha * p2[2] + vec_mean[2]));
//    glEnd();

//    glBegin(GL_LINES);
//    glColor3f(0,0,1);
//    glVertex3d(vec_mean[0], vec_mean[1], vec_mean[2]);
//    glVertex3d( (alpha * p3[0] + vec_mean[0]), (alpha * p3[1] +  vec_mean[1]),  (alpha * p3[2] + vec_mean[2]));
//    glEnd();

//    glEnable(GL_LIGHTING);



//}





#endif // PCA_H
