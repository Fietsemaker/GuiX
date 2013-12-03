/// @file
/// Contains a collection of widget classes that accept text input.

#pragma once

#include <GuiX/Sprites.h>
#include <GuiX/Text.h>

#include <GuiX/Widget.h>
#include <GuiX/wTextEdit.h>

namespace guix {
namespace widgets {

// ===================================================================================
// GxSpinner
// ===================================================================================
/** The GxSpinner widget displays a spinner which is used to input a value.

 The GxSpinner widget displays a spinner box, with which the user can choose a value.
 Unlike a slider or scrollbar a spinner can have an arbitrary range and precision,
 making it useful for editing values that do not have a clearly defined range. The
 default range of a spinner is -224 to 224.

 Internally, spinners always use doubles to store their value. This gives the spinner
 enough precision to be used for both integer values and floating point values. For
 this reason there are integer overloads of the SetValue(), SetRange() and GetValue()
 functions. These functions will cast between double and integer using proper
 rounding.

 @see GxWidget, GxTextEditHelper
*/
class GUIX_API GxSpinner : public GxWidget
{
public:
	GxDeclareWidgetClass(GxSpinner);

	enum Flags
	{
		F_INTEGER = F_WBIT << 0, ///< Set if the spinner only accepts integer values.
	};

	virtual ~GxSpinner();

	/// Creates a spinner with range -2^24 to 2^24.
	GxSpinner();

	/// Creates a spinner with range -2^24 to 2^24 and the given widget id.
	GxSpinner(const char* id);

	// ===================================================================================
	// Overloaded widget functions

	void OnKeyPress(GxKeyEvent& evt);
	void OnMousePress(GxMouseEvent& evt);
	void OnMouseRelease(GxMouseEvent& evt);
	void OnTextInput(GxTextEvent& evt);
	void OnLoseInput();

	void Tick(float dt);
	void Draw();

	// ===================================================================================
	// Functions

	void SetValue(int value);    ///< Sets the current value.
	void SetValue(double value); ///< Sets the current value.

	/// Sets the minimum and maximum value, and the step size of the spinner buttons.
	void SetRange(int min, int max, int step);

	/// Sets the minimum and maximum value, and the step size of the spinner buttons.
	void SetRange(double min, double max, double step, int precision = -1);

	int GetIntValue() const; ///< Returns the current value as an integer.
	double GetValue() const; ///< Returns the current value.

	double GetMin() const; ///< Returns the minimum value of the spinner.
	double GetMax() const; ///< Returns the maximum value of the spinner.

	const GxTextEditHelper& GetField() const; ///< Returns the text field.
	GxTextEditHelper& GetField();             ///< Returns the text field.

	// ===================================================================================
	// Events

	/// Emitted when the user changes the spinner value.
	/// value: double, the new spinner value.
	static const char* eChanged();

protected:
	void myInit();
	GxRecti myGetTextRect() const;
	GxRecti myGetIncRect() const;
	GxRecti myGetDecRect() const;
	int myGetItemAtPos(int x, int y) const;
	int myGetHoverItem() const;
	int myGetFocusItem() const;
	void myUpdateValue(double value, bool emitEvent);

	GxTextEditHelper myField;
	double myValue, myMin, myMax, myStep;
	float myButtonTimer;
	int myPrecision;
	int myMouseOverItem;
};

}; // namespace widgets
}; // namespace guix