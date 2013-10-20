#include "BSPTree.h"
#include <algorithm>
#include "cs314_p4_log.hpp"
const float cBSPNode::kEqualThreshold = 0.002f;

int cBSPTree::PreProcessNodes( std::vector<cTriangle>& tris, std::vector<cTriNodeIndexPair>& in_progress,  int parent_node )
{
	int best_index = 0;
	int best_split_count = static_cast<int>(in_progress.size());
	int best_diff = best_split_count;
	for (int i = 0 ; i < static_cast<int>(in_progress.size()); ++i)
	{
		if(in_progress.at(i).mParentNode!=parent_node){
			continue;
		}
		int split_count = 0;
		int in_count = 0;
		int out_count = 0;
		int opposing_coplanar_count = 0;
		cBSPNode test_node(tris.at(in_progress.at(i).mTriIndex),-1, 0);
		for (cTriNodeIndexPair ipair: in_progress)
		{
			if(ipair.mParentNode != parent_node){
				continue;
			}
			cBSPNode::eClassification in_out = test_node.ClassifyTriangle(tris.at(ipair.mTriIndex));
			if(in_out == cBSPNode::BSP_SPLIT){
				split_count++;
			}
			else if(in_out == cBSPNode::BSP_FRONT){
				in_count++;
			}
			else if(in_out == cBSPNode::BSP_BACK){
				out_count++;
			}
			else if(in_out == cBSPNode::BSP_COPLANAR){
			//	if( tris.at(ipair.mTriIndex).GetNormal().dot(test_node.GetNormal()) <= 0.0){
				//	opposing_coplanar_count ++;
			//	}
			}
		}
		in_progress.at(i).mSplits = split_count;
		//in_progress.at(i).mBadCoplanar = opposing_coplanar_count;
		in_progress.at(i).mInOutDiff = in_count - out_count;
		if(split_count < best_split_count){
			best_split_count = split_count;
			best_diff = in_count - out_count;
			best_index = i;
		}
		else if(split_count == best_split_count &&(best_diff > in_count - out_count)){
			best_diff = in_count - out_count;
			best_index = i;
		}
	}
	return best_index;
}

