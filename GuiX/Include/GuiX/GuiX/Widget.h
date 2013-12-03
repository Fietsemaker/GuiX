/// @file
/// Contains the widget class, and utility classes/functions related to it.

#pragma once

#include <GuiX/Variant.h>
#include <GuiX/Input.h>

namespace guix {
namespace gui {

class GxContextNode;
class GxContext;
class GxWidget;
class GxLayout;

// ===================================================================================
// GxWidgetEvent
// ===================================================================================
/** The GxWidgetEvent struct contains information of an event emitted by a widget.
 
 Widgets use GxWidgetEvent objects to inform the application of events that take
 place, such as the user pressing a button or changing the contents of a input field.
 All events contain the id of the widget that sent the event and the type of event
 that occured. Some events also contain additional data, which is stored in the
 value field.

 @see GxCallback
*/
struct GUIX_API GxWidgetEvent
{
	GxString id;     ///< The widget id of the widget that sent the event.
	GxString type;   ///< A string identifying the type of the event that was sent.
	GxVariant value; ///< Event data. Read the description of individual events for more info.
};

// ===================================================================================
// GxCallback
// ===================================================================================
/** The GxCallback class provides an interface for receiving widget events.
 
 The application can attach a callback to widgets to receive events from that widget.
 If the widget that emits the event does not have a callback, it will propagate the
 event to its parent. If the parent does not have one either, it will continue
 propagating the event upwards in the widget hierarchy until a callback is reached.

  @see GxWidgetEvent
*/
class GUIX_API GxCallback
{
public:
	/// Overloadable function that receives events emitted by widgets.
	virtual void OnEvent(const GxWidgetEvent& event) = 0;
};

// ===================================================================================
// Size Policy
// ===================================================================================

/// Enumeration of size policy flags that determine how a widget resizes.
enum GxSizePolicyFlags
{
	GX_SPF_FIXED  = 0,      ///< The widget has a fixed size equal to size hint.
	GX_SPF_SHRINK = 1 << 0, ///< The widget is allowed to shrink beyond size hint, but will only do so if necessary.
	GX_SPF_GROW   = 1 << 1, ///< The widget is allowed to grow beyond size hint, but will only do so if necessary.
	GX_SPF_EXPAND = 1 << 2, ///< The widget is greedy and will grow whenever space is available.
};

/// Enumeration of common combinations of size policy flags.
enum GxSizePolicyType
{
	GX_SP_FIXED  = GX_SPF_FIXED,
	GX_SP_SHRINK = GX_SPF_SHRINK,
	GX_SP_RESIZE = GX_SPF_GROW | GX_SPF_SHRINK,
	GX_SP_EXPAND = GX_SPF_GROW | GX_SPF_SHRINK | GX_SPF_EXPAND,
};

/** The GxSizePolicy struct specifies how a widget behaves when it resizes.
 
 The policy flags determines how extra space in a layout is used. If a layout has
 space left over for widgets to fill, the left over space will be distributed to
 widgets with the expand flag first. If there are none, the space is distributed to
 widgets with the grow flag. Widgets that do not have the grow flags will never grow
 beyond their preferred size. If a layout does not have enough space to fit all
 widgets, widgets with the shrink flag will be shrunk to make the layout fit. Widgets
 that do not have the shrink flag set will never shrink beyond their preferred size.

 Regardless of any flags, all widgets will never shrink, grow or expand beyond their
 minimum and maximum size.

 The stretch factor determine the portion of space that is assigned to growing and
 expanding widgets. For example, if a layout with two expanding buttons has 30 pixels
 of space left over, and the stretch factors of the two buttons are 2 and 1
 respectively, 20 pixels will go to the first button and 10 will go to the second
 button.

 @see GxSizePolicyFlags, GxSizePolicyType
*/
struct GUIX_API GxSizePolicy
{
	// Constructs a size policy with all default values.
	GxSizePolicy();

