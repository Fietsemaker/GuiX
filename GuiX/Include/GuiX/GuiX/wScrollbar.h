/// @file
/// Contains the scrollbar widget classes.

#pragma once

#include <GuiX/Widget.h>

namespace guix {
namespace widgets {

// ===================================================================================
// GxScrollbar
// ===================================================================================
/** The GxScrollbarAbstract widget displays a horizontal or vertical scrollbar.

 The GxScrollbarAbstract widget is used to display a horizontal or vertical scrollbar,
 with which the user can manipulate a value. It emits an event whenever the user
 changes the value of the scrollbar. The abstract scrollbar base class is implemented
 by two widgets: GxScrollbarH, a horizontal scrollbar, and GxScrollbarV, a vertical
 scrollbar.

 There are a few important values associated with the scrollbar. These are the begin,
 end, scroll range, page size and step size. The following diagram illustrates some
 of their properties:

 \verbatim

 begin                                 end
   +------------------------------------+---------------+
   |                                    [      :::      ]
   +------------------------------------+---------------+
    <---------- scroll range ----------> <- page size -> 

 \endverbatim

 The begin and end value define the scroll range. When the bar is on the very left (or
 top, for vertical bars) the scroll value is equal to begin. When the bar is on the
 very right (or bottom, for vertical bars) the scroll value is equal to end. The page
 size is the size of a single page in the scroll region; it is conceptually the
 portion of the scrollable area that is visible inside the view area.

 @see GxWidget
*/
class GUIX_API GxScrollbarAbstract : public GxWidget
{
public:
	enum Flags
	{
		F_BUTTONS  = F_WBIT << 0, ///< Set if the scrollbar has buttons.
		F_SNAPPING = F_WBIT << 1, ///< Set if the scrollbar value snaps to set intervals. 
		F_DRAGGING = F_WBIT << 2, ///< Set if the user is currently dragging the scrollbar.
	};

	virtual ~GxScrollbarAbstract();

	/// Creates a scrollbar with the given widget id and direction.
	GxScrollbarAbstract(const char* id, bool vertical);

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

	void ScrollUp();   ///< Scrolls the scrollbar up one step.
	void ScrollDown(); ///< Scrolls the scrollbar down one step.

	void SetValue(int value);    ///< Sets the current scrollbar value.
	void SetValue(double value); ///< Sets the current scrollbar value.

	/// Set up the paremeters that determine the scroll behaviour, see class description.
	void SetRange(int begin, int end, int pageSize, int stepSize = 1);

	/// Set up the paremeters that determine the scroll behaviour, see class description.
	void SetRange(double begin, double end, double pageSize, double stepSize = 1);

	void SetButtons(bool enabled);     ///< Enables or disables the arrow buttons.
	void SetSnapToSteps(bool enabled); ///< Enables or disables snapping to step size.

	int GetIntValue() const; ///< Returns the current scroll value as an integer.
	double GetValue() const; ///< Returns the current scroll value.

	double GetBegin() const; ///< Returns the begin value of the scroll range.
	double GetEnd() const;   ///< Returns the end value of the scroll range.

	/// Returns true if the scrollbar is vertical, false if it is horizontal.
	virtual bool IsVertical() const = 0; 

	// ===================================================================================
	// Events
 
	/// Emitted when the user changes the position of the scrollbar.
	/// value: double, the scrollbar position.
	static const char* eChanged();

protected:
	void myUpdateValue(double value, bool emitEvent);
	void myDragScrollbar(int x, int y);
	void myApplyScroll(bool up);
	GxRecti myGetIncRect() const;
	GxRecti myGetDecRect() const;
	GxRecti myGetBarRect() const;
	GxRecti myGetDragRect() const;
	int myGetItemAtPos(int x, int y) const;
	double myGetValueAtPos(int x, int y) const;

	double myValue, myBegin, myEnd;
	double myPageSize, myStepSize;
	double myActionValue;
	double* myVarPtr;
	int myLastItem;
};

// ===================================================================================
// GxScrollbarH
// ===================================================================================
/** GxScrollbarH is an implementation of GxScrollbarAbstract as a horizontal scrollbar.

 @see GxScrollbarAbstract
*/
class GUIX_API GxScrollbarH : public GxScrollbarAbstract
{
public:
	GxDeclareWidgetClass(GxScrollbarH);

	/// Creates a horizontal scrollbar with range 0 to 1.
	GxScrollbarH();

	/// Creates a horizontal scrollbar with range 0 to 1 and the given widget id.
	GxScrollbarH(const char* id);

	bool IsVertical() const; ///< Always returns false.
};

// ===================================================================================
// GxScrollbarV
// ===================================================================================
/** GxScrollbarV is an implementation of GxScrollbarAbstract as a vertical scrollbar.

 @see GxScrollbarAbstract
*/
class GUIX_API GxScrollbarV : public GxScrollbarAbstract
{
public:
	GxDeclareWidgetClass(GxScrollbarV);

	/// Creates a vertical scrollbar with range 0 to 1.
	GxScrollbarV();

	/// Creates a vertical scrollbar with range 0 to 1 and the given widget id.
	GxScrollbarV(const char* id);

	bool IsVertical() const; ///< Always returns true.
};

}; // namespace widgets
}; // namespace guix