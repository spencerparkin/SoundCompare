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

		virtual void GenerateSound(unsigned char* buffer, DWORD bufferSize);
		virtual float EvaluateWaveForm(float timeSeconds);

		float ByteOffsetToTime(int byteOffset);

		float volume;
		const Sound* sound;
	};

	class ToneGenerator : public SoundGenerator
	{
	public:
		ToneGenerator(float frequency, float volume);
		virtual ~ToneGenerator();

		virtual float EvaluateWaveForm(float timeSeconds) override;

	private:
		float frequency;
	};

	class WhiteNoiseGenerator : public SoundGenerator
	{
	public:
		WhiteNoiseGenerator(float volume);
		virtual ~WhiteNoiseGenerator();

		virtual float EvaluateWaveForm(float timeSeconds) override;
	};

	bool CreateSound(int durationSeconds, int& soundHandle);
	bool ComposeSound(int soundHandle, SoundGenerator* soundGenerator);
	bool StartPlayingSound(int soundHandle);
	bool StopPlayingSound(int soundHandle);
	bool DeleteSound(int soundHandle);

	static float RandomNumber(float min, float max);
	static float Clamp(float value, float minValue, float maxValue);

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