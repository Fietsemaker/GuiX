#include <GuiX/Config.h>

#include <GuiX/Core.h>
#include <GuiX/Draw.h>
#include <GuiX/Context.h>
#include <GuiX/ListLayout.h>

#include <GuiX/wScrollArea.h>

#include <Src/GuiUtils.h>

namespace guix {
namespace widgets {

namespace {

static const int barW = 16;

}; // anonymous namespace

// ===================================================================================
// GxScrollArea
// ===================================================================================

GxScrollArea::~GxScrollArea()
{
	delete myScrollbarH;
	delete myScrollbarV;
}

GxScrollArea::GxScrollArea()
	:GxContainer(new GxListLayout)
{
	myInit(true, true);
}

GxScrollArea::GxScrollArea(bool scrollH, bool scrollV)
	:GxContainer(new GxListLayout)
{
	myInit(scrollH, scrollV);
}

void GxScrollArea::myInit(bool scrollH, bool scrollV)
{
	myScrollbarH = NULL;
	myScrollbarV = NULL;

	myPolicy->min.Set(32, 32);
	myPolicy->hint.Set(64, 64);

	myPolicy->flagsH = GX_SP_EXPAND;
	myPolicy->flagsV = GX_SP_EXPAND;

	myLayout->SetMargin(2);

	SetScrollH(scrollH);
	SetScrollV(scrollV);
}

void GxScrollArea::Adjust()
{
	myLayout->Adjust();

	GxSizePolicy& pol = GetPolicy();

	int dx = myIsActiveV() ? barW : 0;
	int dy = myIsActiveH() ? barW : 0;

	if(pol.adjustMin)
	{
		pol.min = myLayout->GetMinimumSize();
		if(myScrollbarH) pol.min.x = 64;
		if(myScrollbarV) pol.min.y = 64;
		pol.min.Add(dx, dy);
	}

	if(pol.adjustHint)
	{
		pol.hint = myLayout->GetPreferredSize();
		if(myScrollbarH) pol.hint.x = 64;
		if(myScrollbarV) pol.hint.y = 64;
		pol.hint.Add(dx, dy);
	}
}

void GxScrollArea::SetRect(const GxRecti& rect)
{
	myRect = rect;

	GxVec2i size = myLayout->GetPreferredSize();
	GxRecti view = rect;

	GxVec2i scrollRange;
	for(int i=0; i<2; ++i)
	{
		if(myScrollbarH)
		{
			scrollRange.x = GxMax(0, size.x - view.w);
			myScrollbarH->SetDisabled(scrollRange.x == 0);
			if(scrollRange.x > 0) view.h = rect.h - barW;
		}
		if(myScrollbarV)
		{
			scrollRange.y = GxMax(0, size.y - view.h);
			myScrollbarV->SetDisabled(scrollRange.y == 0);
			if(scrollRange.y > 0) view.w = rect.w - barW;
		}
	}

	GxRecti layoutRect = view;
	if(myIsActiveH())
	{
		layoutRect.x -= myScrollbarH->GetIntValue();
		layoutRect.w = GxMax(size.x, view.w);

		GxRecti r(view.x, view.y + view.h, view.w, barW);
		myScrollbarH->SetRange(0, scrollRange.x, view.w, 24);
		myScrollbarH->SetRect(r);
	}
	if(myIsActiveV())
	{
		layoutRect.y -= myScrollbarV->GetIntValue();
		layoutRect.h = GxMax(size.y, view.h);

		GxRecti r(view.x + view.w, view.y, barW, view.h);
		myScrollbarV->SetRange(0, scrollRange.y, view.h, 24);
		myScrollbarV->SetRect(r);
	}
	myLayout->Arrange(layoutRect);
}

GxWidget* GxScrollArea::FindHoverWidget(int x, int y)
{
	GxWidget* hover = NULL;

	if(myViewArea().Contains(x, y))
		hover = myLayout->FindHoverWidget(x, y);

	if(myIsActiveH() && !hover)
		hover = myScrollbarH->FindHoverWidget(x, y);

	if(myIsActiveV() && !hover)
		hover = myScrollbarV->FindHoverWidget(x, y);

	if(!hover)
		hover = GxWidget::FindHoverWidget(x, y);

	return hover;
}

void GxScrollArea::Tick(float dt)
{
	if(myIsActiveH()) myScrollbarH->Tick(dt);
	if(myIsActiveV()) myScrollbarV->Tick(dt);

	myLayout->Tick(dt);
}

void GxScrollArea::Draw()
{
	GxDraw* draw = GxDraw::Get();
	GxStyle& style = *GxStyle::Get();

	if(myIsActiveH()) myScrollbarH->Draw();
	if(myIsActiveV()) myScrollbarV->Draw();

	if(myIsActiveH() && myIsActiveV())
	{
		int x = myRect.x + myRect.w - barW;
		int y = myRect.y + myRect.h - barW;
		style.Bar(GxRecti(x, y, barW, barW));
	}

	GxRecti r = myViewArea();
	draw->PushScissorRect(r.x, r.y, r.w, r.h);
	myLayout->Draw();
	draw->PopScissorRect();
}

void GxScrollArea::SetScrollH(bool enabled)
{
	if(enabled && !myScrollbarH)
	{
		myScrollbarH = new GxScrollbarH();
		myScrollbarH->SetParent(this);
		myScrollbarH->SetDisabled(true);
	}
	else if(myScrollbarH && !enabled)
	{
		delete myScrollbarH;
		myScrollbarH = NULL;
	}
}

void GxScrollArea::SetScrollV(bool enabled)
{
	if(enabled && !myScrollbarV)
	{
		myScrollbarV = new GxScrollbarV();
		myScrollbarV->SetParent(this);
		myScrollbarV->SetDisabled(true);
	}
	else if(myScrollbarV && !enabled)
	{
		delete myScrollbarH;
		myScrollbarV = NULL;
	}
}

bool GxScrollArea::myIsActiveH() const
{
	return myScrollbarH && !myScrollbarH->IsHidden();
}

bool GxScrollArea::myIsActiveV() const
{
	return myScrollbarV && !myScrollbarV->IsHidden();
}

GxRecti GxScrollArea::myViewArea() const
{
	int w = GxMax(0, myRect.w - (myIsActiveV() ? barW : 0));
	int h = GxMax(0, myRect.h - (myIsActiveH() ? barW : 0));
	return GxRecti(myRect.x, myRect.y, w, h);
}

}; // namespace widgets
}; // namespace guix