#include "SoundSystem.h"
#include "Application.h"
#include "Frame.h"
#include <wx/msgdlg.h>
#include <random>

//----------------------------------- SoundSystem -----------------------------------

SoundSystem::SoundSystem()
{
	this->device = nullptr;
	this->primaryBuffer = nullptr;
}

/*virtual*/ SoundSystem::~SoundSystem()
{
}

bool SoundSystem::Initialize()
{
	bool success = false;
	HRESULT result = 0;
	wxString error;

	do
	{
		result = ::DirectSoundCreate8(&DSDEVID_DefaultPlayback, &this->device, nullptr);
		if (result != DS_OK || !this->device)
		{
			error = "Failed to create default sound device.";
			break;
		}

		result = this->device->SetCooperativeLevel(wxGetApp().GetFrame()->GetHWND(), DSSCL_PRIORITY);
		if (result != DS_OK)
		{
			error = "Failed to set cooperation level.";
			break;
		}

		DSBUFFERDESC bufferDesc;
		memset(&bufferDesc, 0, sizeof(bufferDesc));
		bufferDesc.dwSize = sizeof(bufferDesc);
		bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER;
		bufferDesc.dwBufferBytes = 0;
		bufferDesc.lpwfxFormat = nullptr;

		result = this->device->CreateSoundBuffer(&bufferDesc, &this->primaryBuffer, nullptr);
		if (result != DS_OK || !this->primaryBuffer)
		{
			error = "Failed to get a hold of the primary sound buffer.";
			break;
		}

		success = true;
	} while (false);

	if (!success)
	{
		if (error.Length() > 0)
			wxMessageBox(error, "Error", wxICON_ERROR | wxOK, wxGetApp().GetFrame());

		this->Shutdown();
	}

	return success;
}

bool SoundSystem::Shutdown()
{
	std::vector<int> soundHandleArray;
	for (SoundMap::iterator iter = this->soundMap.begin(); iter != this->soundMap.end(); iter++)
		soundHandleArray.push_back(iter->first);

	for (int soundHandle : soundHandleArray)
		this->DeleteSound(soundHandle);

	if (this->primaryBuffer)
	{
		this->primaryBuffer->Release();
		this->primaryBuffer = nullptr;
	}

	if (this->device)
	{
		this->device->Release();
		this->device = nullptr;
	}

	return true;
}

bool SoundSystem::CreateSound(int durationSeconds, int& soundHandle)
{
	if (!this->device)
		return false;

	soundHandle = 0;
	for (SoundMap::iterator iter = this->soundMap.begin(); iter != this->soundMap.end(); iter++)
		if (soundHandle <= iter->first)
			soundHandle = iter->first + 1;

	Sound* sound = new Sound();

	memset(&sound->waveFormat, 0, sizeof(sound->waveFormat));
	sound->waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	sound->waveFormat.nChannels = 1;
	sound->waveFormat.nSamplesPerSec = 22010;
	sound->waveFormat.wBitsPerSample = 8;
	sound->waveFormat.nBlockAlign = (sound->waveFormat.wBitsPerSample / 8) * sound->waveFormat.nChannels;
	sound->waveFormat.nAvgBytesPerSec = sound->waveFormat.nSamplesPerSec * sound->waveFormat.nBlockAlign;

	memset(&sound->bufferDesc, 0, sizeof(sound->bufferDesc));
	sound->bufferDesc.dwSize = sizeof(sound->bufferDesc);
	sound->bufferDesc.dwFlags = 0;
	sound->bufferDesc.dwBufferBytes = sound->waveFormat.nSamplesPerSec * (sound->waveFormat.wBitsPerSample / 8) * sound->waveFormat.nChannels * durationSeconds;
	sound->bufferDesc.guid3DAlgorithm = GUID_NULL;
	sound->bufferDesc.lpwfxFormat = &sound->waveFormat;

	HRESULT result = this->device->CreateSoundBuffer(&sound->bufferDesc, &sound->secondaryBuffer, nullptr);
	if (result != DS_OK)
	{
		delete sound;
		return false;
	}

	this->soundMap.insert(std::pair<int, Sound*>(soundHandle, sound));
	return true;
}

