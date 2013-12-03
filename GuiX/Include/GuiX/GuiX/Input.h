/// @file
/// Contains enums and classes related to mouse and keyboard input.

#pragma once

#include <GuiX/Common.h>
#include <GuiX/List.h>
#include <GuiX/String.h>

namespace guix {
namespace core {

/// Enumeration of types of mouse cursor images.
enum GxCursorImage
{
	GX_CI_ARROW,      ///< Default arrow pointer.
	GX_CI_CARET,      ///< A text editing caret.
	GX_CI_HAND,       ///< Indicates a press action.
	GX_CI_DRAG,       ///< Indicates a dragging action.
	GX_CI_SIZE_WE,    ///< Indicates a horizontal resizing action.
	GX_CI_SIZE_NS,    ///< Indicates a vertical resizing action.
	GX_CI_SIZE_NWSE,  ///< Indicates a NW to SE diagonal resizing action.
	GX_CI_SIZE_NESW,  ///< Indicates a NE to SW diagonal resizing action.

	GX_CI_COUNT,      ///< The total number of mouse cursor images.
};

// ===================================================================================
// Keyboard Input
// ===================================================================================

/// Enumeration of key codes.
enum GxKeyCode
{
	GX_KC_NONE = 0,

	GX_KC_A = 'a',
	GX_KC_B = 'b',
	GX_KC_C = 'c',
	GX_KC_D = 'd',
	GX_KC_E = 'e',
	GX_KC_F = 'f',
	GX_KC_G = 'g',
	GX_KC_H = 'h',
	GX_KC_I = 'i',
	GX_KC_J = 'j',
	GX_KC_K = 'k',
	GX_KC_L = 'l',
	GX_KC_M = 'm',
	GX_KC_N = 'n',
	GX_KC_O = 'o',
	GX_KC_P = 'p',
	GX_KC_Q = 'q',
	GX_KC_R = 'r',
	GX_KC_S = 's',
	GX_KC_T = 't',
	GX_KC_U = 'u',
	GX_KC_V = 'v',
	GX_KC_W = 'w',
	GX_KC_X = 'x',
	GX_KC_Y = 'y',
	GX_KC_Z = 'z',

	GX_KC_0 = '0',
	GX_KC_1 = '1',
	GX_KC_2 = '2',
	GX_KC_3 = '3',
	GX_KC_4 = '4',
	GX_KC_5 = '5',
	GX_KC_6 = '6',
	GX_KC_7 = '7',
	GX_KC_8 = '8',
	GX_KC_9 = '9',

	GX_KC_ACC_GRAVE = '`',
	GX_KC_DASH      = '-',
	GX_KC_EQUAL     = '=',
	GX_KC_BRACKET_L = '[',
	GX_KC_BRACKET_R = ']',
	GX_KC_SEMICOLON = ';',
	GX_KC_QUOTE     = '\'',
	GX_KC_BACKSLASH = '\\',
	GX_KC_COMMA     = ',',
	GX_KC_PERIOD    = '.',
	GX_KC_SLASH     = '/',
	GX_KC_SPACE     = ' ',

	// Starting at 128 prevent any clashes with ascii characters.
	GX_KC_ESCAPE = 128,

	GX_KC_CTRL_L,   ///< Left control key.
	GX_KC_CTRL_R,   ///< Right control key.
	GX_KC_ALT_L,    ///< Left alt key.
	GX_KC_ALT_R,    ///< Right alt key.
	GX_KC_SHIFT_L,  ///< Left shift key.
	GX_KC_SHIFT_R,  ///< Right shift key.
	GX_KC_SYSTEM_L, ///< Left system key.
	GX_KC_SYSTEM_R, ///< Right system key.

	GX_KC_TAB,
	GX_KC_CAPS,
	GX_KC_MENU,
	GX_KC_RETURN,
	GX_KC_BACKSPACE,
	
	GX_KC_PAGE_UP,
	GX_KC_PAGE_DOWN,
	GX_KC_HOME,
	GX_KC_END,
	GX_KC_INSERT,
	GX_KC_DELETE,

	GX_KC_PRINT_SCREEN,
	GX_KC_SCROLL_LOCK,
	GX_KC_PAUSE,

