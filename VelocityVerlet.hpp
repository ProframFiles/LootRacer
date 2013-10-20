#pragma once
#include "cs314_math.hpp"
#include "ForceField.hpp"
class cForceField;
class cVelocityVerlet
{
public:
	cVelocityVerlet(void);
	~cVelocityVerlet(void);
	cVelocityVerlet& Update(const float dt){
		P4_ASSERT(dt > 0.0f);
		mVel += (0.5f*dt)*mAccel;
		mLastPos = mPos;
		mPos += mVel*dt;
		if(mForces!=NULL){
			mForces->ApplyForce(this);
		}
		mVel += (0.5f*dt)*mAccel;
		return *this;
	}
	cCoord3 mAccel;
	cCoord3 mVel;
	cCoord3 mPos;
	cCoord3 mLastPos;
	cForceField* mForces;
};
