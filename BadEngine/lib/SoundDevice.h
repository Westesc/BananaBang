#ifndef SOUNDDEVICE_H
#define SOUNDDEVICE_H
#include <AL/alc.h>

class SoundDevice
{
public:
	static SoundDevice* get();

	SoundDevice();
	~SoundDevice();

private:

	ALCdevice* p_ALCDevice;
	ALCcontext* p_ALCContext;

};

#endif