#include "AudioSystem.h"
#include "SDL.h"
#include "SDL_mixer.h"
#include <filesystem>

SoundHandle SoundHandle::Invalid;

// Create the AudioSystem with specified number of channels
// (Defaults to 8 channels)
AudioSystem::AudioSystem(int numChannels)
{
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Failed to initialize SDL_mixer: %s", Mix_GetError());
	}
	Mix_AllocateChannels(numChannels);
	mChannels.resize(numChannels);
}

// Destroy the AudioSystem
AudioSystem::~AudioSystem()
{
	for (auto& soundPair : mSounds) {
		Mix_FreeChunk(soundPair.second);
	}

	mSounds.clear();
	Mix_CloseAudio();
}

// Updates the status of all the active sounds every frame
void AudioSystem::Update(float deltaTime)
{
	for (size_t i = 0; i < mChannels.size(); ++i)
	{
		if (Mix_Playing(static_cast<int>(i)) == 0)
		{
			SoundHandle handle = mChannels[i];

			auto handleIt = mHandleMap.find(handle);
			if (handleIt != mHandleMap.end()) {
				mHandleMap.erase(handleIt);
			}

			mChannels[i].Reset();
		}
	}
}

// Plays the sound with the specified name and loops if looping is true
// Returns the SoundHandle which is used to perform any other actions on the
// sound when active
// NOTE: The soundName is without the "Assets/Sounds/" part of the file
//       For example, pass in "ChompLoop.wav" rather than
//       "Assets/Sounds/ChompLoop.wav".
SoundHandle AudioSystem::PlaySound(const std::string& soundName, bool looping)
{
    // Get the sound with the given name
    Mix_Chunk *sound = GetSound(soundName);

    // TODO

	if (!sound)
	{
		SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Failed to load sound: %s", soundName.c_str());
		return SoundHandle::Invalid;
	}

    int availableChannel = -1;

	// int sameSoundCount = 0;
	// auto oldestSameSound = mHandleMap.begin();
	//
	// for (auto it = mHandleMap.begin(); it != mHandleMap.end();)
	// {
	// 	if (it->second.mSoundName == soundName)
	// 	{
	// 		sameSoundCount++;
	//
	// 		if (sameSoundCount == 1) {
	// 			oldestSameSound = it;
	// 		}
	// 		if (sameSoundCount == 3)
	// 		{
	// 			availableChannel = oldestSameSound->second.mChannel;
	// 			mHandleMap.erase(oldestSameSound);
	// 			break;
	// 		}
	// 	}
	// 	++it;
	// }

	for (size_t i = 0; i < mChannels.size(); ++i)
	{
		if (!mChannels[i].IsValid())
		{
			availableChannel = static_cast<int>(i);
			break;
		}
	}

	if (availableChannel == -1)
	{
		for (auto& [handle, info] : mHandleMap)
		{
			if (info.mSoundName == soundName)
			{
				availableChannel = info.mChannel;
				SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Stoping sound: %s", info.mSoundName.c_str());
				mHandleMap.erase(handle);
				break;
			}
		}
	}

	if (availableChannel == -1)
	{
		for (auto& [handle, info] : mHandleMap)
		{
			if (info.mIsLooping)
			{
				StopSound(handle);
				availableChannel = info.mChannel;
				SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Stoping sound: %s", info.mSoundName.c_str());
				break;
			}
		}
	}

	if (availableChannel == -1 && !mHandleMap.empty())
	{
		auto oldestHandle = mHandleMap.begin()->first;
		SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Stoping oldest sound.");
		StopSound(oldestHandle);
		availableChannel = mHandleMap.begin()->second.mChannel;
	}

	++mLastHandle;
	HandleInfo handleInfo{ soundName, availableChannel, looping, false };
	mHandleMap[mLastHandle] = handleInfo;
	mChannels[availableChannel] = mLastHandle;

	Mix_PlayChannel(availableChannel, sound, looping ? -1 : 0);

    return mLastHandle;
}

// Stops the sound if it is currently playing
void AudioSystem::StopSound(SoundHandle sound)
{
	auto handleIt = mHandleMap.find(sound);
	if (handleIt == mHandleMap.end())
	{
		SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Attempted to stop a non-existent sound handle.");
		return;
	}

	int channel = handleIt->second.mChannel;
	Mix_HaltChannel(channel);
	mHandleMap.erase(handleIt);
	mChannels[channel].Reset();
}