bool SoundSystem::ComposeSound(int soundHandle, SoundGenerator* soundGenerator)
{
	Sound* sound = this->FindSound(soundHandle);
	if (!sound)
		return false;

	unsigned char* buffer = nullptr;
	DWORD bufferSize = 0;

	HRESULT result = sound->secondaryBuffer->Lock(0, sound->bufferDesc.dwBufferBytes, (void**)&buffer, &bufferSize, nullptr, nullptr, 0L);
	if (result != DS_OK)
		return false;

	soundGenerator->sound = sound;
	soundGenerator->GenerateSound(buffer, bufferSize);

	sound->secondaryBuffer->Unlock(buffer, bufferSize, nullptr, 0);
	return true;
}

bool SoundSystem::StartPlayingSound(int soundHandle)
{
	Sound* sound = this->FindSound(soundHandle);
	if (!sound)
		return false;

	if (!sound->secondaryBuffer)
		return false;

	HRESULT result = sound->secondaryBuffer->Play(0, 0, DSBPLAY_LOOPING);
	if (result != DS_OK)
		return false;

	return true;
}

bool SoundSystem::StopPlayingSound(int soundHandle)
{
	Sound* sound = this->FindSound(soundHandle);
	if (!sound)
		return false;

	if (!sound->secondaryBuffer)
		return false;

	HRESULT result = sound->secondaryBuffer->Stop();
	if (result != DS_OK)
		return false;

	return true;
}

bool SoundSystem::DeleteSound(int soundHandle)
{
	Sound* sound = this->FindSound(soundHandle);
	if (!sound || !sound->secondaryBuffer)
		return false;

	sound->secondaryBuffer->Release();
	delete sound;
	this->soundMap.erase(soundHandle);
	return true;
}

/*static*/ double SoundSystem::RandomNumber(double min, double max)
{
	double alpha = double(::rand()) / double(RAND_MAX);
	double number = min + (max - min) * alpha;
	return Clamp(number, min, max);
}

/*static*/ double SoundSystem::Clamp(double value, double minValue, double maxValue)
{
	if (value < minValue)
		value = minValue;

	if (value > maxValue)
		value = maxValue;

	return value;
}

SoundSystem::Sound* SoundSystem::FindSound(int soundHandle)
{
	SoundMap::iterator iter = this->soundMap.find(soundHandle);
	if (iter == this->soundMap.end())
		return nullptr;

	return iter->second;
}

//----------------------------------- SoundSystem::Sound -----------------------------------

SoundSystem::Sound::Sound()
{
	this->secondaryBuffer = nullptr;
	::memset(&this->waveFormat, 0, sizeof(this->waveFormat));
	::memset(&this->bufferDesc, 0, sizeof(this->bufferDesc));
}

/*virtual*/ SoundSystem::Sound::~Sound()
{
}

//----------------------------------- SoundSystem::SoundGenerator -----------------------------------

SoundSystem::SoundGenerator::SoundGenerator(double volume)
{
	this->volume = volume;
	this->sound = nullptr;
}

/*virtual*/ SoundSystem::SoundGenerator::~SoundGenerator()
{
}

/*virtual*/ double SoundSystem::SoundGenerator::CalcMaxAmplitude(DWORD bufferSize)
{
	double maxAmplitude = 0.0;

	for (int i = 0; i < (signed)bufferSize; i++)
	{
		double amplitude = ::fabs(this->EvaluateWaveForm(this->ByteOffsetToTime(i)));
		if (amplitude > maxAmplitude)
			maxAmplitude = amplitude;
	}

	return maxAmplitude;
}

/*virtual*/ void SoundSystem::SoundGenerator::GenerateSound(unsigned char* buffer, DWORD bufferSize)
{
	double maxAmplitude = this->CalcMaxAmplitude(bufferSize);

	for (int i = 0; i < (signed)bufferSize; i++)
	{
		double value = this->EvaluateWaveForm(this->ByteOffsetToTime(i)) * (this->volume / maxAmplitude);
		buffer[i] = (unsigned char)(127.0 + value * 127.0);
	}
}

/*virtual*/ double SoundSystem::SoundGenerator::EvaluateWaveForm(double timeSeconds)
{
	return 0.0f;
}

double SoundSystem::SoundGenerator::ByteOffsetToTime(int byteOffset)
{
	double sampleRate = double(this->sound->waveFormat.nSamplesPerSec);
	return double(byteOffset) / sampleRate;
}

