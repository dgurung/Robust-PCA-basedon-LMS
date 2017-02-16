Robust-PCA-basedon-LMS
======================
Traditional PCA is sensitive to outliers. Paper [1] describes a method to compute principal axis by rejecting those outliers. The method builds up two regions major region that contains inliers and a minor region containing outliers. 
The Least median of Squares (LMS) method is used to reject outliers. 
The major region contributes to determine the principal axis of 3D point cloud. Building a major region is an iterative process. Initially, it starts from certain number of points obtained using Octree based sampling and LMS based error criterion. Major region is then gradually built based on LMS error criterion only.  (Also known as forward search technique). 

Algorithm:
----------
Algorithm to implement the above mentioned method is detailed in our presentation slides.[Link](https://github.com/dgurung/Robust-PCA-basedon-LMS/blob/master/RPCA.v1.1.pdf)

Tools and Libraries Required:
--------------------
- Qt 
- Eigen 
- OpenGL

Usage:
------
The program requires 3D models in vrml and off format. These 3D data can be selected by editing the code in main.cpp. 
(I know this is a bit inconvinient and I am planning to change it to an interactive file selection option.)


Datasets:
--------
VRML and off files. 

Output:
-------
The green colored points indicate the points considered to compute the PCA. The three principal axes are shown by Red (1st PA), Green (2nd PA), and Blue (3rd PA) arrows.
![Ouput snapshot](https://github.com/dgurung/Robust-PCA-basedon-LMS/blob/master/output.png)


References:
-----------
1. Robust principal axes determination for point-based shapes using least median of squares. Y. Liu, K. Ramani, CAD 2009.
https://engineering.purdue.edu/cdesign/wp/wp-content/uploads/2012/05/Liu_princaxes2009.pdf



