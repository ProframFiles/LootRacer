#pragma once
#include "TimedItem.hpp"
#include <string>
#include "cs314_math.hpp"

class cDrawnStringFactory;

class cDrawnStringInternal: public iTimedItem
{
public:
	cDrawnStringInternal(int num_chars, int vbo_index);
	~cDrawnStringInternal(void);
	void SetLifeTime(float lifetime){
		mIsTimed = true;
		mLifetime = lifetime;
	} 
	int GetIndex() const{
		return mVBOindex;
	}
	int GetNumChars() const {
		return mNumChars;
	}
	void SetIndex(int index){
		mVBOindex = index;
	}
	cCoord2 GetScale() const { return mScale; }
	void SetScale(cCoord2 val) { mScale = val; }

	virtual void Update(float elapsed_time);
	virtual void Reset(){};
	virtual bool IsDone();



private:
	cCoord2 mScale;
	int mVBOindex;
	int mNumChars;
	float mElapsedTime;
	float mLifetime;
	bool mIsTimed;

};
class cDrawnString
{
public:
	cDrawnString(cDrawnStringInternal* handle, cDrawnStringFactory* parent, cCoord3 color, cCoord2 pos)
		: mHandle(handle)
		, mPos(pos)
		, mColor(color)
		, mFactory(parent)
	{}
	~cDrawnString();
	void SetLifeTime(float lifetime){
		mHandle->SetLifeTime(lifetime);
	}
	void SetText( std::string text, const cCoord2& pos,const cCoord3& color);
	void SetScale(cCoord2 new_scale);
private:
	cDrawnStringInternal* mHandle;
	cDrawnStringFactory* mFactory;
	cCoord3 mColor;
	cCoord2 mPos;
};
