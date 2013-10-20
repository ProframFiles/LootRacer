#pragma once
#include <memory>
#include "cs314_math.hpp"

class cAudioMixer;
class cPositionalAudioHandle
{
public:
	cPositionalAudioHandle(cAudioMixer& parent, int sample_id);
	~cPositionalAudioHandle(void);
	void SetVolume(double volume);
	void SetPosition(cCoord3 position);
	void SetVelocity(cCoord3 velocity);
	void SetLinearAttenuation(float attenuation);
	void SetQuadraticAttenuation(float attenuation);
private:
	cAudioMixer& mParentMixer;
	const int mSampleID;
};

typedef std::unique_ptr<cPositionalAudioHandle> tPositionalAudioPtr;