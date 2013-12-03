/// @file
/// Contains a collection of the most commonly used widget classes.

#pragma once

#include <GuiX/Text.h>
#include <GuiX/Widget.h>

namespace guix {
namespace widgets {

// ===================================================================================
// GxLabel
// ===================================================================================
/** The GxLabel widget displays one or more lines of text.

 The GxLabel widget is used to display one or more lines of text inside a rectangle.
 The visual appearance of the text can be modified by using the \c GetSettings()
 function to access and modify the label's text settings.

 @see GxWidget
*/
class GUIX_API GxLabel : public GxWidget
{
public:
	GxDeclareWidgetClass(GxLabel);

	virtual ~GxLabel();

	/// Creates a label without any text.
	GxLabel();

	/// Creates a label with the given text.
	GxLabel(GxString text);

	// ===================================================================================
	// Overloaded widget functions

	void Adjust();
	void Draw();

	// ===================================================================================
	// Functions

	/// Sets the label text.
	void SetText(GxString text);

	/// A convenience function. Disables the ellipses flag in the label's text settings.
	void SetMultiLine(bool enabled);

	/// A convenience function. Sets the horizontal text alignment.
	void SetTextAlignH(GxTextAlignH h);

	/// Sets custom text settings on the label.
	void SetTextSettings(const GxText& settings);

	/// Returns the current text settings. This will be either the custom text settings
	/// as set by \c SetTextSettings() or the default text settings of the global style.
	GxText GetTextSettings() const;

	/// Returns the label text.
	const GxString& GetText() const; 

protected:
	void myInit();

	GxText* mySettings;
	GxString myText;
};

}; // namespace widgets
}; // namespace guix