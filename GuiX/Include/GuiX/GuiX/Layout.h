/// @file
/// Contains the layout class.

#pragma once

#include <GuiX/List.h>
#include <GuiX/Widget.h>

namespace guix {
namespace gui {

// ===================================================================================
// GxLayout
// ===================================================================================
/** The GxLayout class arranges a collection of widgets inside a rectangle.
 
 Container widgets can use the GxLayout class to store their children and arrange them
 inside a rectangle according to layout rules and the size policies of the children.
 Layouts also provide information about the minimum size and preferred size required
 to arrange their children.

 @see GxWidget, GxContainer
*/
class GUIX_API GxLayout
{
public:
	typedef GxList<GxWidget*> WidgetList;

	virtual ~GxLayout();

	// Constructs an empty layout with spacing set to 2 and margins to 0.
	GxLayout();

	// ===================================================================================
	// Overloadable functions

	/// Adds a widget to the layout. The layout takes ownership of the widget.
	virtual void Add(GxWidget* widget);

	/// Removes a widget from the layout. Does not delete the widget.
	virtual void Remove(GxWidget* widget);

	/// Removes all widgets from the layout. Does not delete the widgets.
	virtual void RemoveAll();

	/// Called before \c Arrange, allows the layout to update its minimum size, preferred size and flags.
	virtual void Adjust() = 0;

	/// Arranges all widgets inside rectangle rect.
	virtual void Arrange(GxRecti rect) = 0;

	// ===================================================================================
	// Member functions

	/// Calls \c FindHoverWidget() on non-hidden widgets until a hover widget is found.
	/// Returns NULL if no hover widget was find.
	GxWidget* FindHoverWidget(int x, int y);

	/// Returns true if position (x, y) is within a widget rectangle inside the layout.
	bool IsInWidgetRect(int x, int y);

	void Tick(float dt);         ///< Calls \c Tick() on non-hidden widgets.
	void Draw();                 ///< Calls \c Draw() on non-hidden widgets.
	void SetLocked(bool locked); ///< Calls \c SetLocked() on all widgets.
	void SetHidden(bool hidden); ///< Calls \c SetHidden() on all widgets.

	/// Assigns a widget as the owner of the layout. All widgets inside the layout and
	/// new widgets that are added to the layout are parented to this owner. This function
	/// is used by container widgets when a layout is assigned to them.
	void SetOwner(GxWidget* owner);

	// ===================================================================================
	// Inline set functions

	void SetMargin(const GxMargini& margin)  {myMargin = margin;}
	void SetSpacing(int spacing)             {mySpacing = spacing;}

	// ===================================================================================
	// Inline get functions

	GxWidget* GetOwner() const               {return myOwner;}

	GxVec2i GetMinimumSize() const           {return myMinimumSize;}
	GxVec2i GetPreferredSize() const         {return myPreferredSize;}
	GxFlags GetPolicyFlagsH() const          {return myPolicyFlagsH;}
	GxFlags GetPolicyFlagsV() const          {return myPolicyFlagsV;}

	const GxMargini& GetMargin() const       {return myMargin;}
	int GetSpacing() const                   {return mySpacing;}

	const WidgetList& GetWidgets() const     {return myWidgets;}

protected:
	GxWidget* myOwner;       ///< The owner widget, see \c SetOwner().
	GxVec2i myMinimumSize;   ///< The required size to give all widgets their minimum size.
	GxVec2i myPreferredSize; ///< The required size to give all widgets their preferred size.
	GxFlags myPolicyFlagsH;  ///< The horizontal size policy flags.
	GxFlags myPolicyFlagsV;  ///< The vertical size policy flags.
	WidgetList myWidgets;    ///< The list of widgets that are currently inside the layout.
	GxMargini myMargin;      ///< The spacing between widgets and the edges of the layout.
	int mySpacing;           ///< The spacing in-between widgets inside the layout.
};

}; // namespace gui
}; // namespace guix