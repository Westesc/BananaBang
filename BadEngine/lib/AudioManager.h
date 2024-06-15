#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <al/al.h>
#include <al/alc.h>
#include <map>
#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class AudioManager {
private:
    struct SoundData {
        ALuint buffer;
        ALuint source;
    };


    struct WAVHeader {
        char riff[4];            // "RIFF"
        uint32_t chunkSize;      // File size - 8 bytes
        char wave[4];            // "WAVE"
        char fmt[4];             // "fmt "
        uint32_t subchunk1Size;  // Size of the fmt chunk
        uint16_t audioFormat;    // Audio format, 1 for PCM
        uint16_t numChannels;    // Number of channels
        uint32_t sampleRate;     // Sampling frequency
        uint32_t byteRate;       // Byte rate = SampleRate * NumChannels * BitsPerSample / 8
        uint16_t blockAlign;     // Block align = NumChannels * BitsPerSample / 8
        uint16_t bitsPerSample;  // Bits per sample
    };
    struct AudioBuffer
    {
        ALuint buffer;
        ALenum format;
        ALsizei size;
        ALsizei freq;
    };
    struct DataHeader {
        char data[4];            // "data"
        uint32_t dataSize;       // Size of the data section
    };

    std::map<std::string, SoundData> sounds;
    ALCdevice* p_ALCDevice;
    ALCcontext* p_ALCContext;
    bool loadWAV(std::string& path, ALuint& buffer,bool is3D);

public:
    AudioManager();
    ~AudioManager();

    void loadSound(std::string name, std::string path, bool is3D=false);
    void playSound(std::string name, bool loop = true);
    void stopSound(std::string name);
    void changeVolume(std::string name, float volume);
    void setSoundPosition(std::string, float x, float y, float z);
    void setListenerPosition(float x, float y, float z);
    void setListenerOrientation(glm::mat4 viewMatrix);
    void setRollofFactor(std::string name, float value);


};


#endif