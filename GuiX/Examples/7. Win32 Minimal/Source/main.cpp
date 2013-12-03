// ***** Win32Minimal example implementation *****************************************

// This example demonstrates a minimal implementation of GuiX in Win32 and OpenGL. It
// serves as a reference implementation for developers who wish to integrate GuiX into
// their project.

// This example aims to be concise and simple; if you wish to use a more sturdy and
// feature rich Win32 implementation of GuiX, you can check out the Win32 Framework
// supplied with GuiX.

// ***********************************************************************************

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Gl/gl.h>

#include "stb_image.h"

#include <GuiX/Core.h>
#include <GuiX/Input.h>

#include <GuiX/Context.h>
#include <GuiX/Style.h>
#include <GuiX/Widgets.h>
#include <GuiX/GridLayout.h>

using namespace guix;

static const int   WINDOW_W     = 800;
static const int   WINDOW_H     = 600;
static const char* WINDOW_TITLE = "GuiX Application";

// ===================================================================================
// MyRenderInterface
// ===================================================================================

// A basic implementation of the GuiX render interface. The application provides an 
// implementation of the render interface to GuiX, and GuiX uses this interface to
// render its geometry.

class MyRenderInterface : public GxRenderInterface
{
public:
	GxVec2i GetViewSize();

	void DrawTriangles(const GxVertex*, int, GxTextureHandle);
	void DrawTriangles(const GxVertex*, int, const uint*, int, GxTextureHandle);

	void EnableScissorRect(bool);
	void SetScissorRect(int, int, int, int);

	bool LoadTexture(GxTextureHandle&, int&, int&, const char*);
	bool GenerateTexture(GxTextureHandle&, int, int, const uchar*);
	void ReleaseTexture(GxTextureHandle);

private:
	void SetVertexData(const GxVertex*, GxTextureHandle);
};

// Called by GuiX to get the view size, which is the window size in this case.
GxVec2i MyRenderInterface::GetViewSize()
{
	return GxVec2i(WINDOW_W, WINDOW_H);
}

// Called by GuiX to render colored and textured triangles.
void MyRenderInterface::DrawTriangles(const GxVertex* vertices, int vertexCount, GxTextureHandle texture)
{
	SetVertexData(vertices, texture);
	glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}

// Called by GuiX to render colored and textured triangles with indexed vertices.
void MyRenderInterface::DrawTriangles(const GxVertex* vertices, int vertexCount, const uint* indices, int indexCount, GxTextureHandle texture)
{
	SetVertexData(vertices, texture);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, indices);
}

// Called by GuiX to enable or disable scissor rectangle clipping.
void MyRenderInterface::EnableScissorRect(bool enable)
{
	if(enable)
		glEnable(GL_SCISSOR_TEST);
	else
		glDisable(GL_SCISSOR_TEST);
}

// Called by GuiX to set the scissor rectangle.
void MyRenderInterface::SetScissorRect(int x, int y, int width, int height)
{
	glScissor(x, WINDOW_H - (y + height), width, height);
}

// Called by GuiX to load a texture from a file.
bool MyRenderInterface::LoadTexture(GxTextureHandle& outTexture, int& outWidth, int& outHeight, const char* path)
{
	outWidth = outHeight = 0;

	int cfmt, w, h;
	uchar* bits = (uchar*)stbi_load(path, &w, &h, &cfmt, STBI_rgb_alpha);
	const char* reason = stbi_failure_reason();
	if(!bits) return false;

	// If the loading succeeded, we fill in the output data.
	bool result = GenerateTexture(outTexture, w, h, bits);
	if(result) outWidth = w, outHeight = h;
	stbi_image_free(bits);
	return result;
}

// Called by GuiX to generate a texture from a bitmap.
bool MyRenderInterface::GenerateTexture(GxTextureHandle& outTexture, int width, int height, const uchar* pixeldata)
{
	GLuint id = 0;
	glGenTextures(1, &id);
	if(id)
	{
		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixeldata);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		outTexture = (GxTextureHandle)id;
		return true;
	}
	return false;
}

// Called by GuiX to destroy a texture that is no longer used.
void MyRenderInterface::ReleaseTexture(GxTextureHandle texture)
{
	GLuint handle = (GLuint)texture;
	glDeleteTextures(1, &handle);
}

