#pragma once
#include "cs314Interfaces.h"
#include "cs314_math.hpp"
#include <memory>
#include "BSPTree.h"

namespace akj{
	class cVertexArray;
}
class cBSPTree;
class cVoxelGrid;
class cGroundPlane;

class cStaticEnvironment 
{
public:
	cStaticEnvironment(const std::vector<akj::cVertexArray*>& static_meshes,  akj::cVertexArray* base_mesh,  cBSPTree::eBSPHeuristic heuristic = cBSPTree::BSP_ORTHOGONAL);
	cRay GetSurfaceNormal(const cCoord3& start, const cCoord3& direction);
	cRay GetSurfaceNormal( const cCoord3& start, const cCoord3& direction, const cAABB constraint_box );
	float RayCastToBoundary(const cCoord3& start, const cCoord3& direction);
	cAABB GetBoundingBox(){
		return mTotalBounds;
	}
	float DistanceAboveGround(const cCoord3& start);
	float GetGravity() const {
		return mGravity;
	}
	~cStaticEnvironment(void);

private:
	float mGravity;
	cAABB mTotalBounds;
	std::unique_ptr<cGroundPlane> mGroundPlane;
	//std::unique_ptr<cVoxelGrid> mVoxelGrid;
	std::vector<cAABB> mObjectBounds;
	std::vector<std::unique_ptr<cBSPTree>> mObjectTrees;
};

