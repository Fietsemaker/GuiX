/// @file
/// Contains a collection of the most commonly used widget classes.

#pragma once

#include <GuiX/wExpandingButton.h>

namespace guix {
namespace widgets {

class GxSliderEdit;

// ===================================================================================
// GxColorEdit
// ===================================================================================
/** The GxColorEdit widget displays collapsable RGB sliders to select a color.

 The GxColorEdit widget is comparable to GxColorPicker, but is more compact. The
 red, green and blue spinners/sliders are collapsed and can be expanded. The gradient
 and hue slider can be accessed through a pop up GxColorPicker widget.

 @see GxWidget, GxColorPicker
*/
class GUIX_API GxColorEdit : public GxExpandingButton
{
public:
	GxDeclareWidgetClass(GxColorEdit);

	virtual ~GxColorEdit();

	/// Creates a color select set to white, without alpha editing.
	GxColorEdit();

	/// Creates a color select with the given widget id, initial color.
	/// If alpha editing is enabled, the widget has an extra alpha slider.
	GxColorEdit(const char* id, GxVec4f color, bool enableAlphaEditing);

	// ===================================================================================
	// Overloaded widget functions

	void Draw();

	// ===================================================================================
	// Functions

	void SetColor(GxVec4f color);  ///< Sets the selected color.
	GxVec4f GetColorf() const;      ///< Returns the selected color.
	GxColor GetColor() const;     ///< Returns the selected color.

	// ===================================================================================
	// Events

	/// Emitted when the user selects a new color.
	/// value: GxColorf, the color that is selected.
	static const char* eChanged();

protected:
	void myInit(const GxVec4f& col, bool alpha);
	GxSliderEdit* mySliders[4];
};

}; // namespace widgets
}; // namespace guix