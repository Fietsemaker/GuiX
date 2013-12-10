/// @file
/// Contains base classes for widgets that contain other widgets.

#pragma once

#include <GuiX/Widget.h>
#include <GuiX/Layout.h>

namespace guix {
namespace gui {

class GxTabContainer;

// ===================================================================================
// GxContainer
// ===================================================================================
/** The GxContainer class is a base class for widgets that contain other widgets.
 
 GxContainer is a base class which is used for widgets that contain one or more child
 widgets that are arranged inside a layout.

 @see GxLayout, GxTabContainer
*/
class GUIX_API GxContainer : public GxWidget
{
public:
	virtual ~GxContainer();

	/// Creates a new container and assigns a layout to it.
	/// The container takes ownership of the layout and the widgets inside it.
	GxContainer(GxLayout* layout);

	/// Deletes the current layout and assigns a new layout to the container.
	/// The container takes ownership of the layout and the widgets inside it.
	void SetLayout(GxLayout* layout);

	/// Adds a widget to the layout inside the container.
	/// The container takes ownership of the widget.
	void Add(GxWidget* widget);

	GxLayout& GetLayout();             ///< Returns the layout object.
	const GxLayout& GetLayout() const; ///< Returns the layout object.

protected:
	GxLayout* myLayout;
};

// ===================================================================================
// GxTab
// ===================================================================================
/** The GxTab class is a utility class used by GxTabContainer to store tabs.

 For more details, see the GxTabContainer description.

 @see GxTabContainer
*/
class GUIX_API GxTab
{
public:
	~GxTab();

	/// Creates a new tab and assigns a layout to it.
	/// The tab takes ownership of the layout and the widgets inside it.
	GxTab(GxTabContainer* parent, GxLayout* layout);

	/// Deletes the current layout and assigns a new layout to the tab.
	/// The tab takes ownership of the layout and the widgets inside it.
	void SetLayout(GxLayout* layout);

	/// Adds a widget to the layout inside the tab.
	/// The tab takes ownership of the widget.
	void Add(GxWidget* widget);

	/// Sets the tab text; this is mainly used for rendering purposes.
	void SetText(const GxString& text);

	/// Makes this tab the selected tab.
	void Select();

	/// Returns the tab's label text.
	const GxString& GetText() const;

	/// Returns true if the tab is the currently selected, false otherwise.
	bool IsSelected() const;

	GxLayout& GetLayout();              ///< Returns the layout object.
	const GxLayout& GetLayout() const;  ///< Returns the layout object.

private:
	GxTabContainer* myOwner;
	GxLayout* myLayout;
	GxString myText;
};

// ===================================================================================
// GxTabContainer
// ===================================================================================
/** The GxTabContainer class is a base class for widgets that contain tabs.
 
 GxTabContainer is similar to GxContainer but has multiple layouts, which are
 encapsulated in tabs. The functions of the tab class are very similar to those of
 the GxContainer class.

 @see GxLayout, GxTab, GxContainer
*/
class GUIX_API GxTabContainer : public GxWidget
{
public:
	virtual ~GxTabContainer();

	GxTabContainer();

	virtual void ClearTabs(); ///< Destroys all tabs.
	virtual GxTab* AddTab();  ///< Appends a tab to the tab list.
	
	virtual void RemoveTab(int index); ///< Removes a tab from the tab list.
	virtual void SelectTab(int index); ///< Makes a tab the selected tab.

	void RemoveTab(GxTab* tab); ///< Removes a tab from the tab list; calls RemoveTab(index) internally.
	void SelectTab(GxTab* tab); ///< Makes a tab the selected tab; calls SelectTab(index) internally.

	GxTab* GetSelectedTab() const; ///< Returns the selected tab.

	GxTab* GetTab(int index);             ///< Returns the tab at index, or NULL if the index is invalid.
	const GxTab* GetTab(int index) const; ///< Returns the tab at index, or NULL if the index is invalid.

protected:
	GxList<GxTab*> myTabs;
	GxTab* mySelectedTab;
};

}; // namespace gui
}; // namespace guix