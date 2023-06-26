#include "Application.h"
#include "Frame.h"

wxIMPLEMENT_APP(Application);

Application::Application()
{
	this->frame = nullptr;
}

/*virtual*/ Application::~Application()
{
}

/*virtual*/ bool Application::OnInit(void)
{
	if (!wxApp::OnInit())
		return false;

	this->frame = new Frame(nullptr, wxDefaultPosition, wxSize(800, 800));
	this->frame->Show();

	if (!this->soundSystem.Initialize())
		return false;

	//SoundSystem::ToneGenerator toneGenerator(4400.0f, 0.1f);
	//SoundSystem::WhiteNoiseGenerator whiteNoiseGenerator(0.1f);

	//int soundHandleA = 0;
	//this->soundSystem.CreateSound(10, soundHandleA);
	//this->soundSystem.ComposeSound(soundHandleA, &toneGenerator);
	//this->soundSystem.StartPlayingSound(soundHandleA);

	//int soundHandleB = 0;
	//this->soundSystem.CreateSound(10, soundHandleB);
	//this->soundSystem.ComposeSound(soundHandleB, &whiteNoiseGenerator);
	//this->soundSystem.StartPlayingSound(soundHandleB);

	/*SoundSystem::PinkishNoiseGenerator pinkNoiseGenerator(0.1f);
	int soundHandleC = 0;
	this->soundSystem.CreateSound(10, soundHandleC);
	this->soundSystem.ComposeSound(soundHandleC, &pinkNoiseGenerator);
	this->soundSystem.StartPlayingSound(soundHandleC);*/

	SoundSystem::BlueishNoiseGenerator blueNoiseGenerator(0.1f);
	int soundHandleD = 0;
	this->soundSystem.CreateSound(4, soundHandleD);
	this->soundSystem.ComposeSound(soundHandleD, &blueNoiseGenerator);
	this->soundSystem.StartPlayingSound(soundHandleD);

	return true;
}

/*virtual*/ int Application::OnExit(void)
{
	this->soundSystem.Shutdown();

	return wxApp::OnExit();
}