/// @file
/// Contains a collection of the most commonly used widget classes.

#pragma once

#include <GuiX/Widget.h>
#include <GuiX/Layout.h>

namespace guix {
namespace widgets {

// ===================================================================================
// GxExpandingButton
// ===================================================================================
/** The GxExpandingButton widget is a base class for buttons that expand when clicked.

 @see GxColorEdit
*/
class GUIX_API GxExpandingButton : public GxWidget
{
public:
	enum Flags
	{
		F_COLLAPSED = F_WBIT << 0, /// Set if the button is collapsed.
	};

	virtual ~GxExpandingButton();

	/// Creates the expand button with the given layout.
	GxExpandingButton(GxLayout* layout);

	// ===================================================================================
	// Overloaded widget functions

	virtual void OnMousePress(GxMouseEvent& evt);

	virtual void Adjust();
	virtual void SetRect(const GxRecti& rect);
	virtual GxWidget* FindHoverWidget(int x, int y);

	virtual void Tick(float dt);
	virtual void Draw();

	virtual GxVec2i GetMinimumSize() const;
	virtual GxVec2i GetPreferredSize() const;

	// ===================================================================================
	// Functions

	/// Sets the widget state to collapsed on true, or expanded on false.
	void SetCollapsed(bool enabled);

protected:
	GxLayout* myLayout;
	int myButtonH, myButtonW;
};

}; // namespace widgets
}; // namespace guix