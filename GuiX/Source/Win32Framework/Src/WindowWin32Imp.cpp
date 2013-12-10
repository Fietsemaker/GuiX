#include <GuiX/Config.h>
#include <GuiX/Core.h>
#include <GuiX/Localize.h>
#include <GuiX/Widgets.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Commdlg.h>
#include <Shlobj.h>
#include <Shellapi.h>

#include <Gl/gl.h>

#include <GuiX/ApplicationWin32.h>

#include "ApplicationWin32Imp.h"
#include "WindowWin32Imp.h"

//#define GX_DISABLE_VSYNC

// Macro's for mouse events.
#define GET_X_LPARAM(lp)  ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)  ((int)(short)HIWORD(lp))

namespace guix {
namespace framework {

// ===================================================================================
// V-sync code snippet
// ===================================================================================

#ifdef GX_DISABLE_VSYNC

static bool WGLExtensionSupported(const char *extension_name)
{
    PFNWGLGETEXTENSIONSSTRINGEXTPROC func = NULL;
    func = (PFNWGLGETEXTENSIONSSTRINGEXTPROC)wglGetProcAddress("wglGetExtensionsStringEXT");
    if(strstr(func(), extension_name) == NULL)
        return false;

    return true;
}

static void DisableVSync()
{
	PFNWGLSWAPINTERVALEXTPROC func = NULL;
	if(WGLExtensionSupported("WGL_EXT_swap_control"))
	{
		func = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
		func(0);
	}
}

#endif // DISABLE_VSYNC

// ===================================================================================
// GxWin32Wrapper
// ===================================================================================

GxWin32Wrapper::GxWin32Wrapper()
	:myHWND(NULL)
	,myHDC(NULL)
	,myHRC(NULL)
	,myWindow(NULL)
{
	myParent = GxApplicationWin32Imp::instance->topWindow;
	GxApplicationWin32Imp::instance->topWindow = this;

	const char* err = CreateHWND();
}

GxWin32Wrapper::~GxWin32Wrapper()
{
	if(myParent)
		myParent->MakeTopWindow();

	if(myHRC)
	{
		wglMakeCurrent(NULL, NULL); 
		wglDeleteContext(myHRC);
	}

	if(myHWND)
		DestroyWindow(myHWND);
}

const char* GxWin32Wrapper::CreateHWND()
{
	myStyle = WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	myExStyle = WS_EX_WINDOWEDGE;

	HWND parentHWND = NULL;
	if(myParent == NULL)
	{
		myStyle |= WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX;
		myExStyle |= WS_EX_APPWINDOW;
	}
	else
	{
		myStyle |=WS_OVERLAPPEDWINDOW & ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
		parentHWND = myParent->GetHWND();
	}

	myHWND = CreateWindowEx(
		WS_EX_APPWINDOW | WS_EX_WINDOWEDGE,
		GxApplicationWin32Imp::instance->className,
		"createWindowEx",
		myStyle,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		640,
		480,
		parentHWND,
		NULL,
		GxApplicationWin32Imp::instance->hInstance,
		this);

	if(!myHWND)
		return "Could not create window.";

	SetWindowLong(myHWND, GWL_STYLE, myStyle);
	SetWindowLong(myHWND, GWL_EXSTYLE, myExStyle);

	if(parentHWND)
		EnableWindow(parentHWND, FALSE);

	DragAcceptFiles(myHWND,TRUE);

    return CreateRC();
}

const char* GxWin32Wrapper::CreateRC()
{
	myHDC = GetDC(myHWND);

	if(myHDC == NULL)
		return "Could not get device context.";

	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(pfd));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 8;

	int pft = ChoosePixelFormat(myHDC, &pfd);
	if(pft == 0)
		return "Could not choose 32-bit pixel format.";

	if(SetPixelFormat(myHDC, pft, &pfd) == FALSE)
		return "Could not set pixel format.";

	myHRC = wglCreateContext(myHDC);
	if(myHRC == NULL)
		return "Could not create OpenGL rendering context.";

