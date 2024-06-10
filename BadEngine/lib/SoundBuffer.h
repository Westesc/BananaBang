#pragma once
#include <AL\al.h>
#include <vector>
class SoundBuffer
{
public:
	static SoundBuffer* get();

	ALuint addSoundEffect(const char* path);
	bool removeSoundEffect(const ALuint& buffer);
	SoundBuffer();
	~SoundBuffer();

private:

	std::vector<ALuint> p_SoundEffectBuffers;
};
