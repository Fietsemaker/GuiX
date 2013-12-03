  #include <GuiX/Config.h>

#include <GuiX/Core.h>
#include <GuiX/Draw.h>
#include <GuiX/Context.h>
#include <GuiX/ListLayout.h>

#include <GuiX/wGroupbox.h>

#include <Src/GuiUtils.h>

namespace guix {
namespace widgets {

namespace {

static const int boxH = 16;
static const int tabH = 20;

}; // anonymous namespace

// ===================================================================================
// GxGroupbox
// ===================================================================================

GxGroupbox::~GxGroupbox()
{
}

GxGroupbox::GxGroupbox()
	:GxContainer(new GxListLayout())
{
	myInit();
}

GxGroupbox::GxGroupbox(GxString text)
	:GxContainer(new GxListLayout())
{
	myText = text;
	myInit();
}

void GxGroupbox::myInit()
{
	myPolicy->min.Set(64, 16);
	myPolicy->hint.Set(128, 24);
}

void GxGroupbox::OnMousePress(GxMouseEvent& evt)
{
	if(myFlags[F_COLLAPSE] && IsHoverWidget())
	{
		if(evt.button == GX_MC_LEFT && !evt.handled && !IsLockedWidget())
		{
			myFlags.Flip(F_COLLAPSED);
		}
		evt.handled = true;
	}
}

void GxGroupbox::Adjust()
{
	bool isTab = myFlags[F_TAB];
	bool isTitled = isTab || myFlags[F_COLLAPSE] || !myText.Empty();
	int titleH = isTab ? tabH : boxH;

	myLayout->SetMargin(isTab ? GxMargini(0, 2) : 8);
	myLayout->Adjust();

	if(myPolicy->adjustFlags)
	{
		GxFlags flagsH = myLayout->GetPolicyFlagsH();
		GxFlags flagsV = myLayout->GetPolicyFlagsV();

		if(myFlags[F_COLLAPSED])
			flagsV = GX_SP_FIXED;

		myPolicy->flagsH = flagsH;
		myPolicy->flagsV = flagsV;
	}

	if(myPolicy->adjustMin)
	{
		myPolicy->min = myLayout->GetMinimumSize();
		myPolicy->min.x = GxMax(64, myPolicy->min.x);
		myPolicy->min.y += isTitled ? titleH : 0;
	}

	if(myPolicy->adjustHint)
	{
		myPolicy->hint = myLayout->GetPreferredSize();
		myPolicy->hint.x = GxMax(128, myPolicy->hint.x);
		myPolicy->hint.y += isTitled ? titleH : 0;
	}
}

void GxGroupbox::SetRect(const GxRecti& rect)
{
	int titleH = myFlags[F_TAB] ? tabH : boxH;
	bool hasTitle = myFlags[F_TAB] || myFlags[F_COLLAPSE] || !myText.Empty();

	GxRecti r = myRect = rect;
	if(hasTitle) r.Shrink(0, titleH, 0, 0);
	myLayout->Arrange(r);
}

GxWidget* GxGroupbox::FindHoverWidget(int x, int y)
{
	int titleH = myFlags[F_TAB] ? tabH : boxH;
	if(myRect.Contains(x, y))
	{
		GxWidget* hover = myLayout->FindHoverWidget(x, y);
		if(hover) 
			return hover;

		if(myFlags[F_COLLAPSED] || y < myRect.y + titleH)
			return this;
	}
	return NULL;
}

void GxGroupbox::Tick(float dt)
{
	if(!myFlags[F_COLLAPSED])
		myLayout->Tick(dt);
}

void GxGroupbox::Draw()
{
	GxDraw* draw = GxDraw::Get();
	GxStyle& style = *GxStyle::Get();

	const bool lock = IsLockedWidget();
	const bool isBox = !myFlags[F_TAB];
	const int titleH = isBox ? boxH : tabH;
	const int textY = titleH/2 - 3;

	GxRecti r = myRect;
	if(myFlags[F_COLLAPSE] || !myText.Empty())
	{
		// Groupbox frame with title
		if(isBox)
			style.d.box.DrawH(r.x, r.y+8, r.w, r.h-8);
		else
			draw->Rect(r.x, r.y, r.w, titleH, style.c.bgTab);

		GxText settings = style.d.text[lock ? 1 : 0];
		settings.alignV = GX_TA_MIDDLE;

		// Draw groupbox title text
		if(!myText.Empty())
		{
			settings.alignH = GX_TA_LEFT;
			if(isBox)
			{
				GxRecti textR = settings.GetTextRect(r.x+8, r.y+textY, myText);
				draw->Rect(textR.x-4, r.y+titleH/2-4, textR.w+8, 8, style.c.bgPanel);
			}
			settings.Draw(r.x+8, r.y+textY, myText);
		}

		// Draw collapse button
		if(myFlags[F_COLLAPSE] && !lock)
		{
			const float hl = GetHighlightValue();

			if(isBox)
				draw->Rect(r.x+r.w-18, r.y+titleH/2-4, 12, 8, style.c.bgPanel);

			GxSprite& arrow = style.d.arrow;
			arrow.SetMirrorV(!myFlags[F_COLLAPSED]);
			arrow.SetColor(settings.top.Alpha(hl*0.5f+0.5f));
			arrow.Draw(r.x+r.w-12, r.y+titleH/2);
			arrow.SetMirrorV(false);
			arrow.SetColor(GxColor(255));
		}
	}
	else
	{
		// Groupbox frame without title
		if(isBox)
			style.d.box.DrawH(r.x, r.y, r.w, r.h);
		else
			draw->Rect(r.x, r.y, r.w, titleH, style.c.bgTab);
	}

	if(!myFlags[F_COLLAPSED])
		myLayout->Draw();
}

GxVec2i GxGroupbox::GetPreferredSize() const
{
	int titleH = myFlags[F_TAB] ? tabH : boxH;
	GxVec2i result = myPolicy->GetHintSize();
	if(myFlags[F_COLLAPSED]) result.y = titleH;
	return result;
}

GxVec2i GxGroupbox::GetMinimumSize() const
{
	int titleH = myFlags[F_TAB] ? tabH : boxH;
	GxVec2i result = myPolicy->GetMinSize();
	if(myFlags[F_COLLAPSED]) result.y = titleH;
	return result;
}


void GxGroupbox::SetText(const GxString& text)
{
	myText = text;
}

void GxGroupbox::SetCollapsable(bool enabled)
{
	myFlags.Set(F_COLLAPSE, enabled);
}

void GxGroupbox::SetTabAppearance(bool enabled)
{
	myFlags.Set(F_TAB, enabled);
}

const GxString& GxGroupbox::GetText() const
{
	return myText;
}

}; // namespace widgets
}; // namespace guix