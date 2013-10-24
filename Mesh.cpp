/***************************************************************************
                                  Mesh.cpp
                             -------------------
    update               : 2002-12-05
    copyright            : (C) 2002 by Michaël ROY
    email                : michaelroy@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <fstream>
#include <iostream>
#include <GL/glut.h>

#include "Mesh.h"
#include "constante.h"
#include "useful.h"
using namespace std;

Mesh::Mesh(){}
Mesh::~Mesh(){}
//--
//
// ClearAll
//
//--

void Mesh::ClearAll()
{
	// Initialize mesh
	// Clear all data
	ClearVertices();
	ClearFaces();
	ClearColors();
	ClearTextures();
	ClearFaceNormals();
	ClearVertexNormals();
	ClearTextureName();
}

//--
//
// ComputeFaceNormals
//
//--
// Compute unit normal of every faces
void Mesh::ComputeFaceNormals()
{
	// Resize face normal array
	face_normals.resize( FaceNumber() );

	// For every face
	int MaxFaceNumber = FaceNumber();
	for( int i=0; i<MaxFaceNumber; i++ )
	{
		// Compute unit face normal
		//FaceNormal(i) = ComputeFaceNormal(i);
		face_normals[i] = ComputeFaceNormal(i);
	}
}

//--
//
// ComputeVertexNormals
//
//--
// Compute unit normal of every vertex
void Mesh::ComputeVertexNormals()
{

	int i;

	// Assume that face normals are computed
	assert( FaceNormalNumber() == FaceNumber() );

	// Resize and initialize vertex normal array
	vertex_normals.assign( VertexNumber(), Vector3d(0,0,0) );

	// For every face
	for( i=0 ; i<FaceNumber() ; i++ )
	{
		// Add face normal to vertex normal
		VertexNormal(i,0) += FaceNormal(i);
		VertexNormal(i,1) += FaceNormal(i);
		VertexNormal(i,2) += FaceNormal(i);
	}

	// For every vertex
	for( i=0 ; i<VertexNumber() ; i++)
	{
		// Normalize vertex normal
		VertexNormal(i).normalized();
	//	logfile<<vertex_normals[i]<<endl;
	}


}

//
//--
//
// ReadFile
//
//--
// Read mesh from a file
bool Mesh::ReadFile( const string& file_name )
{
	FileFormat file_format(UNKNOWN_FILE);

	cout<<"READ FILE"<<endl;
	// Find file extension
	int pos = file_name.find_last_of('.');
	if( pos == -1 )
	{
		// File has no extension
		return false;
	}

	// Format extension string
	string extension = UpperCase( file_name.substr( ++pos ) );

	cout<<"Extension="<<extension.data()<<endl;

	//Point Cloud extension
	if( extension == "XYZ" )
	{
		file_format = CLOUD_FILE;
	}
	else
	// RANGE TXT extension
	if( extension == "TXT" )
	{
		file_format = RANGE_FILE;
	}
	else
	// WRL extension
	if( extension == "WRL" )
	{
		file_format = VRML_1_FILE;
	}
	// IV extension
	else if( extension == "IV" )
	{
		file_format = IV_FILE;
	}
	// SMF extension
	else if( extension == "SMF" )
	{
		file_format = SMF_FILE;
	}
	// STL extension
	else if( extension == "STL" )
	{
		ifstream file(file_name.c_str(), ios::binary);
		if( file.is_open() == false )
		{
			return false;
		}
		char header[5];
		file.read(header, 5);
		if( strcmp(header, "solid") == 0 )
		{
			file_format = STL_ASCII_FILE;
		}
		else
		{
			file_format = STL_BINARY_FILE;
		}
		file.close();
	}
	// STLA extension
	else if( extension == "STLA" )
	{
		file_format = STL_ASCII_FILE;
	}
	// STLB extension
	else if( extension == "STLB" )
	{
		file_format = STL_BINARY_FILE;
	}
	// Other extension
    else if( extension == "OFF" ){
            file_format = OFF_FILE_1;
        }
	else
	{
		// Unknown file format
		cout<<"Unknown file extension"<<endl;
		return false;
	}

	// Read file
	switch( file_format )
	{

		// OpenInventor file
		case IV_FILE :

		// VRML 1.0 file
		case VRML_1_FILE :
			return ReadIv( *this, file_name );

        // OFF File
        case OFF_FILE_1:
            return ReadOff( *this, file_name );


		// Other file
		default :{cout<<"Unknown file format"<<endl;
			// Unknown file format
			return false;
				 }
	}
}

//--
//
// WriteFile
//
//--
// Write mesh to a file
bool Mesh::WriteFile( const string& file_name, const FileFormat& file_format ) const
{
	// Write file
	switch( file_format )
	{
		// OpenInventor file
		case IV_FILE :
			return WriteIv( *this, file_name, false );
			break;

		// SMF file
		case SMF_FILE :
		//	return WriteSmf( *this, file_name );
			break;

		// STL ascii file
		case STL_ASCII_FILE :
		//	return WriteStlA( *this, file_name );
			break;

		// STL binary file
		case STL_BINARY_FILE :
		//	return WriteStlB( *this, file_name );
			break;

		// VRML 1.0 file
		case VRML_1_FILE :
			return WriteIv( *this, file_name, true );
			break;

		// Other file
		default :
			// Unknown file format
			return false;
	}
}

//--
//
// UpperCase
//
//--
// Upper case string of a given one
string UpperCase( const string& s )
{
	// Upper case string
	string us;

	// For every character in the string
	string::const_iterator it = s.begin();
	while( it != s.end() )
	{
		// Convert character to upper case
		us += toupper( *it );

		// Next character
		++it;
	}

	// Return upper case string
	return us;
}



void Mesh::Draw_Default(int i){


	for(int j=0; j<3; j++)
		glVertex3d(vertices[i][0],vertices[i][1],vertices[i][2]);


}

void Mesh::Draw_Face_Normal(int i){



	for(int j=0; j<3; j++)	{
		glNormal3d(face_normals[i][0],face_normals[i][1],face_normals[i][2]);
		Vector3d V=Vertex(i,j);
		glVertex3d(V[0],V[1],V[2]);
	}


}

void Mesh::Draw_Vertex_Normal(int i){



	for(int j=0; j<3; j++)	{

		int vertex_index = faces[i][j];

		Vector3d N(vertex_normals[vertex_index]);
		Vector3d V(vertices[vertex_index]);
		glNormal3d(N[0],N[1],N[2]);
		glVertex3d(V[0],V[1],V[2]);
	}


}




void Mesh::Draw(int DRAW_MODE)
{
int nb_faces=faces.size(),i;


switch(DRAW_MODE)
	{
	case ONLY_VERTEX	:
		{
			glDisable(GL_LIGHTING);
			cout<<"Vertices displayed="<<vertices.size()<<endl;

            glPointSize(2.0);
			glBegin(GL_POINTS);
			bool color_on=false;
			if(colors.size() == vertices.size()) color_on=true;
			for( i=0 ; i<vertices.size(); i++ )
			{
                //if(color_on)
                    glColor3f(0.0,0.0,0.0);//(0.9,0.3,0.0);//(colors[i][0],colors[i][1],colors[i][2]);
				glVertex3d(vertices[i][0],vertices[i][1],vertices[i][2]);
			}

			glEnd();

			glEnable(GL_LIGHTING);

		}break;

	case FACE_NORMAL:
		{	glBegin(GL_TRIANGLES);
			for( i=0; i<nb_faces; i++) 				Draw_Face_Normal(i);
		}break;

	case FACE_NORMAL_RGB:
		{
			glBegin(GL_TRIANGLES);
			for( i=0; i<nb_faces; i++) Draw_Face_Normal_Rgb(i);
			glEnd();
		}break;

	case VERTEX_NORMAL		:
		{
			glBegin(GL_TRIANGLES);
			for( i=0; i<nb_faces; i++) Draw_Vertex_Normal(i);
			glEnd();
		}	break;

	case VERTEX_NORMAL_RGB	:
		{
			glBegin(GL_TRIANGLES);
			for( i=0; i<faces.size(); i++) Draw_Vertex_Normal_Rgb(i);
			glEnd();
		}	break;

	}


}


void Mesh::Print_Stats()
{
	char s[250];
	sprintf(s,"Faces   : %d", faces.size());
	PrintMessage( 10, 10, s );
	sprintf(s,"Vertices: %d", vertices.size());
	PrintMessage( 10, 25, s );

	//add any information you want to display as text in the OGL window here

}


void Mesh::Draw_Face_Normal_Rgb(int i)
{


	for(int j=0; j<3; j++)	{
		glNormal3d(face_normals[i][0],face_normals[i][1],face_normals[i][2]);
		Vector3d V=vertices[faces[i][j]];
		Vector3d C=colors[faces[i][j]];
		glColor3d(C[0],C[1],C[2]);
		glVertex3d(V[0],V[1],V[2]);
	}


}

void Mesh::Draw_Vertex_Normal_Rgb(int i){

	for(int j=0; j<3; j++)	{

		int vertex_index = faces[i][j];

		Vector3d N(vertex_normals[vertex_index]);
		Vector3d V(vertices[vertex_index]);
		Vector3d Col(colors[vertex_index]);

		glNormal3d( N[0] , N[1] , N[2]);
		glColor3d( Col[0] , Col[1] , Col[2]);
		glVertex3d( V[0] , V[1] , V[2]);
	}


}






