#include "StaticEnvironment.hpp"
#include "objreader.h"
#include "akj_ogl.h"
#include "VertexArray.hpp"
#include "BSPTree.h"
#include "GroundPlane.h"

cStaticEnvironment::cStaticEnvironment(const std::vector<akj::cVertexArray*>& static_meshes, akj::cVertexArray* base_mesh,  cBSPTree::eBSPHeuristic heuristic)
	:mTotalBounds(cCoord3(0.0f),cCoord3(0.0f))
{
	mGravity = 9.8f;
	mGroundPlane.reset(new cGroundPlane(base_mesh));
	std::vector<cTriangle> tris;
	std::vector<cTriangle> sub_tris;
	for (akj::cVertexArray* mesh: static_meshes)
	{
		sub_tris.clear();
		P4_ASSERT(mesh != NULL);
		mesh->GetTriangles(sub_tris);
		p4::Log::Info("creating BSP for mesh \"%s\"", mesh->GetName());
		std::string mesh_name = std::string(mesh->GetName());
		if(mesh_name == std::string("Tube002")){
			mObjectTrees.emplace_back(new cBSPTree(sub_tris, std::string(mesh->GetName()), cBSPTree::BSP_AREA));
		}
		else{
			mObjectTrees.emplace_back(new cBSPTree(sub_tris, std::string(mesh->GetName()), cBSPTree::BSP_AREA));
		}
		
		mTotalBounds.ExpandToFit(mObjectTrees.back()->GetBoundingBox().LowerCorner());
		mTotalBounds.ExpandToFit(mObjectTrees.back()->GetBoundingBox().UpperCorner());
	}
	mTotalBounds.ExpandToFit(cCoord3(100.0f, 200.0f, 100.0f));
	mTotalBounds.ExpandToFit(cCoord3(-100.0f, 0.0f, -100.0f));

	//mVoxelGrid.reset(new cVoxelGrid(96, 8, 96, *mBSPTree.get(), tris));
}


cStaticEnvironment::~cStaticEnvironment(void)
{
}


float cStaticEnvironment::RayCastToBoundary( const cCoord3& start, const cCoord3& direction )
{
	float result = 100000.0f;
	P4_WARN_IF(mTotalBounds.RayIntersection(result, start, direction));
	mGroundPlane->RayCastToBoundary(result, start, direction);
	for (const std::unique_ptr<cBSPTree>& bsp: mObjectTrees )
	{
		cRay temp_result(direction, result);
		if(bsp->RayCastToBoundary3(temp_result, start, direction) && std::abs(temp_result.mLength) < std::abs(result) ){
			result = temp_result.mLength;
		}
	}
	return result;
}

float cStaticEnvironment::DistanceAboveGround( const cCoord3& start )
{
	return mGroundPlane->HeightAboveGround(start);
}
cRay cStaticEnvironment::GetSurfaceNormal( const cCoord3& start, const cCoord3& direction )
{
	return GetSurfaceNormal(start, direction, mTotalBounds);
}
cRay cStaticEnvironment::GetSurfaceNormal( const cCoord3& start, const cCoord3& direction, const cAABB constraint_box )
{
	P4_ASSERT(direction.lengthSquared()>0.0f);
	cRay result(direction, 100000.0f);
	if(!constraint_box.CheckIntersect(mTotalBounds)){
		return result;
	}
	if(mTotalBounds.RayIntersection(result, start, direction) && mTotalBounds.CheckWithin(start)){
		//if we're inside the BB, don't reflect us out...
		result.mDirection = -result.mDirection;
 	}
	if(mGroundPlane->CheckBBIntersect(constraint_box) && mGroundPlane->RayCastToBoundary(result.mLength, start, direction)){
		result.mDirection = mGroundPlane->GetSurfaceNormal(start+result.mLength*direction);
	}
	for (const std::unique_ptr<cBSPTree>& bsp: mObjectTrees )
	{
		if(bsp->CheckIntersect(constraint_box)){
			cRay temp_result = result;
			if(bsp->RayCastToBoundary3(temp_result, start, direction) && std::abs(temp_result.mLength) < std::abs(result.mLength) ){
				result = temp_result;
			}
		}
	}
	return result;
}
