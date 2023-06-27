#pragma once

#include <wx/listctrl.h>
#include "SoundSystem.h"
#include <vector>

class SoundListControl : public wxListCtrl
{
public:
	SoundListControl(wxWindow* parent);
	virtual ~SoundListControl();

	virtual wxString OnGetItemText(long item, long column) const override;

	void AddSound(SoundSystem::SoundGenerator* soundGenerator);
	void RemoveAllSounds();
	void PlayAllSounds();
	void StopAllSounds();

private:

	enum
	{
		ID_PlaySound = wxID_HIGHEST + 500,
		ID_StopSound,
		ID_RemoveSound
	};

	void OnRightClickItem(wxListEvent& event);
	void OnPlaySound(wxCommandEvent& event);
	void OnStopSound(wxCommandEvent& event);
	void OnRemoveSound(wxCommandEvent& event);

	class SoundEntry
	{
	public:
		SoundEntry();
		virtual ~SoundEntry();

		int soundHandle;
		SoundSystem::SoundGenerator* soundGenerator;
	};

	SoundEntry* GetSelectedEntry();
	void SetSelectedEntry();

	std::vector<SoundEntry*> soundArray;
};