#pragma once

#include <dsound.h>

class SoundSystem
{
public:
	SoundSystem();
	virtual ~SoundSystem();

	bool Initialize();
	bool Shutdown();

private:

	IDirectSound8* device;
};