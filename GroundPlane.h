#pragma once
#include "cs314_math.hpp"

namespace akj{
	class cVertexArray;
}
class cGroundPlane
{
public:
	cGroundPlane(akj::cVertexArray* base_mesh);
	~cGroundPlane(void);
	bool RayCastToBoundary(float& result, const cCoord3& start, const cCoord3& direction);
	bool CheckBBIntersect(const cAABB& other){
		return mGroundPlaneBox.CheckIntersect(other);
	}
	cCoord3 GetSurfaceNormal(const cCoord3& start);
	float HeightAboveGround(const cCoord3& start);
	const int kGridX;
	const int kGridZ;
	inline int CellIndex(int x, int z){
		return Clamp(0, 2*(x + kGridX*z),kGridX*kGridZ*2-2);
	}
	inline int CellIndex(cCoord3 coord){
		return CellIndex(coord.x, coord.z);
	}

	inline int CellIndex(float x_real, float z_real){
		x_real = Clamp(mGroundPlaneBox.LowerCorner().x,x_real, mGroundPlaneBox.UpperCorner().x - 0.00001f );
		x_real -= mGroundPlaneBox.LowerCorner().x;
		z_real = Clamp(mGroundPlaneBox.LowerCorner().z,z_real, mGroundPlaneBox.UpperCorner().z - 0.00001f );
		z_real -= mGroundPlaneBox.LowerCorner().z;
		const int x = static_cast<int>(std::floor(x_real*mXCellFreq));
		const int z = static_cast<int>(std::floor(z_real*mZCellFreq));
		return CellIndex(x, z);
	}
private:
	float ClosestDistanceInCel(const cCoord3& start);
	float mXCellFreq;
	float mZCellFreq;
	float mCellRadius;
	cAABB mGroundPlaneBox;

	std::vector<cTriangle> mTriGrid;
	std::vector<float> mHeightGrid;
};

