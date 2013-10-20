#include "AudioMixer.hpp"
#include "SDL_audio.h"
#include "cs314_p4_log.hpp"
#include "cs314_math.hpp"

cAudioMixer* cAudioMixer::sCurrentMixer = NULL; 

cAudioMixer::cAudioMixer( int freq, int bytes_per_sample, int channels, int samples )
	: mFrequency(freq)
	, mBytesPerSample(2)
	, mNumChannels(channels)
	, mSamples(samples)
	, mLastSampleIndex(-1)
{
	mNowPlaying.reserve(16);
	mPositionalSources.reserve(64);
	mPositionalSamples.reserve(64);
	// see http://www.libsdl.org/intro.en/usingsound.html
	SDL_AudioSpec fmt;
    fmt.freq = freq;
	fmt.format = AUDIO_S16;
    fmt.channels = channels;
    fmt.samples = samples;
    fmt.callback = cAudioMixer::MixAudio;
	sCurrentMixer = this;
    fmt.userdata = NULL;
	if ( SDL_OpenAudio(&fmt, NULL) < 0 ) {
		std::string error_string = "Unable to open audio: ";
		error_string.append(SDL_GetError());
        p4::Log::Error(error_string);
        throw std::runtime_error(error_string.c_str());
    }
	SDL_PauseAudio(0);
}


cAudioMixer::~cAudioMixer(void)
{
	SDL_CloseAudio();
	if (sCurrentMixer == this)
	{
		sCurrentMixer = NULL;
	}
}

void cAudioMixer::LoadAudioFile( std::string audio_id, const std::string& file_name )
{
	auto find_result = mFileHandles.find(file_name);
	int handle_index = -1;
	if(find_result == mFileHandles.end()){
		SDL_AudioSpec fmt;
		uint8_t* ptr = NULL;
		uint32_t audio_length = 0;

		if(SDL_LoadWAV(file_name.c_str(), &fmt, &ptr, &audio_length) == NULL){
			std::string error_string = "Unable to open wav: ";
			error_string.append(file_name);
			error_string.append(", ");
			error_string.append(SDL_GetError());
			p4::Log::Error(error_string);
			throw std::runtime_error(error_string.c_str());
		}
		SDL_AudioCVT converter_desc;
		SDL_BuildAudioCVT(&converter_desc, fmt.format, fmt.channels, fmt.freq, AUDIO_S16, mNumChannels, mFrequency);
		mAudioData.emplace_back(audio_length*converter_desc.len_mult);
		for (uint32_t i = 0; i < audio_length; i++)
		{
			mAudioData.back().at(i) = ptr[i];
		}
		SDL_FreeWAV(ptr);

		converter_desc.buf = &mAudioData.back().at(0);
		converter_desc.len = audio_length;
		SDL_ConvertAudio(&converter_desc);
		handle_index = static_cast<int>(mAudioData.size()-1);
		mFileHandles.insert(std::make_pair(file_name,handle_index));
	}
	else{
		handle_index = find_result->second;
	}
	mAudioHandles.insert( std::make_pair(audio_id, handle_index) );
}

int cAudioMixer::PlayAudio( const std::string& audio_id, int num_repeats /*= 0*/ )
{
	const int index = GetSampleIndex(audio_id);
	
	SDL_LockAudio();
	SDL_PauseAudio(false);
	mLastSampleIndex++;
	mNowPlaying.emplace_back(index, mLastSampleIndex, num_repeats);
	SDL_UnlockAudio();
	return mLastSampleIndex;
	
}
int cAudioMixer::AddPositionalSource( const std::string& audio_id )
{
	cPositionalAudioSource source;
	source.mPosition = mListenerPosition;
	source.mVelocity = cCoord3::Zeros();
	source.mAttenuation = cCoord3(1.0f, 0.0f, 0.0f);
	return AddPositionalSource(audio_id, source);
}

int cAudioMixer::AddPositionalSource( const std::string& audio_id, cPositionalAudioSource source )
{
	const int index = GetSampleIndex(audio_id);
	SDL_LockAudio();
	SDL_PauseAudio(false);
	const int sample_id = static_cast<int>(mPositionalSamples.size());
	mPositionalSamples.emplace_back(index, sample_id);
	mPositionalSources.emplace_back(source);
	SDL_UnlockAudio();
	return sample_id;
}


tPositionalAudioPtr cAudioMixer::GetSampleHandle( const std::string& audio_id )
{
	const int sample_index = AddPositionalSource(audio_id);
	return GetSampleHandle(audio_id, mListenerPosition);
}

tPositionalAudioPtr cAudioMixer::GetSampleHandle( const std::string& audio_id, cCoord3 position )
{
	cPositionalAudioSource source;
	source.mPosition = position;
	source.mVelocity = cCoord3::Zeros();
	source.mAttenuation = cCoord3(1.0f, 1.0f, 0.1f);
	const int sample_index = AddPositionalSource(audio_id);
	return tPositionalAudioPtr(new cPositionalAudioHandle(*this, sample_index));
}

