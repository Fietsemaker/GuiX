/// @file
/// Contains the slider edit widget classes.

#pragma once

#include <GuiX/String.h>

namespace guix {

namespace layouts { class GxListLayout; };

namespace widgets {

class GxSliderH;
class GxSpinner;

// ===================================================================================
// GxSliderEdit
// ===================================================================================
/** The GxSliderEdit widget displays a slider/spinner combo to input a value.

 The GxSliderEdit is a slider and spinner conveniently combined into a single widget.
 The user can use either the slider or the spinner to change the value. The slider and
 spinner are automatically synced so the values match.

 @see GxWidget, GxSlider, GxSpinner
*/
class GUIX_API GxSliderEdit : public GxWidget
{
public:
	GxDeclareWidgetClass(GxSliderEdit);

	virtual ~GxSliderEdit();

	/// Creates a slider edit with range 0 to 1.
	GxSliderEdit();

	/// Creates a slider edit with range 0 to 1 and the given widget id.
	GxSliderEdit(const char* id);

	// ===================================================================================
	// Overloaded widget functions

	void Adjust();
	void SetRect(const GxRecti& rect);
	GxWidget* FindHoverWidget(int x, int y);

	void Tick(float dt);
	void Draw();

	// ===================================================================================
	// Functions

	void SetValue(int value);    ///< Sets the current value.
	void SetValue(double value); ///< Sets the current value.

	/// Sets the value range and spinner step size.
	void SetRange(int min, int max, int step);

	/// Sets the value range and spinner step size/precision.
	void SetRange(double min, double max, double step, int precision);

	int GetIntValue() const; ///< Returns the current value as an integer.
	double GetValue() const; ///< Returns the current value.

	GxSliderH* GetSlider();  ///< Returns the slider sub-widget.
	GxSpinner* GetSpinner(); ///< Returns the spinner sub-widget.

	// ===================================================================================
	// Events

	/// Emitted when the user changes the slider or spinner value.
	/// value: double, the new spinner/spinner value.
	static const char* eChanged();

private:
	void myInit();
	void myUpdateValue(double value, bool emitEvent);

	double myValue;
	GxSliderH* mySlider;
	GxSpinner* mySpinner;
	GxListLayout* myLayout;
};

}; // namespace widgets
}; // namespace guix