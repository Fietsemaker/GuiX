/// @file
/// Contains a collection of the most commonly used widget classes.

#pragma once

#include <GuiX/Sprites.h>
#include <GuiX/Text.h>
#include <GuiX/List.h>

#include <GuiX/Widget.h>

namespace guix {
namespace widgets {

class GxSelectList;

// ===================================================================================
// GxDroplist
// ===================================================================================
/** The GxDroplist widget displays a button that expands to a list of selectable items.

 The GxDroplist widget is used to create a vertical list of selectable items. In
 collapsed form, the droplist is displayed as a button with a text label that
 indicates the currently selected item. When the user clicks the button, the droplist
 expands to a vertical list that contains all available items, from which the user can
 select a new item. It emits an event whenever the selected item changes, which occurs
 when the user selects an item from the list that is different from the current item.

 @see GxWidget
*/
class GUIX_API GxDroplist : public GxWidget
{
public:
	GxDeclareWidgetClass(GxDroplist);

	virtual ~GxDroplist();

	/// Creates an empty droplist.
	GxDroplist();

	/// Creates an empty droplist with the given widget id.
	GxDroplist(const char* id);

	// ===================================================================================
	// Overloaded widget functions

	void OnMousePress(GxMouseEvent& evt);

	void Draw();

	// ===================================================================================
	// Functions

	/// Removes all items from the list.
	void Clear();

	/// Adds a new item to the list with the given text.
	/// The item value is set to the index of the item in the list.
	void AddItem(GxString text);

	/// Adds a new item to the list with the given text.
	// The item value is set to the value argument.
	void AddItem(GxString text, const GxVariant& value);

	/// Sets the selected item to the item at index.
	void SetSelectedItem(int index);

	/// Returns the value of the selected item.
	GxVariant GetValue() const;

	int GetItemCount() const;    ///< Returns the number of items in the droplist.
	int GetSelectedItem() const; ///< Returns the index of the selected item.

	// ===================================================================================
	// Events

	/// Emitted when the user selects a different item on the list.
	/// value: generic, the value of the item that was selected.
	static const char* eChanged();

	/// Emitted when the user selects any item on the list, including the item that is already selected.
	/// value: generic, the value of the item that was selected.
	static const char* eSelected();

protected:
	void myInit();
	void myOpenList();

	GxList<GxString> myItems;
	GxList<GxVariant> myValues;
	GxSelectList* mySelectList;
	int mySelectedItem;
};

}; // namespace widgets
}; // namespace guix