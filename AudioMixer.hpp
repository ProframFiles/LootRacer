#pragma once
#include "akj_typedefs.hpp"
#include "PositionalAudioSource.hpp"
#include "PositionalAudioHandle.hpp"
#include <vector>
#include <string>
#include <unordered_map>
#include "cs314_math.hpp"

class cAudioMixer
{
public:
	typedef  std::vector<std::vector<uint8_t> > tAudioDataVector;
	typedef short tMixType;
	static const tMixType kMaxSampleValue = 32767;
	static const tMixType kMinSampleValue = -32768;
	cAudioMixer(int freq, int format, int channels, int samples);
	~cAudioMixer(void);
	void Bind(){
		sCurrentMixer = this;
	}

	tPositionalAudioPtr GetSampleHandle(const std::string& audio_id);
	tPositionalAudioPtr GetSampleHandle(const std::string& audio_id, cCoord3 position);

	void AdjustSampleVolume(int sample_id, double volume);
	void AdjustPositionalVolume(int sample_id, double volume);
	static void MixAudio(void* unused, uint8_t *stream, int length);
	void LoadAudioFile(std::string audio_id, const std::string& file_name);
	int PlayAudio(const std::string& audio_id, int num_repeats = 0);
	int AddPositionalSource(const std::string& audio_id);
	int AddPositionalSource(const std::string& audio_id, cPositionalAudioSource source);
	void SetListenerPosition(cCoord3 pos){
		mListenerPosition = pos;
	}
private:
	struct cQueuedSample{
		cQueuedSample(int index, uint32_t sample_id, int repeat = 0)
			:mIndex(index)
			,mSampleID(sample_id)
			,mRepeatNumber(repeat)
			,mPlayingPosition(0)
			,mPlayRate(1.0)
			,mPlayVolume(1.0)
		{}
		double mPlayRate;
		double mPlayVolume;
		uint32_t mSampleID;
		int mIndex;
		int mPlayingPosition;
		int mRepeatNumber;

	};
	int GetSampleIndex( const std::string& audio_id );
	void MixIntoStream(uint8_t *stream, int len);
	tAudioDataVector mAudioData;
	std::unordered_map<std::string, int> mAudioHandles;
	std::unordered_map<std::string, int> mFileHandles;
	std::vector<cQueuedSample> mNowPlaying;
	std::vector<cQueuedSample> mPositionalSamples;
	std::vector<cPositionalAudioSource> mPositionalSources;
	cCoord3 mListenerPosition;
	static cAudioMixer* sCurrentMixer;
	int mFrequency;
	int mBytesPerSample;
	int mNumChannels;
	int mSamples;
	uint32_t mLastSampleIndex;
};

