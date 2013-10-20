#include "LootObject.hpp"
#include "PositionalAudioHandle.hpp"

cLootObject::cLootObject(cDrawableObject* drawable, int value, cCoord4* light, tPositionalAudioPtr audio_handle)
	:mDrawable(*drawable)
	,mValue(value)
	,mHitRadius(2.5f)
	,mLightPointer(light)
{
	mAudioHandle = std::move(audio_handle);
}


cLootObject::~cLootObject(void)
{
}

void cLootObject::SetInactive()
{
	mIsActive = false;
	mLightPointer->w = 0.1f;
    mDrawable.SetVisibility(false);
	mAudioHandle->SetVolume(0.0);
}

void cLootObject::SetActive()
{
	mIsActive = true;
	mLightPointer->w = 1.6f;
	mDrawable.SetVisibility(true);
	mAudioHandle->SetVolume(1.0);
}

void cLootObject::Draw()
{
	mDrawable.Draw();
}