cBSPTree::cBSPTree(std::vector<cTriangle>& tris, std::string name, eBSPHeuristic heuristic)
	:mName(name)
	,mHeuristic(heuristic)
	,mBoundingBox(cCoord3(0.0f))
{
	if(!tris.empty()){
		mBoundingBox.Reset(tris.back().GetVertex(0));
	}
	//SplitTriangle(cTriangle(cCoord3(0.0f, 2.0f, 0.0f),cCoord3(0.0f, 0.0f, 0.0f),cCoord3(2.0f, 0.0f, 0.0f),cCoord3(0.0f, 0.0f, 1.0f)), cPlane3(cCoord3::XAxis(), 1.0f));
	for (const cTriangle& t: tris)
	{
		mTris.push_back(t);
		mBoundingBox.ExpandToFit(t.GetVertex(0));
		mBoundingBox.ExpandToFit(t.GetVertex(1));
		mBoundingBox.ExpandToFit(t.GetVertex(2));
	}
	mBoundingBox.ExpandToFit(mBoundingBox.LowerCorner()-0.1f*cCoord3::Ones());
	mBoundingBox.ExpandToFit(mBoundingBox.UpperCorner()+0.1f*cCoord3::Ones());
	std::vector<cTriNodeIndexPair> in_progress;
	std::vector<int> index_stack;
	mNodeVec.reserve(mTris.size());
	in_progress.reserve(mTris.size());
	index_stack.reserve(static_cast<size_t>(std::log(mTris.size())+1));
	std::sort(mTris.begin(), mTris.end(), cTrianglePerimeterComparator());
	int count = 0;
	cCoord3 com;
	for (const cTriangle& tri: mTris)
	{
		com += tri.GetVertex(0);
		com += tri.GetVertex(1);
		com += tri.GetVertex(2);
		in_progress.emplace_back(count, 0);
		count++;
	}
	com /= static_cast<float>(count*3);
	index_stack.push_back(0);
	mNodeVec.push_back(cBSPNode(mTris.back(),-1, 0));
	in_progress.back().mParentNode--;
	int total_splits = 0;
	int max_depth = 0;
	while (index_stack.size()>0)
	{
		
		int parent_node = index_stack.back();
		index_stack.pop_back();
		int front_index = static_cast<int>(mNodeVec.size());
		int back_index = front_index + 1;
		float best_front_metric = 0.0f;
		float best_back_metric = 0.0f;
		int best_front_index = -1;
		int best_back_index = -1;
		int num_front_nodes = 0;
		int num_back_nodes = 0;
		int num_split = 0;
		int num_coplanar = 0;
		cCoord3 center;
		float avg_over = 0.0f;
		cCoord3 orientation;
		//PreProcessNodes(mTris,in_progress,parent_node);
		for (int i = 0 ; i < static_cast<int>(in_progress.size()); ++i)
		{
			if(in_progress.at(i).mParentNode != parent_node) continue;
			
			const int tri_index = in_progress.at(i).mTriIndex;
			cBSPNode::eClassification in_out = mNodeVec.at(parent_node).ClassifyTriangle(mTris.at(tri_index));
			const float area = mTris.at(tri_index).Area();
			if(area <  0.0001f) continue;
			float metric = 0.0;
			const float normal_dot = mTris.at(tri_index).GetNormal().dot(mNodeVec.at(parent_node).GetNormal());
			if(mHeuristic == BSP_COPLANAR){
				metric = std::abs(normal_dot);
			}
			else if(mHeuristic == BSP_ORTHOGONAL){
				metric = normal_dot;
			}
			else if(mHeuristic == BSP_AREA){
				metric = area* (1 +  std::abs(normal_dot));
			}
			//const cCoord3 mean_loc = mTris.at(tri_index).MeanLocation();
			//float axis_align = 1.0f - std::abs(mTris.at(tri_index).GetNormal().x);
			//axis_align = GreaterOf(axis_align, 1.0f -  std::abs(mTris.at(tri_index).GetNormal().y));
			//axis_align = GreaterOf(axis_align, 1.0f -  std::abs(mTris.at(tri_index).GetNormal().z));

			//const float dist = -0.0f*(mean_pos/3.0f - center).length();
			if(in_out == cBSPNode::BSP_COPLANAR){
				if(mTris.at(tri_index).GetNormal().dot(mNodeVec.at(parent_node).GetNormal()) <= 0.0){
					in_out = cBSPNode::BSP_BACK;
					float perim1 = std::sqrt(mTris.at(tri_index).Area());
					float normal_length = mNodeVec.at(parent_node).GetNormal().length();
					float ratio = mTris.at(tri_index).GetNormal().dot(mNodeVec.at(parent_node).GetNormal());
					p4::Log::Warn("coplanar triangles with different normals. area = %f, normal_len = %f, ratio: %f",perim1,normal_length, ratio);
					mNodeVec.at(parent_node).ClassifyTriangle(mTris.at(tri_index));
					//mNodeVec.at(parent_node).AddTriangle(mTris.at(tri_index));
					//num_coplanar++;
				}
				else{
					mNodeVec.at(parent_node).AddTriangle(mTris.at(tri_index));
					num_coplanar++;
				}
				//p4::Log::Debug("passing over coplanar tri %d", tri_index);
			}
			if(in_out == cBSPNode::BSP_FRONT){
				num_front_nodes++;
				//p4::Log::Debug("found front tri");
				
				if( metric > best_front_metric ){
					best_front_metric = metric; 
					best_front_index = i;
				}
				in_progress.at(i).mParentNode = front_index;
			}
			else if(in_out == cBSPNode::BSP_BACK){
				num_back_nodes++;
				//p4::Log::Debug("found back tri");
				if( metric > best_back_metric ){
					best_back_metric = metric; 
					best_back_index = i;
				}
				in_progress.at(i).mParentNode = back_index;
			}
			
			else if(in_out == cBSPNode::BSP_SPLIT){
				//p4::Log::Debug("splitting tri %d", tri_index);
				std::vector<cTriangle> split = SplitTriangle(mTris.at(tri_index), mNodeVec.at(parent_node).GetDividingPlane());
				
				// replace our current triangle
				mTris.at(tri_index) = split.at(0);
				// and then add two more
				if(split.size() > 1){
					mTris.emplace_back(split.at(1));
					in_progress.emplace_back(static_cast<int>(mTris.size()-1), parent_node);
				}
				if(split.size() > 2){
					mTris.emplace_back(split.at(2));
					in_progress.emplace_back(static_cast<int>(mTris.size()-1), parent_node);
				}
				//rewind the index to process this one again
				i--;
				num_split ++;
			}
		}
		p4::Log::TMI("Processed for node %d, depth %d, front: %d, back: %d, split: %d, coplanar %d",
			parent_node, mNodeVec.at(parent_node).GetDepth(), num_front_nodes, num_back_nodes, num_split, num_coplanar);
		// okay, classified all the triangles: now create 0-2 nodes
		if(best_front_index >= 0){
			const cCoord3 nrml = mTris.at(best_front_index).GetNormal();
			mNodeVec.push_back(cBSPNode(mTris.at(best_front_index),parent_node,mNodeVec.at(parent_node).GetDepth()+1));
			int new_front_index = static_cast<int>(mNodeVec.size()-1);
			mNodeVec.at(parent_node).SetFrontNodeIndex(new_front_index);
			P4_ASSERT(new_front_index == front_index);
			//TODO:watch out here...
			in_progress.at(best_front_index).mParentNode = parent_node;
			index_stack.push_back(new_front_index);
			p4::Log::TMI("created new front node %d, child of %d, depth %d", new_front_index, parent_node, mNodeVec.at(new_front_index).GetDepth());
			P4_ASSERT(mNodeVec.at(parent_node).GetDepth() >= 0);
			if(mNodeVec.at(new_front_index).GetDepth() > max_depth){
				max_depth = mNodeVec.at(new_front_index).GetDepth();
			}
		}
		if(best_back_index >= 0){
			const cCoord3 nrml = mTris.at(best_back_index).GetNormal();
			mNodeVec.push_back(cBSPNode(mTris.at(best_back_index),parent_node,mNodeVec.at(parent_node).GetDepth()+1 ));
			int new_back_index = static_cast<int>(mNodeVec.size()-1);
			mNodeVec.at(parent_node).SetBackNodeIndex(new_back_index);
			if(new_back_index != back_index){
				for (cTriNodeIndexPair& tri_index: in_progress)
				{
					if(tri_index.mParentNode == back_index){
						tri_index.mParentNode = new_back_index;
					}
				}
			}
			in_progress.at(best_back_index).mParentNode = parent_node;
			index_stack.push_back(new_back_index);
			p4::Log::TMI("created new back node %d, child of %d, depth %d", new_back_index, parent_node, mNodeVec.at(new_back_index).GetDepth());
			P4_ASSERT(mNodeVec.at(parent_node).GetDepth() >= 0);
			if(mNodeVec.at(new_back_index).GetDepth() > max_depth){
				max_depth = mNodeVec.at(new_back_index).GetDepth();
			}
		}
		total_splits += num_split;
	}
	p4::Log::Info("Finished BSP creation: %d nodes (%dkB), max depth = %d, splits = %d", mNodeVec.size(),(mNodeVec.size()*sizeof(cBSPNode)-1)/1024+1, max_depth, total_splits);


}


