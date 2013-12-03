/// @file
/// Contains the gui context class.

#pragma once

#include <GuiX/Common.h>
#include <GuiX/String.h>

#include <GuiX/Widget.h>
#include <GuiX/wFrame.h>

namespace guix {
namespace gui {

// ===================================================================================
// GxContext
// ===================================================================================
/** The GxContext class contains a user interface, which is essentially a hierarchy of widgets.

 The GxContext class is responsible for keeping track of a collection of widgets.
 It can be used by the application to update and draw widgets, and it executes the
 callbacks set by the application when widgets emit events.

 @see GxGui, GxWidget
*/
class GUIX_API GxContext
{
public:
	virtual ~GxContext();

	/// Creates a new context.
	static GxContext* New();

	/// Deletes a context created by \c GxContext::New().
	static void Delete(GxContext* context);

	/// Sets the view area of the top layer of widgets. This rectangle is used to set
	/// the position and size of the root widget each tick.
	virtual void SetView(int x, int y, int width, int height) = 0;

	/// Sets the root widget, which is at the root of the widget hierarchy and is
	/// updated directly from the context. The context takes ownership of the root
	/// widget. The previous root widget is deleted.
	virtual void SetRoot(GxWidget* root) = 0;

	/// This function can be used by widgets to set the mouse cursor image.
	virtual void SetCursor(GxCursorImage c) = 0;

	/// Pushes a new layer with its own root widget on the stack. Widgets higher on
	/// the	stack are drawn first and receive input events before widgets lower on
	/// the stack. The hover widget is always picked from the top layer.
	virtual void PushLayer(GxWidget* root) = 0;

	/// Works like \c PushLayer(root) but also takes a viewRegion argument, which
	/// sets the rectangle of the root widget to a specific view region.
	virtual void PushLayer(GxWidget* root, GxRecti viewRegion) = 0;

	/// Pops the top-most layer of the layer stack and deletes all the widgets in it.
	virtual void PopLayer() = 0;

	/// Enables or disables user input. If user input is disabled, there will be
	/// no mouse over widget and mouse/keyboard events will not be sent to widgets.
	virtual void EnableInput(bool enable) = 0;

	/// Performs the following actions in order:
	/// - Executes layer push/pop requests.
	/// - Calls Adjust on all widgets.
	/// - Calls SetRect on all widgets.
	/// - Finds the current hover widget.
	/// - Calls tick on all non-hidden widgets.
	/// - Executes the callbacks from events that were emitted by widgets.
	virtual void Tick(float dt) = 0;

	/// Draws all non-hidden widgets.
	virtual void Draw() = 0;

	/// Returns the view area, as set by \c SetView().
	virtual GxRecti GetView() = 0;

	/// Returns the current type of the mouse cursor as set by \c SetCursor().
	/// The application can use this information to change the mouse cursor.
	virtual GxCursorImage GetCursor() = 0;

	/// Calculates the view size required to give the widgets their preferred size.
	virtual GxVec2i GetPreferredSize() = 0;

	/// Calculates the view size required to give the widgets their minimum size.
	virtual GxVec2i GetMinimumSize() = 0;

	/// Returns true if the mouse cursor is currently on a GUI element or if the mouse
	/// input is currently being captured by a GUI element.
	virtual bool MouseOccupied() = 0;

	// ===================================================================================
	// Widget management

	virtual GxWidget* GetHoverWidget() = 0;  ///< Returns the current hover widget.
	virtual GxWidget* GetFocusWidget() = 0;  ///< Returns the current focus widget.
	virtual GxWidget* GetInputWidget() = 0;  ///< Returns the current input widget.

	/// Returns a highlight value between 0 and 1 for visual effects. 
	/// The value increases for the hover or focus widget, and decreases for all other widgets.
	virtual float GetWidgetHighlight(const GxWidget* w) = 0;
};

}; // namespace gui
}; // namespace guix