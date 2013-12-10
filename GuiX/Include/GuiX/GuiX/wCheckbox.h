/// @file
/// Contains a collection of the most commonly used widget classes.

#pragma once

#include <GuiX/Sprites.h>
#include <GuiX/Text.h>

#include <GuiX/Widget.h>

namespace guix {
namespace widgets {

// ===================================================================================
// GxCheckbox
// ===================================================================================
/** The GxCheckbox widget displays a checkbox with text next to it.

 The GxCheckbox widget is used to create a interactive checkbox. It emits an event
 when the when checkbox is either checked or unchecked by the user. Checkboxes display
 a text label on the right side of the checkbox to describe their function.

 It is possible to attach a pointer to boolean variable to the checkbox by using the
 SetVarPtr() function. If a boolean is attached, the checkbox will set that boolean
 to true whenever the checkbox is checked, and will set it to false whenever it is
 unchecked.

 @see GxWidget
*/
class GUIX_API GxCheckbox : public GxWidget
{
public:
	GxDeclareWidgetClass(GxCheckbox);

	enum Flags
	{
		F_CHECKED = F_WBIT << 0, ///< Set if the checkbox is checked.
	};

	virtual ~GxCheckbox();

	/// Creates an unchecked checkbox without text.
	GxCheckbox();

	/// Creates a checkbox with the given widget id, text and initial state. 
	GxCheckbox(const char* id, GxString text, bool checked = false);

	// ===================================================================================
	// Overloaded widget functions

	void OnMousePress(GxMouseEvent& evt);
	void OnMouseRelease(GxMouseEvent& evt);

	void Adjust();
	void Draw();

	// ===================================================================================
	// Functions

	/// Attaches a pointer to a boolean to the checkbox.
	/// Whenever the checkbox changes, the boolean will be updated.
	void SetVarPtr(bool* var = NULL);

	void SetText(const GxString& text); ///< Sets the checkbox text.
	void SetChecked(bool checked);      ///< Sets the checked state of the checkbox.

	const GxString& GetText() const;    ///< Returns the checkbox text.
	bool IsChecked() const;             ///< Returns true if the checkbox is checked, false otherwise.

	// ===================================================================================
	// Events

	/// Emitted when the user checks or unchecks a checkbox.
	/// value: boolean, is set to true if the checkbox is now checked.
	static const char* eChanged();

protected:
	GxString myText;
	bool* myVarPtr;
};

}; // namespace widgets
}; // namespace guix