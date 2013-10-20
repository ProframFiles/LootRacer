#include "DrawnString.hpp"
#include "DrawnStringFactory.hpp"

cDrawnStringInternal::cDrawnStringInternal(int num_chars, int vbo_index)
	:mIsTimed(false)
	,mLifetime(0.0f)
	,mElapsedTime(0.0f)
	,mNumChars(num_chars)
	,mVBOindex(vbo_index)
	,mScale(1.0f, 1.0f)
{
}


cDrawnStringInternal::~cDrawnStringInternal(void)
{
}

void cDrawnStringInternal::Update( float elapsed_time )
{
	mElapsedTime += elapsed_time;
}

bool cDrawnStringInternal::IsDone()
{
	return mIsTimed && (mElapsedTime > mLifetime);
}

void cDrawnString::SetText( std::string text,const cCoord2& pos,const cCoord3& color)
{
	mHandle->SetInactive();
	mHandle = mFactory->CreateDrawnStringImpl(text, pos, color, cCoord2(0.0f, 0.0f), cCoord2(0.0f, 0.0f));
}

cDrawnString::~cDrawnString()
{
	mHandle->SetInactive();
}

void cDrawnString::SetScale( cCoord2 new_scale )
{
	mFactory->ScaleString(mHandle, new_scale);
}
