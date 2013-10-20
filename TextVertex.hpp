#pragma once
#include "cs314Interfaces.h"
class cTextVertex
{
public:
	cTextVertex()
	{
		mTexCoord[0]=0.0f;
		mPosition[0]=0.0f;
		mVelocity [0]=0.0f;
		mAcceleration[0]=0.0f;
		mTexCoord[1]=0.0f;
		mPosition[1]=0.0f;
		mVelocity [1]=0.0f;
		mAcceleration[1]=0.0f;
		mColor[0] = 0.0f;
		mColor[1] = 0.0f;
		mColor[2] = 0.0f;
		mStartTime = -1.0f;
	}
	cTextVertex(float x, float y, float u, float v )
	{
		mTexCoord[0]=u;
		mPosition[0]=x;
		mVelocity [0]=0.0f;
		mAcceleration[0]=0.0f;
		mTexCoord[1]=v;
		mPosition[1]=y;
		mVelocity [1]=0.0f;
		mAcceleration[1]=0.0f;
		mColor[0] = 0.0f;
		mColor[1] = 0.0f;
		mColor[2] = 0.0f;
		mStartTime = -1.0f;
	}
	~cTextVertex();
	static void BindAttributes();
	static int ByteStride();

	float mTexCoord[2];
	float mPosition[2];
	float mVelocity [2];
	float mAcceleration[2];
	float mStartTime;
	float mColor[3];
};

