#pragma once

#include <wx/frame.h>

class SoundListControl;

class Frame : public wxFrame
{
public:
	Frame(wxWindow* parent, const wxPoint& pos, const wxSize& size);
	virtual ~Frame();

protected:

	void OnAddSound(wxCommandEvent& event);
	void OnRemoveAllSounds(wxCommandEvent& event);
	void OnPlayAllSounds(wxCommandEvent& event);
	void OnStopAllSounds(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnExit(wxCommandEvent& event);

	enum
	{
		ID_AddSound = wxID_HIGHEST,
		ID_RemoveAllSounds,
		ID_PlayAllSounds,
		ID_StopAllSounds,
		ID_About,
		ID_Exit
	};

	SoundListControl* soundListControl;
};