	/// Returns min if the shrink flag is set, hint otherwise.
	GxVec2i GetMinSize() const;

	/// Returns max if the grow flag is set, hint otherwise.
	GxVec2i GetMaxSize() const;

	/// Returns hint.
	GxVec2i GetHintSize() const;

	GxVec2i hint;     ///< The preferred size of the widget. Default is (96, 24).
	GxVec2i min;      ///< The minimum size of the widget. Default is (0, 0)
	GxVec2i max;      ///< The maximum size of the widget. Default is (2^20, 2^20).
	GxFlags flagsH;   ///< The horizontal size policy flags. Default is GX_SP_RESIZE.
	GxFlags flagsV;   ///< The vertical size policy flags. Default is GX_SP_RESIZE.
	float stretchH;   ///< The horizontal stretch factor; must be non-negative. Default is 1.
	float stretchV;   ///< The vertical stretch factor; must be non-negative. Default is 1.
	bool adjustHint;  ///< True if the widget should adjust its preferred size automatically. Default is true.
	bool adjustMin;   ///< True if the widget should adjust its minimum size automatically. Default is true.
	bool adjustFlags; ///< True if the widget should adjust its policy flags automatically. Default is true.
};

/** The GxSizePolicyResult struct contains the layout data that results from a size policy.
 
 @see GxSizePolicy
*/
struct GUIX_API GxSizePolicyResult
{
	/// Contructs the result of a widgets size policy and its minimum/preferred sizes.
	/// If transpose is set to true, the x-y and h-v components of all properties are swapped.
	GxSizePolicyResult(const GxWidget* w, bool transpose = false);

