#include "ForceField.hpp"
#include "VelocityVerlet.hpp"
#include "StaticEnvironment.hpp"



void cDampedSpringTetherForce::ApplyForce( cVelocityVerlet* verlet )
{
	const cCoord3 dx = mForceCenter-verlet->mPos;
	float d=0.0f;
	float invd=1.0f;
	if (dx.x != 0.0f || dx.y != 0.0f || dx.z != 0.0f){
		d = dx.length();
		invd = 1.0f/(std::abs(dx.x)+std::abs(dx.y)+std::abs(dx.z));
	}

	float v=0.0f;
	float invv=1.0f;
	if (verlet->mVel.x != 0.0f || verlet->mVel.y != 0.0f || verlet->mVel.z != 0.0f){
		v = verlet->mVel.length();
		//p4::Log::Debug("Velocity verlet (%f, %f)", verlet->mVel.x, verlet->mVel.z );
		invv = 1.0f/(std::abs(verlet->mVel.x)+std::abs(verlet->mVel.y)+std::abs(verlet->mVel.z));
	}
	const cCoord3 fxy = (mK*d*invd)*dx;
	verlet->mAccel = mInvMass*(mMultiplier*fxy - (v*mDrag*invv)*verlet->mVel + mExternalForce)+mExternalAccel ;
	/*
	if(v*mMass < mStaticDrag && mMultiplier*mK*d < mStaticDrag){
		verlet->mVel = cCoord3(0.0f);
		verlet->mAccel = cCoord3(0.0f);
	}
	*/
	
	
	
}

