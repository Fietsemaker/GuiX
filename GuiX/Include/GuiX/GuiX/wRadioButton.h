/// @file
/// Contains a collection of the most commonly used widget classes.

#pragma once

#include <GuiX/Sprites.h>
#include <GuiX/Text.h>

#include <GuiX/Widget.h>

namespace guix {
namespace widgets {

// ===================================================================================
// GxRadioButton
// ===================================================================================
/** The GxRadioButton widget displays a radio button with a line of text next to it.

 The GxRadioButton widget is used to display a radio button. Similar to checkboxes,
 radio buttons can be switched on or off. When the user selects a radio button, it
 emits an event and switches off the previously selected radio button. Radio buttons
 display a text label on the right side of the button to describe the option they
 represent.

 Radio button widgets are usually creates in groups. Grouping multiple radio buttons
 together will allow the user to select a single item that group, which is similar to
 selecting a single item from a droplist or a select list. In GuiX, this is done by
 assigning them the same group ID by using the SetGroupId() widget function. When a
 radio button is selected, it will automatically switch off other radio buttons with
 the same group ID.

 @see GxWidget
*/
class GUIX_API GxRadioButton : public GxWidget
{
public:
	GxDeclareWidgetClass(GxRadioButton);

	enum Flags
	{
		F_SELECTED = F_WBIT << 0, ///< Set if the radio button is selected.
	};

	virtual ~GxRadioButton();

	/// Creates a radio button that is not part of a group.
	GxRadioButton();

	/// Creates a radio button with the given widget id and text, and adds it to the given widget group.
	GxRadioButton(const char* id, const char* group, GxString text);

	// ===================================================================================
	// Overloaded widget functions

	void OnMousePress(GxMouseEvent& evt);
	void OnMouseRelease(GxMouseEvent& evt);
	void OnGroupEvent(GxWidget* sender);

	void Adjust();
	void Draw();

	// ===================================================================================
	// Functions

	/// Associates a value with the radio button. This value can be used to identify the
	/// selected radio button when the eSelected event occurs.
	void SetValue(int value);

	/// Selects this radio button, unselecting the other radio buttons in the widget group.
	void SetSelected();

	void SetText(GxString text); ///< Sets the radio button text.

	const GxString& GetText() const; ///< Returns the radio button text.
	bool IsSelected() const;         ///< Returns true if the radio button is selected, false otherwise.

	// ===================================================================================
	// Events

	/// Event: emitted when the user selects a new radio button.
	/// Value: int, the value associated with the selected button.
	static const char* eSelected();

protected:
	GxString myText;
	int myValue;
};

}; // namespace widgets
}; // namespace guix