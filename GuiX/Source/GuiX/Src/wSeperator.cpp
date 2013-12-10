#include <GuiX/Config.h>

#include <GuiX/Common.h>
#include <GuiX/Style.h>
#include <GuiX/Context.h>

#include <GuiX/wSeperator.h>

namespace guix {
namespace widgets {

// ===================================================================================
// GxSeperatorH
// ===================================================================================

GxSeperatorH::GxSeperatorH()
{
	myPolicy->min.Set(2, 2);
	myPolicy->hint.Set(2, 2);

	myPolicy->flagsH = GX_SP_RESIZE;
	myPolicy->flagsV = GX_SP_FIXED;
}

void GxSeperatorH::Draw()
{
	GxDraw* draw = GxDraw::Get();
	GxStyle& style = *GxStyle::Get();

	draw->Rect(myRect.x+1, myRect.y, myRect.w-2, 2, style.c.frameOutline, style.c.frameOutEmboss[1]);
}

// ===================================================================================
// GxSeperatorV
// ===================================================================================

GxSeperatorV::GxSeperatorV()
{
	myPolicy->min.Set(2, 2);
	myPolicy->hint.Set(2, 2);
	myPolicy->flagsH = GX_SP_FIXED;
	myPolicy->flagsV = GX_SP_RESIZE;
}

void GxSeperatorV::Draw()
{
	GxDraw* draw = GxDraw::Get();
	GxStyle& style = *GxStyle::Get();

	draw->Rect(myRect.x, myRect.y+1, 1, myRect.h-2, style.c.frameOutline, style.c.frameOutEmboss[1]);
}

}; // namespace widgets
}; // namespace guix