void WorldConstraintForce::ApplyForce( cVelocityVerlet* object )
{
	cCoord3 new_accel;
	const float total_velocity = object->mVel.length();
	cCoord3 planar_velocity(object->mVel.x, 0.0f, object->mVel.z);
	
	cAABB bb(object->mPos, mRadius);
	bb.ExpandToFitY(object->mPos.y-mHoverHeight);
	
	bool is_out_of_sorts = false;

	cAABB last_move_box(object->mLastPos);
	last_move_box.ExpandToFit(object->mPos);
	const cCoord3 last_move = (object->mPos-object->mLastPos);
	if(last_move.AnyNonZero()){
		const cCoord3 last_direction = last_move.normalized();
		cRay crossed_surf = mEnvironment->GetSurfaceNormal(object->mLastPos,last_direction, last_move_box);
		// abort the move if we crossed something, and reflect the velocity
		if(crossed_surf.mLength >= 0.0f && crossed_surf.mLength <= last_move.length()){
			p4::Log::Debug("Crossed A boundary plane last frame, p = (%f, %f, %f)", object->mPos.x, object->mPos.y, object->mPos.z);
			p4::Log::Debug("last pos = (%f, %f, %f), v = (%f, %f, %f)",object->mLastPos.x, object->mLastPos.y, object->mLastPos.z, object->mVel.x, object->mVel.y, object->mVel.z);
			is_out_of_sorts = true;
			const float total_speed = object->mVel.length();
			
			cPlane3 crossed_plane(crossed_surf.mDirection);
			object->mPos += crossed_plane.ReflectVector(last_move);
			object->mVel = total_speed*crossed_plane.ReflectVector(object->mVel).normalized();
			p4::Log::Debug("Now reflecting velocity to (%f, %f, %f), pos = (%f, %f, %f)\n", object->mVel.x, object->mVel.y, object->mVel.z,  object->mPos.x, object->mPos.y, object->mPos.z);
		}
	}

	float ground_distance = mEnvironment->DistanceAboveGround(object->mPos);
	if(std::abs(object->mVel.x)>0.0 || std::abs(object->mVel.z)>0.0 ){
		const cCoord3 feeler_pos = mRadius*cCoord3(object->mVel.x,0.0f, object->mVel.z).normalized() + object->mPos; 
		const float outer_ground_distance = mEnvironment->DistanceAboveGround(feeler_pos);
		if(std::abs(ground_distance) > std::abs(outer_ground_distance)){
			ground_distance = outer_ground_distance;
		}
	}
	// are we almost underground?
	if(ground_distance < 0.01f){
		//teleport a little bit above ground
		object->mPos.y -= ground_distance -0.02f;
		is_out_of_sorts = true;
		//object->mPos = object->mLastPos;
		p4::Log::Debug("Still underground, teleporting to %f,  v = (%f, %f, %f)", object->mPos.y, object->mVel.x, object->mVel.y, object->mVel.z);
		// reverse the y component of the velocity (if it makes sense)
		if(object->mVel.y < 0.0f){
			object->mVel.y = -object->mVel.y;
			p4::Log::Debug("Also reflecting velocity to (%f, %f, %f)", object->mVel.x, object->mVel.y, object->mVel.z);
		}
		mIsOnGround = true;
		// neglect to turn on gravity for a frame
	}
	// are we within our spring distance?
	// as in: are our feet on the ground?
	else if(ground_distance < mHoverHeight){
		mIsOnGround = true;
		new_accel.y -= mEnvironment->GetGravity();
	}
	// otherwise we're in the air
	else{
		mIsOnGround = false;
		new_accel.y -= mEnvironment->GetGravity();
	}
	if(mIsOnGround){
		// our current position is somewhere between 0 and mHoverHeight
		// in other words, ground_distnce is less than mHoverheight
		// want to adjust the force center to mHoverHeight above the ground
		cCoord3 force_tether_pos = object->mPos + cCoord3::YAxis()*(mHoverHeight-ground_distance);
		mSpringForce.SetPos(force_tether_pos);
		mSpringForce.SetExtForce(mAcceleration*cCoord3(1.0, object->mVel.y<=0.0f ? 1.0: 1.0/(1.0 + 0.1*object->mVel.y), 1.0));
		mSpringForce.SetDamping(mShockDamping);
	}
	else{
		mSpringForce.SetPos(object->mPos);
		mSpringForce.SetExtForce(0.04f*mAcceleration);
		mSpringForce.SetDamping(mAirDrag);
	}
	
	// if we're not moving, don't worry about collisions
	if(total_velocity > 0.0f){
		//200 meters in a level, 1/60 of a second frame time (max)
		//200/(1/60) = 12000, the largest velocity that makes any sense at all
		object->mVel.TruncateMagnitude(2000.0f);
		cCoord3 direction = object->mVel.normalized();
		cRay closest_surf = mEnvironment->GetSurfaceNormal(object->mPos,direction, bb);
	
		mClosestSurf = cPlane3(closest_surf.mDirection, closest_surf.mDirection.dot(object->mPos+direction*closest_surf.mLength));
		if(closest_surf.mLength < mRadius){
			mValidPlane = true;
		}
		else{
			mValidPlane = false;
		}
	}
	else{
			mValidPlane = false;
	}
	mSpringForce.ApplyForce(object);
	if(!is_out_of_sorts){
		// don't mess with the integrator until we're
		object->mAccel += new_accel;
	}
	else{
		// don't accellerate this frame
		mSpringForce.SetExtForce(cCoord3::Zeros());
	}
	
	if(mValidPlane){
		//skip this for now
		//BounceOffPlane(object, mClosestSurf);
	}
}

void WorldConstraintForce::BounceOffPlane( cVelocityVerlet* object, const cPlane3& plane)
{
	const float plane_distance = plane.DistanceToPoint(object->mPos)-mRadius;
	if(plane_distance < 0.0f){
		object->mPos += plane.NormalVec()*(-plane_distance);
		const float dot_factor = object->mVel.dot(plane.NormalVec());
		if(false&& dot_factor > 0.0f){
			const float v_size = object->mVel.length();
			const cCoord3 rvec = (object->mVel-1.2f*dot_factor*plane.NormalVec()).normalized();
			object->mVel = rvec*v_size;
		}
	}
}