cBSPTree::~cBSPTree(void)
{
}


int cBSPTree::IsOutsidePoly(const cCoord3& p ) const
{
	int index = 0;
	int search_depth = 0;
	cBSPNode::eClassification in_out = cBSPNode::BSP_FRONT;
	cBSPNode::eClassification last_in_out = cBSPNode::BSP_FRONT;
	while(index != -1 && (in_out = mNodeVec[index].ClassifyPoint(p, 0.0f)) != cBSPNode::BSP_COPLANAR){
		if(in_out == cBSPNode::BSP_FRONT){
			index = mNodeVec[index].GetFrontNodeIndex();
		}
		else{
			index = mNodeVec[index].GetRearNodeIndex();
		
		}
		last_in_out = in_out;
		search_depth ++;
	}
	p4::Log::TMI("BSP search_depth: %d", search_depth);
	if(cBSPNode::BSP_COPLANAR == in_out){
		in_out = last_in_out;
	}
	return cBSPNode::BSP_BACK == in_out ? -1 : 1;
}

bool cBSPTree::IsOutsidePoly( const cCoord3& p, cBSPNode* node )
{
	P4_ASSERT(node!=NULL);
	bool is_front = false;
	while(node != NULL){
		cBSPNode::eClassification front_back = node->ClassifyPoint(p, 0.0f);
		if(front_back == cBSPNode::BSP_BACK){
			is_front = false;
			node = GetNode(node->GetRearNodeIndex());
		}
		else if (front_back == cBSPNode::BSP_FRONT){
			is_front  = true;
			node = GetNode(node->GetFrontNodeIndex());
		}
		else{
			p4::Log::Warn("coplanar");
			node = GetNode(node->GetRearNodeIndex());
		}
	}
	return is_front;
}
cBSPNode* cBSPTree::ClosestContainingPlane( const cCoord3& p, const cCoord3& direction, cBSPNode* node )
{
	P4_ASSERT(node!=NULL);
	bool is_front = true;
	float closest_plane = MaxFloat();
	cBSPNode* best_node = node;
	while(node != NULL){
		const float dist = node->DistanceToPlane(p,direction);
		//const float dist2 = node->GetDividingPlane().DistanceToPoint(p);

		const float normal_dot = direction.dot(node->GetDividingPlane().NormalVec());
		
		cBSPNode::eClassification front_back = cBSPNode::ClassifyPoint(-normal_dot*dist, 0.0f);
		//p4::Log::Debug("dist1 %f, dist2 %f, normal dot %f", dist, dist2, normal_dot);
		if(dist <= 0.0f && std::abs(dist) < closest_plane){
				closest_plane = std::abs(dist);
				best_node = node;
		}
		if(front_back == cBSPNode::BSP_BACK){
			
			node = GetNode(node->GetRearNodeIndex());
		}
		else if (front_back == cBSPNode::BSP_FRONT){
			//closest_plane = MaxFloat();
			//best_node = NULL;
			node = GetNode(node->GetFrontNodeIndex());
		}
		else if( best_node != NULL){
			return best_node;
		}
	}

	return best_node;
}

