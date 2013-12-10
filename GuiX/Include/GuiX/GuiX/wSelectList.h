/// @file
/// Contains a collection of the most commonly used widget classes.

#pragma once

#include <GuiX/Sprites.h>
#include <GuiX/Text.h>
#include <GuiX/List.h>

#include <GuiX/Widget.h>
#include <GuiX/wScrollbar.h>

namespace guix {
namespace widgets {

// ===================================================================================
// GxSelectList
// ===================================================================================
/** The GxSelectList widget displays a vertical list of selectable items.

 The GxSelectList widget is used to create a vertical list of selectable items and emits
 a select list changed event whenever the user selects a new item. The select list
 displays its items as a list, a single column table, with the currently selected item
 highlighted. Each item is displayed as a line of text, possibly with a sprite next to
 it (this is up to the widget style).

 @see GxWidget
*/
class GUIX_API GxSelectList : public GxWidget
{
public:
	GxDeclareWidgetClass(GxSelectList);

	virtual ~GxSelectList();

	/// Creates an empty select list.
	GxSelectList();

	/// Creates an empty select list with the given widget id.
	GxSelectList(const char* id);

	// ===================================================================================
	// Overloaded widget functions

	void OnMousePress(GxMouseEvent& evt);
	void OnMouseRelease(GxMouseEvent& evt);
	void OnMouseScroll(GxScrollEvent& evt);

	void Adjust();
	void SetRect(const GxRecti& rect);
	GxWidget* FindHoverWidget(int x, int y);

	void Tick(float dt);
	void Draw();

	// ===================================================================================
	// Functions

	/// Sets the horizontal alignment of the text displayed on the items.
	void SetTextAlignH(GxTextAlignH alignH);

	/// Removes all items from the list.
	void Clear();

	/// Adds a new item to the list with the given text.
	void AddItem(GxString text);

	/// Sets the selected item to the item at index.
	void SetSelectedItem(int index);

	/// Returns the text of the item at index.
	GxString GetItem(int index) const;

	int GetSelectedItem() const; ///< Returns the index of the selected item.
	int GetItemCount() const;    ///< Returns the number of items in the droplist.

	// ===================================================================================
	// Events

	/// Emitted when the user selects a different item on the list.
	/// value: int, the index of the item that was selected.
	static const char* eChanged();

	/// Emitted when the user selects any item on the list, including the current item.
	/// value: int, the index of the item that was selected.
	static const char* eSelected();

protected:
	int myGetItemAtPos(int x, int y) const;
	GxRecti myGetListRect() const;

	GxTextAlignH myAlignH;
	GxList<GxString> myItems;
	GxScrollbarV* myScrollbar;
	int myMouseOverItem;
	int mySelectedItem;
};

}; // namespace widgets
}; // namespace guix