	if(wglMakeCurrent(myHDC, myHRC) == FALSE)
		return "Unable to make rendering context current.";

	if(myParent)
	{
		HGLRC parentRC = myParent->GetHRC();
		wglShareLists(parentRC, myHRC); 
	}

#ifdef GX_DISABLE_VSYNC
	DisableVsync();
#endif

	return NULL;
}

int GxWin32Wrapper::Execute(GxWindowWin32* window)
{
	if(!window) return 0;
	myWindow = window;

	// Resize the window.
	GxVec2i size = myWindow->GetSize();
	if(size.x > 0 && size.y > 0)
	{
		RECT r;
		r.left   = GxMax(0, GetSystemMetrics(SM_CXSCREEN)/2 - size.x/2);
		r.top    = GxMax(0, GetSystemMetrics(SM_CYSCREEN)/2 - size.y/2);
		r.right  = r.left + size.x;
		r.bottom = r.top + size.y;
		AdjustWindowRectEx(&r, myStyle, FALSE, myExStyle);
		SetWindowPos(myHWND, NULL, r.left, r.top, r.right-r.left, r.bottom-r.top, SWP_NOACTIVATE);
	}

	// Set the window title.
	SetWindowText(myHWND, myWindow->GetTitle().Raw());

	// Show the window.
	ShowWindow(myHWND, SW_SHOW);
	SetFocus(myHWND);

	// Set up OpenGL.
	wglMakeCurrent(myHDC, myHRC);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glClearColor(0, 0, 0, 1);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	HandleResize();

	// Enter the message loop.
	MSG message;
	while(!myWindow->IsTerminated())
	{
		GxApplicationWin32Imp* app = GxApplicationWin32Imp::instance;

		wglMakeCurrent(myHDC, myHRC);
		app->renderInterface.SetViewSize(myWindow->GetSize());

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		while(PeekMessage(&message, NULL, 0, 0, PM_NOREMOVE))
		{
			GetMessage(&message, NULL, 0, 0);
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		if(!myInputStr.Empty())
		{
			GxInput* input = GxInput::Get();
			if(input)
			{
				GxList<wchar_t> wstr = GxSystemInterfaceWin32::Widen(myInputStr.Raw());
				GxString utf8 = GxUnicode::FromWide(wstr.Data());
				input->OnTextInput(utf8);
			}
			myInputStr.Clear();
		}

		myWindow->Tick(1 / 60.f);
		app->cursorType = myWindow->GetCursor();

		SwapBuffers(myHDC);
	}

	// Complete the message loop, return the termination result.
	int result = myWindow->GetTerminateResult();
	myWindow = NULL;
	return result;
}

void GxWin32Wrapper::HandleResize()
{
	GxVec2i size = myWindow->GetSize();

	glViewport(0, 0, size.x, size.y);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, size.x, size.y, 0, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void GxWin32Wrapper::MakeTopWindow()
{
	GxApplicationWin32Imp::instance->topWindow = this;

	EnableWindow(myHWND, TRUE);
	SetActiveWindow(myHWND);
	wglMakeCurrent(myHDC, myHRC);
}

void GxWin32Wrapper::WinProc(UINT msg, WPARAM wparam, LPARAM lparam)
{
	GxInput* input = GxInput::Get();

	switch(msg)
	{
	case WM_CLOSE:
		if(myWindow)
			myWindow->OnClose();
		break;

	case WM_ACTIVATE:
		if(input && wparam == WA_INACTIVE)
			input->OnWindowInactive();
		break;

	case WM_GETMINMAXINFO:
		if(myWindow)
		{
			GxVec2i size = myWindow->GetMinSize();
			if(size.x > 0 && size.y > 0)
			{
				RECT r;
				r.left = r.top = 0;
				r.right = size.x;
				r.bottom = size.y;
				AdjustWindowRectEx(&r, myStyle, FALSE, myExStyle);

				MINMAXINFO* mm = reinterpret_cast<MINMAXINFO*>(lparam);
				mm->ptMinTrackSize.x = (LONG)(r.right - r.left);
				mm->ptMinTrackSize.y = (LONG)(r.bottom - r.top);
			}			
		}
		break;

	case WM_SIZE:
		if(myWindow)
		{
			GxVec2i size((int)LOWORD(lparam), (int)HIWORD(lparam));
			myWindow->OnResize(size.x, size.y);
		}
		HandleResize();
		break;

	case WM_SETCURSOR:
		GxApplicationWin32Imp::instance->UpdateCursor(); 
        break;

	case WM_DROPFILES:
		{
			// Extract position of the cursor when it dropped the files
			GxVec2i point;
			POINT winpoint;
			DragQueryPoint((HDROP)wparam, &winpoint);
			point.x = winpoint.x;
			point.y = winpoint.y;

			// Get the list of files, can also be just one
			GxList<GxString> file_list;
			unsigned int file_count = DragQueryFile((HDROP)wparam, 0xFFFFFFFF, 0, 0); // Get the amount of dropped files, passing 0xFFFFFFFF will return the file count.
			for(unsigned int i=0; i<file_count; ++i)
			{
				uint pathlen = DragQueryFile((HDROP)wparam, i, 0, 0) + 1; // Get the filesize, giving 0 for the stringbuffer returns size without nullbyte.
				char* filename = GxMalloc<char>(pathlen + 1);
				memset(filename, 0, pathlen + 1);
				DragQueryFile((HDROP)wparam, i, (LPSTR)filename, pathlen); // Get the filename with the given data.
				file_list.Append(filename);
				GxFree(filename);
			}
			DragFinish((HDROP)wparam);

			if(input)
				input->OnFileDrop(file_list, point);
			break;
		}

	case WM_LBUTTONDOWN:
		SetCapture(myHWND);
		if(input)
			input->OnMousePress(GX_MC_LEFT, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
		break;

	case WM_LBUTTONUP:
		ReleaseCapture();
		if(input)
			input->OnMouseRelease(GX_MC_LEFT, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
		break;

	case WM_RBUTTONDOWN:
		SetCapture(myHWND);
		if(input)
			input->OnMousePress(GX_MC_RIGHT, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
		break;

	case WM_RBUTTONUP:
		ReleaseCapture();
		if(input)
			input->OnMouseRelease(GX_MC_RIGHT, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
		break;

	case WM_MBUTTONDOWN:
		SetCapture(myHWND);
		if(input)
			input->OnMousePress(GX_MC_MIDDLE, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
		break;

	case WM_MBUTTONUP:
		ReleaseCapture();
		if(input)
			input->OnMouseRelease(GX_MC_MIDDLE, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
		break;

	case WM_MOUSEMOVE:
		if(input)
			input->SetMousePos(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
		break;

	case WM_MOUSEWHEEL:
		if(input)
		{
			int delta = GET_WHEEL_DELTA_WPARAM(wparam);
			input->OnMouseScroll(delta > 0);
		}
		break;

	case WM_KEYDOWN:
		if(input)
			input->OnKeyPress(GxApplicationWin32Imp::instance->keyMap[wparam]);
		break;

	case WM_KEYUP:
		if(input)
			input->OnKeyRelease(GxApplicationWin32Imp::instance->keyMap[wparam]);
		break;

	case WM_CHAR:
		     if(wparam >=   32) myInputStr += (char)wparam;
		else if(wparam == '\r')	myInputStr += '\n';
		break;
	};
}

// ===================================================================================
// GxWindowWin32
// ===================================================================================

GxWindowWin32::~GxWindowWin32()
{
}

GxWindowWin32::GxWindowWin32()
{
	mySize.Set(640, 480);
	myTitle = "GuiX Window";
	myIsTerminated = false;
	myTerminateResult = 0;
}

int GxWindowWin32::Execute()
{
	GxWin32Wrapper wrapper;
	return wrapper.Execute(this);
}

void GxWindowWin32::Terminate(int result)
{
	myIsTerminated = true;
	myTerminateResult = result;
}

}; // namespace framework
}; // namespace guix