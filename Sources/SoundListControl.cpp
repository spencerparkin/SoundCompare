#include "SoundListControl.h"
#include "Application.h"
#include <wx/utils.h>
#include <wx/menu.h>

//----------------------------------- SoundListControl -----------------------------------

SoundListControl::SoundListControl(wxWindow* parent) : wxListCtrl(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT)
{
	this->AppendColumn("Volume");
	this->AppendColumn("Description");
	this->AppendColumn("Status");
	this->AppendColumn("Duration");

	this->Bind(wxEVT_LIST_ITEM_RIGHT_CLICK, &SoundListControl::OnRightClickItem, this);
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

void SoundListControl::OnRightClickItem(wxListEvent& event)
{
	wxMenu contextMenu;

	contextMenu.Append(new wxMenuItem(&contextMenu, ID_PlaySound, "Play Sound", "Play the selected sound."));
	contextMenu.Append(new wxMenuItem(&contextMenu, ID_StopSound, "Stop Sound", "Stop the selected sound."));
	contextMenu.AppendSeparator();
	contextMenu.Append(new wxMenuItem(&contextMenu, ID_RemoveSound, "Remove Sound", "Remove the selected sound from the list."));

	contextMenu.Bind(wxEVT_MENU, &SoundListControl::OnPlaySound, this, ID_PlaySound);
	contextMenu.Bind(wxEVT_MENU, &SoundListControl::OnStopSound, this, ID_StopSound);
	contextMenu.Bind(wxEVT_MENU, &SoundListControl::OnRemoveSound, this, ID_RemoveSound);

	this->PopupMenu(&contextMenu);
}

SoundListControl::SoundEntry* SoundListControl::GetSelectedEntry()
{
	long selectedItem = this->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (selectedItem < 0)
		return nullptr;

	if (selectedItem >= (signed)this->soundArray.size())
		return nullptr;

	SoundEntry* entry = this->soundArray[selectedItem];
	return entry;
}

void SoundListControl::OnPlaySound(wxCommandEvent& event)
{
	SoundEntry* entry = this->GetSelectedEntry();
	if (entry)
	{
		SoundSystem* soundSystem = wxGetApp().GetSoundSystem();
		if (!soundSystem->IsSoundPlaying(entry->soundHandle))
			soundSystem->StartPlayingSound(entry->soundHandle);

		this->Refresh();
	}
}

void SoundListControl::OnStopSound(wxCommandEvent& event)
{
	SoundEntry* entry = this->GetSelectedEntry();
	if (entry)
	{
		SoundSystem* soundSystem = wxGetApp().GetSoundSystem();
		if (soundSystem->IsSoundPlaying(entry->soundHandle))
			soundSystem->StopPlayingSound(entry->soundHandle);

		this->Refresh();
	}
}

void SoundListControl::OnRemoveSound(wxCommandEvent& event)
{
	SoundEntry* entry = this->GetSelectedEntry();
	if (entry)
	{
		SoundSystem* soundSystem = wxGetApp().GetSoundSystem();
		
		for (int i = 0; i < (signed)this->soundArray.size(); i++)
		{
			if (this->soundArray[i] == entry)
			{
				delete entry;
				this->soundArray.erase(this->soundArray.begin() + i);
				this->SetItemCount(this->soundArray.size());
				break;
			}
		}
	}
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