#include "PositionalAudioHandle.hpp"
#include "AudioMixer.hpp"


cPositionalAudioHandle::cPositionalAudioHandle(cAudioMixer& parent, int sample_id)
	: mParentMixer(parent)
	, mSampleID(sample_id)
{

}


cPositionalAudioHandle::~cPositionalAudioHandle(void)
{
}

void cPositionalAudioHandle::SetVolume( double volume )
{
	mParentMixer.AdjustPositionalVolume(mSampleID, volume);
}