//----------------------------------- SoundSystem::ToneGenerator -----------------------------------

SoundSystem::ToneGenerator::ToneGenerator(double frequency, double volume) : SoundGenerator(volume)
{
	this->frequency = frequency;
}

/*virtual*/ SoundSystem::ToneGenerator::~ToneGenerator()
{
}

/*virtual*/ double SoundSystem::ToneGenerator::CalcMaxAmplitude(DWORD bufferSize)
{
	return 1.0;
}

/*virtual*/ double SoundSystem::ToneGenerator::EvaluateWaveForm(double timeSeconds)
{
	return sin(2.0 * M_PI * this->frequency * timeSeconds);
}

//----------------------------------- SoundSystem::WhiteNoiseGenerator -----------------------------------

SoundSystem::WhiteNoiseGenerator::WhiteNoiseGenerator(double volume) : SoundGenerator(volume)
{
}

/*virtual*/ SoundSystem::WhiteNoiseGenerator::~WhiteNoiseGenerator()
{
}

/*virtual*/ double SoundSystem::WhiteNoiseGenerator::CalcMaxAmplitude(DWORD bufferSize)
{
	return 1.0;
}

/*virtual*/ double SoundSystem::WhiteNoiseGenerator::EvaluateWaveForm(double timeSeconds)
{
	return SoundSystem::RandomNumber(-1.0, 1.0);
}

//----------------------------------- SoundSystem::MultiToneGenerator -----------------------------------

SoundSystem::MultiToneGenerator::MultiToneGenerator(double volume) : SoundGenerator(volume)
{
}

/*virtual*/ SoundSystem::MultiToneGenerator::~MultiToneGenerator()
{
}

/*virtual*/ void SoundSystem::MultiToneGenerator::GenerateSound(unsigned char* buffer, DWORD bufferSize)
{
	this->GenerateToneParametersArray();

	SoundGenerator::GenerateSound(buffer, bufferSize);
}

/*virtual*/ double SoundSystem::MultiToneGenerator::EvaluateWaveForm(double timeSeconds)
{
	double totalValue = 0.0;

	for (const ToneParameters& toneParameters : this->toneParametersArray)
	{
		double toneValue = toneParameters.amplitude * sin(2.0 * M_PI * toneParameters.frequency * timeSeconds + toneParameters.phaseShift);
		totalValue += toneValue;
	}

	return totalValue;
}

/*virtual*/ void SoundSystem::MultiToneGenerator::GenerateToneParametersArray()
{
}

//----------------------------------- SoundSystem::PinkishNoiseGenerator -----------------------------------

SoundSystem::PinkishNoiseGenerator::PinkishNoiseGenerator(double volume) : MultiToneGenerator(volume)
{
}

/*virtual*/ SoundSystem::PinkishNoiseGenerator::~PinkishNoiseGenerator()
{
}

/*virtual*/ void SoundSystem::PinkishNoiseGenerator::GenerateToneParametersArray()
{
	this->toneParametersArray.clear();

	for (int i = 0; i < 100; i++)
	{
		ToneParameters toneParameters;
		toneParameters.frequency = SoundSystem::RandomNumber(250.0, 5000.0);
		toneParameters.amplitude = 1.0 / toneParameters.frequency;
		toneParameters.phaseShift = SoundSystem::RandomNumber(0.0, 2.0 * M_PI);

		this->toneParametersArray.push_back(toneParameters);
	}
}

//----------------------------------- SoundSystem::BlueishNoiseGenerator -----------------------------------

SoundSystem::BlueishNoiseGenerator::BlueishNoiseGenerator(double volume) : MultiToneGenerator(volume)
{
}

/*virtual*/ SoundSystem::BlueishNoiseGenerator::~BlueishNoiseGenerator()
{
}

/*virtual*/ void SoundSystem::BlueishNoiseGenerator::GenerateToneParametersArray()
{
	this->toneParametersArray.clear();

	for (int i = 0; i < 500; i++)
	{
		ToneParameters toneParameters;
		toneParameters.frequency = SoundSystem::RandomNumber(250.0, 5000.0);
		toneParameters.amplitude = toneParameters.frequency / 5000.0;
		toneParameters.phaseShift = SoundSystem::RandomNumber(0.0, 2.0 * M_PI);

		this->toneParametersArray.push_back(toneParameters);
	}
}