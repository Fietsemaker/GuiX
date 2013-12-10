/// @file
/// Contains a collection of the most commonly used widget classes.

#pragma once

#include <GuiX/Sprites.h>
#include <GuiX/Text.h>

#include <GuiX/Widget.h>

namespace guix {
namespace widgets {

// ===================================================================================
// GxSeperatorH
// ===================================================================================
/** The GxSeperatorH widget displays a horizontal seperation line between widgets.

 The GxSeperatorAbstract widget is used to create a horizontal line that seperates
 groups of widgets. It has no user interaction and is a visual widget only.

 @see GxWidget
*/
class GUIX_API GxSeperatorH : public GxWidget
{
public:
	GxDeclareWidgetClass(GxSeperatorH);

	GxSeperatorH();

	void Draw();
};

// ===================================================================================
// GxSeperatorV
// ===================================================================================
/** The GxSeperatorV widget displays a vertical seperation line between widgets.

 The GxSeperatorAbstract widget is used to create a vertical line that seperates
 groups of widgets. It has no user interaction and is a visual widget only.

 @see GxWidget
*/
class GUIX_API GxSeperatorV : public GxWidget
{
public:
	GxDeclareWidgetClass(GxSeperatorV);

	GxSeperatorV();

	void Draw();
};

}; // namespace widgets
}; // namespace guix