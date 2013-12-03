/// @file
/// Contains the slider widget classes.

#pragma once

#include <GuiX/Widget.h>

namespace guix {
namespace widgets {

class GxSpinner;

// ===================================================================================
// GxSlider
// ===================================================================================
/** The GxSliderRange widget displays a horizontal or vertical slider.

 The GxSliderRange widget is used to create a horizontal or vertical slider that is
 used by the user to manipulate a value. It emits a slider pressed event when the user
 grabs hold of the slider and a slider released event when the slider is released. It
 also emits a slider changed events while the user is dragging the slider to new
 values.

 @see GxWidget
*/
class GUIX_API GxSliderRange : public GxWidget, public GxCallback
{
public:
	GxDeclareWidgetClass(GxSliderRange);

	enum Flags
	{
		F_DRAGGING  = F_WBIT << 0, ///< Set if the user is currently dragging the slider.
		F_LEFT_END  = F_WBIT << 1, ///, Set is the left end is being dragged.
	};

	virtual ~GxSliderRange();

	/// Creates a slider with the given widget id.
	GxSliderRange(const char* id = NULL, bool addSpinners = true);

	// ===================================================================================
	// Overloaded widget functions

	void SetRect(const GxRecti& r);
	GxWidget* FindHoverWidget(int x, int y);
	
	void OnMousePress(GxMouseEvent& evt);
	void OnMouseRelease(GxMouseEvent& evt);
	void OnEvent(const GxWidgetEvent& event);

	void Tick(float dt);
	void Draw();

	// ===================================================================================
	// Functions

	void SetValue(int begin, int end);       ///< Sets the current slider value.
	void SetValue(double begin, double end); ///< Sets the current slider value.

	/// Sets the minimum and maximum value, and the step size of the spinner buttons.
	void SetRange(int min, int max, int step = 1);

	/// Sets the minimum and maximum value, and the step size of the spinner buttons.
	void SetRange(double min, double max, double step = 1.0);

	GxVec2i GetIntValue() const; ///< Returns the current slider value as an integer.
	GxVec2d GetValue() const;    ///< Returns the current slider value.
	GxVec2d GetRange() const;    ///< Returns the range of the slider.

	// ===================================================================================
	// Events

	/// Emitted when the user changes the position of the slider.
	/// value: GxVec2f, the new slider value.
	static const char* eChanged();

protected:
	void myUpdateValue(double value, bool left, bool emitEvent);
	void myDragSlider(int x, int y);
	GxRecti myGetBarRect() const;
	GxVec2i myGetBeginPos() const;
	GxVec2i myGetEndPos() const;
	GxVec3i myGetValuePos() const;

	GxSpinner* mySpinners;
	GxVec2d myValue, myRange;
};

}; // namespace widgets
}; // namespace guix