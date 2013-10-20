#pragma once
#include "BSPTree.h"


class cVoxelGrid
{
public:
	struct cGridMember{
		cGridMember()
			:mClosestRay(cCoord3(), 1e10f)
			,mIndexStart(-1)
			,mNumIndices(0)
		{}
		cCoord3 location;
		cRay mClosestRay;
		int mIndexStart;
		int mNumIndices;
	};
	cVoxelGrid(int num_x, int num_y, int num_z, const cBSPTree& tree, const std::vector<cTriangle>& tris);
	float ClosestTri(cCoord3 p);

	~cVoxelGrid(void);
private:
	inline int CellIndex(int x, int y, int z){
		return y*(kNumX*kNumZ)+z*(kNumX)+x;
	}
	std::vector<cRay> mSpaceGrid;
	std::vector<int> mTriIndices;
	const cBSPTree& mBSPTree;
	std::vector<cTriangle> mTris;
	std::vector<cGridMember> mGrid;
	cAABB mBounds;
	cCoord3 mGridSpacing;
	const int kNumX;
	const int kNumY;
	const int kNumZ;
};

