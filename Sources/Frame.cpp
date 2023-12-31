#include "Frame.h"
#include "SoundListControl.h"
#include <wx/aboutdlg.h>
#include <wx/menu.h>
#include <wx/sizer.h>
#include <wx/choicdlg.h>
#include <wx/numdlg.h>
#include <wx/toolbar.h>

Frame::Frame(wxWindow* parent, const wxPoint& pos, const wxSize& size) : wxFrame(parent, wxID_ANY, "Sound Compare", pos, size)
{
	wxBitmap addBitmap, removeAllBitmap;
	wxBitmap playBitmap, stopBitmap;

	addBitmap.LoadFile(wxGetCwd() + "/Textures/Add.png", wxBITMAP_TYPE_PNG);
	removeAllBitmap.LoadFile(wxGetCwd() + "/Textures/RemoveAll.png", wxBITMAP_TYPE_PNG);
	playBitmap.LoadFile(wxGetCwd() + "/Textures/Play.png", wxBITMAP_TYPE_PNG);
	stopBitmap.LoadFile(wxGetCwd() + "/Textures/Stop.png", wxBITMAP_TYPE_PNG);

	wxMenu* programMenu = new wxMenu();
	programMenu->Append(new wxMenuItem(programMenu, ID_AddSound, "Add Sound", "Add a sound to the list."));
	programMenu->Append(new wxMenuItem(programMenu, ID_RemoveAllSounds, "Remove All Sounds", "Remove all sounds that have been added to the list."));
	programMenu->AppendSeparator();
	programMenu->Append(new wxMenuItem(programMenu, ID_PlayAllSounds, "Play All Sounds", "Begin playing all sounds in the list."));
	programMenu->Append(new wxMenuItem(programMenu, ID_StopAllSounds, "Stop All Sounds", "Stop all currently playing sounds in the list."));
	programMenu->AppendSeparator();
	programMenu->Append(new wxMenuItem(programMenu, ID_Exit, "Exit", "Terminate this program."));

	wxMenu* helpMenu = new wxMenu();
	helpMenu->Append(new wxMenuItem(helpMenu, ID_About, "About", "Show the about-box."));

	wxMenuBar* menuBar = new wxMenuBar();
	menuBar->Append(programMenu, "Program");
	menuBar->Append(helpMenu, "Help");
	this->SetMenuBar(menuBar);

	programMenu->FindItem(ID_AddSound)->SetBitmap(addBitmap);
	programMenu->FindItem(ID_RemoveAllSounds)->SetBitmap(removeAllBitmap);
	programMenu->FindItem(ID_PlayAllSounds)->SetBitmap(playBitmap);
	programMenu->FindItem(ID_StopAllSounds)->SetBitmap(stopBitmap);

	this->SetStatusBar(new wxStatusBar(this));

	this->Bind(wxEVT_MENU, &Frame::OnAddSound, this, ID_AddSound);
	this->Bind(wxEVT_MENU, &Frame::OnRemoveAllSounds, this, ID_RemoveAllSounds);
	this->Bind(wxEVT_MENU, &Frame::OnPlayAllSounds, this, ID_PlayAllSounds);
	this->Bind(wxEVT_MENU, &Frame::OnStopAllSounds, this, ID_StopAllSounds);
	this->Bind(wxEVT_MENU, &Frame::OnAbout, this, ID_About);
	this->Bind(wxEVT_MENU, &Frame::OnExit, this, ID_Exit);

	wxToolBar* toolBar = this->CreateToolBar();

	toolBar->AddTool(ID_AddSound, "Add sound", addBitmap, "Add a sound to the list.", wxITEM_NORMAL);
	toolBar->AddTool(ID_RemoveAllSounds, "Remove all sounds", removeAllBitmap, "Remove all sounds from the list.", wxITEM_NORMAL);
	toolBar->AddSeparator();
	toolBar->AddTool(ID_PlayAllSounds, "Play all sounds", playBitmap, "Play all the sounds in the list.", wxITEM_NORMAL);
	toolBar->AddTool(ID_StopAllSounds, "Stop all sounds", stopBitmap, "Stop all sounds in the list from playing.", wxITEM_NORMAL);

	toolBar->Realize();

	this->soundListControl = new SoundListControl(this);

	wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
	boxSizer->Add(this->soundListControl, 1, wxGROW);
	this->SetSizer(boxSizer);
}

/*virtual*/ Frame::~Frame()
{
}

void Frame::OnAddSound(wxCommandEvent& event)
{
	wxNumberEntryDialog volumeDialog(this, "What volume?  (0 -- 100)", "Volume:", "Pick Volume", 0, 0, 100);
	if (wxID_OK != volumeDialog.ShowModal())
		return;

	double volume = double(volumeDialog.GetValue()) / 100.0;

	wxArrayString soundTypeArray;
	soundTypeArray.Add("Tone");
	soundTypeArray.Add("White noise");
	soundTypeArray.Add("Pinkish noise");
	soundTypeArray.Add("Blueish noise");
	wxSingleChoiceDialog soundTypeDialog(this, "What kind of sound?", "Pick Sound Type", soundTypeArray);
	if (wxID_OK != soundTypeDialog.ShowModal())
		return;

	SoundSystem::SoundGenerator* soundGenerator = nullptr;

	wxString soundType = soundTypeArray[soundTypeDialog.GetSelection()];
	if (soundType == "Tone")
	{
		wxNumberEntryDialog frequencyDialog(this, "What frequency? (250 -- 8000)", "Frequency:", "Pick Frequency", 250, 250, 8000);
		if (wxID_OK != frequencyDialog.ShowModal())
			return;

		soundGenerator = new SoundSystem::ToneGenerator(double(frequencyDialog.GetValue()), volume);
	}
	else if (soundType == "White noise")
		soundGenerator = new SoundSystem::WhiteNoiseGenerator(volume);
	else if (soundType == "Pinkish noise")
		soundGenerator = new SoundSystem::PinkishNoiseGenerator(volume);
	else if (soundType == "Blueish noise")
		soundGenerator = new SoundSystem::BlueishNoiseGenerator(volume);

	if (soundGenerator)
	{
		this->soundListControl->AddSound(soundGenerator);
		this->soundListControl->SetColumnWidth(1, wxLIST_AUTOSIZE);
	}
}

void Frame::OnRemoveAllSounds(wxCommandEvent& event)
{
	this->soundListControl->RemoveAllSounds();
}

void Frame::OnPlayAllSounds(wxCommandEvent& event)
{
	this->soundListControl->PlayAllSounds();
	this->soundListControl->Refresh();
}

void Frame::OnStopAllSounds(wxCommandEvent& event)
{
	this->soundListControl->StopAllSounds();
	this->soundListControl->Refresh();
}

void Frame::OnAbout(wxCommandEvent& event)
{
	wxAboutDialogInfo aboutDialogInfo;

	aboutDialogInfo.SetName("Sound Compare");
	aboutDialogInfo.SetVersion("1.0");
	aboutDialogInfo.SetDescription("This programs lets you procedurally synthesize basic sounds and play them for comparison.\n"
									"The goal was to illustrate which sounds are better or worse for tinnitus.");
	aboutDialogInfo.SetCopyright("Copyright (C) 2023 -- Spencer T. Parkin <SpencerTParkin@gmail.com>");

	wxAboutBox(aboutDialogInfo);
}

void Frame::OnExit(wxCommandEvent& event)
{
	this->Close(true);
}