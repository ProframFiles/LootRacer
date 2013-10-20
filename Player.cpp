#include "Player.hpp"
#include "akj_ogl.h"
#include "StaticEnvironment.hpp"

cPlayer::cPlayer(cStaticEnvironment* environment)
	:mEnvironment(environment)
	,mCollisionForce(environment,2.0, 0.5, 50.0f, 5.2f, 1500.0f)
	,mDrivingForce(900.0f)
	,mIsJumping(false)
	,mScore(0)
	,mTargetOrientation(cCoord3::ZAxis())
{
	float ground_distance = mEnvironment->DistanceAboveGround(mCenterOfMass.mPos);
	if(ground_distance < 0.01f){
		//teleport a little bit above ground
		mCenterOfMass.mPos.y -= ground_distance -0.01f;
	}
	mGroundDistance = mEnvironment->DistanceAboveGround(mCenterOfMass.mPos);
	mCollisionForce.SetDamping(cCoord3(1.1f, 0.7f, 1.1f));
	mHeading = -0.0f;
	mPitch = 0.0;
	mPosition = cCoord3(0.0f, 0.1f, 0.0f);
	mCenterOfMass.mForces = &mCollisionForce;
}


cPlayer::~cPlayer(void)
{
}

void cPlayer::SetPOV()
{

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	cAngleAxis aay(mHeading, cCoord3::YAxis());
	cAngleAxis aaz(-mPitch, cCoord3::XAxis());
	akj::glRotateaa(aaz);
	akj::glRotateaa(aay);
	akj::gltranslatec3(-1.0f*mPosition);
	
}

void cPlayer::Update( float elapsed_time )
{
	const float time_constant = 1.0f/60.0f;
	const float kalman = 0.15f;
	const float smoothing = LesserOf((elapsed_time / time_constant)*kalman, 1.0f);
	cCoord3 heading = GetHeadingVector();
	cRay closest_distance = mEnvironment->GetSurfaceNormal(GetPosition(), heading);
	cRay bb_hit; 
	if(mEnvironment->GetBoundingBox().RayIntersection(bb_hit, GetPosition(), heading) && bb_hit.mLength == closest_distance.mLength){
		closest_distance.mDirection = heading;
		closest_distance.mLength = 1000.0;
	}
	mTargetLocation = mPosition + heading*closest_distance.mLength;
	cUnitQuat target_rot(mTargetOrientation, closest_distance.mDirection);
	mTargetOrientation = target_rot.scale(smoothing).rotate(mTargetOrientation);

	if(mIsJumping && !mCollisionForce.IsOnGround()){
		mIsJumping = false;
	}
	cCoord3 desired_v = GetForwardDirection()*mCurrentMovement.z+GetLeftRightDirection()*mCurrentMovement.x;
	if(mIsJumping){
		desired_v.y = 20.0f;
	}
	else{
		desired_v.y = 0.0;
	}
	if(mIsOnGround != mCollisionForce.IsOnGround()){
		mIsOnGround = !mIsOnGround;
		if(mIsOnGround){
			mHitGround = true;
		}
	} 
	else
	{
		mHitGround = false;
	}

	mCollisionForce.SetExternalAccel(desired_v*mDrivingForce);
	mCenterOfMass.Update(elapsed_time);
	mPosition = mCenterOfMass.mPos;
	mGroundDistance = mEnvironment->DistanceAboveGround(mCenterOfMass.mPos);
}