// Utility function to set up vertex data for rendering.
void MyRenderInterface::SetVertexData(const GxVertex* vertices, GxTextureHandle texture)
{
	glVertexPointer(2, GL_FLOAT, sizeof(GxVertex), &vertices[0].pos);
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(GxVertex), &vertices[0].color);

	if(texture)
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, (GLuint)texture);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, sizeof(GxVertex), &vertices[0].uvs);
	}
	else
	{
		glDisable(GL_TEXTURE_2D);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
}

// ===================================================================================
// MyWindow
// ===================================================================================

// GuiX is a middleware library; it does not own a window or rendering context.
// Therefore, we will create our own window with OpenGL so we can use GuiX inside it.

class MyWindow
{
public:
	MyWindow();
	~MyWindow();

	void HandleMessages();
	void Swap();
	bool IsOpen();

	static LRESULT CALLBACK MessageProc(HWND, UINT, WPARAM, LPARAM);

private:
	HWND  hwnd;
	HDC   hdc;
	HGLRC hrc;
	static bool terminated;
};

// Creates a new window with an OpenGL context.
MyWindow::MyWindow()
{
	hwnd = NULL;
	hdc  = NULL;
	hrc  = NULL;

	// Register the window class.
	WNDCLASS wndc;

	memset(&wndc, 0, sizeof(WNDCLASS));
	wndc.style         = CS_HREDRAW | CS_VREDRAW | CS_CLASSDC;
	wndc.lpfnWndProc   = MessageProc;
	wndc.hInstance     = GetModuleHandle(NULL);
	wndc.lpszClassName = "GuiXWin32";

	if(!RegisterClass(&wndc)) return;

	// Determine the window rectangle.
	RECT r = {0, 0, WINDOW_W, WINDOW_H};
	AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME, FALSE);

	// Create the window.
	hwnd = CreateWindow("GuiXWin32", WINDOW_TITLE, WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME,
		CW_USEDEFAULT, CW_USEDEFAULT, r.right - r.left, r.bottom - r.top, 0, 0, 0, 0);
	if(!hwnd) return;

	hdc = GetDC(hwnd);

	// Choose the pixel format.
	PIXELFORMATDESCRIPTOR pfd = 
	{
		sizeof(PIXELFORMATDESCRIPTOR), 1, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA, 32, 8, 0, 8, 0, 8, 0, 8, 0, 0, 0, 0, 0, 0, 24, 8, 0, 0, 0, 0, 0, 0
	};
	int format = ChoosePixelFormat(hdc, &pfd);
	if(!SetPixelFormat(hdc, format, &pfd)) return;

	// Create the rendering context.
	hrc = wglCreateContext(hdc);
	if(!wglMakeCurrent(hdc, hrc)) return;

	// Initialize up OpenGL.
	glClearColor(0.25, 0.25, 0.25, 1);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glMatrixMode(GL_PROJECTION);
	glOrtho(0, WINDOW_W, WINDOW_H, 0, -1, 1);

	// Show the window.
	ShowWindow(hwnd, SW_SHOW);
}

// Destroys the window and the OpenGL context.
MyWindow::~MyWindow()
{
	wglMakeCurrent(NULL, NULL);
	if(hrc) wglDeleteContext(hrc);
	if(hwnd) DestroyWindow(hwnd);
}

