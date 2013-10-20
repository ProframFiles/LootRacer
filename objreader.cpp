#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdexcept>
#include "objreader.h"
#include "VertexArray.hpp"

namespace cs314
{
cObjReader::cObjReader(const char *filename)
	:mFaceGroupIndex(0)
{
	//Initialize Data
	m_maxX = m_minX = m_maxY = m_minY = m_maxZ = m_minZ = 0;
	loadObj(filename);
}

cObjReader::~cObjReader()
{
	
}

void cObjReader::getBoundingVolume(float *minX, float *maxX,
                                  float *minY, float *maxY,
                                  float *minZ, float *maxZ)
{
	*minX = m_minX;
	*minY = m_minY;
	*minZ = m_minZ;
	*maxX = m_maxX;
	*maxY = m_maxY;
	*maxZ = m_maxZ;
}

void cObjReader::loadObj(const char* filename)
{
	m_maxZ = -3000;
	char token[128];
	FILE *fp;
	fp = fopen(filename, "r");
	if( !fp )
	{
		throw std::runtime_error("Obj reader error: file opening failed");
		return;
	}
	rewind(fp);
	// Preprocess the Text File
	// Get the size number of vertices
	// Get the number of faces
	int vertex_count, normal_count, texcoord_count, current_face_count;
	vertex_count = normal_count = texcoord_count = current_face_count = 0;
	char group_name[128];
	mFaceGroups.reserve(16);
	while(    fgets(token, 128, fp) != NULL)
	{
		if(strlen(token) > 2)
		{
			if( token[0] == 'v' && token[1] == ' ' )
			{
				vertex_count++;
			}
			else if( token[0] == 'v' && token[1] == 'n' )
			{
				normal_count++;
			}
			else if( token[0] == 'v' && token[1] == 't' )
			{
				texcoord_count++;
			}
			else if( token[0] == 'g' && token[1] == ' ' )
			{
				if(current_face_count > 0){
					mFaceGroups.emplace_back(group_name, current_face_count);
					current_face_count = 0;
				}
				sscanf(&token[2], "%s", group_name);

			}
			else if( token[0] == 'f' && token[1] == ' ' )
			{
				current_face_count++;
			}
		}
	}
	if(current_face_count > 0){
		mFaceGroups.emplace_back(group_name, current_face_count);
		current_face_count = 0;
	}
	mVertexList.reserve(vertex_count);
	mNormalList.reserve(normal_count);
	mTexcoordList.reserve(texcoord_count);
	int current_face_group = -1;
	// Reset the filestream
	rewind(fp);
	// Read in the data
	while( fgets(token, 128, fp) != NULL )
	{
		if( token[1] == ' ' )
		{
			token[1] = '\0';
		}
		else
		{
			token[2] = '\0';
		}
		if(strcmp(token, "v") == 0 )
		{
			cCoord3 new_vertex;
			sscanf(&token[2], "%f%f%f", &new_vertex.x, &new_vertex.y, &new_vertex.z);
			if( mVertexList.empty() )
			{
				m_maxX = new_vertex.x;
				m_maxY = new_vertex.y;
				m_maxZ = new_vertex.z;
				m_minX = new_vertex.x;
				m_minY = new_vertex.y;
				m_minZ = new_vertex.z;
			}
			else
			{
				if( new_vertex.x > m_maxX ) m_maxX = new_vertex.x;
				if( new_vertex.y > m_maxY ) m_maxY = new_vertex.y;
				if( new_vertex.z > m_maxZ ) m_maxZ = new_vertex.z;
				if( new_vertex.x < m_minX ) m_minX = new_vertex.x;
				if( new_vertex.y < m_minY ) m_minY = new_vertex.y;
				if( new_vertex.z < m_minZ ) m_minZ = new_vertex.z;
			}
			mVertexList.push_back(new_vertex);
		}
		else if(strcmp(token, "g") == 0 ){
			current_face_group ++;

		}
		else if(strcmp(token, "vn") == 0 )
		{
			cCoord3 new_normal;
			sscanf(&token[3], "%f%f%f", &new_normal.x, &new_normal.y, &new_normal.z);
			mNormalList.push_back(new_normal.normalized());
		}
		else if(strcmp(token, "vt") == 0 )
		{
			cCoord3 new_texcoord;
			sscanf(&token[3], "%f%f", &new_texcoord.x, &new_texcoord.y);
			
			mTexcoordList.push_back(new_texcoord);
		}
		else if(strcmp(token, "f") == 0 )
		{
			int vnum, nnum, tnum;
			char *tempptr;
			tempptr = &token[2];
			int face_vertex = 0;
			mFaceGroups.at(current_face_group).mFaceList.emplace_back();
			face& new_face = mFaceGroups.at(current_face_group).mFaceList.back();
			const int current_verts =  static_cast<int>(mVertexList.size());
			while( tempptr && (3 == sscanf(tempptr, "%i/%i/%i", &vnum, &tnum, &nnum)) )
			{
				if(vnum < 0) vnum = current_verts + vnum + 1;
				if(nnum < 0) nnum = current_verts + nnum + 1;
				if(tnum < 0) tnum = current_verts + tnum + 1;
				vnum -= 1;
				nnum -= 1;
				tnum -= 1;
				new_face.v[face_vertex] = vnum;
				new_face.n[face_vertex] = nnum;
				new_face.t[face_vertex] = tnum;
				face_vertex++;
				tempptr = strchr(tempptr, ' ');
				if(tempptr)
				{
					tempptr += 1;
				}
			}
		}
		
	}/* end of while loop */
	fclose(fp);
	if( mVertexList.size() != vertex_count || mNormalList.size() != normal_count 
		|| mTexcoordList.size() != texcoord_count)
	{
		throw std::runtime_error("ObjReader Error: object counts are wrong");
	}
	m_maxX += m_maxX * (m_maxX > 0 ? .1f : -0.1f);
	m_maxY += m_maxY * (m_maxY > 0 ? .1f : -0.1f);
	m_maxZ += m_maxZ * (m_maxZ > 0 ? .1f : -0.1f);
	m_minX -= m_minX * (m_minX > 0 ? .1f : -0.1f);
	m_minY -= m_minY * (m_minY > 0 ? .1f : -0.1f);
	m_minZ -= m_minZ * (m_minZ > 0 ? .1f : -0.1f);

};

