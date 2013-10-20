#pragma once
#include "cs314_math.hpp"

class cPositionalAudioSource
{
public:
	cPositionalAudioSource(void);
	~cPositionalAudioSource(void);
	cCoord3 mPosition;
	cCoord3 mVelocity;
	cCoord3 mAttenuation;
	uint32_t mSampleID;
};

