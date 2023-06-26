#pragma once

#include <dsound.h>
#include <map>
#include <vector>

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
		SoundGenerator(double volume);
		virtual ~SoundGenerator();

		virtual void GenerateSound(unsigned char* buffer, DWORD bufferSize);
		virtual double CalcMaxAmplitude(DWORD bufferSize);
		virtual double EvaluateWaveForm(double timeSeconds);

		double ByteOffsetToTime(int byteOffset);

		double volume;
		const Sound* sound;
	};

	class ToneGenerator : public SoundGenerator
	{
	public:
		ToneGenerator(double frequency, double volume);
		virtual ~ToneGenerator();

		virtual double CalcMaxAmplitude(DWORD bufferSize) override;
		virtual double EvaluateWaveForm(double timeSeconds) override;

	private:
		double frequency;
	};

	class WhiteNoiseGenerator : public SoundGenerator
	{
	public:
		WhiteNoiseGenerator(double volume);
		virtual ~WhiteNoiseGenerator();

		virtual double CalcMaxAmplitude(DWORD bufferSize) override;
		virtual double EvaluateWaveForm(double timeSeconds) override;
	};

	class MultiToneGenerator : public SoundGenerator
	{
	public:
		MultiToneGenerator(double volume);
		virtual ~MultiToneGenerator();

		virtual void GenerateSound(unsigned char* buffer, DWORD bufferSize) override;
		virtual double EvaluateWaveForm(double timeSeconds) override;

	protected:
		virtual void GenerateToneParametersArray();

		struct ToneParameters
		{
			double amplitude;
			double frequency;
			double phaseShift;
		};

		std::vector<ToneParameters> toneParametersArray;
	};

	class PinkishNoiseGenerator : public MultiToneGenerator
	{
	public:
		PinkishNoiseGenerator(double volume);
		virtual ~PinkishNoiseGenerator();

		virtual void GenerateToneParametersArray() override;
	};

	class BlueishNoiseGenerator : public MultiToneGenerator
	{
	public:
		BlueishNoiseGenerator(double volume);
		virtual ~BlueishNoiseGenerator();

		virtual void GenerateToneParametersArray() override;
	};

	bool CreateSound(int durationSeconds, int& soundHandle);
	bool ComposeSound(int soundHandle, SoundGenerator* soundGenerator);
	bool StartPlayingSound(int soundHandle);
	bool StopPlayingSound(int soundHandle);
	bool DeleteSound(int soundHandle);

	static double RandomNumber(double min, double max);
	static double Clamp(double value, double minValue, double maxValue);

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