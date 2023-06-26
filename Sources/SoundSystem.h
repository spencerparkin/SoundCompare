#pragma once

#include <dsound.h>
#include <map>

class SoundSystem
{
private:

	class Sound;

public:
	SoundSystem();
	virtual ~SoundSystem();

	bool Initialize();
	bool Shutdown();

	class SoundGenerator
	{
	public:
		SoundGenerator(float volume);
		virtual ~SoundGenerator();

		virtual void GenerateSound(const Sound* sound, unsigned char* buffer, DWORD bufferSize) = 0;

		float volume;
	};

	class ToneGenerator : public SoundGenerator
	{
	public:
		ToneGenerator(float frequency, float volume);
		virtual ~ToneGenerator();

		virtual void GenerateSound(const Sound* sound, unsigned char* buffer, DWORD bufferSize) override;

	private:
		float frequency;
	};

	class ColorNoiseGenerator : public SoundGenerator
	{
	public:
		ColorNoiseGenerator(float volume);
		virtual ~ColorNoiseGenerator();

		virtual void GenerateSound(const Sound* sound, unsigned char* buffer, DWORD bufferSize) override;
	};

	bool CreateSound(int durationSeconds, int& soundHandle);
	bool ComposeSound(int soundHandle, SoundGenerator* soundGenerator);
	bool StartPlayingSound(int soundHandle);
	bool StopPlayingSound(int soundHandle);
	bool DeleteSound(int soundHandle);

	static float RandomNumber(float min, float max);

private:

	class Sound
	{
	public:
		Sound();
		virtual ~Sound();

		LPDIRECTSOUNDBUFFER secondaryBuffer;
		WAVEFORMATEX waveFormat;
		DSBUFFERDESC bufferDesc;
	};

	Sound* FindSound(int soundHandle);

	IDirectSound8* device;
	LPDIRECTSOUNDBUFFER primaryBuffer;
	typedef std::map<int, Sound*> SoundMap;
	SoundMap soundMap;
};