#pragma once
#include "cs314_math.hpp"
#include "cs314_p4_log.hpp"

class cBSPNode{
public:
		cBSPNode(const cTriangle& tri, int parent_index, int depth)
		:mFrontNodeIndex(-1)
		,mRearNodeIndex(-1)
		,mParentNodeIndex(parent_index)
		,mDepth(depth)
		,mTriangle(tri)
		,mPlane(tri.GetNormal(), tri.GetNormal().dot(tri.MeanLocation()))
	{
		P4_ASSERT(mPlane.NormalVec().length() > 0.98);
	}
	// the first two have to stay as the first two
	// they're used as matrix indices later
	enum eClassification{
		BSP_FRONT,
		BSP_BACK,
		BSP_SPLIT,
		BSP_COPLANAR
	};
	// defines the equation of a plane
	void SetFrontNodeIndex(int index){
		mFrontNodeIndex = index;
	}
	void SetBackNodeIndex(int index){
		mRearNodeIndex = index;
	}
	int GetFrontNodeIndex() const {
		return mFrontNodeIndex;
	}
	int GetRearNodeIndex() const {
		return mRearNodeIndex;
	}
	int GetParentNodeIndex() const {
		return mParentNodeIndex;
	}
	int GetDepth() const{
		return mDepth;
	}
	const cTriangle& GetTriangle() const{
		return mTriangle;
	}
	void AddTriangle(cTriangle triangle){
		mTriVec.push_back(triangle);
	}
	cCoord3 GetNormal(){
		return mPlane.NormalVec();
	}
	const cPlane3& GetDividingPlane() const{
		return mPlane;
	}
	float DistanceToPlane(const cCoord3& start, const cCoord3& direction){
		float result = 0.0f;
		mPlane.DistanceAlongRay(result, start, direction);
		return result;
	}
	eClassification ClassifyPoint(const cCoord3& p) const{
		const float plane_distance = mPlane.DistanceToPoint(p);
		return ClassifyPoint(plane_distance);
	}
	eClassification ClassifyPoint(const cCoord3& p, float tolerance) const{
		const float plane_distance = mPlane.DistanceToPoint(p);
		return ClassifyPoint(plane_distance, tolerance);
	}
	static inline  eClassification ClassifyPoint(float plane_distance){
		if(plane_distance > kEqualThreshold){
			return BSP_FRONT;
		}
		else if(plane_distance < -kEqualThreshold ){
			return BSP_BACK;
		}
		return BSP_COPLANAR;
	}
	static inline  eClassification ClassifyPoint(float plane_distance, float threshold){
		if(plane_distance > threshold){
			return BSP_FRONT;
		}
		else if(plane_distance < -threshold ){
			return BSP_BACK;
		}
		return BSP_COPLANAR;
	}
	

	eClassification ClassifyTriangle(cTriangle& tri){
		const eClassification c0 = ClassifyPoint(tri.GetVertex(0));
		const eClassification c1 = ClassifyPoint(tri.GetVertex(1));
		const eClassification c2 = ClassifyPoint(tri.GetVertex(2));
		if(BSP_COPLANAR == c0 && c0 == c1 && c2 == c0){
			return BSP_COPLANAR;
		}
		//options are front and split
		else if(c0 == BSP_FRONT || c1 == BSP_FRONT || c2 == BSP_FRONT){
			if(c0 == BSP_BACK || c1 == BSP_BACK || c2 == BSP_BACK){
				return BSP_SPLIT;
			}
			return BSP_FRONT;
		}
		return BSP_BACK;
	}
	static const float kEqualThreshold;
private:
	
	cPlane3 mPlane;
	std::vector<cTriangle> mTriVec;
	cTriangle mTriangle;
	int mFrontNodeIndex;
	int mRearNodeIndex;
	int mParentNodeIndex;
	int mDepth;
};


class cBSPTree
{
public:
	enum eBSPHeuristic{
		BSP_DISTANCE_FROM_COM,
		BSP_COPLANAR,
		BSP_ORTHOGONAL,
		BSP_PROXIMITY,
		BSP_AREA
	};
	cBSPTree(std::vector<cTriangle>& tris, std::string name, eBSPHeuristic heuristic);
	~cBSPTree(void);
	int IsOutsidePoly(const cCoord3& p) const;
	
	std::vector<cTriangle>& SplitTriangle(const cTriangle& t1,const cPlane3& plane );
	float ClosestBoundary(const cCoord3& p, const cCoord3& direction ) const;
	bool RayCastToBoundary(float& result, const cCoord3& start, const cCoord3& direction);
	bool RayCastToBoundary2(float &result, const cCoord3& start, const cCoord3& direction );
	bool RayCastToBoundary3(cRay &ret, const cCoord3& start, const cCoord3& direction );
	bool CheckBoxHit( const cCoord3& start, const cCoord3& direction );
	bool CheckIntersect(const cAABB& other){
		return mBoundingBox.CheckIntersect(other);
	}
	cAABB GetBoundingBox(){
		return mBoundingBox;
	}
private:
	bool IsOutsidePoly(const cCoord3& p, cBSPNode* start);
	cBSPNode* ClosestContainingPlane( const cCoord3& p, const cCoord3& direction, cBSPNode* node );
	bool IsOutsidePoly(cRay& best_distance, const cCoord3& p, const cCoord3& direction, cBSPNode* node );
	cBSPNode* GetNode(int i){
		if(i<0 || i > mNodeVec.size() - 1){
			return NULL;
		}
		return &mNodeVec[i];
	}
	cBSPNode* HeadNode(){
		return &mNodeVec[0];
	}
	struct cTriNodeIndexPair{
		cTriNodeIndexPair(int tri, int node)
			:mTriIndex(tri)
			,mParentNode(node)
			,mSplits(0)
			,mInOutDiff(0)
			,mBadCoplanar(0)
			,mPerimeter(0.0f)
		{}
		int mTriIndex;
		int mParentNode;
		int mSplits;
		int mInOutDiff;
		int mBadCoplanar;
		float mPerimeter;
	};
	struct cNodeRecord{
		cBSPNode* mNode;
		float mMax;
		float mMin;
		float mLocation;
		float mLastSplitLocation;
		cBSPNode::eClassification mInOut;
	};
	int PreProcessNodes(std::vector<cTriangle>& tris, std::vector<cTriNodeIndexPair>& in_progress,  int parent_node);
	
	std::string mName;
	cAABB mBoundingBox;
	std::vector<cTriangle> mSplitTriangleBuffer;
	std::vector<cBSPNode> mNodeVec;
	std::vector<cTriangle> mTris;
	std::vector<cNodeRecord> mNodeStack;
	eBSPHeuristic mHeuristic;

};