// Pauses the sound if it is currently playing
void AudioSystem::PauseSound(SoundHandle sound)
{
	auto handleIt = mHandleMap.find(sound);
	if (handleIt == mHandleMap.end())
	{
		SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Attempted to pause a non-existent sound handle.");
		return;
	}

	int channel = handleIt->second.mChannel;
	if (handleIt->second.mIsPaused) {
		return;
	}
	Mix_Pause(channel);
	handleIt->second.mIsPaused = true;
}

// Resumes the sound if it is currently paused
void AudioSystem::ResumeSound(SoundHandle sound)
{
	auto handleIt = mHandleMap.find(sound);

	if (handleIt == mHandleMap.end())
	{
		SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Attempted to resume a non-existent sound handle.");
		return;
	}

	int channel = handleIt->second.mChannel;
	if (!handleIt->second.mIsPaused) {
		return;
	}
	Mix_Resume(channel);
	handleIt->second.mIsPaused = false;
}

// Returns the current state of the sound
SoundState AudioSystem::GetSoundState(SoundHandle sound)
{
    if(mHandleMap.find(sound) == mHandleMap.end())
    {
        return SoundState::Stopped;
    }

    if(mHandleMap[sound].mIsPaused)
    {
        return SoundState::Paused;
    }

	return SoundState::Playing;
}

// Stops all sounds on all channels
void AudioSystem::StopAllSounds()
{
    Mix_HaltChannel(-1);

    for(auto & mChannel : mChannels)
    {
        mChannel.Reset();
    }

    mHandleMap.clear();
}

// Cache all sounds under Assets/Sounds
void AudioSystem::CacheAllSounds()
{
#ifndef __clang_analyzer__
	std::error_code ec{};
	for (const auto& rootDirEntry : std::filesystem::directory_iterator{"Assets/Sounds", ec})
	{
		std::string extension = rootDirEntry.path().extension().string();
		if (extension == ".ogg" || extension == ".wav")
		{
			std::string fileName = rootDirEntry.path().stem().string();
			fileName += extension;
			CacheSound(fileName);
		}
	}
#endif
}

// Used to preload the sound data of a sound
// NOTE: The soundName is without the "Assets/Sounds/" part of the file
//       For example, pass in "ChompLoop.wav" rather than
//       "Assets/Sounds/ChompLoop.wav".
void AudioSystem::CacheSound(const std::string& soundName)
{
	GetSound(soundName);
}

// If the sound is already loaded, returns Mix_Chunk from the map.
// Otherwise, will attempt to load the file and save it in the map.
// Returns nullptr if sound is not found.
// NOTE: The soundName is without the "Assets/Sounds/" part of the file
//       For example, pass in "ChompLoop.wav" rather than
//       "Assets/Sounds/ChompLoop.wav".
Mix_Chunk* AudioSystem::GetSound(const std::string& soundName)
{
	std::string fileName = "../Assets/Sounds/";
	fileName += soundName;

	Mix_Chunk* chunk = nullptr;
	auto iter = mSounds.find(fileName);
	if (iter != mSounds.end())
	{
		chunk = iter->second;
	}
	else
	{
		chunk = Mix_LoadWAV(fileName.c_str());
		if (!chunk)
		{
			SDL_Log("[AudioSystem] Failed to load sound file %s", fileName.c_str());
			return nullptr;
		}

		mSounds.emplace(fileName, chunk);
	}
	return chunk;
}

// Input for debugging purposes
void AudioSystem::ProcessInput(const Uint8* keyState)
{
	// Debugging code that outputs all active sounds on leading edge of period key
	if (keyState[SDL_SCANCODE_PERIOD] && !mLastDebugKey)
	{
		SDL_Log("[AudioSystem] Active Sounds:");
		for (size_t i = 0; i < mChannels.size(); i++)
		{
			if (mChannels[i].IsValid())
			{
				auto iter = mHandleMap.find(mChannels[i]);
				if (iter != mHandleMap.end())
				{
					HandleInfo& hi = iter->second;
					SDL_Log("Channel %d: %s, %s, looping = %d, paused = %d",
							static_cast<unsigned>(i), mChannels[i].GetDebugStr(),
							hi.mSoundName.c_str(), hi.mIsLooping, hi.mIsPaused);
				}
				else
				{
					SDL_Log("Channel %d: %s INVALID", static_cast<unsigned>(i),
							mChannels[i].GetDebugStr());
				}
			}
		}
	}

	mLastDebugKey = keyState[SDL_SCANCODE_PERIOD];
}