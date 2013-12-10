/// @file
/// Contains the list layout class.

#pragma once

#include <GuiX/Layout.h>
#include <GuiX/List.h>

namespace guix {
namespace layouts {

// ===================================================================================
// GxFreeLayout
// ===================================================================================
/** The GxFreeLayout class arranges widgets freely through manual positioning.
 
 description.

 @see GxLayout
*/
class GUIX_API GxFreeLayout : public GxLayout
{
public:
	~GxFreeLayout();

	GxFreeLayout();
	
	void Remove(GxWidget* widget);
	void RemoveAll();

	void Add(GxWidget* widget);
	void Add(GxWidget* widget, int x, int y);
	void Add(GxWidget* widget, int x, int y, int w, int h);

	void Adjust();
	void Arrange(GxRecti rect);

private:
	struct Item { GxWidget* widget; GxVec2i pos; };

	GxList<Item> myItems;
};

}; // namespace layouts
}; // namespace guix