	GX_KC_LEFT,  ///< Left arrow.
	GX_KC_RIGHT, ///< Right arrow.
	GX_KC_UP,    ///< Up arrow.
	GX_KC_DOWN,  ///< Down arrow.

	GX_KC_NUM_LOCK,
	GX_KC_DEL,

	GX_KC_NUMPAD_DIVIDE,   ///< Numpad /
	GX_KC_NUMPAD_MULTIPLY, ///< Numpad *
	GX_KC_NUMPAD_SUBTRACT, ///< Numpad -
	GX_KC_NUMPAD_ADD,      ///< Numpad +

	GX_KC_NUMPAD_0,
	GX_KC_NUMPAD_1,
	GX_KC_NUMPAD_2,
	GX_KC_NUMPAD_3,
	GX_KC_NUMPAD_4,
	GX_KC_NUMPAD_5,
	GX_KC_NUMPAD_6,
	GX_KC_NUMPAD_7,
	GX_KC_NUMPAD_8,
	GX_KC_NUMPAD_9,

	GX_KC_F1,
	GX_KC_F2,
	GX_KC_F3,
	GX_KC_F4,
	GX_KC_F5,
	GX_KC_F6,
	GX_KC_F7,
	GX_KC_F8,
	GX_KC_F9,
	GX_KC_F10,
	GX_KC_F11,
	GX_KC_F12,
	GX_KC_F13,
	GX_KC_F14,
	GX_KC_F15,

	GX_KC_SIZE ///< The minimum size of an array that can be indexed with all key codes.
};

/// Enumeration of key flags to check if a modifier key is currently down.
enum GxKeyFlags
{
	// Flags.
	GX_KF_CTRL_L  = 0x01, ///< Left control key.
	GX_KF_CTRL_R  = 0x02, ///< Right control key.
	GX_KF_ALT_L   = 0x04, ///< Left alt key.
	GX_KF_ALT_R   = 0x08, ///< Right alt key.
	GX_KF_SHIFT_L = 0x10, ///< Left shift key.
	GX_KF_SHIFT_R = 0x20, ///< Right shift key.

	// Flag combinations.
	GX_KF_CTRL    = GX_KF_CTRL_L  | GX_KF_CTRL_R,  ///< Any control key.
	GX_KF_ALT     = GX_KF_ALT_L   | GX_KF_ALT_R,   ///< Any alt key.
	GX_KF_SHIFT   = GX_KF_SHIFT_L | GX_KF_SHIFT_R, ///< Any shift key.
};

/// GxKeyEvent contains data from a keyboard input event.
struct GxKeyEvent
{
	GxKeyCode key; ///< The keycode of the key for which the event happend.
	int flags;     ///< A combination of GxKeyFlags of modifier keys that were down when the event occured.
	bool handled;  ///< A flag for signalling that the event was handled.
};

/// GxTextEvent contains data from a text input event.
struct GxTextEvent
{
	GxString text; ///< String containing the text that was input.
	bool handled;  ///< A flags for signalling that the event was handled.
};

// ===================================================================================
// Mouse Input
// ===================================================================================

/// Enumeration of mouse buttons.
enum GxMouseCode
{
	GX_MC_NONE = 0,

	GX_MC_LEFT,   ///< Left mouse button.
	GX_MC_MIDDLE, ///< Middle mouse button.
	GX_MC_RIGHT,  ///< Right mouse button.