	GxVec2i hint;     ///< The preferred size of the widget.
	GxVec2i min;      ///< The minimum size of the widget.
	GxVec2i max;      ///< The maximum size of the widget.
	float stretchH;   ///< The horizontal stretch factor.
	float stretchV;   ///< The vertical stretch factor.
	GxFlags flagsH;   ///< The horizontal size policy flags.
	GxFlags flagsV;   ///< The vertical size policy flags.
};

// ===================================================================================
// GxWidget
// ===================================================================================

/// Macro for declaring a widget class; use inside the widget class definition.
#define GxDeclareWidgetClass(typeString) \
	static const char* GetClassId() {return #typeString;} \
	virtual const char* GetTypeId() const {return #typeString;}

/// Macro for defining a widget event; use in the widget class source file.
#define GxDefineWidgetEvent(widgetClass, evt) \
	const char* widgetClass::evt() {return #widgetClass ":" #evt;}

/** The GxWidget class is the abstract base class from which all widgets are derived.
 
 GxWidget is the abstract base class from which all widget classes (such as buttons
 and sliders, but also widget containers such as groupboxes and tabs) are derived.
 The widget class contains a number of overloadable functions that allow widgets to
 communicate with the context or with the application. All overloadable functions have
 default behaviour and most of them will not need to be reimplemented. The following
 functions are most frequently reimplemented:

 - \c Adjust()
 - \c Tick()
 - \c Draw()

 The widget class is a generic base class, and does not have many properties except
 those that are shared by all widgets. In GuiX, all widgets are (for layout purposes)
 rectangular areas with a position and size. Each widget is part of a context, which
 updates and renders the widget and sends it input events. In return, the widget sends
 widget events to the context, which can trigger event callbacks that are attached to
 the widget by the application.

 @see GxWidgetEvent, GxContext
*/
class GUIX_API GxWidget
{
public:
	enum Flags
	{
		F_LOCKED      = 1 << 0, ///< Set if the widget is locked. Locked widgets do not accept input, but still receive input events.
		F_HIDDEN      = 1 << 1, ///< Set if the widget is hidden. Hidden widgets are neither ticked nor drawn.
		F_UNARRANGED  = 1 << 2, ///< Set if the widget should not be taken into account during layout arrangement.
		F_DISABLED    = F_HIDDEN | F_UNARRANGED, ///< Set if the widget is both hidden and unarranged.
		F_CHANGED     = 1 << 3, ///< Set if the state of the widget was changed as a result of user interaction.
		F_INTERACTED  = 1 << 4, ///< Set if the widget has received user interaction (even if it did not change the widget state).

		F_WBIT        = 1 << 5, ///< The first bit that can be used by specialized widget classes for flags.
		F_WMASK       = F_WBIT-1, ///< The mask that contains the flag bits for the GxWidget class.
	};

	virtual ~GxWidget();

	GxWidget();
	GxWidget(const char* id);

	// ===================================================================================
	// Overloadable functions

	/// Returns the type id of the widget, which indicates the widget class.
	/// This function is overloaded by using the GxDeclareWidgetClass macro.
	virtual const char* GetTypeId() const = 0;

	virtual void OnKeyPress(GxKeyEvent& evt)        {} ///< Called by the context when a key is pressed.
	virtual void OnKeyRelease(GxKeyEvent& evt)      {} ///< Called by the context when a key is released.
	virtual void OnTextInput(GxTextEvent& evt)      {} ///< Called by the context when text input occurs.
	virtual void OnMousePress(GxMouseEvent& evt)    {} ///< Called by the context when a mouse button is pressed.
	virtual void OnMouseRelease(GxMouseEvent& evt)  {} ///< Called by the context when a mouse button is released.
	virtual void OnMouseScroll(GxScrollEvent& evt)  {} ///< Called by the context when the mouse wheel is scrolled.

	virtual void OnLoseFocus()                      {} ///< Called on the old focus widget when a new widget grabs focus.
	virtual void OnLoseInput()                      {} ///< Called on the old input widget when a new widget grabs input.

	virtual void OnGroupEvent(GxWidget* sender)     {} ///< Called when a widget in the group has sent a group event.

	/// Called during the context tick, before display rectangles are updated, by parent widgets to adjust the policy of their
	/// children. This allows widgets to adjust their size hint, minimum size and policy types based on their properties.
	virtual void Adjust()                           {}

	/// Called during the context tick by parent widgets to update the display rectangle of their children.
	/// The display rectangle might not have the same dimensions as the size hint, this is up to the parent widget and its layout.
	virtual void SetRect(const GxRecti& rect)       {myRect = rect;}

	/// Called during the context tick to find the current mouse over widget. This function is recursively called by parent widgets
	/// on their children until a hover widget is found. Widgets without children should either return themselves or NULL.
	virtual GxWidget* FindHoverWidget(int x, int y) {return myRect.Contains(x,y) ? this : NULL;}

	/// Called by the parent widget during context tick.
	virtual void Tick(float dt)                     {} 

	/// Called by the parent widget during context draw, as a first draw pass.
	virtual void Draw()                             {} 

	/// Used by GxSizePolicyResult to determine preferred size of the widget.
	virtual GxVec2i GetPreferredSize() const        {return myPolicy->GetHintSize();}

	/// Used by GxSizePolicyResult to determine the maximum size the widget can grow to.
	virtual GxVec2i GetMinimumSize() const          {return myPolicy->GetMinSize();}

	/// Used by GxSizePolicyResult to determine the minimum size widget can shrink to.
	virtual GxVec2i GetMaximumSize() const          {return myPolicy->GetMaxSize();}

	// ===================================================================================
	// Member functions

	void SetWidgetId(const char* id); ///< Sets the widget id, which is used to identify widgets in widget events.
	void SetGroupId(const char* id); ///< Sets the group id, which is used to send group events.
	void SetToolTip(GxString text); ///< Sets the tooltip text, which is displayed on mouse over.

	void SetParent(GxWidget* parent); ///< Sets this widget's parent widget.
	void SetCallback(GxCallback* callback); ///< Sets a callback, which is called when the widget emits an event.

	void EmitEvent(const char* type); ///< Emits an event that can be received by GxCallback objects. Fills in id and type.
	void EmitEvent(const char* type, const GxVariant& value); ///< Similar to \c EmitEvent(type), but also fills in value.
	void EmitGroupEvent(); ///< Emits a group event to other widgets with the same group id.

	void GrabFocus(); ///< Makes this widget the current focus widget.
	void GrabInput(); ///< Makes this widget the current input widget.
	void ReleaseFocus(); ///< If the current focus widget is this widget, the focus widget is reset to NULL.
	void ReleaseInput(); ///< If the current input widget is this widget, the input widget is reset to NULL.

	bool IsHoverWidget() const; ///< Returns true if this widget is the current hover widget, false otherwise.
	bool IsFocusWidget() const; ///< Returns true if this widget is the current focus widget, false otherwise.
	bool IsInputWidget() const; ///< Returns true if this widget is the current input widget, false otherwise.
	bool IsLockedWidget() const; ///< Returns true if this widget or any of its parent widgets are locked, false otherwise.

	/// Returns a value between 0 and 1 that increases when the widget is the hover widget. Useful for highlight effects.
	float GetHighlightValue() const;

	GxWidget* GetParent() const; ///< Returns the widget that owns this widget, or NULL if there is none.
	GxContext* GetContext() const; ///< Returns the context this widget is part of, or NULL if there is none.
	GxCallback* GetCallback() const; ///< Returns the callback assigned to this widget, or NULL if there is none.

	GxString GetWidgetId() const; /// Returns the widget id, or an empty string if there is none.
	GxString GetGroupId() const; /// Returns the group id, or an empty string if there is none.
	GxString GetToolTip() const; /// Returns the tooltip text, or an empty string if there is none.

	// ===================================================================================
	// Inline set functions

	void SetSizeHint(int w, int h)        {myPolicy->hint.Set(w, h);}
	void SetSizeMin(int w, int h)         {myPolicy->min.Set(w, h);}
	void SetSizeMax(int w, int h)         {myPolicy->max.Set(w, h);}

	void SetAdjustHint(bool enabled)      {myPolicy->adjustHint = enabled;}
	void SetAdjustMin(bool enabled)       {myPolicy->adjustMin = enabled;}
	void SetAdjustFlags(bool enabled)     {myPolicy->adjustFlags = enabled;}

	void SetLocked(bool locked)           {myFlags.Set(F_LOCKED, locked);}
	void SetHidden(bool hidden)           {myFlags.Set(F_HIDDEN, hidden);}
	void SetUnarranged(bool unarranged)   {myFlags.Set(F_UNARRANGED, unarranged);}
	void SetDisabled(bool disabled)       {myFlags.Set(F_DISABLED, disabled);}

	// ===================================================================================
	// Inline get functions

	const GxRecti& GetRect() const        {return myRect;}

	GxSizePolicy& GetPolicy()             {return *myPolicy;}
	const GxSizePolicy& GetPolicy() const {return *myPolicy;}

	GxContextNode* GetContextNode()             {return myContextNode;}
	const GxContextNode* GetContextNode() const {return myContextNode;}

	bool IsLocked() const                 {return myFlags[F_LOCKED];}
	bool IsHidden() const                 {return myFlags[F_HIDDEN];}
	bool IsUnarranged() const             {return myFlags[F_UNARRANGED];}
	bool IsChanged() const                {return myFlags[F_CHANGED];}
	bool IsInteracted() const             {return myFlags[F_INTERACTED];}

	GxFlags GetFlags() const              {return myFlags;}

	template <typename T>
		bool HasType() const              {return GxStrCmp(GetTypeId(), T::GetClassId()) == 0;}

protected:
	GxRecti myRect;               ///< The widget's position and size in view coordinates.
	GxSizePolicy* myPolicy;       ///< The size policy data used for resizing the widget.
	GxContextNode* myContextNode; ///< Contains internal context data for the widget.
	GxFlags myFlags;              ///< Combination of widget flags. See Flags enumeration.
};

}; // namespace gui
}; // namespace guix