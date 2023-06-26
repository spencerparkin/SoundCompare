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

	class SoundEntry
	{
	public:
		SoundEntry();
		virtual ~SoundEntry();

		int soundHandle;
		SoundSystem::SoundGenerator* soundGenerator;
	};

	std::vector<SoundEntry*> soundArray;
};