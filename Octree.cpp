#include "Octree.h"

Octree :: Octree()
{
    _density = 0;
    for(int i =0; i<8; i++)
        _child[i] = NULL;
}

Octree :: ~Octree()
{
    vector <Vector3d> temp;
    temp.swap(_points);         // just swap the vector _points with empty variable that goes out of scope.

}



//--------------------------------------------------------------------------------------------
//                Build boundary cell
//--------------------------------------------------------------------------------------------

const bool Octree :: build(multimap<unsigned int, Vector3d> &points,
                           const unsigned int count,
                           const unsigned int maximumdepth,
                           const Bounds &bounds,
                           const unsigned int currentdepth)
{
//    cout << "\n ----------------------------------- " <<endl;
//    cout << "CURRENT DEPTH: " << currentdepth << endl;
//    cout << "Current Center: " << bounds.center << endl;
//    cout << "Current radius: " << bounds.radius << endl;

    // STOPPING condition of recursive
    if (currentdepth >= maximumdepth){

        // The node is now no longer subjected to sub-division.
        // At this point node is a leaf
        // Store final count of points
        _density = count;

        // Store the Node center, also known as feature point
        _center = bounds.center;

        // copy all points from multimap into _points of node
        multimap<unsigned int, Vector3d>::const_iterator iter;  //iterator to traverse the vector
        for ( iter = points.begin();  iter != points.end(); ++iter)
        {
            _points.push_back(iter->second);
        }

        //        cout << "\nHere we go" << endl;
        //        for(unsigned int i = 0; i < _points.size() ; i++){
        //            cout << _points[i] << endl;
        //        }

        // cout << "_Center: "  << _center << endl;
        // cout << "BREAKING CONDITION REACHED" << endl;
        // cout << " -----------------------------------------------------------" << endl;
        // cout << " -----------------------------------------------------------" << endl;

        return true;

    }


    //The variable below keeps count of points in 8 child nodes
    unsigned int childPointCounts[8] = {0,0,0,0,0,0,0,0};

    // CLASSIFY EACH POINT to a child node
    // This means 'count' is DIVIDED into 8 different 'childPointCounts'.
    multimap<unsigned int, Vector3d>::iterator it;    // iterator to traverse all the points of this node

    // create a new multimap whose key holds the information about the point's childnode location
    // we have to go through this trouble as we cannot edit key of multimap 'points' // i should change this later
    multimap<unsigned int, Vector3d> newpoint;

    for(it = points.begin(); it != points.end(); it++)
    {

        // center of this node
        Vector3d cur_c;
        cur_c = bounds.center;

        // determine the node current point, cur_p belongs to and store this information on cell location in "code" base on
        // relative position of the point to the center of the current node
        unsigned int key = 0;

        if (it->second[0] > cur_c[0]){ key |= 1; }
        if (it->second[1] > cur_c[1]){ key |= 2; }
        if (it->second[2] > cur_c[2]){ key |= 4; }

        // Keep track of number of points in each child.
        childPointCounts[key]++;

        // update points with key information
        newpoint.insert(pair< unsigned int, Vector3d>(key,it->second));     //cout << it->second; ok
    }

    // Recursively call build() for EACH OF 8 CHILD NODE

    for(unsigned int i= 0; i < 8; i++)
    {

        // If there aren't any more vector <Vector3d> in this child, do not subdivided
        if(!childPointCounts[i]) continue;

        // ALLOCATE the child
        _child[i] = new Octree;

        // create TEMPORARY list of points that were coded just for this child only
        // That is ,temporary list to hold points of i'th node
        multimap<unsigned int, Vector3d> newMMap;

        // Go through the input list of points and copy over the points that
        // were coded for this child,
        for (it = newpoint.begin(); it != newpoint.end(); it++)
        {
            if (it->first == i)
            {
                newMMap.insert(pair<unsigned int, Vector3d>(i, it->second));
            }
        }

        unsigned int newCount = childPointCounts[i];

        // At this vector <Vector3d>, we have a list of points that will belong to this child node.
        // We'll want to remove any vector <Vector3d> with a duplicate 'n' in them
        // This results 'childPointCounts' to be turned into 'newCount' with duplication removed

        // Generating a new bounding volume
        Vector3d boundOffsetTable[8] =
        {
            {-0.5, -0.5, -0.5},
            {+0.5, -0.5, -0.5},
            {-0.5, +0.5, -0.5},
            {+0.5, +0.5, -0.5},
            {-0.5, -0.5, +0.5},
            {+0.5, -0.5, +0.5},
            {-0.5, +0.5, +0.5},
            {+0.5, +0.5, +0.5}
        };



        // Vector3d boundOffsetTable[8];
        // boundOffsetTable[0](-0.5, -0.5, -0.5);
        // calculate offset from the center of the parent's node to the center of the child's node

        Vector3d offset;
        offset = boundOffsetTable[i] * bounds.radius;


        // create a new Bounds, with the center offset and half the radius
        Bounds newBounds;
        newBounds.radius = bounds.radius * 0.5;

        newBounds.center = bounds.center + offset;

        // Recurse
        _child[i]->build(newMMap, newCount, maximumdepth, newBounds, currentdepth+1 );

        // Clean Up the temporary multimap newMMap

        multimap<unsigned int, Vector3d> temp;
        temp.swap(newMMap);     // just swap the multimap newMMap with empty variable that goes out of scope.
    }
    return true;
}