bool cBSPTree::IsOutsidePoly(cRay& best_distance, const cCoord3& p, const cCoord3& direction,  cBSPNode* node )
{
	P4_ASSERT(node!=NULL);
	bool is_front = true;
	float closest_plane = MaxFloat();
	cBSPNode* best_node = node;
	while(node != NULL){
		const float dist = node->DistanceToPlane(p,direction);
		//const float dist2 = node->GetDividingPlane().DistanceToPoint(p);

		const float normal_dot = direction.dot(node->GetDividingPlane().NormalVec());
		
		cBSPNode::eClassification front_back = cBSPNode::ClassifyPoint(-normal_dot*dist, 0.0f);
		//p4::Log::Debug("dist1 %f, dist2 %f, normal dot %f", dist, dist2, normal_dot);
		if(dist >=0.0f && dist < closest_plane){
			closest_plane = dist;
			best_node = node;
		}
		if(front_back == cBSPNode::BSP_BACK){
			is_front = false;
			node = GetNode(node->GetRearNodeIndex());
		}
		else if (front_back == cBSPNode::BSP_FRONT){
			is_front  = true;
			node = GetNode(node->GetFrontNodeIndex());
		}
		else{
			//This one is designed for raycasting: this is likely
			best_distance.mLength = 0.0f;
			best_distance.mDirection = direction;
			return true;
		}
	}
	best_distance.mLength = closest_plane;
	best_distance.mDirection = best_node->GetNormal();
	return is_front;
}

float  cBSPTree::ClosestBoundary(const cCoord3& p, const cCoord3& direction ) const
{
	int index = 0;
	int search_depth = 0;
	float closest_distance = 1e10f;
	
	float ray_dist = 1e10f; 
	mNodeVec[index].GetDividingPlane().DistanceAlongRay(ray_dist, p, direction);
	float best_result = ray_dist >= 0.0 ? ray_dist : 1e10f;
	cBSPNode::eClassification in_out = cBSPNode::ClassifyPoint(ray_dist, 0.0f);
	cBSPNode::eClassification last_in_out = in_out;
	while(index != -1 && in_out != cBSPNode::BSP_COPLANAR){
		if(in_out == cBSPNode::BSP_FRONT){
			index = mNodeVec[index].GetFrontNodeIndex();
		}
		else{
			index = mNodeVec[index].GetRearNodeIndex();
		}
		search_depth ++;
		if(index > -1){
			float temp_result = 0.0f;
			mNodeVec[index].GetDividingPlane().DistanceAlongRay(temp_result, p, direction);
			in_out = cBSPNode::ClassifyPoint(temp_result, 0.0f);
			if(in_out == cBSPNode::BSP_COPLANAR){
				in_out = last_in_out;
			}
			if( temp_result >= 0.0f && std::abs(temp_result) < best_result){
				best_result = std::abs(temp_result);
			}
			last_in_out = in_out;
		}
	}
	p4::Log::Debug("BSP search_depth: %d, %f", search_depth, best_result);
	if(in_out == cBSPNode::BSP_BACK){
		best_result = -best_result;
	}
	P4_ASSERT(best_result < 1e9f);
	return best_result;
}


