#pragma once

/// @file
/// Contains a collection of widget classes that contain other widgets.

#include <GuiX/Widget.h>
#include <GuiX/Container.h>
#include <GuiX/wScrollbar.h>

namespace guix {
namespace widgets {

// ===================================================================================
// GxScrollArea
// ===================================================================================
/** The GxScrollArea widget behaves as viewport into a larger widget area, accompanied by scrollbars.

 The GxScrollArea widget is a container widget used to encapsulate other widgets into
 a small view area. The user can navigate the view area by dragging the horizontal
 and/or vertical scrollbar. If the view area is actually large enough to display the
 contained widgets, the scrollbars are hidden.

 By default, the horizontal and vertical scrollbars are both enabled. By using the
 SetScrollH() and SetScrollV() functions, both scrollbars can be enabled or disabled.
 If a scrollbar is disabled, the widgets will be shrunken in that direction if they
 exceed the available space.

 @see GxContainer
*/
class GUIX_API GxScrollArea : public GxContainer
{
public:
	GxDeclareWidgetClass(GxScrollArea);

	virtual ~GxScrollArea();

	/// Creates a scroll area with horizontal and vertical scrolling enabled.
	GxScrollArea();

	/// Creates a scroll area with scrolling enabled according to scrollH and scrollV.
	GxScrollArea(bool scrollH, bool scrollV);

	// ===================================================================================
	// Overloaded widget functions

	void Adjust();
	void SetRect(const GxRecti& rect);
	GxWidget* FindHoverWidget(int x, int y);

	void Tick(float dt);
	void Draw();

	// ===================================================================================
	// Functions

	void SetScrollH(bool enabled); ///< Enables or disables horizontal scrolling.
	void SetScrollV(bool enabled); ///< Enables or disables vertical scrolling.

private:
	void myInit(bool scrollH, bool scrollV);
	bool myIsActiveH() const;
	bool myIsActiveV() const;
	GxRecti myViewArea() const;

	GxScrollbarH* myScrollbarH;
	GxScrollbarV* myScrollbarV;
};

}; // namespace widgets
}; // namespace guix