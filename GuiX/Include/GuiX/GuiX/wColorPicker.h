/// @file
/// Contains a collection of the most commonly used widget classes.

#pragma once

#include <GuiX/Widget.h>
#include <GuiX/GridLayout.h>

namespace guix {
namespace widgets {

class GxSliderEdit;

// ===================================================================================
// GxColorPicker
// ===================================================================================
/** The GxColorPicker widget displays a gradient and RGB sliders to select a color.

 The GxColorPicker widget is a composite widget that combines a color gradient, a
 luminance slider and three color channel slider edits. The values of the color
 gradient and the luminance slider are combined to create a HSL (hue, saturation,
 luminance) color value. The values of the three color channel slider edits are
 combined to create an RGB (red, green, blue) color value.

 When the user modifies either the HSL controls or the RGB controls, the other
 controls will be adjusted automatically to match the color values. Regardless of
 whether HSL or RGB controls are used, the application will receive the color value
 in RGB format.

 @see GxWidget, GxColorEdit
*/
class GUIX_API GxColorPicker : public GxWidget
{
public:
	GxDeclareWidgetClass(GxColorPicker);

	virtual ~GxColorPicker();

	/// Creates a color picker with the initial color set to white.
	GxColorPicker();

	/// Creates a color picker with the given widget id and initial color.
	GxColorPicker(const char* id, GxVec4f color = GxVec4f(1,1,1,1));

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

	void SetColor(GxVec4f color); ///< Sets the selected color.
	GxVec4f GetColorf() const;    ///< Returns the selected color.
	GxColor GetColor() const;     ///< Returns the selected color.

	// ===================================================================================
	// Events

	/// Emitted when the user selects a new color.
	/// value: GxColorf, the color that is selected.
	static const char* eChanged();

protected:
	void myInit();
	GxRecti myGetGradientRect() const;
	GxRecti myGetLuminanceRect() const;
	GxRecti myGetPreviewRect() const;
	GxRecti myGetControlsRect() const;
	void myDragGradient(int x, int y);
	void myDragLuminance(int x, int y);
	int myGetItemAtPos(int x, int y) const;
	void myUpdateHSL(const GxVec4f& hsl);

	GxGridLayout* myGrid;
	GxSliderEdit* mySliders[3];
	GxVec4f myRGB;
	GxVec4f myHSL;
	int myHoverItem;
};

}; // namespace widgets
}; // namespace guix