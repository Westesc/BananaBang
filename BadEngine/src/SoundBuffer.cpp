#include "../lib/SoundBuffer.h"
#include <iostream>
#include <fstream>
#include <inttypes.h>
#include <AL\alext.h>

SoundBuffer* SoundBuffer::get()
{
	static SoundBuffer* sndbuf = new SoundBuffer();
	return sndbuf;
}
//
//ALuint SoundBuffer::addSoundEffect(const char* path)
//{
//
//	//ALenum err, format;
//	//ALuint buffer;
//	//SNDFILE* sndfile;
//	//SF_INFO sfinfo;
//	//short* membuf;
//	//sf_count_t num_frames;
//	//ALsizei num_bytes;
//
//	///* Open the audio file and check that it's usable. */
//	//sndfile = sf_open(filename, SFM_READ, &sfinfo);
//	//if (!sndfile)
//	//{
//	//	fprintf(stderr, "Could not open audio in %s: %s\n", filename, sf_strerror(sndfile));
//	//	return 0;
//	//}
//	//if (sfinfo.frames < 1 || sfinfo.frames >(sf_count_t)(INT_MAX / sizeof(short)) / sfinfo.channels)
//	//{
//	//	fprintf(stderr, "Bad sample count in %s (%" PRId64 ")\n", filename, sfinfo.frames);
//	//	sf_close(sndfile);
//	//	return 0;
//	//}
//
//	///* Get the sound format, and figure out the OpenAL format */
//	//format = AL_NONE;
//	//if (sfinfo.channels == 1)
//	//	format = AL_FORMAT_MONO16;
//	//else if (sfinfo.channels == 2)
//	//	format = AL_FORMAT_STEREO16;
//	//else if (sfinfo.channels == 3)
//	//{
//	//	if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
//	//		format = AL_FORMAT_BFORMAT2D_16;
//	//}
//	//else if (sfinfo.channels == 4)
//	//{
//	//	if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
//	//		format = AL_FORMAT_BFORMAT3D_16;
//	//}
//	//if (!format)
//	//{
//	//	fprintf(stderr, "Unsupported channel count: %d\n", sfinfo.channels);
//	//	sf_close(sndfile);
//	//	return 0;
//	//}
//
//	///* Decode the whole audio file to a buffer. */
//	//membuf = static_cast<short*>(malloc((size_t)(sfinfo.frames * sfinfo.channels) * sizeof(short)));
//
//	//num_frames = sf_readf_short(sndfile, membuf, sfinfo.frames);
//	//if (num_frames < 1)
//	//{
//	//	free(membuf);
//	//	sf_close(sndfile);
//	//	fprintf(stderr, "Failed to read samples in %s (%" PRId64 ")\n", filename, num_frames);
//	//	return 0;
//	//}
//	//num_bytes = (ALsizei)(num_frames * sfinfo.channels) * (ALsizei)sizeof(short);
//
//	///* Buffer the audio data into a new buffer object, then free the data and
//	// * close the file.
//	// */
//	//buffer = 0;
//	//alGenBuffers(1, &buffer);
//	//alBufferData(buffer, format, membuf, num_bytes, sfinfo.samplerate);
//
//	//free(membuf);
//	//sf_close(sndfile);
//
//	///* Check if an error occured, and clean up if so. */
//	//err = alGetError();
//	//if (err != AL_NO_ERROR)
//	//{
//	//	fprintf(stderr, "OpenAL Error: %s\n", alGetString(err));
//	//	if (buffer && alIsBuffer(buffer))
//	//		alDeleteBuffers(1, &buffer);
//	//	return 0;
//	//}
//
//	//p_SoundEffectBuffers.push_back(buffer);  // add to the list of known buffers
//
//	//return buffer;
//	// 
//	// 
//	// 
//	// 
//	// 
//	// 
//	//ALuint buffer;
//	//ALenum format;
//	//ALsizei size;
//	//ALsizei freq;
//
//	//std::ifstream file;
//	//file.open(path, std::ios::binary);
//
//	//if (!file.is_open())
//	//{
//	//	std::cout << "Could not open file " << path << std::endl;
//	//	return 0;
//	//}
//
//	//char buf[4];
//
//	//// ChunkID (RIFF)
//	//file.read(buf, 4);
//	//if (strncmp(buf, "RIFF", 4) != 0)
//	//{
//	//	std::cout << "Invalid WAV file" << std::endl;
//	//	return 0;
//	//}
//
//	//// ChunkSize
//	//file.read(buf, 4);
//
//	//// Format (WAVE)
//	//file.read(buf, 4);
//	//if (strncmp(buf, "WAVE", 4) != 0)
//	//{
//	//	std::cout << "Invalid WAV file" << std::endl;
//	//	return 0;
//	//}
//
//	//// Subchunk1ID
//	//file.read(buf, 4);
//	//if (strncmp(buf, "fmt ", 4) != 0)
//	//{
//	//	std::cout << "Invalid WAV file" << std::endl;
//	//	return 0;
//	//}
//
//	//// Subchunk1Size
//	//file.read(buf, 4);
//	//if (buf[0] != (char)(16))
//	//{
//	//	std::cout << "Invalid WAV file" << std::endl;
//	//	return 9;
//	//}
//
//	//// AudioFormat
//	//file.read(buf, 2);
//	//if (buf[0] != (char)(1))
//	//{
//	//	std::cout << "Invalid WAV file" << std::endl;
//	//	return 0;
//	//}
//
//	//// NumChannels
//	//file.read(buf, 2);
//	//int no_channels = (int)((unsigned char)buf[0] | ((unsigned char)buf[1] << 8));
//
//	//// SampleRate
//	//file.read(buf, 4);
//	//int sample_rate = (int)((unsigned char)buf[0] | ((unsigned char)buf[1] << 8) | ((unsigned char)buf[2] << 16) | ((unsigned char)buf[3] << 24));
//
//	//// ByteRate
//	//file.read(buf, 4);
//
//	//// BlockAlign
//	//file.read(buf, 2);
//
//	//// BitsPerSample
//	//file.read(buf, 2);
//	//int bits_per_sample = (int)((unsigned char)buf[0] | ((unsigned char)buf[1] << 8));
//
//	//// Subchunk2ID
//	//file.read(buf, 4);
//	//if (strncmp(buf, "data", 4) != 0)
//	//{
//	//	std::cout << "Invalid WAV file" << std::endl;
//	//	return 0;
//	//}
//
//	//// Subchunk2Size
//	//file.read(buf, 4);
//	//int data_bytes = (int)((unsigned char)buf[0] | ((unsigned char)buf[1] << 8) | ((unsigned char)buf[2] << 16) | ((unsigned char)buf[3] << 24));
//
//	//audio_buffer.size = data_bytes;
//
//	//int format = AL_FORMAT_MONO8 + 2 * (no_channels - 1) + (bits_per_sample / 8) - 1;
//	//audio_buffer.format = format;
//
//	//audio_buffer.freq = sample_rate;
//
//	//// Data
//	//char* data = new char[data_bytes];
//	//file.read(data, data_bytes);
//
//	//file.close();
//
//	//return data;
//}

bool SoundBuffer::removeSoundEffect(const ALuint& buffer)
{
	auto it = p_SoundEffectBuffers.begin();
	while (it != p_SoundEffectBuffers.end())
	{
		if (*it == buffer)
		{
			alDeleteBuffers(1, &*it);

			it = p_SoundEffectBuffers.erase(it);

			return true;
		}
		else {
			++it;
		}
	}
	return false;  // couldn't find to remove
}


SoundBuffer::SoundBuffer()
{
	p_SoundEffectBuffers.clear();

}

SoundBuffer::~SoundBuffer()
{
	alDeleteBuffers(p_SoundEffectBuffers.size(), p_SoundEffectBuffers.data());

	p_SoundEffectBuffers.clear();
}