//--------------------------------------------------------------------------------------------------------------------------------
//  Determine the cubic bounding volume for a set of points
//---------------------------------------------------------------------------------------------------------------------------------

const Bounds Octree::calcCubicBounds(const vector <Vector3d> &points,
                                     const unsigned int count)
{
    //what we'll give to the caller

    Bounds b;

    // Determine min/max of the given set of points

    Vector3d   min = points[0];
    Vector3d   max = points[0];

    for (unsigned int i = 1; i < count; i++){
        const Vector3d cur_p = points[i];
        if (cur_p[0] < min[0]) min[0] = cur_p[0];
        if (cur_p[1] < min[1]) min[1] = cur_p[1];
        if (cur_p[2] < min[2]) min[2] = cur_p[2];
        if (cur_p[0] > max[0]) max[0] = cur_p[0];
        if (cur_p[1] > max[1]) max[1] = cur_p[1];
        if (cur_p[2] > max[2]) max[2] = cur_p[2];
    }

    // The radius of the volume (dimension in each direction)

    Vector3d  radius;
    radius = max - min;

    b.center = min + radius * 0.5;


    // Construct a cubic bounding box not just a rectangular bounding box

    b.radius = radius[0];
    if (b.radius < radius[1]) b.radius = radius[1];
    if (b.radius < radius[2]) b.radius = radius[2];

    return b;
}






//-------------------------------------------------------------------------------------------------------------------
//              Traversing octree and copying 3d datasets into a vector of vectors.
//      Objective:  Populate map by cumulative density function and 3d points of leaf node.
//
//      Input: 1. map:  leafNodePoints:   key    -->     cumulative density
//                                        value  -->     vector of all 3d points in a given leaf node
//             2. map:  featurePoints:   key     -->    cumulative density
//                                       value   -->    feature
//-------------------------------------------------------------------------------------------------------------------

// Logic:
// Recursive traversing:
// Recursive stops after a certain depth when       1. there is no child node
//                                                  2. the node has some points, that is _density != 0
//
// On reaching stopping condition, that on reaching the leaf node, populate the vector of map by 3d points
//

bool Octree :: traverse2Map_CdfnPointsnFeature( vector< pair <unsigned int, vector<Vector3d> > > & leafNodePoints,
                                                vector <Vector3d> & featurePoints )
{
    // if this node is empty stop traversing
    // if _density is also 0 then the node is an empty leave this current node

    if( _density == 0 )
    {
        for(unsigned int i = 0; i < 8; i++)
        {
            // Store incomplete trees, since it is not guarenteed that a node has all 8 children

            if(!_child[i]) continue;

            else
            {
                _child[i]->traverse2Map_CdfnPointsnFeature(leafNodePoints, featurePoints);
            }
        }
    }

    // At this point the node is a leaf node, that is no child node
    // that is in a c++ sense, _child[i] = NULL for all i.
    // populate map by _cumulativedensity and 3d points.
    // and populate map

    if (_density !=0 )
    {
        // key of map is cumulative density
        // add _density of this node with previous cumulative density.

        vector< pair <unsigned int, vector<Vector3d> > >::iterator it;

        unsigned int temp = 0;

        // Discard this for the first entry..
        if(!leafNodePoints.empty())
        {
            it = leafNodePoints.end();
            --it;
            temp = it->first;
        }

        unsigned int cumulative_density = temp + _density;

        //cout << "Cumulative Density: " << cumulative_density << "\t" ;
        //cout << "density n : " << (_density ) << "\t\t" ;

        // Copy all 3d points of vectors into map

        //leafNodePoints.insert( pair<unsigned int, vector <Vector3d> >(cumulative_density, _points));
        leafNodePoints.push_back( pair <unsigned int, vector<Vector3d> >(cumulative_density, _points));

        // copy all feature into map
        featurePoints.push_back( _center);

        //cout << "Index: " << leafNodePoints.size() -1 << endl;
        //cout << "\n";

    }
    return true;
}




// Draw Bounding box of leaf node
void Octree::DrawCubicCell(vector<Vector3d> & feature, Bounds *B)
{

    glDisable(GL_LIGHTING);

//    glPointSize(5.0);
//    glBegin(GL_POINTS);
//    glColor3d(1,0,0);
//    glVertex3d ( (B->center)[0],(B->center)[0],(B->center)[0] );
//    glEnd();

    glPointSize(4.0);
    glBegin(GL_POINTS);
    glColor3d(1,0,1);
    for (int i=0; i < feature.size();i++)
    {


        glVertex3f((feature[i][0]),
                    feature[i][1],
                    feature[i][2]);

    }
    glEnd();

    //glBegin(GL_LINES);
    //        glVertex3f(1039.8, -406.752, 249.042);
    //        glVertex3f(3000.0, -406.752, 249.042);
    //        glEnd();

    glEnable(GL_LIGHTING);



}


