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

	SoundSystem::ToneGenerator toneGeneratorA(440.0f, 1.0f);
	SoundSystem::ColorNoiseGenerator colorNoiseGenerator(1.0f);

	int soundHandleA = 0;
	this->soundSystem.CreateSound(10, soundHandleA);
	this->soundSystem.ComposeSound(soundHandleA, &toneGeneratorA);
	this->soundSystem.StartPlayingSound(soundHandleA);

	int soundHandleB = 0;
	this->soundSystem.CreateSound(10, soundHandleB);
	this->soundSystem.ComposeSound(soundHandleB, &colorNoiseGenerator);
	this->soundSystem.StartPlayingSound(soundHandleB);

	return true;
}

/*virtual*/ int Application::OnExit(void)
{
	this->soundSystem.Shutdown();

	return wxApp::OnExit();
}