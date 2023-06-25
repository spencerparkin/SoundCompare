#include "SoundSystem.h"
#include "Application.h"
#include "Frame.h"
#include <wx/msgdlg.h>

SoundSystem::SoundSystem()
{
	this->device = nullptr;
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
	if (this->device)
	{
		this->device->Release();
		this->device = nullptr;
	}

	return true;
}