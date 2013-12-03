#include <GuiX/Config.h>
#include <GuiX/FreeLayout.h>

#include <Src/GuiUtils.h>

namespace guix {
namespace layouts {

// ===================================================================================
// GxFreeLayout
// ===================================================================================

GxFreeLayout::~GxFreeLayout()
{
}

GxFreeLayout::GxFreeLayout()
{
	myPolicyFlagsH = GX_SPF_GROW;
	myPolicyFlagsV = GX_SPF_GROW;
}

void GxFreeLayout::Remove(GxWidget* widget)
{
	for(int i=myItems.Size()-1; i>=0; --i)
		if(myItems[i].widget == widget)
			myItems.Erase(i);

	GxLayout::Remove(widget);
}

void GxFreeLayout::RemoveAll()
{
	myItems.Clear();

	GxLayout::RemoveAll();
}

void GxFreeLayout::Add(GxWidget* widget)
{
	GxLayout::Add(widget);

	Item item = {widget, GxVec2i(0, 0)};
	myItems.Append(item);
}

void GxFreeLayout::Add(GxWidget* widget, int x, int y)
{
	GxLayout::Add(widget);

	Item item = {widget, GxVec2i(x, y)};
	myItems.Append(item);
}

void GxFreeLayout::Add(GxWidget* widget, int x, int y, int w, int h)
{
	GxLayout::Add(widget);

	widget->SetAdjustHint(false);
	widget->SetSizeHint(w, h);

	Item item = {widget, GxVec2i(x, y)};
	myItems.Append(item);
}

void GxFreeLayout::Adjust()
{
	GxVec2i dim(0, 0);

	for(int i=0; i<myItems.Size(); ++i)
	{
		GxWidget* w = myItems[i].widget;
		w->Adjust();
		if(w->IsUnarranged()) continue;

		GxSizePolicyResult pol(w);
		GxVec2i pos = myItems[i].pos;

		dim.x = GxMax(dim.x, pos.x + pol.hint.x);
		dim.y = GxMax(dim.y, pos.y + pol.hint.y);
	}

	dim.x += myMargin.l + myMargin.r;
	dim.y += myMargin.t + myMargin.b;

	myPreferredSize = dim;
	myMinimumSize = dim;
}

void GxFreeLayout::Arrange(GxRecti rect)
{
	int dx = rect.x + myMargin.l;
	int dy = rect.y + myMargin.t;

	for(int i=0; i<myItems.Size(); ++i)
	{
		GxWidget* w = myItems[i].widget;

		GxSizePolicyResult pol(w);
		GxVec2i pos = myItems[i].pos;

		GxRecti r(dx + pos.x, dy + pos.y, 0, 0);
		r.w = GxClamp(pol.hint.x, pol.min.x, pol.max.x);
		r.h = GxClamp(pol.hint.y, pol.min.y, pol.max.y);

		w->SetRect(r);
	}
}

}; // namespace layouts
}; // namespace guix