/// @file
/// Contains a collection of the most commonly used widget classes.

#pragma once

#include <GuiX/Sprites.h>
#include <GuiX/Text.h>
#include <GuiX/Widget.h>

namespace guix {
namespace widgets {

// ===================================================================================
// GxSpacer
// ===================================================================================
/** The GxSpacer widget is empty space that does not expand.

 The GxSpacer widget has no appearance or interaction. It is useful for layout 
 purposes such as inserting empty space between widgets in a list or grid.

 @see GxWidget
*/
class GUIX_API GxSpacer : public GxWidget
{
public:
	GxDeclareWidgetClass(GxSpacer);

	GxSpacer();
};

// ===================================================================================
// GxSpacerH
// ===================================================================================
/** The GxSpacerH widget is empty space that expands horizontally.

 The GxSpacerH widget has no appearance or interaction. It is useful for layout 
 purposes such as inserting empty space between widgets in a horizontal list.

 @see GxWidget
*/
class GUIX_API GxSpacerH : public GxWidget
{
public:
	GxDeclareWidgetClass(GxSpacerH);

	GxSpacerH();
};

// ===================================================================================
// GxSpacerV
// ===================================================================================
/** The GxSpacerV widget is empty space that expands vertically.

 The GxSpacerV widget has no appearance or interaction. It is useful for layout 
 purposes such as inserting empty space between widgets in a vertical list.

 @see GxWidget
*/
class GUIX_API GxSpacerV : public GxWidget
{
public:
	GxDeclareWidgetClass(GxSpacerV);

	GxSpacerV();
};

}; // namespace widgets
}; // namespace guix