#include "../lib/AudioSource.h"

AudioSource::AudioSource(std::string name, ALuint source)
{
	this->name = name;
	this->source = source;

}

AudioSource::~AudioSource() 
{
	alDeleteSources(1,&this->source);
}


void AudioSource::play()
{
	alSourcei(this->source, AL_LOOPING, this->p_LoopSound);
	alSourcePlay(this->source);

}
void AudioSource::stop()
{
	alSourceStop(this->source);
}

void AudioSource::update(Transform* transform) 
{
	glm::vec3 position = transform->localPosition;
	alSource3f(this->source, AL_POSITION, position.x, position.y, position.z);
}

void AudioSource::setVolume(float volume) 
{

	alSourcef(this->source, AL_GAIN, volume);
}