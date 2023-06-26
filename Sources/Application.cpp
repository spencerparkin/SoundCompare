#include "Application.h"
#include "Frame.h"
#include <wx/image.h>

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

	wxInitAllImageHandlers();

	this->frame = new Frame(nullptr, wxDefaultPosition, wxSize(600, 300));
	this->frame->Show();

	if (!this->soundSystem.Initialize())
		return false;

	return true;
}

/*virtual*/ int Application::OnExit(void)
{
	this->soundSystem.Shutdown();

	return wxApp::OnExit();
}