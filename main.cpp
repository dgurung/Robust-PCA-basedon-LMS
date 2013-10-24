/***************************************************************************
                                 main.cpp
                             -------------------
    update               : 2013-01-01
    copyright            : (C) 2013 by Deepak Gurung
    email                : dpaq.grg@gmail.com
 ***************************************************************************/


#include <iostream>
#include <fstream>
#include <time.h>
#include <string>
#include <algorithm>
#include <map>

// Headers for Mesh processing
#include "scene.h"
#include "constante.h"
#include "useful.h"
#include "Stopwatch.h"
#include "NeighborMesh.h"
#include "Mesh.h"

// Headers for Robust PCA
#include "Octree.h"
#include "SamplingBasedOnOctree.h"
#include "PCA.h"
#include "Robust_PCA.h"
#include "constante.h"

using namespace std;

// horrible global variables next
// opengl rendering of the scene implemented "quick and (very) dirty" way

Stopwatch timer;   // in case you want to have a class to measure time

scene My_Scene;    // class to handle lights, position, etc

NeighborMesh globalmesh;   // yes another infamous global variable

int id_globalmesh; // identifier for display list. See function display() in scene.cpp

//global variables... could be handled as attributes in the scene class
Vector3d Camera_Target;
Vector3d Camera_Position;
double znear, zfar;

