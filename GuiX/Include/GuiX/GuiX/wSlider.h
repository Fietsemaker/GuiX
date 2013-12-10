/// @file
/// Contains the slider widget classes.

#pragma once

#include <GuiX/Widget.h>

namespace guix {
namespace widgets {

// ===================================================================================
// GxSlider
// ===================================================================================
/** The GxSliderAbstract widget displays a horizontal or vertical slider.

 The GxSliderAbstract widget is used to create a horizontal or vertical slider that is
 used by the user to manipulate a value. It emits a slider pressed event when the user
 grabs hold of the slider and a slider released event when the slider is released. It
 also emits a slider changed events while the user is dragging the slider to new
 values.

 @see GxWidget
*/
class GUIX_API GxSliderAbstract : public GxWidget
{
public:
	enum Flags
	{
		F_SHOWTICKS = F_WBIT << 0, ///< Set if the slider displays tick lines at set intervals.
		F_SNAPPING  = F_WBIT << 1, ///< Set if the slider value snaps to the tick lines.
		F_DRAGGING  = F_WBIT << 2, ///< Set if the user is currently dragging the slider.
	};

	virtual ~GxSliderAbstract();

	/// Creates a slider with the given widget id and direction.
	GxSliderAbstract(const char* id, bool vertical);

	// ===================================================================================
	// Overloaded widget functions

	void OnMousePress(GxMouseEvent& evt);
	void OnMouseRelease(GxMouseEvent& evt);
	void OnMouseScroll(GxScrollEvent& evt);

	void Tick(float dt);
	void Draw();

	// ===================================================================================
	// Functions

	/// Attaches a pointer to a variable to the scrollbar.
	/// Whenever the scrollbar value changes, the variable will be updated.
	void SetVarPtr(double* var = NULL);

	void SetValue(int value);    ///< Sets the current scrollbar value.
	void SetValue(double value); ///< Sets the current scrollbar value.

	/// Set up the paremeters that define the scroll range, see class description.
	void SetRange(int begin, int end, int tickInterval = 0);

	/// Set up the paremeters that define the scroll range, see class description.
	void SetRange(double begin, double end, double tickInterval = 0);

	void SetSnapToTicks(bool enabled); ///< Enables or disables snapping to the tick interval.
	void SetShowTicks(bool enabled);   ///< Shows or hides the tick lines.

	int GetIntValue() const; ///< Returns the current slider value as an integer.
	double GetValue() const; ///< Returns the current slider value.

	double GetBegin() const; ///< Returns the begin value of the slider range.
	double GetEnd() const;	 ///< Returns the end value of the slider range.
	double GetTickInterval() const; ///< Returns the interval between the tick lines.

	/// Returns true if the slider is vertical, false if it is horizontal.
	virtual bool IsVertical() const = 0;

	// ===================================================================================
	// Events

	/// Emitted when the user changes the position of the slider.
	/// value: double, the new slider value.
	static const char* eChanged();

protected:
	void myUpdateValue(double value, bool emitEvent);
	void myDragSlider(int x, int y);
	GxRecti myGetBarRect() const;
	GxVec2i myGetBeginPos() const;
	GxVec2i myGetEndPos() const;
	GxVec2i myGetValuePos() const;
	double myGetTickDelta() const;

	double myValue, myBegin, myEnd;
	double myTickInterval;
	double* myVarPtr;
};

// ===================================================================================
// GxSliderH
// ===================================================================================
/** GxSliderH is an implementation of GxSliderAbstract as a horizontal slider.

 @see GxSliderAbstract
*/
class GUIX_API GxSliderH : public GxSliderAbstract
{
public:
	GxDeclareWidgetClass(GxSliderH);

	/// Creates a horizontal slider with range 0 to 1.
	GxSliderH();

	/// Creates a horizontal slider with range 0 to 1 and the given widget id.
	GxSliderH(const char* id);

	bool IsVertical() const; ///< Always returns false.
};

// ===================================================================================
// GxSliderV
// ===================================================================================
/** GxSliderV is an implementation of GxSliderAbstract as a vertical slider.

 @see GxSliderAbstract
*/
class GUIX_API GxSliderV : public GxSliderAbstract
{
public:
	GxDeclareWidgetClass(GxSliderV);

	/// Creates a vertical slider with range 0 to 1.
	GxSliderV();

	/// Creates a vertical slider with range 0 to 1 and the given widget id.
	GxSliderV(const char* id);

	bool IsVertical() const; ///< Always returns true.
};

}; // namespace widgets
}; // namespace guix