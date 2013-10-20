#include "GroundPlane.h"
#include "VertexArray.hpp"
#include "cs314_p4_log.hpp"

cGroundPlane::cGroundPlane(akj::cVertexArray* base_mesh)
	:kGridX(128)
	,kGridZ(128)
	,mGroundPlaneBox(cCoord3(0.0f),cCoord3(0.0f))
{
	std::vector<cTriangle> tris;
	P4_ASSERT(base_mesh!= NULL);
	base_mesh->GetTriangles(tris);
	p4::Log::Info("making the ground plane from %d triangles", tris.size());
	for (const cTriangle& t: tris)
	{
		mGroundPlaneBox.ExpandToFit(t.GetVertex(0));
		mGroundPlaneBox.ExpandToFit(t.GetVertex(1));
		mGroundPlaneBox.ExpandToFit(t.GetVertex(2));
	}
	mXCellFreq = kGridX/(mGroundPlaneBox.XRange());
	mZCellFreq = kGridZ/(mGroundPlaneBox.ZRange());
	const float cell_size_x = 1.0f/mXCellFreq;
	const float cell_size_z = 1.0f/mZCellFreq;
	mCellRadius = std::sqrt(cell_size_z*cell_size_z+cell_size_x*cell_size_x);
	// now CellIndex will work
	mTriGrid.resize(kGridZ*kGridX*2);
	mHeightGrid.resize(kGridZ*kGridX*2, -1e10f);
	for (const cTriangle& t: tris)
	{
		cCoord3 mid = t.MeanLocation();
		int index = CellIndex(mid.x, mid.z);
		if(mTriGrid.at(index).GetNormal().lengthSquared() == 0.0f){
			mTriGrid.at(index) = t;
		}
		else{
			P4_ASSERT(mTriGrid.at(index+1).GetNormal().lengthSquared() == 0.0f);
			mTriGrid.at(index+1) = t;
		}
		if(t.GetVertex(0).y > mHeightGrid.at(index)) mHeightGrid.at(index) = t.GetVertex(0).y;
		if(t.GetVertex(1).y > mHeightGrid.at(index)) mHeightGrid.at(index) = t.GetVertex(1).y;
		if(t.GetVertex(2).y > mHeightGrid.at(index)) mHeightGrid.at(index) = t.GetVertex(2).y;
	}
}


cGroundPlane::~cGroundPlane(void)
{
}

bool cGroundPlane::RayCastToBoundary( float& result, const cCoord3& start, const cCoord3& direction )
{
	float min_bound = 0.0f;
	float max_bound = MaxFloat();
	if(mGroundPlaneBox.CheckWithin(start)){
		mGroundPlaneBox.RayIntersection(max_bound, start, direction);
	}
	else{
		if(!mGroundPlaneBox.RayIntersection(min_bound, start, direction)){
			return false;
		}
		mGroundPlaneBox.RayIntersection(max_bound, start+(min_bound+0.0001f)*direction, direction);
		max_bound+=min_bound+0.0001f;
	}
	cCoord3 pos = start + (min_bound +0.0001f)*direction;
	cCoord3 adjusted_direction = direction;
	if(std::abs(direction.z) > std::abs(direction.x)){
		//walk in the z direction;
		adjusted_direction /= std::abs(direction.z);
	}
	else if(std::abs(direction.x) >0.0f){
		adjusted_direction /= std::abs(direction.x);
	}
	else{
		P4_ASSERT(direction.y != 0.0f);
		result = -HeightAboveGround(start)/direction.y;
		return true;
	}
	const float step_length = adjusted_direction.length();
	while(mGroundPlaneBox.CheckWithin(pos)){
		const int index = CellIndex(pos.x, pos.z);
		if(mHeightGrid.at(index) > pos.y || mHeightGrid.at(index) > pos.y+adjusted_direction.y){
			cPlane3 plane(mTriGrid.at(index));
			float temp_result_1 = 1000.0f;
			float temp_result_2 = 1000.0f;
			P4_WARN_IF(plane.DistanceAlongRay(temp_result_1, start, direction));
			plane = cPlane3(mTriGrid.at(index+1));
			P4_WARN_IF(plane.DistanceAlongRay(temp_result_2, start, direction));
			const float intersect = LesserOf(std::abs(temp_result_1), std::abs(temp_result_2));
			const int intersect_index = CellIndex(start+direction*intersect);
			if(intersect_index == index){
				result = intersect;
				return true;
			}
			//make doubly sure we didn't hit
			else if(intersect > 0.0f){
				cCoord3 intersect_point = start+intersect*direction;
				intersect_point.y = pos.y;
				if((intersect_point-pos).lengthSquared() < 1.4f*mCellRadius*mCellRadius){
					result = intersect;
					return true;
				}
			}
		}
		pos += adjusted_direction;
	}
	return false;
}

float cGroundPlane::HeightAboveGround( const cCoord3& start )
{
	int index = CellIndex(start.x, start.z);
	cCoord3 t1_loc = mTriGrid.at(index).MeanLocation();
	t1_loc.y = start.y;
	cCoord3 t2_loc = mTriGrid.at(index+1).MeanLocation();
	t2_loc.y = start.y;
	if((t1_loc-start).lengthSquared() > (t2_loc-start).lengthSquared()){
		//select the second tri
		index = index + 1;
	}
	float result = 0.0f;
	cPlane3 plane(mTriGrid.at(index));
	P4_WARN_IF(plane.DistanceAlongRay(result, start, cCoord3::YAxis()));
	return -result;
}

cCoord3 cGroundPlane::GetSurfaceNormal( const cCoord3& start )
{
	int index = CellIndex(start.x, start.z);
	cCoord3 t1_loc = mTriGrid.at(index).MeanLocation();
	t1_loc.y = start.y;
	cCoord3 t2_loc = mTriGrid.at(index+1).MeanLocation();
	t2_loc.y = start.y;
	if((t1_loc-start).lengthSquared() > (t2_loc-start).lengthSquared()){
		//select the second tri
		index = index + 1;
	}
	float result = 0.0f;
	cPlane3 plane(mTriGrid.at(index));
	return plane.NormalVec();
}