int main(int argc,char ** argv)
{
    //openGL initialization
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA |GLUT_DEPTH);
    glutInitWindowSize(800,600);
    glutInitWindowPosition(200,200);
    glutCreateWindow("Mesh Viewer");
    glutReshapeFunc(reshape); // function call when reshape the window
    glutMotionFunc(Motion);   // function called when mouse move
    glutMouseFunc(Mouse);     // function called when mouse click
    glutKeyboardFunc(Keyboard); // if key pressed
    glutSpecialFunc(Special);   // if special key pressed
    glutDisplayFunc(display);   // display function

    //my own inits here
    Init();
    My_Scene.Axis_Id_List=My_Scene.Draw_Axis(30);

    //example on How To Use the timer
    timer.Reset();
    timer.Start();

    //load a mesh from a file
    string file_name ("VRML/Axle.wrl");
    if (!globalmesh.ReadFile(file_name)) exit(0);
    timer.Stop();
    cout<<"Loading time :"<< timer.GetTotal()/1000.0<<" s"<<endl;

    //construct P2P : point to point connectivity
    timer.Reset();
    timer.Start();
    globalmesh.Build_P2P_Neigh();

    timer.Stop();
    cout<<"P2P construction :"<< timer.GetTotal()/1000.0<<" s"<<endl;

    //construct P2F : point to face connectivity
    timer.Reset();
    timer.Start();
    globalmesh.Build_P2F_Neigh();
    timer.Stop();
    cout<<"P2F construction :"<< timer.GetTotal()/1000.0<<" s"<<endl;

    //construct F2F : face to face connectivity
    timer.Reset();
    timer.Start();
    globalmesh.Build_F2F_Neigh();
    timer.Stop();
    cout<<"F2F construction :"<< timer.GetTotal()/1000.0<<" s"<<endl;

    //construct Edges
    timer.Reset();
    timer.Start();
    globalmesh.Build_Edges();
    timer.Stop();
    cout<<"Egdes construction :"<< timer.GetTotal()/1000.0<<" s"<<endl;


    //compute normals
    globalmesh.ComputeFaceNormals(); //normal to faces
    globalmesh.ComputeVertexNormals(); //normals to vertex

    //------------------------------------------------------------------------------------------------------------
    //      Traditional global pca
    //------------------------------------------------------------------------------------------------------------
    PCA <MatrixXd> globalpca;

    MatrixXd MatglobalNx3 = VectVect3d2Mat(globalmesh.vertices);
    //cout << MatglobalNx3;
    globalpca.pca_mat = MatglobalNx3;
    //cout << globalpca.pca_mat;
    globalpca.compute();

    //----------------------------------------------------------
    //                  parameters
    //----------------------------------------------------------

    double rmax = 0;
    unsigned int MAX_ITERS = 5000;

    unsigned int no_of_nodes = 4;
    double rmin = 100000000;

    int T = 5000;


    //--------------------------------------------------------------------------------------------------------------
    //                      Algorithm2: LMS
    //--------------------------------------------------------------------------------------------------------------

    //--------------------------------------------------------------------------------------------------
    //                      Step 1: Sampling points
    //--------------------------------------------------------------------------------------------------
    //--------------------------- BOUNDING BOX ------------
    // calculate Bounding box
    Octree base;
    Bounds *BB = new Bounds ;
    unsigned int count = globalmesh.vertices.size();
    *BB = base.calcCubicBounds(globalmesh.vertices, count);

    //-------------------------- OCTREE CONSTRUCTION -----

    // convert globalmesh.vertices which is in 'vector<Vector3d>' datatype to 'multimap<unsigned int,vector<Vector3d> >' datatype
    // the key of multimap 'unsigned int' holds the information of the selected_node of point,
    // that is about the point's position among 8 child node, within a given root node.

    vector<Vector3d> &points = globalmesh.vertices;
    multimap<unsigned int, Vector3d> map_points;

    vector2Map(points, map_points);

    timer.Reset();
    timer.Start();

    // Build an octree of mapped points
    bool state = base.build(map_points, count, 5, *BB, 0);

    timer.Stop();
    if (state = true)
    {   cout << "\nOctree constructed" << endl; }
    cout<<"\nOctree construction :"<< timer.GetTotal()/1000.0 <<" s"<<endl;


    //------------------------- OCTREE TRAVERSE TO MAP:  1.CDF with LEAF NODE POINTS and 2. CDF with FEATURE POINTS---------------------

    // Sampled vector of density and 3d points of leaf node

    // The table that I create are all vector types because at a later stage I'll have to use random iterators to access data
    // Specifically, I use random iterators when searching for the key. (I use binary search to speed up search)

    // vector of cdf associated with all points
    vector< pair <unsigned int, vector<Vector3d> > >            sample_table;

    // vector of cdf associated with feature points
    vector <Vector3d>    feature;

    // Fill up above 2 tabels
    base.traverse2Map_CdfnPointsnFeature(sample_table, feature);

    cout << "Traverse Completed. Total Leaf nodes: " << sample_table.size() << endl;


    //-------------------------------- END OF ------- OCTREE TRAVERSE TO MAP------------------------------
    // At this point, we have: 1. points separated out in different nodes, and 2. feature points of each node


    //------------------------------------------------------------------------
    //
    //The following loop is to obtain table of lambda and corresponding angle_difference.
    //
    //-----------------------------------------------------------------------

    int n_lambda = 1;       // number of different lambda value
    double *lambda = new double [n_lambda];
    double *angle_dif = new double [n_lambda];
    double *ang_pc = new double [n_lambda];

    for( int i = 0; i < n_lambda; i++  )
    {
        lambda[i] = 1.1 + 0.1 * (i+1);
        angle_dif[i] = 0;
        ang_pc[i] = 0;
    }

    //--------------------------------------------- Octree based point sampling--------------------------------------------

    srand(time(NULL));

    // the varying values of lambda in this loop controls the number of points to be considered for computing
    // major principal axis.
    for(int p = 0; p < n_lambda; p++)
    {
        no_of_nodes = 4;
        rmin = 100000000;
        T = 5000;

        vector<Vector3d> Q;     // Q is a container to hold points obatined from octree based approximation
        timer.Reset();
        timer.Start();
        PCbasedOnOctree(sample_table,feature, no_of_nodes, Q, rmin, T);
        timer.Stop();
        cout<<"\nLMS Computation: "<< timer.GetTotal()/1000.0 <<" s"<<endl;


        //---------------------------------------------------
        //          Algorithm 2 ends here
        //----------------------------------------------------

        //---------------------------------------------------------------------------------------------------
        //                      Algorithm1: Robust Principal Axis
        //---------------------------------------------------------------------------------------------------
        // Find points in Major region that is used for PCA calulation
        // Major region grows from 4 points obtained by octree based sampling

        // compute initial pc of Q
        MatrixXd MatNx3 =  VectVect3d2Mat(Q);
        PCA <MatrixXd> pca1(MatNx3);
        //PCA Computation
        pca1.compute();

        rmax = 0;
        MAX_ITERS = 5000;

        timer.Reset();
        timer.Start();
        RobustPCA rpca;
        rpca.RPCA_compute(Q, globalmesh, rmax, lambda[p], MAX_ITERS);      // resulting instance, rpca, will hold points of Major region

        cout<<"\nRobust PCA Computation: "<< timer.GetTotal()/1000.0 <<" s"<<endl;

        //--------------------------------------------------------------------------------------------------------------------------                    
        //              Robust PCA ends Here        
        //--------------------------------------------------------------------------------------------------------------------------
        // angle difference between inital pc and rpc
        ang_pc[p] = pca1.PcaAngleDif(rpca.pca_best);
        //------------------------------------------


        //-------------------------------------------------------------------------
        //
        //  roughly adjust view frustrum to object and camera position
        //
        //-------------------------------------------------------------------------

        Vector3d Pmin(globalmesh.vertices[0]), Pmax(globalmesh.vertices[0]);
        Vector3d Center(0,0,0);

        for( int i=0; i<globalmesh.vertices.size(); i++)
        {
            Vector3d P(globalmesh.vertices[i]);
            for( int j=0; j<2; j++)
            {
                Pmin[j] = min(Pmin[j],P[j]);
                Pmax[j] = max(Pmax[j],P[j]);
            }
            Center += P;
        }

        Center/=globalmesh.vertices.size();
        Camera_Target = Center;

        //length of the diagonal of the bouding box
        double distance = (Pmax-Pmin).norm();

        //set arbitraty position to camera and adjusts max and min view planes
        Camera_Position = Camera_Target + Vector3d(0,0,distance*3);
        znear = distance*0.1;
        zfar = distance*5;

        //adjust displacements consequently
        My_Scene.Object_Move[0]=My_Scene.Object_Move[1]=My_Scene.Object_Move[2] = distance/20;

        //creates lights accordingly to the position and size of the object
        My_Scene.Create_Lighting(Pmax,Pmin, Camera_Position, Camera_Target);

        //    //compute normals
        //    globalmesh.ComputeFaceNormals(); //normal to faces
        //    globalmesh.ComputeVertexNormals(); //normals to vertex

        //    cout << globalmesh.vertices.size() << endl;
        //    cout << globalmesh.faces.size() << endl;

        //    cout << globalmesh.vertex_normals.size() << endl;
        //    cout << globalmesh.face_normals.size() << endl;

        glDisable(GL_LIGHTING);


        //Example of distance computing:

        int startpointindex = 0;

        timer.Reset();
        timer.Start();
        // will compute approximate geodesic distance from this point to all other points
        // consider the mesh as a graph and run shortest path algo
        // then stores distances in the label array
        globalmesh.BuildDistanceLabels(startpointindex);
        timer.Stop();
        cout<<"Distance label construction :"<< timer.GetTotal()/1000.0<<" s"<<endl;

        //construct colors from labels
        timer.Reset();
        timer.Start();
        globalmesh.SetColorsFromLabels();
        timer.Stop();
        cout<<"Constructing colors from distance :"<< timer.GetTotal()/1000.0<<" s"<<endl;


        //--------------------------------------------------------
        //                      RENDERING
        //--------------------------------------------------------
        // glNewList creates display list.
        // Display lists are group of GL commands that are stored for subsequent execution
        id_globalmesh=glGenLists(1);    // for managing display(rendering) list
        glNewList(id_globalmesh,GL_COMPILE_AND_EXECUTE);        // 1st argument is id of this particular list
        // 2nd argument is symbolic constant, that makes the GL commands to execute as they are compiled into the display list        
        glDisable(GL_LIGHTING);
        glEnable(GL_LIGHTING);
        glLineWidth(1);
        globalmesh.Draw_PCA(rpca.pca_best);
        globalmesh.Draw_major_region(rpca.Q_major);
        glEndList();

        cout << "Major %: " << ( double(rpca.Q_major.size())/ double(globalmesh.vertices.size()) )<< endl;
    }
    cout << "Robust PCA computed"  << endl;
    glutMainLoop();
    return 0;
}


