#pragma once

/// @file
/// Contains a collection of widget classes that contain other widgets.

#include <GuiX/Widget.h>
#include <GuiX/Container.h>
#include <GuiX/wScrollbar.h>

namespace guix {
namespace widgets {

// ===================================================================================
// GxGroupbox
// ===================================================================================
/** The GxGroupbox widget behaves as a group box that contains other widgets.

 The GxGroupbox widget is used to create a groupbox that contains other widgets. The
 groupbox displays a text label that describes the contents of the groupbox, and a
 thin border around the groupbox area. If the collapsable option is enabled, the user
 can collapse the groupbox to hide or show its contents.

 The default appearance of the groupbox is a box with a thin border. By using the
 SetTabAppearance() function, the groupbox can be switched to tab appearance, which
 displays the groupbox as if it were a tab and hides the border. This does not affect
 the groupbox behaviour.

 @see GxContainer
*/
class GUIX_API GxGroupbox : public GxContainer
{
public:
	GxDeclareWidgetClass(GxGroupbox);

	enum Flags
	{
		F_TAB       = F_WBIT << 0, ///< Set if the groupbox has a tab appearance.
		F_COLLAPSE  = F_WBIT << 1, ///< Set if the groupbox is collapsable.
		F_COLLAPSED = F_WBIT << 2, ///< Set if the groupbox is currently collapsed.
	};

	virtual ~GxGroupbox();

	// Creates an empty groupbox without title text.
	GxGroupbox();

	// Creates an empty groupbox with the given title text.
	GxGroupbox(GxString text);

	// ===================================================================================
	// Overloaded widget functions

	void OnMousePress(GxMouseEvent& evt);

	void Adjust();
	void SetRect(const GxRecti& rect);
	GxWidget* FindHoverWidget(int x, int y);

	void Tick(float dt);
	void Draw();

	GxVec2i GetMinimumSize() const;
	GxVec2i GetPreferredSize() const;

	// ===================================================================================
	// Functions

	void SetText(const GxString& text);  /// Sets the title text of the groupbox.
	void SetCollapsable(bool enabled);   /// Enables or disables the collapse button.
	void SetTabAppearance(bool enabled); /// Enables or disables tab-like appearance.

	const GxString& GetText() const; /// Returns the title text of the groupbox.

private:
	void myInit();

	GxString myText;
};

}; // namespace widgets
}; // namespace guix