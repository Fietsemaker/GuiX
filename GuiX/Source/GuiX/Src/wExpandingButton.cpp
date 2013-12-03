#include <GuiX/Config.h>

#include <GuiX/Style.h>
#include <GuiX/Context.h>
#include <GuiX/GridLayout.h>
#include <GuiX/wExpandingButton.h>

namespace guix {
namespace widgets {

// ===================================================================================
// GxColorEdit
// ===================================================================================

GxExpandingButton::~GxExpandingButton()
{
	delete myLayout;
}

GxExpandingButton::GxExpandingButton(GxLayout* layout)
{
	myFlags.Set(F_COLLAPSED);

	myLayout = layout;
	myLayout->SetOwner(this);

	myPolicy->min.Set(48, 16);
	myPolicy->hint.Set(128, 24);
	myPolicy->flagsH = GX_SP_EXPAND;

	myButtonW = 20;
	myButtonH = 24;
}

void GxExpandingButton::OnMousePress(GxMouseEvent& evt)
{
	if(IsHoverWidget())
	{
		if(evt.button == GX_MC_LEFT && !evt.handled && !IsLockedWidget())
		{
			myFlags.Flip(F_COLLAPSED);
		}
		evt.handled = true;
	}
}

void GxExpandingButton::Adjust()
{
	myLayout->SetMargin(4);
	myLayout->Adjust();

	if(myPolicy->adjustFlags)
	{
		if(myFlags[F_COLLAPSED])
			myPolicy->flagsV = GX_SP_FIXED;
		else
			myPolicy->flagsV = GX_SP_RESIZE;
	}

	if(myPolicy->adjustMin)
	{
		myPolicy->min = myLayout->GetMinimumSize();
		myPolicy->min.x = GxMax(48, myPolicy->min.x);
		myPolicy->min.y += myButtonH;
	}

	if(myPolicy->adjustHint)
	{
		myPolicy->hint = myLayout->GetPreferredSize();
		myPolicy->hint.x = GxMax(96, myPolicy->hint.x);
		myPolicy->hint.y += myButtonH;
	}
}

void GxExpandingButton::SetRect(const GxRecti& rect)
{
	GxRecti r = myRect = rect;
	r.Shrink(0, myButtonH, 0, 0);
	myLayout->Arrange(r);
}

GxWidget* GxExpandingButton::FindHoverWidget(int x, int y)
{
	if(myRect.Contains(x, y))
	{
		GxWidget* hover = myLayout->FindHoverWidget(x, y);
		if(hover)
			return hover;

		if(myFlags[F_COLLAPSED] || y < myRect.y + myButtonH)
			return this;
	}
	return NULL;
}

GxVec2i GxExpandingButton::GetPreferredSize() const
{
	GxVec2i result = myPolicy->GetHintSize();

	if(myFlags[F_COLLAPSED])
		result.y = myButtonH;

	return result;
}

GxVec2i GxExpandingButton::GetMinimumSize() const
{
	GxVec2i result = myPolicy->GetMinSize();

	if(myFlags[F_COLLAPSED])
		result.y = myButtonH;

	return result;
}

void GxExpandingButton::Tick(float dt)
{
	if(!myFlags[F_COLLAPSED])
		myLayout->Tick(dt);
}

void GxExpandingButton::Draw()
{
	GxDraw* draw = GxDraw::Get();
	GxStyle& style = *GxStyle::Get();

	const bool lock = IsLockedWidget();
	const float hl = GetHighlightValue();

	// Draw the outline.
	GxRecti r = myRect;
	if(myFlags[F_COLLAPSED])
	{
		r.h = myButtonH;
		style.Bar(r);
	}
	else
	{
		style.Bar(r);
		r.h = myButtonH;
	}
	r.Shrink(1);

	// Draw collapse button.
	style.Button(r, hl, lock);
	GxSprite& arrow = style.d.arrow;
	arrow.SetMirrorV(!myFlags[F_COLLAPSED]);
	arrow.Draw(myRect.x+myRect.w-myButtonW/2, myRect.y+myButtonH/2);
	arrow.SetMirrorV(false);

	// Draw expanded controls.
	if(!myFlags[F_COLLAPSED])
	{
		r = myRect;
		r.Shrink(2, myButtonH, 2, 2);
		draw->Rect(r.x, r.y, r.w, r.h, style.c.bgPanel.Alpha(128));
		myLayout->Draw();
	}	
}

void GxExpandingButton::SetCollapsed(bool enabled)
{
	myFlags.Set(F_COLLAPSED, enabled);
}

}; // namespace widgets
}; // namespace guix