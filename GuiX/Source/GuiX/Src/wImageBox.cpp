#include <GuiX/Config.h>

#include <math.h>

#include <GuiX/Common.h>

#include <GuiX/Style.h>
#include <GuiX/Context.h>
#include <GuiX/wImageBox.h>

namespace guix {
namespace widgets {

// ===================================================================================
// GxImageBox
// ===================================================================================

GxImageBox::~GxImageBox()
{
}

GxImageBox::GxImageBox()
{
	myInit();
}

GxImageBox::GxImageBox(const GxSprite& sprite)
	:mySprite(sprite)
	,myBgColor(0,0)
{
	myInit();
}

GxImageBox::GxImageBox(const GxTexture& texture)
	:mySprite(texture)
	,myBgColor(0,0)
{
	myInit();
}

void GxImageBox::myInit()
{
	myPolicy->min.Set(16, 16);

	GxVec2i size = mySprite.GetTexture().GetSize();
	if(size.x > 0 && size.y > 0)
		myPolicy->hint = size;

	myPolicy->flagsH = GX_SP_RESIZE;
	myPolicy->flagsV = GX_SP_RESIZE;
}

void GxImageBox::Adjust()
{
	if(myPolicy->adjustHint)
	{
		myPolicy->hint = mySprite.GetSize();
		if(myFlags[F_FRAME])
			myPolicy->hint.Add(4, 4);
	}
}

void GxImageBox::Draw()
{
	GxDraw* draw = GxDraw::Get();
	GxStyle& style = *GxStyle::Get();

	GxVec2f size = mySprite.GetSize();
	GxRectf rect = myRect;

	if(myFlags[F_FRAME])
		rect.Shrink(2);

	if(size.x < rect.w)
	{
		rect.x += (rect.w - size.x) / 2;
		rect.w = size.x;
	}
	if(size.y < rect.h)
	{
		rect.y += (rect.h - size.y) / 2;
		rect.h = size.y;
	}
	mySprite.SetSize(rect.w, rect.h);

	if(myFlags[F_FRAME])
	{
		GxRecti r = rect;
		r.Expand(2);
		style.Bar(r);
	}
	if(myBgColor.a > 0)
	{
		GxRecti r = rect;
		r.Expand(1);
		draw->Rect(r.x, r.y, r.w, r.h, myBgColor);
	}

	mySprite.Draw(floor(rect.x), floor(rect.y));
	mySprite.SetSize(size.x, size.y);
}

void GxImageBox::SetFrame(bool enabled)
{
	myFlags.Set(F_FRAME, enabled);
}

void GxImageBox::SetBackgroundColor(GxColor c)
{
	myBgColor = c;
}

GxSprite& GxImageBox::GetSprite()
{
	return mySprite;
}

const GxSprite& GxImageBox::GetSprite() const
{
	return mySprite;
}

}; // namespace widgets
}; // namespace guix