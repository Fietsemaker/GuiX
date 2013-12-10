#pragma once

/// @file
/// Contains a collection of widget classes that contain other widgets.

#include <GuiX/Widget.h>
#include <GuiX/Container.h>
#include <GuiX/wScrollbar.h>

namespace guix {
namespace widgets {

// ===================================================================================
// GxFrame
// ===================================================================================
/** The GxFrame widget behaves as a rectangular area that contains other widgets.

 The GxFrame widget is used to create an area that contains other widgets. It is a
 very basic extension of the container widget class and does not have any visual
 appearance. The frame widget comes in three flavors: the GxFrame, which has a free
 layout as the default layout, the GxFrameV, which has a vertical list layout as the
 default layout, and the GxFrameH, which has a horizontal list layout as the default
 layout.

 It is also possible to create a frame widget with a custom layout object. When this
 happens, the frame takes ownership of the layout and will makes sure the layout is
 deleted when the frame is destroyed or when a new layout is assigned to the frame.

 @see GxContainer
*/
class GUIX_API GxFrame : public GxContainer
{
public:
	GxDeclareWidgetClass(GX_WT_FRAME);

	virtual ~GxFrame();

	/// Creates an empty frame with a free layout.
	GxFrame();

	/// Creates an empty frame with a custom layout.
	/// The frame takes ownership of the given layout.
	GxFrame(GxLayout* layout);

	// ===================================================================================
	// Overloaded widget functions

	void Adjust();
	void SetRect(const GxRecti& rect);
	GxWidget* FindHoverWidget(int x, int y);

	void Tick(float dt);
	void Draw();
};

// ===================================================================================
// GxFrameH
// ===================================================================================
/** The GxFrameH widget inherits GxFrame, but is a horizontal list by default.

 GxFrameH functions exactly like GxFrame, but the default layout is set to a
 horizontal list layout instead of a free layout.

 @see GxFrame
*/
class GUIX_API GxFrameH : public GxFrame
{
public:
	GxDeclareWidgetClass(GxFrameH);

	GxFrameH();
};

// ===================================================================================
// GxFrameV
// ===================================================================================
/** The GxFrameV widget inherits GxFrame, but is a vertical list by default.

 GxFrameV functions exactly like GxFrame, but the default layout is set to a
 vertical list layout instead of a free layout.

 @see GxFrame
*/
class GUIX_API GxFrameV : public GxFrame
{
public:
	GxDeclareWidgetClass(GxFrameV);

	GxFrameV();
};

}; // namespace widgets
}; // namespace guix