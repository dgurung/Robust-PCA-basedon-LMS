/***************************************************************************
                                    Robust_PCA.h
                             -------------------
    update               : 2012-12-11
    Author               : Deepak Gurung and Lee Yuan Hang
    email                : dpaq.grg@gmail.com

 ***************************************************************************/

#ifndef ROBUST_PCA_H
#define ROBUST_PCA_H

#include <iostream>

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/Dense>
using namespace Eigen;

#include <vector>
#include <cmath>
#include <algorithm>

#include "PCA.h"
#include "Mesh.h"
#include "NeighborMesh.h"

#include <GL/glut.h>


using namespace std;


class RobustPCA : public Mesh
{
public:
    vector <unsigned int> Q_major;      // Holds the index of points in Major region
    PCA <MatrixXd> pca_best;            // Holds the final Principal Components computed using Q_major

public:
    RobustPCA();
    virtual ~RobustPCA();

    double AnlgeDifference(RobustPCA &P);

    void RPCA_compute(vector <Vector3d> & Q,
                      NeighborMesh & globalmesh,
                      double rmax = 0,
                      double lambda = 1.25,
                      unsigned int MAX_ITERS = 5000);
};


#endif // ROBUST_PCA_H
