#pragma once

#include <GuiX/Input.h>
#include <GuiX/String.h>

namespace guix {
namespace framework {

// ===================================================================================
// GxWindowWin32
// ===================================================================================

class GUIX_API GxWindowWin32
{
public:
	GxWindowWin32();

	virtual ~GxWindowWin32();

	virtual void Tick(float dt)         {}
	virtual void OnClose()              {Terminate(0);}
	virtual void OnResize(int w, int h) {mySize.Set(w, h);}
	virtual GxCursorImage GetCursor()   {return GX_CI_ARROW;}

	void Terminate(int result);
	int Execute();

	GxVec2i GetSize() const             {return mySize;}
	GxVec2i GetMinSize() const			{return myMinSize;}
	GxString GetTitle() const           {return myTitle;}
	bool IsTerminated() const           {return myIsTerminated;}
	int GetTerminateResult() const      {return myTerminateResult;}

protected:
	GxVec2i mySize;
	GxVec2i myMinSize;
	GxString myTitle;
	int myTerminateResult;
	bool myIsTerminated;
};

// ===================================================================================
// Main window
// ===================================================================================

// This function is called inside main() to create the main window of the application.
// The application should implement this function and return an object created from
// class derived from the GxWindowWin32 base class.
extern GxWindowWin32* GxCreateMainWindow();

}; // namespace framework
}; // namespace guix