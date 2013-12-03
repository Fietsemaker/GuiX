#include <GuiX/Config.h>
#include <GuiX/Layout.h>

namespace guix {
namespace gui {

#define GX_LAYOUT_ITER(i) \
	for(int i=0; i<myWidgets.Size(); ++i)

#define GX_LAYOUT_ITER_R(i, condition) \
	for(int i=myWidgets.Size()-1; i>=0 && (condition); --i)

// ===================================================================================
// GxLayout
// ===================================================================================

GxLayout::~GxLayout()
{
	for(int i=0; i<myWidgets.Size(); ++i)
		delete myWidgets[i];
}

GxLayout::GxLayout()
	:mySpacing(2)
	,myOwner(NULL)
{
}

void GxLayout::Add(GxWidget* w)
{
	myWidgets.Append(w);

	w->SetParent(myOwner);
}

void GxLayout::Remove(GxWidget* widget)
{
	myWidgets.EraseValue(widget);
}

void GxLayout::RemoveAll()
{
	myWidgets.Clear();
}

GxWidget* GxLayout::FindHoverWidget(int x, int y)
{
	GxWidget* h = NULL;

	GX_LAYOUT_ITER_R(i, !h)
		if(!myWidgets[i]->IsHidden())
			h = myWidgets[i]->FindHoverWidget(x, y);

	return h;
}

bool GxLayout::IsInWidgetRect(int x, int y)
{
	bool r = false;

	GX_LAYOUT_ITER_R(i, !r)
	{
		const GxWidget* w = myWidgets[i];
		r = w->GetRect().Contains(x, y);
	}

	return r;
}

void GxLayout::Tick(float dt)
{
	GX_LAYOUT_ITER(i)
	{
		GxWidget* w = myWidgets[i];
		if(!w->IsHidden()) w->Tick(dt);
	}
}

void GxLayout::Draw()
{
	GX_LAYOUT_ITER(i)
	{
		GxWidget* w = myWidgets[i];
		if(!w->IsHidden()) w->Draw();
	}
}

void GxLayout::SetLocked(bool locked)
{
	GX_LAYOUT_ITER(i)
		myWidgets[i]->SetLocked(locked);
}

void GxLayout::SetHidden(bool hidden)
{
	GX_LAYOUT_ITER(i)
		myWidgets[i]->SetHidden(hidden);
}

void GxLayout::SetOwner(GxWidget* owner)
{
	GX_LAYOUT_ITER(i)
		myWidgets[i]->SetParent(owner);

	myOwner = owner;
}

}; // namespace gui
}; // namespace guix