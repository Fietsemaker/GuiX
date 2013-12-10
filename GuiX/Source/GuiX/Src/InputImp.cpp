#include <GuiX/Config.h>

#include <Src/InputImp.h>

namespace guix {
namespace core {

// ===================================================================================
// GxInputImp
// ===================================================================================

GxInputImp* GxInputImp::singleton = NULL;

GxInput::~GxInput()
{
}

GxInput* GxInput::Get()
{
	return GxInputImp::singleton;
}

void GxInputImp::Create()
{
	singleton = new GxInputImp();
}

void GxInputImp::Destroy()
{
	delete singleton;
	singleton = NULL;
}

GxInputImp::GxInputImp()
	:myMousePos(0, 0)
{
}

GxInputImp::~GxInputImp()
{
}

void GxInputImp::AddListener(GxInputListener* listener)
{
	myListeners.insert(listener);
}

void GxInputImp::RemoveListener(GxInputListener* listener)
{
	myListeners.erase(listener);
}

bool GxInputImp::OnKeyPress(GxKeyCode key)
{
	GxKeyEvent evt = {key, 0, false};
	myKeysDown.set(key);
	mySetKeyFlags(evt);
	
	Listeners::iterator it = myListeners.begin();
	for(; it != myListeners.end(); ++it)
		(*it)->OnKeyPress(evt);

	return evt.handled;
}

bool GxInputImp::OnKeyRelease(GxKeyCode key)
{
	GxKeyEvent evt = {key, 0, false};
	myKeysDown.reset(key);
	mySetKeyFlags(evt);

	Listeners::iterator it = myListeners.begin();
	for(; it != myListeners.end(); ++it)
		(*it)->OnKeyRelease(evt);

	return evt.handled;
}

bool GxInputImp::OnTextInput(const GxString& text)
{
	GxTextEvent evt = {text, false};

	Listeners::iterator it = myListeners.begin();
	for(; it != myListeners.end(); ++it)
		(*it)->OnTextInput(evt);

	return evt.handled;
}

bool GxInputImp::OnMousePress(GxMouseCode button, int x, int y)
{
	GxMouseEvent evt = {button, x, y, false};
	myMouseDown.set(button);

	Listeners::iterator it = myListeners.begin();
	for(; it != myListeners.end(); ++it)
		(*it)->OnMousePress(evt);

	return evt.handled;
}

bool GxInputImp::OnMouseRelease(GxMouseCode button, int x, int y)
{
	GxMouseEvent evt = {button, x, y, false};
	myMouseDown.reset(button);
	
	Listeners::iterator it = myListeners.begin();
	for(; it != myListeners.end(); ++it)
		(*it)->OnMouseRelease(evt);

	return evt.handled;
}

bool GxInputImp::OnMouseScroll(bool up)
{
	GxScrollEvent evt = {up, false};

	Listeners::iterator it = myListeners.begin();
	for(; it != myListeners.end(); ++it)
		(*it)->OnMouseScroll(evt);

	return evt.handled;
}

bool GxInputImp::OnFileDrop(GxList<GxString> files, GxVec2i point)
{
	GxFileDropEvent evt = {files, point, false};

	Listeners::iterator it = myListeners.begin();
	for(; it != myListeners.end(); ++it)
		(*it)->OnFileDrop(evt);

	return evt.handled;
}

void GxInputImp::OnWindowInactive()
{
	Listeners::iterator it = myListeners.begin();
	for(; it != myListeners.end(); ++it)
		(*it)->OnWindowInactive();

	myMouseDown.reset();
	myKeysDown.reset();
}

void GxInputImp::SetMousePos(int x, int y)
{
	myMousePos.Set(x, y);
}

bool GxInputImp::IsKeyDown(GxKeyCode key)
{
	return myKeysDown.test(key);
}

bool GxInputImp::IsMouseDown(GxMouseCode button)
{
	return myMouseDown.test(button);
}

bool GxInputImp::IsMouseInRect(int x, int y, int w, int h)
{
	const int mx = myMousePos.x;
	const int my = myMousePos.y;
	return (mx >= x && my >= y && mx <= x+w && my <= y+h);
}

bool GxInputImp::IsMouseInRect(const GxRecti& rect)
{
	return rect.Contains(myMousePos.x, myMousePos.y);
}

const GxVec2i& GxInputImp::GetMousePos()
{
	return myMousePos;
}

void GxInputImp::mySetKeyFlags(GxKeyEvent& evt)
{
	evt.flags = 0;

	if(myKeysDown[GX_KC_SHIFT_L]) evt.flags |= GX_KF_SHIFT_L;
	if(myKeysDown[GX_KC_SHIFT_R]) evt.flags |= GX_KF_SHIFT_R;
	if(myKeysDown[GX_KC_CTRL_L ]) evt.flags |= GX_KF_CTRL_L;
	if(myKeysDown[GX_KC_CTRL_R ]) evt.flags |= GX_KF_CTRL_R;
	if(myKeysDown[GX_KC_ALT_L  ]) evt.flags |= GX_KF_ALT_L;
	if(myKeysDown[GX_KC_ALT_R  ]) evt.flags |= GX_KF_ALT_R;
}

}; // namespace gui
}; // namespace guix