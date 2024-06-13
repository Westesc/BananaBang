#include "../lib/AudioManager.h"

AudioManager::AudioManager() {
	p_ALCDevice = alcOpenDevice(nullptr); // nullptr = get default device
	if (!p_ALCDevice)
		std::cout << "failed to get sound device" << std::endl;

	p_ALCContext = alcCreateContext(p_ALCDevice, nullptr);  // create context
	if (!p_ALCContext)
		std::cout << "Failed to set sound context" << std::endl;

	if (!alcMakeContextCurrent(p_ALCContext))   // make context current
		std::cout << "failed to make context current" << std::endl;

	const ALCchar* name = nullptr;
	if (alcIsExtensionPresent(p_ALCDevice, "ALC_ENUMERATE_ALL_EXT"))
		name = alcGetString(p_ALCDevice, ALC_ALL_DEVICES_SPECIFIER);
	if (!name || alcGetError(p_ALCDevice) != AL_NO_ERROR)
		name = alcGetString(p_ALCDevice, ALC_DEVICE_SPECIFIER);
	std::cout << "Opened " << name << std::endl;
}

AudioManager::~AudioManager() {
	for (auto& sound : sounds) {
		alDeleteSources(1, &sound.second.source);
		alDeleteBuffers(1, &sound.second.buffer);
	}

	alcMakeContextCurrent(nullptr);
	alcDestroyContext(p_ALCContext);
	alcCloseDevice(p_ALCDevice);
}
void AudioManager::loadSound(std::string name, std::string path) {
	ALuint buffer;
	if (!loadWAV(path, buffer)) {
		std::cout << "Failed to load WAV file" << std::endl;
	}

	ALuint source;

	alGenSources(1, &source);
	alSourcei(source, AL_BUFFER, buffer);

	sounds[name] = { buffer,source };
}


bool AudioManager::loadWAV(std::string& path, ALuint& buffer) {
    WAVHeader header;
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open WAV file: " << path << std::endl;
        return false;
    }

    file.read(reinterpret_cast<char*>(&header), sizeof(header));

    // Debugowanie odczytu nag��wka
    std::cout << "RIFF header: " << std::string(header.riff, 4) << std::endl;
    std::cout << "WAVE header: " << std::string(header.wave, 4) << std::endl;
    std::cout << "fmt header: " << std::string(header.fmt, 4) << std::endl;

    if (std::strncmp(header.riff, "RIFF", 4) != 0 || 
        std::strncmp(header.wave, "WAVE", 4) != 0 || 
        std::strncmp(header.fmt, "fmt ", 4) != 0) {
        std::cerr << "Invalid WAV file format: " << path << std::endl;
        return false;
    }

    // Przesuni�cie wska�nika pliku za fmt chunk
    file.seekg(header.subchunk1Size - 16, std::ios::cur);

    // Szukanie sekcji data
    DataHeader dataHeader;
    while (file.read(reinterpret_cast<char*>(&dataHeader), sizeof(dataHeader))) {
        if (std::strncmp(dataHeader.data, "data", 4) == 0) {
            break;
        }

        // Je�li nie "data", przesu� wska�nik pliku o rozmiar chunk + 8 (nag��wek chunk)
        file.seekg(dataHeader.dataSize, std::ios::cur);
    }

    if (std::strncmp(dataHeader.data, "data", 4) != 0) {
        std::cerr << "Failed to find data chunk in WAV file: " << path << std::endl;
        return false;
    }

    // Odczyt danych audio
    std::vector<char> data(dataHeader.dataSize);
    file.read(data.data(), dataHeader.dataSize);

    // Ustalenie formatu d�wi�ku w OpenAL
    ALenum format;
    if (header.bitsPerSample == 8) {
        format = (header.numChannels == 1) ? AL_FORMAT_MONO8 : AL_FORMAT_STEREO8;
    } else if (header.bitsPerSample == 16) {
        format = (header.numChannels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
    } else {
        std::cerr << "Unsupported bit depth: " << header.bitsPerSample << " bits" << std::endl;
        return false;
    }

    // Generowanie bufora i �adowanie danych do OpenAL
    alGenBuffers(1, &buffer);
    alBufferData(buffer, format, data.data(), dataHeader.dataSize, header.sampleRate);

    return true;
}

void AudioManager::playSound(std::string name) {
    auto it = sounds.find(name);
    if (it != sounds.end()) {
        alSourcePlay(it->second.source);
    }
    else {
        std::cerr << "Sound " << name << " not found!" << std::endl;
    }
}