 akj::cVertexArray* cObjReader::GetNextVertexArray(const char* prefix )
{
	if(mFaceGroupIndex >= mFaceGroups.size()){
		mFaceGroupIndex = 0;
		return NULL;
	}
	const auto face_group = mFaceGroups.begin() + mFaceGroupIndex;
	const auto& face_list = face_group->mFaceList;
	akj::cVertexArray* ptr = new akj::cVertexArray(prefix + face_group->mGroupName, face_list.size()*3);
	for( size_t i = 0; i < face_list.size(); i++ )
	{
		for( int j = 0; j < 3; j++ )
		{
			ptr->PlaceVertex(i*3+j	,  mVertexList[face_list.at(i).v[j]]
									,  mNormalList[face_list.at(i).n[j]]
									,mTexcoordList[face_list.at(i).t[j]], face_list.at(i).v[j] );
		} // END: for j
	} // END: for i
	ptr->GenerateBiTangents();

	mFaceGroupIndex++;
	return ptr;
}


void cObjReader::centerObject()
{
	float cx = m_minX + (m_maxX - m_minX) / 2.0f;
	float cy = m_minY + (m_maxY - m_minY) / 2.0f;
	float cz = m_minZ + (m_maxZ - m_minZ) / 2.0f;

	for(size_t i = 0; i < mVertexList.size(); i++ )
	{
		mVertexList[i].x -= cx;
		mVertexList[i].y -= cy;
		mVertexList[i].z -= cz;
	} // END for i
	m_minX -= cx;
	m_maxX -= cx;
	m_minY -= cy;
	m_maxY -= cy;
	m_minZ -= cz;
	m_maxZ -= cz;
}

void cObjReader::resizeObject()
{
	float s = 0.0;

	if (-m_minX > s)
	{
		s = -m_minX;
	}
	if (-m_minY > s)
	{
		s = -m_minY;
	}
	if (-m_minZ > s)
	{
		s = -m_minZ;
	}
	if (m_maxX > s)
	{
		s = m_maxX;
	}
	if (m_maxY > s)
	{
		s = m_maxY;
	}
	if (m_maxZ > s)
	{
		s = m_maxZ;
	}

	s = 1.0f / s;

	for (size_t i = 0; i < mVertexList.size(); i++)
	{
		mVertexList[i].x *= s;
		mVertexList[i].y *= s;
		mVertexList[i].z *= s;
	}
	m_minX *= s;
	m_maxX *= s;
	m_minY *= s;
	m_maxY *= s;
	m_minZ *= s;
	m_maxZ *= s;
}
}
