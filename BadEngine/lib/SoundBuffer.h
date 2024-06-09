#ifndef SOUNDBUFFER_H
#define SOUNDBUFFER_H
#include <AL\al.h>
#include <vector>
class SoundBuffer
{
public:
	static SoundBuffer* get();

	ALuint addSoundEffect(const char* filename);
	bool removeSoundEffect(const ALuint& buffer);
	SoundBuffer();
	~SoundBuffer();

private:

	std::vector<ALuint> p_SoundEffectBuffers;
};


#endif