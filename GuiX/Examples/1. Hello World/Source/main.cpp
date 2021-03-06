// GuiX core.
#include <GuiX/Core.h>
#include <GuiX/Text.h>

// Win32 Framework.
#include <GuiX/WindowWin32.h>

using namespace guix;
using namespace guix::framework;

// ===================================================================================
// The DemoWindow is derived from the window class in the Win32 Framework, and is
// going to be the main window of the application.

class DemoWindow : public GxWindowWin32
{
public:
	DemoWindow();
	~DemoWindow();

	void Tick(float dt);
};

DemoWindow::DemoWindow()
{
	// Set the initial window size and window title.
	myTitle.Set("GuiX Examples - Hello World");
	mySize.Set(640, 480);

	// Load a default font.
	GxFont("Assets/Fonts/SegoeUI.txt").MakeDefault();
}

DemoWindow::~DemoWindow()
{
}

void DemoWindow::Tick(float dt)
{
	// Use standard text settings to render the text "Hello World!" at position (16, 16).
	GxText text;
	text.Draw(16, 16, "Hello World!");
}

// ===================================================================================
// This function is used by the Win32 Framework to create the main application window.

GxWindowWin32* guix::framework::GxCreateMainWindow()
{
	return new DemoWindow;
}