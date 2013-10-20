#ifndef OBJREADER
#define OBJREADER
#include <memory>
#include "cs314_math.hpp"
namespace akj
{
class cVertexArray;
}
namespace cs314
{


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%   class objReader                   %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/

class cObjReader
{
public:
	cObjReader(const char *filename);
	~cObjReader();

	void loadObj(const char *filename);

	void getBoundingVolume(float *minX, float *maxX,
	                       float *minY, float *maxY,
	                       float *minZ, float *maxZ);

	int getVertexCount();
	int getTriangleCount();

	void centerObject();
	void resizeObject();
	akj::cVertexArray* GetNextVertexArray(const char* prefix = "" );
private:

	struct face
	{
		int v[3]; // vertices
		int n[3]; // normals
		int t[3]; // texture coords
	};
	struct cFaceGroup
	{
		cFaceGroup(){}
		~cFaceGroup(){}
		cFaceGroup(const char* name, int reserved_size)
			: mGroupName(name)
		{
			mFaceList.reserve(reserved_size);
		}
		std::string mGroupName;
		std::vector<face> mFaceList;
	};
	std::vector<cCoord3> mVertexList;         // loaded vertex data
	std::vector<cCoord3> mNormalList;         // loaded normal data
	std::vector<cCoord3> mTexcoordList;       // loaded texture coordinate data
	std::vector<cFaceGroup> mFaceGroups;
	int mFaceGroupIndex;

	float     m_maxX;             // maximum and mininum x, y and z values
	float     m_minX;             // for loaded object
	float     m_maxY;             //
	float     m_minY;             //
	float     m_maxZ;             //
	float     m_minZ;             //

};
}
#endif
