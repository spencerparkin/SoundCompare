#pragma once

#include <wx/setup.h>
#include <wx/app.h>

class Frame;

class Application : public wxApp
{
public:
	Application();
	virtual ~Application();

	virtual bool OnInit(void) override;
	virtual int OnExit(void) override;

private:

	Frame* frame;
};

wxDECLARE_APP(Application);