void cAudioMixer::MixIntoStream( Uint8 *stream, int len )
{
	if(mNowPlaying.empty()){
		SDL_PauseAudio(true);
		return;
	}
	int bytes_played = 0;
	while(bytes_played < len){
		const int remaining_sample_bytes = len - bytes_played;
		int smallest_length = remaining_sample_bytes;
		for (size_t i = 0; i < mNowPlaying.size(); ++i)
		{
			cQueuedSample& sample = mNowPlaying.at(i);
			const int audio_size =  static_cast<int>(mAudioData.at(sample.mIndex).size());
			int bytes_to_play = remaining_sample_bytes;
			if(sample.mPlayingPosition + remaining_sample_bytes >= audio_size){
				bytes_to_play = audio_size - sample.mPlayingPosition;
				if(bytes_to_play == 0){
					if(sample.mRepeatNumber == 0){
						mNowPlaying.at(i) = mNowPlaying.back();
						mNowPlaying.pop_back();
					}
					else{
						sample.mRepeatNumber--;
						sample.mPlayingPosition = 0;
					}
					i--;
					continue;
				}
				if(bytes_to_play < smallest_length){
					smallest_length = bytes_to_play;
				}
			}
		}
		if(mNowPlaying.empty()){
			SDL_PauseAudio(true);
			return;
		}
		const int sample_bytes = sizeof(tMixType);
		P4_ASSERT(mNumChannels == 2);
		for (int i = 0; i < smallest_length; i+= sample_bytes*mNumChannels)
		{
			double mixed_sample_left = 0.0;
			double mixed_sample_right = 0.0;
			for (int ps_i = 0; ps_i < static_cast<int>(mPositionalSamples.size()); ps_i++)
			{
				/*
				cQueuedSample& sample = mPositionalSamples.at(ps_i);
				const double d = (mListenerPosition-mPositionalSources.at(ps_i).mPosition).length();
				const tMixType audio_sample_left = (*reinterpret_cast<tMixType*>(&mAudioData.at(sample.mIndex).at(i+sample.mPlayingPosition)));
				const tMixType audio_sample_right = (*reinterpret_cast<tMixType*>(&mAudioData.at(sample.mIndex).at(i+sample_bytes+sample.mPlayingPosition)));
				const double la = mPositionalSources.at(ps_i).mAttenuation.y;
				const double qa = mPositionalSources.at(ps_i).mAttenuation.z;
				double vol = 1.0/(1.0+d*(la+qa*d));
				mixed_sample_left += vol*sample.mPlayVolume*(static_cast<double>(audio_sample_left)) ;
				mixed_sample_right += vol*sample.mPlayVolume*(static_cast<double>(audio_sample_right)) ;
				*/
			}
			for (const cQueuedSample& sample: mNowPlaying)
			{
				const tMixType audio_sample_left = (*reinterpret_cast<tMixType*>(&mAudioData.at(sample.mIndex).at(i+sample.mPlayingPosition)));
				const tMixType audio_sample_right = (*reinterpret_cast<tMixType*>(&mAudioData.at(sample.mIndex).at(i+sample_bytes+sample.mPlayingPosition)));
				mixed_sample_left += sample.mPlayVolume*(static_cast<double>(audio_sample_left)) ;
				mixed_sample_right += sample.mPlayVolume*(static_cast<double>(audio_sample_right)) ;
			}
			mixed_sample_left = Clamp(static_cast<double>(kMinSampleValue), mixed_sample_left, static_cast<double>(kMaxSampleValue));
			mixed_sample_right = Clamp(static_cast<double>(kMinSampleValue), mixed_sample_right, static_cast<double>(kMaxSampleValue));
			*reinterpret_cast<tMixType*>(stream+i) = static_cast<tMixType>(mixed_sample_left);
			*reinterpret_cast<tMixType*>(stream+i+sample_bytes) = static_cast<tMixType>(mixed_sample_right);
		}

		for (size_t i = 0; i < mNowPlaying.size(); ++i)
		{
			cQueuedSample& sample = mNowPlaying.at(i);
			sample.mPlayingPosition += smallest_length;
		}
		bytes_played += smallest_length;
		stream += smallest_length;
	}
}

void cAudioMixer::MixAudio( void* unused, uint8_t *stream, int length )
{
	P4_ASSERT(sCurrentMixer != NULL);
	sCurrentMixer->MixIntoStream(stream, length);
}

void cAudioMixer::AdjustSampleVolume( int sample_id, double volume )
{
	for (cQueuedSample& sample: mNowPlaying)
	{
		if(sample.mSampleID == sample_id){
			sample.mPlayVolume = volume;
			return;
		}
	}
	p4::Log::Error("tried to adjust the volume of a non-existent sample (%d)", sample_id);
	P4_ASSERT(false);
}

int cAudioMixer::GetSampleIndex( const std::string& audio_id )
{
	int audio_size =  static_cast<int>(mAudioData.size());
	auto find_result = mAudioHandles.find(audio_id);
	if(find_result == mAudioHandles.end()){
		p4::Log::Warn("Unable to find data index for audio id %s", audio_id);
		P4_ASSERT(find_result != mAudioHandles.end());
	}
	const int index = find_result->second;
	if(audio_size <= index){
		p4::Log::Warn("cannot play audio index %d, only have %d files loaded", index, audio_size);
		P4_ASSERT(audio_size > index);
	}
	return index;
}

void cAudioMixer::AdjustPositionalVolume( int sample_id, double volume )
{
	mPositionalSamples.at(sample_id).mPlayVolume = volume;
}



