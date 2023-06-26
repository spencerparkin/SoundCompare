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

/*static*/ float SoundSystem::RandomNumber(float min, float max)
{
	float alpha = float(::rand()) / float(RAND_MAX);
	float number = min + (max - min) * alpha;
	return Clamp(number, min, max);
}

/*static*/ float SoundSystem::Clamp(float value, float minValue, float maxValue)
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

SoundSystem::SoundGenerator::SoundGenerator(float volume)
{
	this->volume = volume;
	this->sound = nullptr;
}

/*virtual*/ SoundSystem::SoundGenerator::~SoundGenerator()
{
}

/*virtual*/ void SoundSystem::SoundGenerator::GenerateSound(unsigned char* buffer, DWORD bufferSize)
{
	for (int i = 0; i < (signed)bufferSize; i++)
	{
		float amplitude = this->EvaluateWaveForm(this->ByteOffsetToTime(i)) * this->volume;
		amplitude = SoundSystem::Clamp(amplitude, -1.0f, 1.0f);
		buffer[i] = (unsigned char)(127.0f + amplitude * 127.0f);
	}
}

/*virtual*/ float SoundSystem::SoundGenerator::EvaluateWaveForm(float timeSeconds)
{
	return 0.0f;
}

float SoundSystem::SoundGenerator::ByteOffsetToTime(int byteOffset)
{
	float sampleRate = float(this->sound->waveFormat.nSamplesPerSec);
	return float(byteOffset) / sampleRate;
}

//----------------------------------- SoundSystem::ToneGenerator -----------------------------------

SoundSystem::ToneGenerator::ToneGenerator(float frequency, float volume) : SoundGenerator(volume)
{
	this->frequency = frequency;
}

/*virtual*/ SoundSystem::ToneGenerator::~ToneGenerator()
{
}

/*virtual*/ float SoundSystem::ToneGenerator::EvaluateWaveForm(float timeSeconds)
{
	return sin(2.0f * M_PI * this->frequency * timeSeconds);
}

//----------------------------------- SoundSystem::WhiteNoiseGenerator -----------------------------------

SoundSystem::WhiteNoiseGenerator::WhiteNoiseGenerator(float volume) : SoundGenerator(volume)
{
}

/*virtual*/ SoundSystem::WhiteNoiseGenerator::~WhiteNoiseGenerator()
{
}

/*virtual*/ float SoundSystem::WhiteNoiseGenerator::EvaluateWaveForm(float timeSeconds)
{
	return SoundSystem::RandomNumber(-1.0f, 1.0f);
}