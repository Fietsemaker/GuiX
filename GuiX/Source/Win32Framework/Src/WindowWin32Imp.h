#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <GuiX/WindowWin32.h>

namespace guix {
namespace framework {

// ===================================================================================
// GxWin32Wrapper
// ===================================================================================

class GxWin32Wrapper
{
public:
	GxWin32Wrapper();
	~GxWin32Wrapper();

	int Execute(GxWindowWin32* window);
	void WinProc(UINT message, WPARAM wparam, LPARAM lparam);

	HGLRC GetHRC() {return myHRC;}
	HWND GetHWND() {return myHWND;}

private:
	const char* CreateHWND();
	const char* CreateRC();
	void HandleResize();
	void MakeTopWindow();

	GxWin32Wrapper* myParent;
	DWORD myStyle, myExStyle;
	HWND myHWND;
	HDC myHDC;
	HGLRC myHRC;
	GxWindowWin32* myWindow;
	GxString myInputStr;
};

}; // namespace framework
}; // namespace guix