#include <GuiX/Config.h>

#include <GuiX/Core.h>
#include <GuiX/Draw.h>
#include <GuiX/Context.h>
#include <GuiX/ListLayout.h>

#include <GuiX/wTabs.h>

#include <Src/GuiUtils.h>

namespace guix {
namespace widgets {

namespace {

static const int tabH = 24;
static const int tabW = 64;
static const int tabMinW = 32;

}; // anonymous namespace

// ===================================================================================
// GxTabs
// ===================================================================================

GxTabs::GxTabs()
	:myHoverTab(-1)
	,myMargin(2,2,2,2)
{
	myPolicy->flagsH = GX_SP_EXPAND;
	myPolicy->flagsV = GX_SP_EXPAND;
	myPolicy->min.Set(24, 24);
	myPolicy->hint.Set(96, 24);
}

GxTabs::~GxTabs()
{
}

void GxTabs::Adjust()
{
	for(int i=0; i<myTabs.Size(); ++i)
		myTabs[i]->GetLayout().Adjust();

	const int tabCount = GxMax(myTabs.Size(), 1);
	GxSizePolicy& pol = GetPolicy();

	if(pol.adjustMin)
	{
		pol.min.Set(tabCount * tabMinW, tabH);
		for(int i=0; i<myTabs.Size(); ++i)
		{
			const GxVec2i size = myTabs[i]->GetLayout().GetMinimumSize();
			pol.min.x = GxMax(pol.min.x, size.x);
			pol.min.y = GxMax(pol.min.y, size.y + tabH);
		}
	}

	if(pol.adjustHint)
	{
		pol.hint.Set(tabCount * tabW, tabH);
		for(int i=0; i<myTabs.Size(); ++i)
		{
			const GxVec2i size = myTabs[i]->GetLayout().GetPreferredSize();
			pol.hint.x = GxMax(pol.hint.x, size.x);
			pol.hint.y = GxMax(pol.hint.y, size.y + tabH);
		}
	}
}

void GxTabs::SetRect(const GxRecti& rect)
{
	myRect = rect;
	for(int i=0; i<myTabs.Size(); ++i)
	{
		GxRecti r = rect;
		r.Shrink(0, tabH, 0, 0);
		myTabs[i]->GetLayout().Arrange(r);
	}
}

void GxTabs::OnMousePress(GxMouseEvent& evt)
{
	if(IsHoverWidget())
	{
		if(evt.button == GX_MC_LEFT && !evt.handled && !IsLockedWidget())
		{
			GrabFocus();
			int hover = myGetTabAtPos(evt.x, evt.y);
			if(hover >= 0 && myTabs[hover] != mySelectedTab)
			{
				SelectTab(hover);
				EmitEvent(eChanged(), hover);
			}
		}	
		evt.handled = true;
	}
}

void GxTabs::OnMouseRelease(GxMouseEvent& evt)
{
	if(evt.button == GX_MC_LEFT && IsFocusWidget())
	{
		ReleaseFocus();
	}
}

GxWidget* GxTabs::FindHoverWidget(int x, int y)
{
	GxTab* t = mySelectedTab;
	GxWidget* w = t ? t->GetLayout().FindHoverWidget(x, y) : NULL;
	return w ? w : GxWidget::FindHoverWidget(x, y);
}

void GxTabs::Tick(float dt)
{
	GxVec2i mpos = GxInput::Get()->GetMousePos();
	myHoverTab = myGetTabAtPos(mpos.x, mpos.y);

	if(mySelectedTab)
		mySelectedTab->GetLayout().Tick(dt);
}

void GxTabs::Draw()
{
	GxDraw* draw = GxDraw::Get();
	GxStyle& style = *GxStyle::Get();

	bool lock = IsLockedWidget();

	// Draw frame
	GxRecti r(myRect.x, myRect.y, myRect.w, tabH);
	style.Bar(r);

	// Draw tabs	
	r = GetRect();
	for(int i=0; i<myTabs.Size(); ++i)
	{
		GxTab* tab = GetTab(i);
		GxRecti tr = myGetTabRect(i);
		if(i == myHoverTab || tab->IsSelected())
			style.Tab(tr, i == myHoverTab && !lock, tab->IsSelected());

		style.Label(tr, GX_TA_CENTER, GX_TA_MIDDLE, tab->GetText(), lock);
	}

	if(mySelectedTab) 
		mySelectedTab->GetLayout().Draw();
}

GxTab* GxTabs::AddTab()
{
	GxTab* tab = GxTabContainer::AddTab();
	tab->GetLayout().SetMargin(myMargin);
	return tab;
}

void GxTabs::SetTabMargin(const GxMargini& margin)
{
	myMargin = margin;
}

GxRecti GxTabs::myGetTabRect(int index) const
{
	GxRecti r(myRect.x, myRect.y, 0, tabH);
	if(index < 0 || index >= myTabs.Size()) return r;

	int x0 = r.x + (index + 0) * myRect.w / myTabs.Size();
	int x1 = r.x + (index + 1) * myRect.w / myTabs.Size();
	if(index == myTabs.Size() - 1) x1 = r.x + myRect.w;
	r.x = x0;
	r.w = x1 - x0;
	return r;
}

int GxTabs::myGetTabAtPos(int x, int y) const
{
	for(int i=0; i<myTabs.Size(); ++i)
		if(myGetTabRect(i).Contains(x, y))
			return i;

	return -1;
}

int GxTabs::myGetHoverTab() const
{
	return (IsHoverWidget() ? myHoverTab : -1);
}

// ===================================================================================
// Widget events
// ===================================================================================

GxDefineWidgetEvent (GxTabs, eChanged)

}; // namespace widgets
}; // namespace guix