std::vector<cTriangle>& cBSPTree::SplitTriangle(const cTriangle& t1,const cPlane3& plane)
{
	mSplitTriangleBuffer.clear();
	const float d0 = plane.DistanceToPoint(t1.GetVertex(0));
	const float d1 = plane.DistanceToPoint(t1.GetVertex(1));
	const float d2 = plane.DistanceToPoint(t1.GetVertex(2));
	const float d2a = std::abs(d2);
	const float d1a = std::abs(d1);
	const float d0a = std::abs(d0);

	//all on the same side (or coplanar)
	P4_ASSERT(std::abs(d0+d1+d2) != (d2a+d1a+d0a));

	//d0 d1 have the same sign, d2 left out
	if(d0*d1 == d0a*d1a){
		//along v2->v1
		cCoord3 intersect1 = t1.GetVertex(2)+(d2a/(d2a + d0a))*(t1.GetVertex(0)-t1.GetVertex(2));
		//along v2->v0
		cCoord3 intersect2 = t1.GetVertex(2)+(d2a/(d2a + d1a))*(t1.GetVertex(1)-t1.GetVertex(2));
		mSplitTriangleBuffer.emplace_back(intersect1,intersect2, t1.GetVertex(2), t1.GetNormal(), false);
		mSplitTriangleBuffer.emplace_back(t1.GetVertex(0),t1.GetVertex(1),intersect1, t1.GetNormal(), false);
		mSplitTriangleBuffer.emplace_back(intersect1,t1.GetVertex(1),intersect2, t1.GetNormal(), false);
	}
	//d0 left out
	else if(d1*d2 == d1a*d2a){
		//along v0->v1
		cCoord3 intersect1 = t1.GetVertex(0)+(d0a/(d0a + d1a))*(t1.GetVertex(1)-t1.GetVertex(0));
		//along v0->v2
		cCoord3 intersect2 = t1.GetVertex(0)+(d0a/(d0a + d2a))*(t1.GetVertex(2)-t1.GetVertex(0));
		mSplitTriangleBuffer.emplace_back(t1.GetVertex(0),intersect1, intersect2, t1.GetNormal(), false);
		mSplitTriangleBuffer.emplace_back(intersect1,t1.GetVertex(1), t1.GetVertex(2), t1.GetNormal(), false);
		mSplitTriangleBuffer.emplace_back(intersect1,t1.GetVertex(2), intersect2, t1.GetNormal(),false);
		
	}
	// d1 left out
	else if(d0*d2 == d0a*d2a){
		//along v1->v0
		cCoord3 intersect1 = t1.GetVertex(1)+(d1a/(d1a + d0a))*(t1.GetVertex(0)-t1.GetVertex(1));
		//along v1->v2
		cCoord3 intersect2 = t1.GetVertex(1)+(d1a/(d1a + d2a))*(t1.GetVertex(2)-t1.GetVertex(1));

		mSplitTriangleBuffer.emplace_back(intersect1,t1.GetVertex(1),intersect2, t1.GetNormal(), false);
		mSplitTriangleBuffer.emplace_back(t1.GetVertex(0),intersect1,t1.GetVertex(2), t1.GetNormal(), false);
		mSplitTriangleBuffer.emplace_back(intersect1,intersect2,t1.GetVertex(2), t1.GetNormal(),false);
	}
	for (int i = 0; i < static_cast<int>(mSplitTriangleBuffer.size()); i++)
	{
		if(mSplitTriangleBuffer.at(i).Area() < 0.000001f){
			mSplitTriangleBuffer.at(i) = mSplitTriangleBuffer.back();
			mSplitTriangleBuffer.pop_back();
			i--;
		}
	}
	float t1_area = t1.Area();
	P4_ASSERT(mSplitTriangleBuffer.size() > 0);
	return mSplitTriangleBuffer;
}
bool cBSPTree::RayCastToBoundary3(cRay &ret,  const cCoord3& start, const cCoord3& direction )
{

	if(!CheckBoxHit(start, direction)){
		return false;
	}
		p4::Log::TMI("%s Raycast", mName);
	float min_bound = 0.0f;
	float max_bound = MaxFloat();
	if(mBoundingBox.CheckWithin(start)){
		mBoundingBox.RayIntersection(max_bound, start, direction);
	}
	else{
		mBoundingBox.RayIntersection(min_bound, start, direction);
		mBoundingBox.RayIntersection(max_bound, start+(min_bound+0.01f)*direction, direction);
		max_bound+=min_bound+0.01f;
	}
	p4::Log::TMI("ray bounds = (%f, %f)", min_bound, max_bound );
	cRay last_result(direction, 0.0f);
	float current_distance = min_bound+cBSPNode::kEqualThreshold*0.5f;
	cCoord3 best_plane = direction;
	cCoord3 p = start+current_distance*direction;
	while(mBoundingBox.CheckWithin(p) && IsOutsidePoly(last_result, p, direction, HeadNode())){
		current_distance+=GreaterOf(last_result.mLength, cBSPNode::kEqualThreshold*0.5f);
		best_plane = last_result.mDirection;
		p4::Log::TMI("current result = %f, last result = %f", current_distance, last_result.mLength);
		p = start+(current_distance+cBSPNode::kEqualThreshold*0.5f)*direction;
	}
	p4::Log::TMI("Ready to return %f", current_distance);
	/*
	for (int i = 0; i < 100; i++)
	{
		const float d = (max_bound-min_bound)*(i/100.0f)+min_bound;
		p=start+d*direction;
		bool is_out = IsOutsidePoly(last_result, p, direction, HeadNode());
		if(is_out){
			p4::Log::Debug("%d\t OUT current result = %f, last result = %f",i, d, last_result);
		}
		else{
			p4::Log::Debug("%d\t IN  current result = %f, last result = %f",i, d, last_result);
		}
	}
	*/
	p = start+(current_distance)*direction;
	p4::Log::TMI("Done result = %f, last result = %f", current_distance, last_result.mLength);
	if( mBoundingBox.CheckWithin(p)){
		cBSPNode* best = ClosestContainingPlane(p+direction*cBSPNode::kEqualThreshold*0.5f, direction, HeadNode());
		if(best == NULL){
			return false;
		}
		ret.mLength = current_distance;
		ret.mDirection = best->GetDividingPlane().NormalVec();
		return true;
	}
	p4::Log::TMI("Not within Bounds? result = %f, last result = %f", current_distance, last_result.mLength);
	return false;
}
bool cBSPTree::RayCastToBoundary2(float &ret,  const cCoord3& start, const cCoord3& direction )
{
	p4::Log::GetInstance()->SetLogLevel(p4::Log::LOG_LEVEL_TMI);
	if(!CheckBoxHit(start, direction)){
		return false;
	}
	float min_bound = 0.0f;
	float max_bound = MaxFloat();
	if(mBoundingBox.CheckWithin(start)){
		mBoundingBox.RayIntersection(max_bound, start, direction);
	}
	else{
		mBoundingBox.RayIntersection(min_bound, start, direction);
		mBoundingBox.RayIntersection(max_bound, start+(min_bound+0.01f)*direction, direction);
		max_bound+=min_bound+0.01f;
	}

	float min_d = min_bound;
	float max_d = max_bound;
	float best_result = max_bound+1.0f;

	p4::Log::TMI("ray bounds = (%f, %f)", min_bound, max_bound );
	mNodeStack.clear();
	int search_depth = 0;
	cBSPNode* current_node = HeadNode();
	float current_distance = 0.0f;
	bool terminal_candidate = false;
	cBSPNode::eClassification in_out = cBSPNode::BSP_SPLIT;

	while((current_node != NULL )){
		in_out = current_node->ClassifyPoint(start+direction*current_distance, 0.0f);
		//nudge it a bit if we're coplanar
		if(in_out == cBSPNode::BSP_COPLANAR){
			current_distance += 0.5f*cBSPNode::kEqualThreshold;
			in_out = current_node->ClassifyPoint(start+direction*current_distance, 0.0f);
		}

		P4_ASSERT(in_out != cBSPNode::BSP_COPLANAR);
		const float  next_plane_dist = current_node->DistanceToPlane(start+direction*current_distance, direction);
		p4::Log::TMI("%s, %d, %d  dist: %f, next plane = %f", mName, search_depth, in_out, current_distance,  current_distance+next_plane_dist);
		const float new_distance = current_distance + next_plane_dist;
		const bool too_far = (new_distance > max_bound) || (new_distance > max_d);
		const bool too_near = (new_distance < min_bound) || (new_distance < min_d);
		current_distance = Clamp(min_d, current_distance, max_d);
		if(too_far){
			p4::Log::TMI("%s, %d,-|Too far (%f, %f)", mName, search_depth, min_d, max_d);
		}
		if(too_near){
			p4::Log::TMI("%s, %d,|- Too near (%f, %f)", mName, search_depth, min_d, max_d);
		}

		//terminal_candidate = (in_out == cBSPNode::BSP_FRONT && next_plane_dist > 0.0f) || (in_out == cBSPNode::BSP_BACK && next_plane_dist < 0.0f);
		//terminal_candidate &= (!too_near && !too_far);
		terminal_candidate = in_out == cBSPNode::BSP_BACK;
		int next_index = -1;
		int next_stack_index = -1;
		if(in_out == cBSPNode::BSP_FRONT){
			if( !IsOutsidePoly(start+(new_distance+1.5f*cBSPNode::kEqualThreshold)*direction, current_node)  ){
				p4::Log::TMI("point is inside");
				if(new_distance <= max_bound && new_distance >= min_bound && new_distance < best_result){
					best_result = new_distance;
					p4::Log::TMI("saved new best: %f", new_distance);
				}
			}
			else
			{
				p4::Log::TMI("point is outside");
			}
			//we're front, and moving forward will give us rear
			//queue up the forward move, and check the rear
			if(next_plane_dist > 0.0f){
				if(!too_near){
					p4::Log::TMI("%s, %d,0<- Back to the front node", mName, search_depth);
					next_index = current_node->GetFrontNodeIndex();
				}
				// don't bother queueing up the forward move, just go back for now
				if(!too_far){
					

					next_stack_index = current_node->GetRearNodeIndex();
					p4::Log::TMI("%s, %d,->1 forward to the rear node %d", mName, search_depth, next_stack_index);
					
				}
			}
			//we're front, and moving forward will give us more front
			// descend back for now, queue up the rest
			if(next_plane_dist < 0.0f){
				if(!too_near){
					p4::Log::TMI("%s, %d,1<- descend to the rear node", mName, search_depth);
					next_index = current_node->GetRearNodeIndex();
				}
				if(!too_far){
					p4::Log::TMI("%s, %d,->0 descend to the front node", mName, search_depth);
					next_stack_index = current_node->GetFrontNodeIndex();
				}
			}
		}
		else if(in_out == cBSPNode::BSP_BACK){
			if( !IsOutsidePoly(start+(new_distance+0.5f*cBSPNode::kEqualThreshold)*direction, current_node)  ){
				p4::Log::TMI("point is inside");
				if(new_distance <= max_bound && new_distance >= min_bound && new_distance < best_result){
					best_result = new_distance;
					p4::Log::TMI("saved new best: %f", new_distance);
				}
			}
			else
			{
				p4::Log::TMI("point is outside");
			}
			//we're back, and moving forward will give us front
			if(next_plane_dist > 0.0f){
				if(!too_near){
					p4::Log::TMI("%s, %d,1<- descend to the rear node", mName, search_depth);
					next_index = current_node->GetRearNodeIndex();
					
				}
				if(!too_far){
					p4::Log::TMI("%s, %d,->0 descend to the front node", mName, search_depth);
					next_stack_index = current_node->GetFrontNodeIndex();
				}
			}
			//we're rear, and moving backward will give us front
			if(next_plane_dist < 0.0f){
				if(!too_near){

					p4::Log::TMI("%s, %d,0<- descend to the front node", mName, search_depth);
					next_index = current_node->GetFrontNodeIndex();
					
				}
				if (!too_far){
					
					p4::Log::TMI("%s, %d,->1  descend to the rear node", mName, search_depth);
					next_stack_index = current_node->GetRearNodeIndex();
				}
			}
		}
		else
		{
			p4::Log::Debug("Coplanar");
		}
		
		current_node = NULL;
		if(next_index != -1 || next_stack_index != -1){
			terminal_candidate = false;
			current_distance = Clamp(min_d,new_distance,max_d);
			search_depth ++;
	
			if(next_index != -1 ){
				current_node = &mNodeVec[next_index];
				if(next_plane_dist > 0.0f && current_distance < max_d){
					max_d = current_distance;
				}
			}

			float stack_max = max_d;
			float stack_min = min_d;
			if(next_plane_dist < 0.0f && current_distance > min_d){
				stack_min = current_distance;
			}
			if(next_index == -1 && next_stack_index != -1){
				current_node = &mNodeVec[next_stack_index];
				min_d = stack_min;
				max_d = stack_max;
				//clear so it doesn't get queued
				next_stack_index = -1;
			}

			
			if(next_stack_index != -1){
				p4::Log::TMI("%s, %d,-%d-\\ placing on stack", mName, search_depth, mNodeStack.size());

				mNodeStack.push_back(cNodeRecord());
				mNodeStack.back().mNode=&mNodeVec[next_stack_index];
				mNodeStack.back().mMax = stack_max;
				mNodeStack.back().mMin = stack_min;
				mNodeStack.back().mLocation = current_distance;
			}
			else if(!too_near && !too_far)
			{
				p4::Log::TMI("%s, %d, nothing to put on stack", mName, search_depth);
			}
		}
		else if( !mBoundingBox.CheckWithin(start+best_result*direction) && !mNodeStack.empty()){
			current_node = mNodeStack.back().mNode;
			current_distance = mNodeStack.back().mLocation;
			min_d = mNodeStack.back().mMin;
			max_d = mNodeStack.back().mMax;
			mNodeStack.pop_back();
			p4::Log::TMI("%d unstacking d = %f (stack size = %d)", search_depth, current_distance, mNodeStack.size());
		}

		if(current_node == NULL){
				p4::Log::TMI("%s, %d,--- Scratch that, not descending: this is a terminal node", mName, search_depth);
		}
	}
	P4_ASSERT(in_out != cBSPNode::BSP_SPLIT);
	P4_ASSERT(current_distance < 10000.0);
	bool in_bounds = (mBoundingBox.CheckWithin(start+best_result*direction));
	if(in_bounds){
		p4::Log::TMI(">>HIT, depth = %d, ret = %f, best = %f", search_depth, current_distance, best_result);
		ret = best_result;
		return true;
	}
	
	p4::Log::TMI(">>MISS, depth = %d, ret = %f, best result:%f", search_depth, current_distance, best_result);
	p4::Log::GetInstance()->SetLogLevel(p4::Log::LOG_LEVEL_DEBUG);
	return false;
}
bool cBSPTree::RayCastToBoundary(float &result,  const cCoord3& start, const cCoord3& direction )
{
	cCoord3 pos = start;
	int num_searches = 0;
	const float extra_jump = cBSPNode::kEqualThreshold*0.9f;
	bool in_bounds = true;
	float next_bound = 0.0f;
	float total_distance = 0.0f;
	if(!mBoundingBox.CheckWithin(pos)){
		float distance;
		if(!mBoundingBox.RayIntersection(distance, pos, direction)){
			//early exit: not even in the right area
			return false;
		}
		pos += (distance+extra_jump)*direction;
		total_distance += distance+extra_jump;
		next_bound = ClosestBoundary( pos, direction );
		if(next_bound < 0.0f){
			//the plane was right on the bb edge
			result = total_distance;
			return true;
		}
		num_searches++;
	}
	else{
		//starting within the box
		next_bound = ClosestBoundary( pos, direction );
		if(next_bound < 0.0){
			in_bounds = false;
			next_bound = -next_bound;
		}
	}
	
	while(next_bound >= 0.0f){
		next_bound+=extra_jump;
		pos += (next_bound)*direction;
		if(!mBoundingBox.CheckWithin(pos)){
			P4_ASSERT(mBoundingBox.RayIntersection(total_distance, start, direction));
			return false;
			break;
		}
		total_distance += next_bound;
		next_bound = ClosestBoundary( pos, direction );
		num_searches++;
		next_bound = !in_bounds && next_bound <= 0.0 ? -next_bound : next_bound; 
	}
	if(num_searches>100){
		p4::Log::Debug("total raycast searches = %d", num_searches);
	}
	
	result = total_distance -= extra_jump;
	return true;
}

bool cBSPTree::CheckBoxHit( const cCoord3& start, const cCoord3& direction )
{
	if(!mBoundingBox.CheckWithin(start)){
		float raycast_dist = 0.0f;
		if(mBoundingBox.RayIntersection(raycast_dist, start, direction)){
			//outside, but intersecting
			return true;
		}
		//outside and not intersecting
		return false;
	}
	//within the BB
	return true;
}



