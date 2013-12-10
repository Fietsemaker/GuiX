/// @file
/// Contains the list layout class.

#pragma once

#include <GuiX/Layout.h>

namespace guix {
namespace layouts {

// ===================================================================================
// GxListLayout
// ===================================================================================
/** The GxListLayout class arranges widgets as a horizontal or vertical list.
 
 description.

 @see GxLayout
*/
class GUIX_API GxListLayout : public GxLayout
{
public:
	~GxListLayout();

	GxListLayout(bool isHorizontal = false);

	void Adjust();
	void Arrange(GxRecti rect);

private:
	bool myIsHorizontal;
	float myStretch;
};

}; // namespace layouts
}; // namespace guix