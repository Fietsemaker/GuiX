#pragma once

/// @file
/// Contains a collection of widget classes that contain other widgets.

#include <GuiX/Widget.h>
#include <GuiX/Container.h>
#include <GuiX/wScrollbar.h>

namespace guix {
namespace widgets {

// ===================================================================================
// GxTabs
// ===================================================================================
/** The GxTabs widget behaves as a group of tabs that can contain other widgets.

 The GxTabs widget is a container widget that contains multiple layouts with other
 widgets, encapsulated into seperate tabs. It emits an event when the user selects a
 new tab. The contents of only one tab are visible at at any given time, and the user
 can switch between tabs by using the tab buttons at the top of the widget. Each tab
 button displays a text label that describes the contents of the tab.

 @see GxTabContainer
*/
class GUIX_API GxTabs : public GxTabContainer
{
public:
	GxDeclareWidgetClass(GxTabs);

	virtual ~GxTabs();

	/// Creates an empty tab container.
	GxTabs();

	// ===================================================================================
	// Overloaded widget functions

	void OnMousePress(GxMouseEvent& evt);
	void OnMouseRelease(GxMouseEvent& evt);

	void Adjust();
	void SetRect(const GxRecti& rect);
	GxWidget* FindHoverWidget(int x, int y);

	void Tick(float dt);
	void Draw();

	// ===================================================================================
	// Functions

	/// Adds a new tab to the container. Returns a pointer to the new tab.
	GxTab* AddTab();

	/// Sets the default margin for tabs. When a new tab is created, the tab layout has
	/// its margin property set to the given margin.
	void SetTabMargin(const GxMargini& margin);

	// ===================================================================================
	// Events

	/// Emitted when the user selects a new tab.
	/// value: int, the index of the new tab that was selected.
	static const char* eChanged();

private:
	GxRecti myGetTabRect(int index) const;
	int myGetTabAtPos(int x, int y) const;
	int myGetHoverTab() const;

	GxMargini myMargin;
	int myHoverTab;
};

}; // namespace widgets
}; // namespace guix