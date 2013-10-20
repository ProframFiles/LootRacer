#include "VoxelGrid.hpp"


cVoxelGrid::cVoxelGrid(int num_x, int num_y, int num_z, const cBSPTree& tree, const std::vector<cTriangle>& tris)
	: mBSPTree(tree)
	, kNumX(num_x)
	, kNumY(num_y)
	, kNumZ(num_z)
	, mBounds(cCoord3(0.0f),cCoord3(0.0f))
{
	mTris.reserve(tris.size());
	mTriIndices.reserve(num_z*num_x*num_y);
	for (const cTriangle& t: tris)
	{
		mBounds.ExpandToFit(t.GetVertex(0));
		mBounds.ExpandToFit(t.GetVertex(1));
		mBounds.ExpandToFit(t.GetVertex(2));
		mTris.push_back(t);
	}
	mGridSpacing = mBounds.Range();
	mGridSpacing.x /= num_x;
	mGridSpacing.y /= num_y;
	mGridSpacing.z /= num_z;
	p4::Log::Info("Grid spacing = (%f, %f, %f)", mGridSpacing.x, mGridSpacing.y, mGridSpacing.z);
	const float max_spacing = mGridSpacing.length();
	mGrid.resize(num_z*num_x*num_y);
	int index_offset = 0;
	for (int iy = 0; iy < num_y; iy++)
	{
		const float y_offset = mGridSpacing.y*(iy+0.5f);
		for (int iz = 0; iz < num_z; iz++)
		{
			const float z_offset = mGridSpacing.z*(iz+0.5f);
			for (int ix = 0; ix < num_x; ix++)
			{
				const cCoord3 cell_location = mBounds.LowerCorner() + cCoord3(mGridSpacing.x*(ix+0.5f), y_offset, z_offset);
				cAABB cell(cell_location-mGridSpacing/2.0f,cell_location+mGridSpacing/2.0f);
				int tri_index = 0;
				int cell_index = CellIndex(ix, iy, iz);
				mGrid.at(cell_index).mIndexStart=index_offset;
				mGrid.at(cell_index).location = cell_location;
				if(std::abs(mBSPTree.ClosestBoundary(cell_location, cCoord3::ZAxis())) < max_spacing ){
					for (const cTriangle& t: mTris)
					{
						cCoord3 closest_point = t.ClosestPoint(cell_location);
						const float distance_to = (closest_point - cell_location).length();
						cCoord3 mean_loc = t.MeanLocation();
						const float other_distance = (mean_loc - cell_location).length();
						if(distance_to > other_distance && std::abs(other_distance- distance_to)> 0.0001){
							t.DebugPrint();
							p4::Log::Info("cell = [%f,%f,%f];cp = [%f,%f,%f]; mp = [%f,%f,%f];", cell_location.x, cell_location.y, cell_location.z, closest_point.x, closest_point.y, closest_point.z,
								mean_loc.x, mean_loc.y, mean_loc.z);
							closest_point = t.ClosestPoint(cell_location);
							P4_ASSERT(distance_to <= other_distance);
						}
						
						if(distance_to < mGrid.at(cell_index).mClosestRay.mLength){
							mGrid.at(cell_index).mClosestRay.mDirection = (closest_point - cell_location)/distance_to;
							mGrid.at(cell_index).mClosestRay.mLength = distance_to;
							//p4::Log::Debug("%d closest", distance_to);
						}
						if(cell.CheckWithin(closest_point)){
							mGrid.at(cell_index).mNumIndices++;
							mTriIndices.push_back(tri_index);
						}
						tri_index++;
					}
					p4::Log::TMI("\ncell = [%f,%f,%f]; dist = %f;", cell_location.x, cell_location.y, cell_location.z, mGrid.at(cell_index).mClosestRay.mLength );
				}
				if(mGrid.at(cell_index).mNumIndices != 0){
				//	p4::Log::Debug("%d triangles in bin %d,%d,%d", mGrid.at(cell_index).mNumIndices, ix, iy, iz);
				}
				index_offset += mGrid.at(cell_index).mNumIndices;
			}
		}
	}
	p4::Log::Info("%d triange indices created", mTriIndices.size());
}


cVoxelGrid::~cVoxelGrid(void)
{
}

float cVoxelGrid::ClosestTri(cCoord3 p)
{
	p.x = Clamp(mBounds.LowerCorner().x,p.x, mBounds.UpperCorner().x - 0.00001f );
	p.y = Clamp(mBounds.LowerCorner().y,p.y, mBounds.UpperCorner().y - 0.00001f );
	p.z = Clamp(mBounds.LowerCorner().z,p.z, mBounds.UpperCorner().z - 0.00001f );
	p -= mBounds.LowerCorner();
	p /= mGridSpacing;
	const int x = static_cast<int>(std::floor(p.x));
	const int y = static_cast<int>(std::floor(p.y));
	const int z = static_cast<int>(std::floor(p.z));
	const int index = CellIndex(x, y, z);
	p4::Log::Debug("%d: (%d %d %d) at (%d, %d, %d)",index, x, y, z, mGrid.at(index).location.x, mGrid.at(index).location.y, mGrid.at(index).location.z);
	return mGrid.at(index).mClosestRay.mLength;
}