	GX_MC_SIZE ///< The minimum size of an array that can be indexed with all mouse buttons.
};

/// GxMouseEvent contains data from a mouse input event.
struct GxMouseEvent
{
	GxMouseCode button; ///< The mouse button for which the event happend.
	int x, y;           ///< The position of the mouse cursor at the time the event occured.
	bool handled;       ///< A flag for signalling that the event was handled.
};

/// GxScrollEvent contains data from a mouse scroll event.
struct GxScrollEvent
{
	bool up;      ///< Set to true if the scrolling direction was up.
	bool handled; ///< A flag for signalling that the event was handled.
};

/// GxFileDropEvent contains data from a file drop event.
struct GxFileDropEvent
{
	GxList<GxString> files; ///< List of paths to the files that were dropped.
	GxVec2i point;          ///< Position in the window on which the files were dropped.
	bool handled;           ///< A flag for signalling that the event was handled.
};

// ===================================================================================
// GxInputListener
// ===================================================================================
/** The GxInputListener class receives input events from the GxInput singleton.

 Classes that inherit GxInputListener can be attached to the GxInput singleton as
 listeners. When the GxInput singleton receives input events, it sends them to all 
 its listeners in the order they were added. Listeners can use the event's handled
 field to signal to other listeners that the event was already handled.

 @see GxInput
*/
class GxInputListener
{
public:
	virtual void OnKeyPress(GxKeyEvent& evt)       {}
	virtual void OnKeyRelease(GxKeyEvent& evt)     {}
	virtual void OnTextInput(GxTextEvent& evt)     {}
	virtual void OnMousePress(GxMouseEvent& evt)   {}
	virtual void OnMouseRelease(GxMouseEvent& evt) {}
	virtual void OnMouseScroll(GxScrollEvent& evt) {}
	virtual void OnFileDrop(GxFileDropEvent& evt)  {}
	virtual void OnWindowInactive()                {}
};

// ===================================================================================
// GxInput
// ===================================================================================
/** The GxInput class processes and manages keyboard and mouse input.

 The GxInput class provides an interface for handling keyboard and mouse input events.
 It keeps track of which keys and mouse buttons are pressed, and the mouse position.
 It also forwards input events to all the registered input listener objects.

 @see GxInputListener
*/
class GUIX_API GxInput
{
public:
	virtual ~GxInput();

	/// Returns the input singleton.
	static GxInput* Get();

	/// Adds a listener to the list of listeners that receive input events.
	virtual void AddListener(GxInputListener* listener) = 0;

	/// Removes a listener from the list of listeners that receive input events.
	virtual void RemoveListener(GxInputListener* listener) = 0;

	/// Sends a key press event to all listeners, and flags the key as down.
	/// Returns true if the event was handled by a listener.
	virtual bool OnKeyPress(GxKeyCode key) = 0;

	/// Sends a key release event to all listeners, and resets the key down flag.
	/// Returns true if the event was handled by a listener.
	virtual bool OnKeyRelease(GxKeyCode key) = 0;

	/// Sends a text input event to all listeners.
	/// Returns true if the event was handled by a listener.
	virtual bool OnTextInput(const GxString& text) = 0;

	/// Sends a mouse button press event to all listeners, and flags the mouse button as down.
	/// Returns true if the event was handled by a listener.
	virtual bool OnMousePress(GxMouseCode button, int x, int y) = 0;

	/// Sends a mouse button release event to all listeners, and resets the mouse button down flag.
	/// Returns true if the event was handled by a listener.
	virtual bool OnMouseRelease(GxMouseCode button, int x, int y) = 0;

	/// Sends a mouse wheel scroll event to all listeners.
	/// Returns true if the event was handled by a listener.
	virtual bool OnMouseScroll(bool up) = 0;

	/// Send a file drop event to all listeners.
	/// Returns true if the event was handled by a listener.
	virtual bool OnFileDrop(GxList<GxString> files, GxVec2i point) = 0;

	/// Releases all currently pressed mouse buttons and keys.
	/// Sends a window inactive event to all listeners.
	virtual void OnWindowInactive() = 0;

	/// Sets the position of the mouse cursor to (x, y).
	virtual void SetMousePos(int x, int y) = 0;

	/// Returns true if the key is currently down.
	virtual bool IsKeyDown(GxKeyCode key) = 0;

	/// Returns true if the mouse button is currently down.
	virtual bool IsMouseDown(GxMouseCode button) = 0;

	/// Returns true if the current position of the mouse cursor is inside the rectangle.
	virtual bool IsMouseInRect(int x, int y, int w, int h) = 0;

	/// Returns true if the current position of the mouse cursor is inside the rectangle.
	virtual bool IsMouseInRect(const GxRecti& rect) = 0;

	/// Returns the current position of the mouse cursor.
	virtual const GxVec2i& GetMousePos() = 0;
};

}; // namespace core
}; // namespace guix