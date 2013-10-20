#pragma once
#include "cs314_math.hpp"
#include "TimedItem.hpp"
#include "VelocityVerlet.hpp"

class cStaticEnvironment;

class cPlayer : public iTimedItem
{
public:
	cPlayer(cStaticEnvironment* environment);
	~cPlayer(void);
	void SetPOV();

	virtual void Update(float elapsed_time);;

	virtual void Reset(){};
	virtual bool IsDone(){
		// a players work is never done
		return false;
	};
	cCoord3 GetForwardDirection() const{
		return -cAngleAxis(mHeading, cCoord3::YAxis()).rev_rotate(cCoord3::ZAxis());
	}

	cCoord3 GetLeftRightDirection() const {
		return cAngleAxis(mHeading, cCoord3::YAxis()).rev_rotate(cCoord3::XAxis());
	}
	cCoord3 GetCOMVelocity() const {
		return mCenterOfMass.mVel;
	}
	void SetForwardVelocity(float v){
		cCoord3 direction = GetForwardDirection();
		//mCenterOfMass.mVel = v*direction;
		mCurrentMovement.z = v;
	}
	bool IsOnGround() const{
		return mIsOnGround;
	}
	bool JustHitGround() const{
		return mHitGround;
	}
	void Jump(){
		mIsJumping = true;
	}
	void SetLateralVelocity(float v){
		cCoord3 direction = GetLeftRightDirection();
		mCurrentMovement.x = v;
	}
	void AddForwardVelocity(float v){
		cCoord3 direction = GetForwardDirection();
		mCurrentMovement.z += v;
	}
	void AddLateralVelocity(float v){
		cCoord3 direction = GetLeftRightDirection();
		mCurrentMovement.x += v;
	}
	cCoord3 GetCOMPosition() const{
		return mCenterOfMass.mPos;
	}
	float GetHeadingAngle(){
		return mHeading;
	}
	void RotateLeftRight(float radians){
		mHeading = NormalizeRadians(mHeading - radians);
	}
	void TiltUpDown(float radians){
		mPitch = Clamp(-0.49f*AKJ_PIf, NormalizeRadians(mPitch+radians), 0.49f*AKJ_PIf);
	}
	void SetPosition(cCoord3 pos){
		mPosition = pos;
	}
	void RotatePOV(cUnitQuat rot){
		mOrientation = rot*mOrientation;
	}
	void Translate(cCoord3& translate){
		mPosition += translate;
	}
	cCoord3 GetTargetLocation() const { return mTargetLocation; }
	cCoord3 GetTargetOrientation() const { return mTargetOrientation; }
	cCoord3 GetHeadingVector(){
		p4::Log::TMI("Heading: %f", mHeading);
		cAngleAxis aay(-mHeading, cCoord3::YAxis());
		cAngleAxis aaz(mPitch, cCoord3::XAxis());
		return aay.rotate(aaz.rotate(-cCoord3::ZAxis())).normalized();
	}
	cCoord3 GetPosition() const{
		return mPosition;
	}
	float GetGroundDistance() const { return mGroundDistance; }
	cCoord3 GetHeightOffset() const{
		return cCoord3(0.0, 2.0, 0.0);
	}
	int GetScore() const { return mScore; }
	void SetScore(int val) { mScore = val; }
	void AddToScore(int val) { mScore += val; }
private:
	float mPitch;
	float mHeading;
	float mDrivingForce;
	bool mIsJumping;
	bool mHitGround;
	bool mIsOnGround;
	int mScore;

	cVelocityVerlet mCenterOfMass;
	WorldConstraintForce mCollisionForce;
	cCoord3 mCurrentMovement;
	cCoord3 mHeadingVector;
	cCoord3 mPosition;
	cCoord3 mTargetLocation;
	cCoord3 mTargetOrientation;
	float mGroundDistance;

	cUnitQuat mOrientation;
	cStaticEnvironment* mEnvironment;
};

