#include <GuiX/Config.h>

#include <GuiX/GridLayout.h>
#include <GuiX/wLabel.h>

#include <Src/GuiUtils.h>

namespace guix {
namespace layouts {

// ===================================================================================
// GxGridLayout
// ===================================================================================

GxGridLayout::~GxGridLayout()
{
}

GxGridLayout::GxGridLayout(int columnCount)
	:myCols(GxClamp(columnCount, 1, 256), Data())
	,myAddIndex(0)
{
	myPolicyFlagsH = GX_SP_RESIZE;
	myPolicyFlagsV = GX_SP_RESIZE;
}

void GxGridLayout::Remove(GxWidget* widget)
{
	GxLayout::Remove(widget);

	GxWidget** grid = myGrid.Edit();
	for(int i=0; i<myGrid.Size(); ++i)
		if(grid[i] == widget)
			grid[i] = NULL;
}

void GxGridLayout::RemoveAll()
{
	GxLayout::RemoveAll();

	myAddIndex = 0;
	myGrid.Clear();
	myRows.Clear();
}

void GxGridLayout::Add(GxWidget* widget)
{
	myAdd(widget, 1);
}

void GxGridLayout::Add(GxWidget* widget, int columnSpan)
{
	myAdd(widget, columnSpan);
}

void GxGridLayout::Add(GxString label, GxWidget* widget)
{
	myAdd(new GxLabel(label), 1);
	myAdd(widget, myCols.Size());
}

void GxGridLayout::Skip(int colSpan)
{
	myAdd(NULL, colSpan);
}

void GxGridLayout::Adjust()
{
	int numRows = myRows.Size();
	int numCols = myCols.Size();
	Data* rowData = myRows.Edit();
	Data* colData = myCols.Edit();

	memset(rowData, 0, sizeof(Data) * numRows);
	memset(colData, 0, sizeof(Data) * numCols);

	// Store the width and height of all rows and columns.
	GxWidget* const* it = myGrid.Data();
	GxWidget* w = NULL;

	for(int i=0; i<numRows; ++i)
	{
		Data& row = rowData[i];
		for(int j=0; j<numCols; ++j, ++it)
		{
			Data& col = colData[j];
			if(w != *it && *it)
			{
				w = *it;
				w->Adjust();
				if(!w->IsUnarranged())
				{
					GxSizePolicyResult pol(w);

					col.min = GxMax(col.min, pol.min.x);
					row.min = GxMax(row.min, pol.min.y);

					col.hint = GxMax(col.hint, pol.hint.x);
					row.hint = GxMax(row.hint, pol.hint.y);

					col.flags.Set(pol.flagsH.bits);
					row.flags.Set(pol.flagsV.bits);
				}
			}
		}
	}

	GxVec2i min(0, 0);
	GxVec2i hint(0, 0);
	GxVec2i flags(GX_SP_RESIZE, GX_SP_RESIZE);
	GxVec2f stretch(0, 0);

	// Sum the resize properties of the columns.
	for(int i=0; i<numCols; ++i)
	{
		min.x += colData[i].min;
		hint.x += colData[i].hint;
		flags.x |= colData[i].flags.bits;

		if(colData[i].flags[GX_SPF_EXPAND])
			stretch.x += 1.f;
	}

	// Sum the resize properties of the rows.
	for(int i=0; i<numRows; ++i)
	{
		min.y += rowData[i].min;
		hint.y += rowData[i].hint;
		flags.y |= rowData[i].flags.bits;

		if(rowData[i].flags[GX_SPF_EXPAND])
			stretch.y += 1.f;
	}

	// Apply margins and spacing to the sizes.
	int mx = myMargin.l + myMargin.r + GxMax(0, numCols-1) * mySpacing;
	int my = myMargin.t + myMargin.b + GxMax(0, numRows-1) * mySpacing;
	min.Add(mx, my);
	hint.Add(mx, my);

	// Store the resulting values.
	myMinimumSize = min;
	myPreferredSize = hint;
	myPolicyFlagsH = flags.x;
	myPolicyFlagsV = flags.y;
	myStretch = stretch;
}

void GxGridLayout::Arrange(GxRecti rect)
{
	int numRows = myRows.Size();
	int numCols = myCols.Size();
	Data* rowData = myRows.Edit();
	Data* colData = myCols.Edit();

	GxVec2i hint = myPreferredSize;
	GxVec2i min = myMinimumSize;

	// If there is extra space available for widgets, calculate the expand rate.
	GxVec2f expandRate(0, 0);
	if(rect.w > hint.x && myStretch.x > 0)
	{
		float extraSpace = (float)(rect.w - hint.x);
		expandRate.x = extraSpace / myStretch.x;
	}
	if(rect.h > hint.y && myStretch.y > 0)
	{
		float extraSpace = (float)(rect.h - hint.y);
		expandRate.y = extraSpace / myStretch.y;
	}

	// If there is not enough space for the preferred size, calculate the shrink factor.
	GxVec2f shrinkFactor(0, 0);
	if(rect.w < hint.x)
	{
		shrinkFactor.x = GxLerpFactor((float)hint.x, (float)min.x, (float)rect.w);
		shrinkFactor.x = GxClamp(shrinkFactor.x, 0.f, 1.f);
	}
	if(rect.h < hint.y)
	{
		shrinkFactor.y = GxLerpFactor((float)hint.y, (float)min.y, (float)rect.h);
		shrinkFactor.y = GxClamp(shrinkFactor.y, 0.f, 1.f);
	}

	// Calculate the size of all columns.
	for(int i=0; i<numCols; ++i)
	{
		Data& col = colData[i];

		float w = (float)col.hint;

		if(col.flags[GX_SPF_SHRINK] && shrinkFactor.x > 0)
			w = GxLerp(w, (float)col.min, shrinkFactor.x);

		if(col.flags[GX_SPF_EXPAND] && expandRate.x > 0)
			w += expandRate.x;

		col.size = GxInt(w);
	}

	// Calculate the size of all rows.
	for(int i=0; i<numRows; ++i)
	{
		Data& row = rowData[i];

		float h = (float)row.hint;

		if(row.flags[GX_SPF_SHRINK] && shrinkFactor.y > 0)
			h = GxLerp(h, (float)row.min, shrinkFactor.y);

		if(row.flags[GX_SPF_EXPAND] && expandRate.y > 0)
			h += expandRate.y;

		row.size = GxInt(h);
	}

	// Assign rectangles to all children.
	GxWidget* const* it = myGrid.Data();
	GxWidget* w = NULL;

	rect.Shrink(myMargin);
	
	float y = (float)rect.y;
	for(int i=0; i<numRows; ++i)
	{
		float x = (float)rect.x;
		const Data& row = rowData[i];
		for(int j=0; j<numCols; ++j, ++it)
		{
			const Data& col = colData[j];
			if(w != *it && *it)
			{
				w = *it;
				if(!w->IsUnarranged())
				{
					GxSizePolicyResult pol(w);

					int width = col.size;
					for(int k=j+1; k<numCols && it[k-j] == w; ++k)
						width += colData[k].size + mySpacing;

					GxRecti r(GxInt(x), GxInt(y), width, row.size);
					r.w = GxClamp(r.w, pol.min.x, pol.max.x);
					r.h = GxClamp(r.h, pol.min.y, pol.max.y);
					w->SetRect(r);
				}
			}
			x += (float)(col.size + mySpacing);
		}
		y += (float)(row.size + mySpacing);
	}
}

int GxGridLayout::GetRowCount() const
{
	return myRows.Size();
}

int GxGridLayout::GetColumnCount() const
{
	return myCols.Size();
}

void GxGridLayout::myAdd(GxWidget* widget, int span)
{
	if(widget) GxLayout::Add(widget);

	int numCols = myCols.Size();
	int gridSize = myGrid.Size();

	// If we are adding at the end of the grid, we need a new row.
	if(myAddIndex == gridSize)
	{
		gridSize += numCols;
		myGrid.Resize(gridSize, NULL);
		Data data = {0, 0, 0};
		myRows.Append(data);
	}

	// GxClamp the column span to make sure the widget fits within the current row.
	span = GxMin(span, gridSize - myAddIndex);

	// Add the widget to the grid, spanning span number of columns.
	GxWidget** grid = myGrid.Edit() + myAddIndex;
	for(int i=0; i<span; ++i)
		grid[i] = widget;

	myAddIndex += span;
}

}; // namespace layouts
}; // namespace guix