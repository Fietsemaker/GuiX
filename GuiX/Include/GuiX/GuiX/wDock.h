#pragma once

/// @file
/// Contains a collection of widgets related to docks.

#include <GuiX/Widget.h>

namespace guix {
namespace widgets {

class GxDockArea;
class GxDockBin;

// ===================================================================================
// GxDock
// ===================================================================================
/** The GxDock widget behaves as a dock or window that contains other widgets.

 The GxDock widget is a container widget that encapsulates other widgets inside a dock
 window. The dock window has a title bar with a text label, and a collapse/expand
 button which allows the user to hide or show the dock contents.

 When a dock is embedded inside another container such as a frame or group box, the
 dock position and size can not be modified by the user. This makes the dock behaviour
 similar to that of a group box. To allow the user to drag and resize the dock, create
 the dock through the interface of a GxDockArea widget. This is a special container
 designed for docks; it enhances the dock interaction by allowing the user to z-order
 docks, to drag and resize docks, and to dock docks by placing them in dock bins.

 @see GxDockArea
*/
class GUIX_API GxDock : public GxContainer
{
public:
	GxDeclareWidgetClass(GxDock);

	enum Flags
	{
		F_COLLAPSE    = F_WBIT << 0, ///< The dock has a collapse button.
		F_CLOSE       = F_WBIT << 1, ///< The dock has a close button.
		F_DRAG        = F_WBIT << 2, ///< The dock can be repositioned by dragging the title bar.
		F_RESIZE      = F_WBIT << 3, ///< The dock can be resized by dragging the frame.
		F_HINT_AS_MIN = F_WBIT << 4, ///< The dock's minimum size is set to its preferred size.

		F_ADJUSTABLE    = F_RESIZE | F_DRAG,
		F_ALL           = F_COLLAPSE | F_CLOSE | F_ADJUSTABLE | F_HINT_AS_MIN,
		F_ALL_BUT_CLOSE = F_ALL & ~F_CLOSE,
	};

	enum Bin
	{
		BIN_LEFT,
		BIN_RIGHT
	};

	virtual ~GxDock();

	/// Creates an empty floating dock with the flags set of F_ALL_BUT_CLOSE.
	GxDock();

	// ===================================================================================
	// Overloaded widget functions

	void Adjust();
	void SetRect(const GxRecti& rect);
	GxWidget* FindHoverWidget(int x, int y);

	void Tick(float dt);
	void Draw();

	GxVec2i GetMinimumSize() const;
	GxVec2i GetPreferredSize() const;

	// ===================================================================================
	// Functions

	/// Sets the dock's position when it is floating.
	void SetFloatingPos(int x, int y);

	/// Sets the dock's size when it is floating.
	void SetFloatingSize(int w, int h);

	/// Sets the dock's flags; the original widget flags will not be affected.
	void SetFlags(GxFlags flags);

	/// Sets the text that is displayed in the dock's title bar.
	void SetTitle(GxString text);

	/// Collapses or expands the dock.
	void Collapse(bool collapsed);

	/// Move the dock to the top of the floating docks z-order.
	void MoveToTop();

	/// If the dock is docked, it is undocked and turned into a floating dock.
	void Undock();

	/// If the dock is undocked, it is docked in the given dock bin.
	void Dock(Bin b = BIN_LEFT);

	GxString GetTitle() const; ///< Returns the dock's title text.
	bool IsFloating() const;   ///< Returns true if the dock is floating, false otherwise.

	// ===================================================================================
	// Events
 
	/// Emitted when the user clicks on the dock's close button.
	static const char* eClose();

private:
	friend class GxDockArea;
	friend class GxDockBin;

	enum Item
	{
		I_NONE,
		I_BAR,
		I_COLLAPSE,
		I_CLOSE,
		I_FRAME,
		I_CONTENTS,
	};
	
	GxVec2i myGetResizeDir(int x, int y) const;
	void myClampFloatSize(GxVec2i dir);
	void myClampFloatPos(GxVec2i view);
	Item myGetItemAt(int x, int y) const;

	GxDockArea* myOwner;
	GxList<Item> myButtons;
	GxRecti myFloatRect;
	GxString myTitle;
	GxFlags myState;
};

// ===================================================================================
// GxDockArea
// ===================================================================================
/** The GxDockArea widget is a special container for dock widgets.

 The GxDockArea widget is used to create an area in which the user can manipulate dock
 widgets. The dock area does not have any visual appearance by itself, but rectangle
 of the dock area functions as the available space which the user can use to move
 around the floating docks inside it. The edges of the dock area can also be set up
 as docking bins, into which the user can drag floating docks in order to dock them.

 Note that even though the dock area contains other widgets (docks), it is not a
 container widget and it does not use a layout object. This is because the dock area
 is a special container only used for docks. Docks can only be added to it through the
 AddDock() function, which also establishes a relation between the dock and the dock
 area.

 @see GxDock, GxWidget
*/
class GUIX_API GxDockArea : public GxWidget
{
public:
	GxDeclareWidgetClass(GxDockArea);

	/// Enumeration of dock bins that can be enabled or disabled.
	enum DockBins
	{
		DOCK_NONE = 0, ///< No docking bins.

		DOCK_L = 1 << 0, ///< Left side dock bin.
		DOCK_R = 1 << 1, ///< Right side dock bin.

		DOCK_LR  = DOCK_L | DOCK_R, ///< Left and right side dock bins.
	};

	virtual ~GxDockArea();

	/// Creates an empty docking area.
	GxDockArea();

	// ===================================================================================
	// Overloaded widget functions

	void OnMousePress(GxMouseEvent& evt);
	void OnMouseRelease(GxMouseEvent& evt);

	void Adjust();
	void SetRect(const GxRecti& rect);
	GxWidget* FindHoverWidget(int x, int y);

	void Tick(float dt);
	void Draw();

	// ===================================================================================
	// Functions

	/// Adds a new dock to the dock area. Returns a pointer to the new dock.
	GxDock* AddDock();

	/// Deletes a dock from the dock area.
	// The dock must have been created through the AddDock() function.
	void DeleteDock(GxDock* dock);

	/// Enables the docking bins specified by bins, and disables the others.
	void SetDockBins(DockBins bins);

protected:
	friend class GxDock;

	void myStartPress(GxDock* d);
	void myStartDrag(GxDock* d, int x, int y);
	void myStartResize(GxDock* d, int x, int y);
	void myMoveToTop(GxDock* d);
	void myUndock(GxDock* d);
	void myDock(GxDock* d, int bin);
	int myGetBinAt(int x, int y) const;

	typedef GxList<GxDock*> DockList;

	DockList myDocks;
	GxDockBin* myBins[2];
	GxDock* myHoverDock;
	GxDock* myFocusDock;
	GxVec2i myResizeDir;
	GxRecti myActionDims;
	GxVec2i myActionPos;
	int myActionType;
	float myDragHl;
};

}; // namespace widgets
}; // namespace guix