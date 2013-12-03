#pragma once

#include <GuiX/Config.h>

#include <set>
#include <bitset>

#include <GuiX/Input.h>

namespace guix {
namespace core {

// ===================================================================================
// GxInputImp
// ===================================================================================

class GxInputImp : public GxInput
{
public:
	static GxInputImp* singleton;

	static void Create();
	static void Destroy();

	GxInputImp();
	~GxInputImp();

	void AddListener(GxInputListener* listener);
	void RemoveListener(GxInputListener* listener);

	bool OnKeyPress(GxKeyCode key);
	bool OnKeyRelease(GxKeyCode key);
	bool OnTextInput(const GxString& text);

	bool OnMousePress(GxMouseCode button, int x, int y);
	bool OnMouseRelease(GxMouseCode button, int x, int y);
	bool OnMouseScroll(bool up);

	void OnWindowInactive();

	bool GxInputImp::OnFileDrop(GxList<GxString> files, GxVec2i points);

	void SetMousePos(int x, int y);

	bool IsKeyDown(GxKeyCode key);
	bool IsMouseDown(GxMouseCode button);
	bool IsMouseInRect(int x, int y, int w, int h);
	bool IsMouseInRect(const GxRecti& rect);

	const GxVec2i& GetMousePos();

private:
	typedef std::set<GxInputListener*> Listeners;

	void mySetKeyFlags(GxKeyEvent& evt);
	std::bitset<GX_MC_SIZE> myMouseDown;
	std::bitset<GX_KC_SIZE> myKeysDown;
	Listeners myListeners;
	GxVec2i myMousePos;
};

}; // namespace core
}; // namespace guix