// Handles window messages inside the main loop.
void MyWindow::HandleMessages()
{
	MSG message;
	while(PeekMessage(&message, NULL, 0, 0, PM_NOREMOVE))
	{
		GetMessage(&message, NULL, 0, 0);
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
}

// Swaps buffers.
void MyWindow::Swap()
{
	SwapBuffers(hdc);
}

// Returns whether the window is still open.
bool MyWindow::IsOpen()
{
	return !terminated;
}

// Flag for terminating the window from inside message loop.
bool MyWindow::terminated = false;

// ===================================================================================
// Message procedure
// ===================================================================================

// This map only maps the most important keys for the sake of brevity. For a more
// comprehensive key map, check out the GuiX Win32 framework.

GxKeyCode MapKey(int key)
{
	if(key >= 'A' && key <= 'Z') return (GxKeyCode)(key - 'A' + GX_KC_A);
	if(key >= '0' && key <= '9') return (GxKeyCode)(key - '0' + GX_KC_0);

	switch(key)
	{
		case VK_BACK:    return GX_KC_BACKSPACE;
		case VK_TAB:     return GX_KC_TAB;
		case VK_RETURN:  return GX_KC_RETURN;
		case VK_SPACE:   return GX_KC_SPACE;
		case VK_END:     return GX_KC_END;
		case VK_HOME:    return GX_KC_HOME;
		case VK_LEFT:    return GX_KC_LEFT;
		case VK_UP:      return GX_KC_UP;
		case VK_RIGHT:   return GX_KC_RIGHT;
		case VK_DOWN:    return GX_KC_DOWN;
		case VK_DELETE:  return GX_KC_DELETE;
		case VK_SHIFT:   return GX_KC_SHIFT_L;
		case VK_CONTROL: return GX_KC_CTRL_L;
	};
	return GX_KC_NONE;
}

// Performs a basic translation of the WM_CHAR parameter to make sure its within valid
// ascii range and uses newline characters instead of carriage returns.

void TranslateInput(int c)
{
	if(c >= 32 && c < 128)
		GxInput::Get()->OnTextInput(GxString(c, 1));
	else if(c == '\r')
		GxInput::Get()->OnTextInput(GxString('\n', 1));
}

// The window message procedure.
LRESULT CALLBACK MyWindow::MessageProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	GxInput* input = GxInput::Get();

	int mouseX = (short)LOWORD(lparam);
	int mouseY = (short)HIWORD(lparam);
	int scroll = GET_WHEEL_DELTA_WPARAM(wparam);

	switch(message)
	{
	case WM_LBUTTONDOWN: input->OnMousePress(GX_MC_LEFT, mouseX, mouseY); break;
	case WM_MBUTTONDOWN: input->OnMousePress(GX_MC_MIDDLE, mouseX, mouseY); break;
	case WM_RBUTTONDOWN: input->OnMousePress(GX_MC_RIGHT, mouseX, mouseY); break;

	case WM_LBUTTONUP:   input->OnMouseRelease(GX_MC_LEFT, mouseX, mouseY); break;
	case WM_RBUTTONUP:   input->OnMouseRelease(GX_MC_RIGHT, mouseX, mouseY); break;
	case WM_MBUTTONUP:   input->OnMouseRelease(GX_MC_MIDDLE, mouseX, mouseY); break;

	case WM_MOUSEMOVE:   input->SetMousePos(mouseX, mouseY); break;
	case WM_MOUSEWHEEL:  input->OnMouseScroll(scroll > 0); break;

	case WM_KEYDOWN:     input->OnKeyPress(MapKey(wparam));	break;
	case WM_KEYUP:       input->OnKeyRelease(MapKey(wparam)); break;

	case WM_CHAR:        TranslateInput(wparam); break;
	case WM_CLOSE:       terminated = true; return 0;
	};

	return DefWindowProc(hwnd, message, wparam, lparam);
}

// ===================================================================================
// Main loop
// ===================================================================================

int APIENTRY WinMain(HINSTANCE, HINSTANCE, char*, int)
{
	// Create a window and rendering context for the platform of your choice.
	MyWindow window;

	// Give GuiX access to the custom render interface.
	MyRenderInterface renderer;
	GxCore::SetRenderInterface(&renderer);

	// Initialize the GuiX library. 
	GxCore::Initialize();

	// Create an gui context with some example widgets.
	GxFont("Assets/Fonts/SegoeUI.txt").MakeDefault();

	GxGridLayout* root = new GxGridLayout(3);

	root->Add(new GxCheckbox(NULL, "check me!"));
	root->Add(new GxButton(NULL, "click me!"));
	root->Add(new GxButton(NULL, "...or me!"));
	root->Add(new GxTextEdit(NULL, "edit me!"), 3);
	root->Add(new GxColorPicker, 3);

	GxContext* context = GxContext::New();
	context->SetRoot(new GxFrame(root));
	context->SetView(8, 8, WINDOW_W-16, WINDOW_H-16);

	// Enter the application loop.
	int tickCount = GetTickCount();
	while(window.IsOpen())
	{
		window.HandleMessages();

		glClear(GL_COLOR_BUFFER_BIT);

		int newTickCount = GetTickCount();
		float dt = GxClamp((float)(newTickCount - tickCount) * 0.001f, 0.0001f, 1.f);
		tickCount = newTickCount;

		context->Tick(dt);
		context->Draw();

		window.Swap();
	};

	// Destroy the context.
	GxContext::Delete(context);

	// Release all GuiX resources.
	GxCore::Shutdown();

	return 0;
}