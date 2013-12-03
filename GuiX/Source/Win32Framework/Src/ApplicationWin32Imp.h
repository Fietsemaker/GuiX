#pragma once

#include <GuiX/Config.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <GuiX/Widgets.h>

#include <GuiX/RenderInterfaceGL.h>
#include <GuiX/SystemInterfaceWin32.h>
#include <GuiX/ApplicationWin32.h>

namespace guix {
namespace framework {

class GxWin32Wrapper;

class GxApplicationWin32Imp : public GxApplicationWin32
{
public:
	enum { KEYMAP_SIZE = 256 };

	GxApplicationWin32Imp();
	~GxApplicationWin32Imp();

	void OpenConsole();
	GxString FolderDialog(GxString title, GxString defaulFolder);
	GxString FileDialog(GxString title, GxString defaulFile, GxString filters);
	void MessageDialog(GxString title, GxString message);

	void SetCurrentDirToExe();
	void UpdateCursor();

	static LRESULT CALLBACK GlobalProc(HWND, UINT, WPARAM, LPARAM);

public:
	const char* className;
	HINSTANCE hInstance;

	GxWin32Wrapper* topWindow;
	HCURSOR cursors[GX_CI_COUNT];
	GxCursorImage cursorType;
	GxKeyCode keyMap[KEYMAP_SIZE];
	GxRenderInterfaceGL renderInterface;
	GxSystemInterfaceWin32 systemInterface;
	bool myIsConsoleOpen;

	static GxApplicationWin32Imp* instance;

private:
	void myLoadCursors();
	void myInitKeymap();
	void myOpenConsole();
	bool myInitClass();
};

}; // namespace framework
}; // namespace guix