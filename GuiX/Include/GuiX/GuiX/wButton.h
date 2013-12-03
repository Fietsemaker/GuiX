/// @file
/// Contains a collection of the most commonly used widget classes.

#pragma once

#include <GuiX/Widget.h>

namespace guix {
namespace widgets {

// ===================================================================================
// GxButton
// ===================================================================================
/** The GxButton widget displays a push button with text on it.

 The GxButton widget is used to create a simple push button. It emits an event when
 the button is pressed and when the button is released. It also has an IsDown()
 function to check if the button is currently being pressed, so the button can be used
 to trigger an action (such as opening a dialog box) and for continuous actions (such
 as moving an object). Each buttons displays a text label describing its action.

 @see GxWidget
*/
class GUIX_API GxButton : public GxWidget
{
public:
	GxDeclareWidgetClass(GxButton);

	virtual ~GxButton();

	/// Creates a button without text.
	GxButton();

	/// Creates a button with the given widget id and button text.
	GxButton(const char* id, GxString text);

	// ===================================================================================
	// Overloaded widget functions

	void OnMousePress(GxMouseEvent& evt);
	void OnMouseRelease(GxMouseEvent& evt);

	void Adjust();
	void Draw();

	// ===================================================================================
	// Functions

	void SetText(GxString text);     ///< Sets the button text.
	const GxString& GetText() const; ///< Returns the button text.

	bool IsDown() const; /// Returns true if the button is currently being pressed.

	// ===================================================================================
	// Events

	/// Emitted when the user starts pressing a button.
	static const char* ePressed();

	/// Emitted when the user stops pressing a button.
	static const char* eReleased();

protected:
	GxString myText;
};

}; // namespace widgets
}; // namespace guix