#pragma once
#include "cs314_math.hpp"

class cVelocityVerlet;
class cStaticEnvironment;
class cForceField{
public:
	virtual void ApplyForce(cVelocityVerlet* object)=0;
private:
};

class cDampedSpringTetherForce: public cForceField {
	public:
	cDampedSpringTetherForce( float in_mass, float in_drag, float in_k)
		:mMultiplier(1.0f)
		,mForceCenter()
		,mInvMass(1.0f/in_mass)
		,mMass(in_mass)
		,mDrag(in_drag)
		,mK(in_k)
		,mExternalForce(0.0f)
		,mExternalAccel(0.0f)
		,mStaticDrag(0.0f)
	{};
	~cDampedSpringTetherForce(){};
	inline void SetPos(const cCoord3& in_center){
		mForceCenter=in_center;
	};
	inline cCoord3 GetPos() const{
		return mForceCenter;
	};
	inline void  ShiftPosition(const cCoord3& shift) {
		mForceCenter += shift;
	}
	inline void SetDamping(const cCoord3& dampRatio){
		mDrag.x=std::sqrt((1.0f/mInvMass)*mK.x)*2*dampRatio.x;
		mDrag.y=std::sqrt((1.0f/mInvMass)*mK.y)*2*dampRatio.y;
		mDrag.z=std::sqrt((1.0f/mInvMass)*mK.z)*2*dampRatio.z;
	}
	inline void SetExtAccel(const cCoord3& in_accel){
		mExternalAccel=in_accel;
	}
	inline void SetExtForce(const cCoord3& in_force){
		mExternalForce=in_force;
	}
	void ApplyForce(cVelocityVerlet* verlet);
	inline void SetMultiplier(const float mul){
		mMultiplier=mul;
	}
	inline float GetMultiplier() const {
		return mMultiplier;
	}
private:
	float mStaticDrag;
	float mInvMass;
	float mMass;
	cCoord3 mDrag;
	cCoord3 mK;
	float mMultiplier;
	cCoord3 mForceCenter;
	cCoord3 mExternalForce;
	cCoord3 mExternalAccel;
};
class WorldConstraintForce: public cForceField{
public:
	WorldConstraintForce(cStaticEnvironment* environment,float height, float radius,  float in_mass, float in_drag, float in_k)
	:mEnvironment(environment)
	,mRadius(0.5f)
	,mClosestSurf(cCoord3(),-1.0f)
	,mIsOnGround(false)
	,mValidPlane(false)
	,mSpringForce(in_mass, in_drag, in_k)
	,mHoverHeight(height)
	,mAirDrag(0.01f, 0.001f, 0.01f)
	,mShockDamping(in_drag)
	{
	}
	virtual void ApplyForce(cVelocityVerlet* object);
	cCoord3 GetAdjustment(){
		return mLastAdjustMent;
	}
	void SetExternalAccel(const cCoord3& accel){
		mAcceleration = accel;
	}
	bool IsOnGround() const { return mIsOnGround; }
	inline void SetDamping(const cCoord3& dampRatio){
		mShockDamping = dampRatio;
	}
private:
	void BounceOffPlane( cVelocityVerlet* object, const cPlane3& plane );
	float mRadius;
	bool mValidPlane;
	bool mIsOnGround;
	
	void IsOnGround(bool val) { mIsOnGround = val; }
	cPlane3 mClosestSurf;
	float mHoverHeight;
	cCoord3 mLastAdjustMent;
	cCoord3 mAirDrag;
	cCoord3 mShockDamping;
	cCoord3 mAcceleration;
	cStaticEnvironment* mEnvironment;
	cDampedSpringTetherForce mSpringForce;
};