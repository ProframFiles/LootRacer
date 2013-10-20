#pragma once
#include "cs314Interfaces.h"
#include "DrawableObject.hpp"
#include "PositionalAudioHandle.hpp"


class cLootObject: public iDrawable
{
public: 
	cLootObject(cDrawableObject* drawable, int value, cCoord4* light, tPositionalAudioPtr audio_handle);
	~cLootObject(void);
	cDrawableObject* GetDrawable(){
		return &mDrawable;
	}
	virtual void Draw();
	bool IsTouching(const cCoord3& point ){
		return mIsActive && (cCoord3::YAxis()*0.5f+mDrawable.GetPosition() - point).lengthSquared() <= mHitRadius*mHitRadius;
	}
	int GetValue() const { return mValue; }
	void SetInactive();
	void SetActive();
private:
	int mValue;

	float mHitRadius;
	cCoord4* mLightPointer;
	int mDrawableIndex;
	cDrawableObject mDrawable;
	bool mIsActive;
	tPositionalAudioPtr mAudioHandle;
};

