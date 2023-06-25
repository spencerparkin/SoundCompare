#pragma once

#include "SoundSystem.h"
#include <wx/setup.h>
#include <wx/app.h>

class Frame;

class Application : public wxApp
{
public:
	Application();
	virtual ~Application();

	virtual bool OnInit(void) override;
	virtual int OnExit(void) override;

	Frame* GetFrame() { return this->frame; }
	SoundSystem* GetSoundSystem() { return &this->soundSystem; }

private:

	Frame* frame;
	SoundSystem soundSystem;
};

wxDECLARE_APP(Application);