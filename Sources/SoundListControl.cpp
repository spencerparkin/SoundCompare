#include "SoundListControl.h"
#include "Application.h"
#include <wx/utils.h>

//----------------------------------- SoundListControl -----------------------------------

SoundListControl::SoundListControl(wxWindow* parent) : wxListCtrl(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT)
{
	this->AppendColumn("Volume");
	this->AppendColumn("Description");
	this->AppendColumn("Status");
	this->AppendColumn("Duration");
}

/*virtual*/ SoundListControl::~SoundListControl()
{
	this->RemoveAllSounds();
}

/*virtual*/ wxString SoundListControl::OnGetItemText(long item, long column) const
{
	if (0 <= item && item < (signed)this->soundArray.size())
	{
		SoundEntry* entry = this->soundArray[item];
		
		switch (column)
		{
			case 0:
			{
				return wxString::Format("%f", entry->soundGenerator->volume);
			}
			case 1:
			{
				return entry->soundGenerator->GetDescription();
			}
			case 2:
			{
				return wxGetApp().GetSoundSystem()->IsSoundPlaying(entry->soundHandle) ? "Playing" : "Stopped";
			}
			case 3:
			{
				double durationSeconds = 0.0;
				wxGetApp().GetSoundSystem()->GetSoundDuration(entry->soundHandle, durationSeconds);
				return wxString::Format("%f", durationSeconds);
			}
		}
	}

	return "?";
}

void SoundListControl::AddSound(SoundSystem::SoundGenerator* soundGenerator)
{
	wxBusyCursor busyCursor;

	SoundEntry* entry = new SoundEntry();
	entry->soundGenerator = soundGenerator;
	wxGetApp().GetSoundSystem()->CreateSound(4, entry->soundHandle);
	wxGetApp().GetSoundSystem()->ComposeSound(entry->soundHandle, soundGenerator);

	this->soundArray.push_back(entry);

	this->SetItemCount(this->soundArray.size());
}

void SoundListControl::RemoveAllSounds()
{
	for (SoundEntry* entry : this->soundArray)
		delete entry;

	this->soundArray.clear();
	this->SetItemCount(0);
}

void SoundListControl::PlayAllSounds()
{
	SoundSystem* soundSystem = wxGetApp().GetSoundSystem();

	for (SoundEntry* entry : this->soundArray)
	{
		if (!soundSystem->IsSoundPlaying(entry->soundHandle))
		{
			soundSystem->StartPlayingSound(entry->soundHandle);
		}
	}
}

void SoundListControl::StopAllSounds()
{
	SoundSystem* soundSystem = wxGetApp().GetSoundSystem();

	for (SoundEntry* entry : this->soundArray)
	{
		if (soundSystem->IsSoundPlaying(entry->soundHandle))
		{
			soundSystem->StopPlayingSound(entry->soundHandle);
		}
	}
}

//----------------------------------- SoundListControl::Sound -----------------------------------

SoundListControl::SoundEntry::SoundEntry()
{
	this->soundHandle = -1;
	this->soundGenerator = nullptr;
}

/*virtual*/ SoundListControl::SoundEntry::~SoundEntry()
{
	delete this->soundGenerator;
	wxGetApp().GetSoundSystem()->DeleteSound(this->soundHandle);
}