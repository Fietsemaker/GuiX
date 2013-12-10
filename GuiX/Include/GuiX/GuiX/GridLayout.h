/// @file
/// Contains the list layout class.

#pragma once

#include <GuiX/Layout.h>

namespace guix {
namespace layouts {

// ===================================================================================
// GxListLayout
// ===================================================================================
/** The GxGridLayout class arranges widgets as a grid with a fixed column count.
 
 description.

 @see GxLayout
*/
class GUIX_API GxGridLayout : public GxLayout
{
public:
	~GxGridLayout();

	GxGridLayout(int columnCount);

	void Add(GxWidget* widget);
	void Add(GxWidget* widget, int colSpan);
	void Add(GxString label, GxWidget* widget);
	void Skip(int colSpan);

	void Remove(GxWidget* widget);
	void RemoveAll();

	void Adjust();
	void Arrange(GxRecti rect);

	int GetRowCount() const;
	int GetColumnCount() const;

private:
	struct Data { int min, hint, size; GxFlags flags; };

	void myAdd(GxWidget* widget, int span);
	GxVec2i myGetSize(bool min) const;

	GxList<GxWidget*> myGrid;
	GxList<Data> myCols;
	GxList<Data> myRows;
	GxVec2f myStretch;
	int myAddIndex;
};

}; // namespace layouts
}; // namespace guix