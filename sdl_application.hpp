#pragma once
#include "cs314Interfaces.h"
#include "TimedItem.hpp"
#include "cs314_timing.hpp"
#include "ShaderPool.hpp"
#include "MeshPool.hpp"
#include "TexturePool.hpp"
#include "cs314_p4_log.hpp"
#include "Player.hpp"
#include "StaticObjectPool.hpp"
#include "DrawnStringFactory.hpp"
#include "LootObject.hpp"
#include "LightManager.hpp"
#include "PositionalAudioHandle.hpp"

class cStaticEnvironment;
class cDrawnStringFactory;
class cAudioMixer;
union SDL_Event;
namespace akj{
	class cGLCube;
}
class cSDLGLContext{
public:
	cSDLGLContext(int init_width, int init_height);
	~cSDLGLContext();
	void SwapBuffers();
	bool ToggleVSync();
	bool UsingVSync();
	int CheckErrors();
private:
	static const int kErrorBufferSize = 1024;
	std::string mErrorMessageBuffer;
	bool mUseVsync;
};


class cSDLApplication
{
public:
	cSDLApplication(cSDLGLContext& context);
	void Run();

	void UpdateHUD();

	void AddTimedItem(iTimedItem& timed_item){
		mTimedItems.push_back(&timed_item);
		timed_item.SetActive();
	}
	int CheckErrors();
	bool IsDone() const {
		if(mIsDone){
			p4::Log::Debug("Application is done!");
		}
		return mIsDone;
	}
	cSDLApplication& SetMinTickTime(float seconds){
		mMinTickTime = seconds;
		return *this;
	}

	~cSDLApplication();

private:
	void MoveToBackOfDrawingOrder(iDrawable* pointer);
	void StartLevel();
	void StopLevel();
	void DoGameplay();
	void SetConfig();
	void LoadShaders();
	void LoadMeshes();
	void LoadTextures();
	void AssembleStaticObjects();
	void InitStringFactory();
	void InitAudio();
	void CreateCollisionStructures();
	void CreateDebugDisplay();
	void CreateLoot();
	void HandleSingleEvent(SDL_Event& event);
	void DrawAll();
	void HandleEvents();
	void HandleTimedItems();
	void SetProjection();
	void ToggleCaptureMouse();
	static const int kErrorBufferSize = 1024;
	

	cCoord2 mMouseSensitivity;
	cCoord2 mCurrentMovement;
	float mAccelRate;
	float mStrafeRate;
	cSDLGLContext& mGLContext;
	cShaderPool mShaderPool;
	cTexturePool mTexturePool;
	cMeshPool mMeshPool;
	std::unique_ptr<cDrawnStringFactory> mDrawnStringFactory;
	std::unique_ptr<cObjectPool> mStaticObjectPool;
	std::unique_ptr<cObjectPool> mDynamicObjectPool;
	std::unique_ptr<cAudioMixer> mAudioMixer;

	tPositionalAudioPtr mHitGroundSound;

	std::string mErrorMessageBuffer;
	std::vector<iTimedItem*> mTimedItems;
	std::vector<iDrawable*> mDrawnItems;
	std::vector<std::unique_ptr<cLootObject> > mLootVec;
	
	cLightManager mLightManager;
	std::unique_ptr<akj::cGLCube> mCube;




	std::unique_ptr<cStaticEnvironment> mStaticEnvironment;
	tStringHandle mFrameRateText;
	tStringHandle mPlayerScoreText;
	tStringHandle mTimeRemainingText;
	tStringHandle mRayCastText;
	tStringHandle mYDistanceText;
	tStringHandle mXDistanceText;
	tStringHandle mZDistanceText;
	tStringHandle mTitleString;
	tStringHandle mTitleSubString;

	//SDL state:
	bool mIsMouseCaptured;


	std::unique_ptr<cPlayer> mPlayer;
	float mMinTickTime;
	bool mIsDone;
	cStopWatch mTimer;
	cStopWatch mAbsoluteTimer;
	bool mUsingVsync;
	cStopWatch mFrameTimer;
	uint64_t mTotalFrameCount;
	uint64_t mElapsedFrames;
	double mSmoothedFrameTime;
	double mLastFrameTime;
	double mElapsedEventTime;
	double mLevelTime;
	double mLevelElapsedTime;
	float mFixedFrameTime;
	float mFOV;
	float mAspectRatio;

	bool mIsPlayingLevel;
};

