Robust-PCA-basedon-LMS
======================
Traditional PCA is sensitive to outliers. Paper [1] describes a method to compute principal axis by rejecting those outliers. The method builds up two regions major region that contains inliers and a minor region containing outliers. 
The Least median of Squares (LMS) method is used to reject outliers. 
The major region contributes to determine the principal axis of 3D shape. Building a major region is an iterative process. Initially, it starts from certain number of points obtained using Octree based sampling and LMS based error criterion. Major region is then gradually built based on LMS error criterion only.  (Also known as forward search technique). 


Tools and Libraries Required:
--------------------
- Qt 
- Eigen 
- OpenGl

Usage:
------
The program requires 3D models in vrml and off format. These 3D data can be selected by editing the code in main.cpp. 
(I know this is a bit inconvinient and I am planning to change it to an interactive file selection option.)


Datasets:
--------
VRML and off files. 


References:
-----------
1. Robust principal axes determination for point-based shapes using least median of squares. Y. Liu, K. Ramani, CAD 2009.
https://engineering.purdue.edu/cdesign/wp/wp-content/uploads/2012/05/Liu_princaxes2009.pdf



