/// @file
/// Contains a collection of widget classes that accept text input.

#pragma once

#include <GuiX/Sprites.h>
#include <GuiX/Text.h>

#include <GuiX/Widget.h>

namespace guix {
namespace widgets {

// ===================================================================================
// GxTextEditHelper
// ===================================================================================
/** The GxTextEditHelper is a helper class used by text input widgets to create an editable text field.

 The GxTextEditHelper class is used by text input widgets to create a rectangle in
 which the user can edit plain text. It supports a variety of features such as
 commonly used hotkeys for selection, copy and pasting to clipboard, multi-line
 editing, password setting, filtering for numerical input and custom fonts.

 @see GxTextEditAbstract, GxSpinner
*/
class GUIX_API GxTextEditHelper
{
public:
	GxTextEditHelper();

	void Select();
	void Deselect();

	void OnKeyPress(GxKeyEvent& evt);
	void OnMousePress(GxMouseEvent& evt);
	void OnMouseRelease(GxMouseEvent& evt);
	void OnTextInput(const GxString& evt);

	void Tick(int mouseX, int mouseY, float dt);
	void Draw();

	void SetNumerical(bool isNumerical);
	void SetMultiLine(bool isMultiLine);
	void SetPassword(bool isPassword, char replacement = '*');
	void SetRect(const GxRecti& rect);
	void SetHighlightColor(GxColor color);
	void SetMaxLength(size_t maxLength);
	void SetText(const GxString& text);

	bool IsSelected() const;
	bool IsMultiLine() const;
	bool IsNumerical() const;
	bool IsPassword() const;
	const GxRecti& GetRect() const;
	GxColor GetHighlightColor() const;
	size_t GetMaxLength() const;
	const GxString& GetText() const;
	const GxText& GetSettings() const;
	GxText& GetSettings();

private:
	void myAdjustSettings();
	void myDeleteSelection();
	int myAdvanceCursorLine(bool forward);
	void myFilterText(GxString& text) const;
	GxString myGetDisplayString() const;
	GxVec2i myGetScrollOffset() const;

	GxRecti myRect;
	GxVec2i myCursor;
	GxString myContent;
	GxText mySettings;
	GxColor myHlColor;
	int myMaxLength;
	float myBlinkTime, myScrollOffset;
	bool myIsMultiLine, myIsNumerical, myIsPassword;
	bool myIsSelected, myIsDragging;
	bool myForceScrollUpdate;
	char myReplacementChar;
};

// ===================================================================================
// GxTextEditAbstract
// ===================================================================================
/** The GxTextEditAbstract displays an editable text box.

 The GxTextEditAbstract widget is used to create a create a rectangle in which the
 user can edit plain text. It is essentially a wrapper class which wraps the 
 GxTextEditHelper class in a widget. This allows the text edit helper to receive
 selection and input event through the gui context. To change the text edit field
 properties, use the \c GetField() function to access and modify the text field.
 The text edit emits a text edit changed event every time the user deselects the text
 field and the contents has changed.

 @see GxWidget, GxTextEditHelper
*/
class GUIX_API GxTextEditAbstract : public GxWidget
{
public:
	virtual ~GxTextEditAbstract();

	/// Creates an text edit with the given widget id, text and multi-line setting.
	GxTextEditAbstract(const char* id, GxString text, bool isMultiLine);

	// ===================================================================================
	// Overloaded widget functions

	void OnKeyPress(GxKeyEvent& evt);
	void OnMousePress(GxMouseEvent& evt);
	void OnMouseRelease(GxMouseEvent& evt);
	void OnTextInput(GxTextEvent& evt);
	void OnLoseInput();

	void SetRect(const GxRecti& rect);

	void Tick(float dt);
	void Draw();

	// ===================================================================================
	// Functions

	/// Sets the contents of the text box.
	void SetText(GxString text);

	/// Returns the contents of the text box.
	const GxString& GetText() const;

	const GxTextEditHelper& GetField() const; ///< Returns the text field.
	GxTextEditHelper& GetField();			  ///< Returns the text field.

	// ===================================================================================
	// Events

	/// Emitted when the user changes the text.
	/// value: string, the new contents of the text box.
	static const char* eChanged();

protected:
	GxTextEditHelper myField;
};

// ===================================================================================
// GxTextEdit
// ===================================================================================
/** GxTextEdit is an implementation of GxTextEditAbstract for multi-line editing.

 @see GxTextEditAbstract
*/
class GUIX_API GxTextEdit : public GxTextEditAbstract
{
public:
	GxDeclareWidgetClass(GxTextEdit);

	/// Creates an empty multi-line text edit.
	GxTextEdit();

	/// Creates an multi-line text edit with the given widget id and text.
	GxTextEdit(const char* id, GxString text);
};

// ===================================================================================
// GxLineEdit
// ===================================================================================
/** GxLineEdit is an implementation of GxTextEditAbstract for single line editing.

 @see GxTextEditAbstract
*/
class GUIX_API GxLineEdit : public GxTextEditAbstract
{
public:
	GxDeclareWidgetClass(GxLineEdit);

	/// Creates an empty single line text edit.
	GxLineEdit();

	/// Creates an single line text edit with the given widget id and text.
	GxLineEdit(const char* id, GxString text);
};

}; // namespace widgets
}; // namespace guix