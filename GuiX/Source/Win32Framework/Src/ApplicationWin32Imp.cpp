#include <GuiX/Config.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Commdlg.h>
#include <Shlobj.h>

#include <fcntl.h>
#include <stdio.h>
#include <io.h>
#include <process.h>

#include <GuiX/Core.h>
#include <GuiX/Style.h>
#include <GuiX/Context.h>
#include <GuiX/Widgets.h>

#include "ApplicationWin32Imp.h"
#include "WindowWin32Imp.h"

namespace guix {
namespace framework {

// ===================================================================================
// GxApplicationWin32
// ===================================================================================

GxApplicationWin32::~GxApplicationWin32()
{
}

GxApplicationWin32* GxApplicationWin32::Get()
{
	return GxApplicationWin32Imp::instance;
}

// ===================================================================================
// GxApplicationWin32Imp
// ===================================================================================

GxApplicationWin32Imp* GxApplicationWin32Imp::instance = NULL;

GxApplicationWin32Imp::GxApplicationWin32Imp()
{
	hInstance = GetModuleHandle(NULL);
	className = "GuiXWin32";
	topWindow = NULL;
	myIsConsoleOpen = false;

	SetCurrentDirToExe();

	myLoadCursors();
	myInitKeymap();
	myInitClass();

	cursorType = GX_CI_ARROW;
}

GxApplicationWin32Imp::~GxApplicationWin32Imp()
{
	UnregisterClass(className, hInstance);
}

void GxApplicationWin32Imp::SetCurrentDirToExe()
{
	char exePath[MAX_PATH];

	if(GetModuleFileName(hInstance, exePath, MAX_PATH) >= MAX_PATH &&
	   GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		exePath[0] = 0;

	char* slash = strrchr(exePath, '\\');
	if(slash) slash[1] = 0;
	SetCurrentDirectory(exePath);
}

void GxApplicationWin32Imp::UpdateCursor()
{
	SetCursor(cursors[cursorType]);
}

void GxApplicationWin32Imp::myLoadCursors()
{
	cursors[GX_CI_ARROW]     = LoadCursor(NULL, IDC_ARROW);
	cursors[GX_CI_CARET]     = LoadCursor(NULL, IDC_IBEAM);
	cursors[GX_CI_HAND]      = LoadCursor(NULL, IDC_HAND);
	cursors[GX_CI_DRAG]      = LoadCursor(NULL, IDC_SIZEALL);
	cursors[GX_CI_SIZE_WE]   = LoadCursor(NULL, IDC_SIZEWE);
	cursors[GX_CI_SIZE_NS]   = LoadCursor(NULL, IDC_SIZENS);
	cursors[GX_CI_SIZE_NWSE] = LoadCursor(NULL, IDC_SIZENWSE);
	cursors[GX_CI_SIZE_NESW] = LoadCursor(NULL, IDC_SIZENESW);
}

void GxApplicationWin32Imp::myInitKeymap()
{
	memset(keyMap, 0, KEYMAP_SIZE * sizeof(GxKeyCode));

	keyMap['A'] = GX_KC_A;
	keyMap['B'] = GX_KC_B;
	keyMap['C'] = GX_KC_C;
	keyMap['D'] = GX_KC_D;
	keyMap['E'] = GX_KC_E;
	keyMap['F'] = GX_KC_F;
	keyMap['G'] = GX_KC_G;
	keyMap['H'] = GX_KC_H;
	keyMap['I'] = GX_KC_I;
	keyMap['J'] = GX_KC_J;
	keyMap['K'] = GX_KC_K;
	keyMap['L'] = GX_KC_L;
	keyMap['M'] = GX_KC_M;
	keyMap['N'] = GX_KC_N;
	keyMap['O'] = GX_KC_O;
	keyMap['P'] = GX_KC_P;
	keyMap['Q'] = GX_KC_Q;
	keyMap['R'] = GX_KC_R;
	keyMap['S'] = GX_KC_S;
	keyMap['T'] = GX_KC_T;
	keyMap['U'] = GX_KC_U;	
	keyMap['V'] = GX_KC_V;
	keyMap['W'] = GX_KC_W;
	keyMap['X'] = GX_KC_X;
	keyMap['Y'] = GX_KC_Y;
	keyMap['Z'] = GX_KC_Z;

	keyMap['0'] = GX_KC_0;
	keyMap['1'] = GX_KC_1;
	keyMap['2'] = GX_KC_2;
	keyMap['3'] = GX_KC_3;
	keyMap['4'] = GX_KC_4;
	keyMap['5'] = GX_KC_5;
	keyMap['6'] = GX_KC_6;
	keyMap['7'] = GX_KC_7;
	keyMap['8'] = GX_KC_8;
	keyMap['9'] = GX_KC_9;

	keyMap[VK_BACK]    = GX_KC_BACKSPACE;
	keyMap[VK_TAB]     = GX_KC_TAB;
	keyMap[VK_RETURN]  = GX_KC_RETURN;
	keyMap[VK_PAUSE]   = GX_KC_PAUSE;
	keyMap[VK_ESCAPE]  = GX_KC_ESCAPE;
	keyMap[VK_SPACE]   = GX_KC_SPACE;
	keyMap[VK_END]     = GX_KC_END;
	keyMap[VK_HOME]    = GX_KC_HOME;
	keyMap[VK_LEFT]    = GX_KC_LEFT;
	keyMap[VK_UP]      = GX_KC_UP;
	keyMap[VK_RIGHT]   = GX_KC_RIGHT;
	keyMap[VK_DOWN]    = GX_KC_DOWN;
	keyMap[VK_DELETE]  = GX_KC_DELETE;
	keyMap[VK_LWIN]    = GX_KC_SYSTEM_L;
	keyMap[VK_RWIN]    = GX_KC_SYSTEM_R;
	keyMap[VK_SHIFT]   = GX_KC_SHIFT_L;
	keyMap[VK_CONTROL] = GX_KC_CTRL_L;

	keyMap[VK_NUMPAD0]  = GX_KC_NUMPAD_0;
	keyMap[VK_NUMPAD1]  = GX_KC_NUMPAD_1;
	keyMap[VK_NUMPAD2]  = GX_KC_NUMPAD_2;
	keyMap[VK_NUMPAD3]  = GX_KC_NUMPAD_3;
	keyMap[VK_NUMPAD4]  = GX_KC_NUMPAD_4;
	keyMap[VK_NUMPAD5]  = GX_KC_NUMPAD_5;
	keyMap[VK_NUMPAD6]  = GX_KC_NUMPAD_6;
	keyMap[VK_NUMPAD7]  = GX_KC_NUMPAD_7;
	keyMap[VK_NUMPAD8]  = GX_KC_NUMPAD_8;
	keyMap[VK_NUMPAD9]  = GX_KC_NUMPAD_9;
	keyMap[VK_MULTIPLY] = GX_KC_NUMPAD_MULTIPLY;
	keyMap[VK_ADD]      = GX_KC_NUMPAD_ADD;
	keyMap[VK_SUBTRACT] = GX_KC_NUMPAD_SUBTRACT;
	keyMap[VK_DIVIDE]   = GX_KC_NUMPAD_DIVIDE;

	keyMap[VK_F1]  = GX_KC_F1;
	keyMap[VK_F2]  = GX_KC_F2;
	keyMap[VK_F3]  = GX_KC_F3;
	keyMap[VK_F4]  = GX_KC_F4;
	keyMap[VK_F5]  = GX_KC_F5;
	keyMap[VK_F6]  = GX_KC_F6;
	keyMap[VK_F7]  = GX_KC_F7;
	keyMap[VK_F8]  = GX_KC_F8;
	keyMap[VK_F9]  = GX_KC_F9;
	keyMap[VK_F10] = GX_KC_F10;
	keyMap[VK_F11] = GX_KC_F11;
	keyMap[VK_F12] = GX_KC_F12;
	keyMap[VK_F13] = GX_KC_F13;
	keyMap[VK_F14] = GX_KC_F14;
	keyMap[VK_F15] = GX_KC_F15;
}

bool GxApplicationWin32Imp::myInitClass()
{
	WNDCLASS wndclass;
	memset(&wndclass, 0, sizeof(WNDCLASS));

	wndclass.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.lpfnWndProc   = GlobalProc;
	wndclass.hInstance     = hInstance;
	wndclass.hIcon         = LoadIcon(NULL, IDI_WINLOGO);
	wndclass.hCursor       = NULL;
	wndclass.lpszClassName = className;

	if(!RegisterClass(&wndclass))
		return false;

	return true;
}

// ===================================================================================
// Dialog windows
// ===================================================================================

class GxMessageDialogWin32 : public GxWindowWin32
{
public:
	GxMessageDialogWin32(GxString title, GxString text)
	{
		myTitle = title;

		GxFrameV* frameV = new GxFrameV;
		GxFrameH* frameH = new GxFrameH;

		frameV->Add(myLabel = new GxLabel(text));
		frameV->Add(new GxSpacerV);
		frameV->Add(frameH);

		frameH->Add(new GxSpacerH);
		frameH->Add(myButton = new GxButton(NULL, "OK"));

		myLabel->SetMultiLine(true);

		myContext = GxContext::New();
		myContext->SetRoot(frameV);

		mySize = myContext->GetPreferredSize();
		mySize.x = GxClamp(mySize.x, 128, 512);
		mySize.y = GxClamp(mySize.y, 64, 768);

		myMinSize = mySize;
	}

	~GxMessageDialogWin32()
	{
		GxContext::Delete(myContext);
	}

	void Tick(float dt)
	{
		GxColor bg = GxStyle::Get()->c.bgPanel;
		GxDraw::Get()->Rect(0, 0, mySize.x, mySize.y, bg);

		myContext->Tick(dt);
		myContext->Draw();

		if(myButton->IsInteracted())
			Terminate(0);
	}

	void OnResize(int w, int h)
	{
		myContext->SetView(0, 0, w, h);
		GxWindowWin32::OnResize(w, h);
	}

	void OnClose()
	{
		Terminate(0);
	}

private:
	GxContext* myContext;
	GxButton* myButton;
	GxLabel* myLabel;
};

void GxApplicationWin32Imp::MessageDialog(GxString title, GxString message)
{
	GxMessageDialogWin32 dialog(title, message);
	dialog.Execute();
}

void GxApplicationWin32Imp::OpenConsole()
{
	if(myIsConsoleOpen) return;

	CONSOLE_SCREEN_BUFFER_INFO coninfo;
	int hConHandle;
	long lStdHandle;

	AllocConsole();
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
	coninfo.dwSize.Y = 512;
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

	lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	*stdout = *_fdopen(hConHandle, "w");
	setvbuf(stdout, NULL, _IONBF, 0);

	lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	*stdin = *_fdopen(hConHandle, "r");
	setvbuf(stdin, NULL, _IONBF, 0);

	lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	*stderr = *_fdopen(hConHandle, "w");
	setvbuf(stderr, NULL, _IONBF, 0);

	ShowWindow(GetConsoleWindow(), SW_SHOW);

	myIsConsoleOpen = true;
}

GxString GxApplicationWin32Imp::FolderDialog(GxString title, GxString defaulFolder)
{
    char outPath[MAX_PATH];
    outPath[0] = 0;

	BROWSEINFO bi = {0};

	bi.hwndOwner = topWindow->GetHWND();
    bi.pidlRoot  = NULL;
	bi.pszDisplayName = outPath;
    bi.lpszTitle = title.Raw();
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT | BIF_NEWDIALOGSTYLE;

	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	if(pidl)
	{
		SHGetPathFromIDList(pidl, outPath);
		IMalloc* imalloc = NULL;
        if(SUCCEEDED(SHGetMalloc(&imalloc)))
        {
            imalloc->Free(pidl);
            imalloc->Release();
        }
	}

	GxString result(outPath);
	result.Replace('\\', '/');

	SetCurrentDirToExe();

	return result;
}

GxString GxApplicationWin32Imp::FileDialog(GxString title, GxString defaulFile, GxString filterStr)
{
	GxList<char> filters;
	if(!filterStr.Empty())
	{
		filters = GxList<char>(filterStr.Length() + 1, filterStr.Raw());
		char* filterData = filters.Edit();

		for(int i=0; i<filters.Size(); ++i)
			if(filterData[i] == '\n')
				filterData[i] = NULL;

		filters.Append(NULL);
	}

	char outPath[MAX_PATH];
    outPath[0] = 0;

    OPENFILENAME ofns = {0};

	if(!filters.Empty())
		ofns.lpstrFilter = filters.Data();

	ofns.hwndOwner = topWindow->GetHWND();
    ofns.lStructSize = sizeof(ofns);
    ofns.lpstrFile = outPath;
    ofns.nMaxFile = MAX_PATH;
    ofns.lpstrTitle = title.Raw();
    GetOpenFileName(&ofns);

	GxString result(outPath);
	result.Replace('\\', '/');

	SetCurrentDirToExe();

	return result;
}

// ===================================================================================
// Global WinProc
// ===================================================================================

LRESULT CALLBACK GxApplicationWin32Imp::GlobalProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    if(message == WM_CREATE)
    {
        LPVOID ptr = reinterpret_cast<CREATESTRUCT*>(lparam)->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)ptr);
    }

	GxWin32Wrapper* window = reinterpret_cast<GxWin32Wrapper*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	if(window)
		window->WinProc(message, wparam, lparam);

	if(message == WM_CLOSE)
		return 0;

	return DefWindowProc(hwnd, message, wparam, lparam);
}

}; // namespace framework
}; // namespace guix

// ===================================================================================
// Main
// ===================================================================================

using namespace guix;
using namespace framework;

int APIENTRY WinMain(HINSTANCE, HINSTANCE, char*, int)
{
	GxApplicationWin32Imp* app = new GxApplicationWin32Imp;
	GxApplicationWin32Imp::instance = app;

	GxWin32Wrapper* wrapper = new GxWin32Wrapper;

	GxCore::SetRenderInterface(&app->renderInterface);
	GxCore::SetSystemInterface(&app->systemInterface);
	GxCore::Initialize();

	GxWindowWin32* window = GxCreateMainWindow();
	int result = wrapper->Execute(window);
	delete window;

	GxCore::Shutdown();

	delete wrapper;
	delete app;

#ifdef _CRTDBG_MAP_ALLOC
	_CrtDumpMemoryLeaks();
#endif

	return result;
}