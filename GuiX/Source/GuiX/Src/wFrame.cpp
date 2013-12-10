#include <GuiX/Config.h>

#include <GuiX/Core.h>
#include <GuiX/Draw.h>
#include <GuiX/Context.h>
#include <GuiX/FreeLayout.h>
#include <GuiX/ListLayout.h>

#include <GuiX/wFrame.h>

#include <Src/GuiUtils.h>

namespace guix {
namespace widgets {

// ===================================================================================
// GxFrame
// ===================================================================================

GxFrame::~GxFrame()
{
}

GxFrame::GxFrame()
	:GxContainer(new GxFreeLayout)
{
	myPolicy->min.Set(8, 8);
	myPolicy->hint.Set(8, 8);
}

GxFrame::GxFrame(GxLayout* layout)
	:GxContainer(layout)
{
	myPolicy->min.Set(8, 8);
	myPolicy->hint.Set(8, 8);
}

void GxFrame::Adjust()
{
	myLayout->Adjust();

	if(myPolicy->adjustFlags)
	{
		myPolicy->flagsH = myLayout->GetPolicyFlagsH();
		myPolicy->flagsV = myLayout->GetPolicyFlagsV();
	}

	if(myPolicy->adjustMin)
		myPolicy->min = myLayout->GetMinimumSize();

	if(myPolicy->adjustHint)
		myPolicy->hint = myLayout->GetPreferredSize();
}

void GxFrame::SetRect(const GxRecti& rect)
{
	myLayout->Arrange(rect);

	myRect = rect;
}

GxWidget* GxFrame::FindHoverWidget(int x, int y)
{
	GxWidget* w = myLayout->FindHoverWidget(x, y);
	if(w) return w;

	if(myRect.Contains(x, y))
		if(!myLayout->IsInWidgetRect(x, y))
			return this;

	return NULL;
}

void GxFrame::Tick(float dt)
{
	myLayout->Tick(dt);
}

void GxFrame::Draw()
{
	myLayout->Draw();
}

// ===================================================================================
// GxFrameH
// ===================================================================================

GxFrameH::GxFrameH()
	:GxFrame(new GxListLayout(true))
{
}

// ===================================================================================
// GxFrameV
// ===================================================================================

GxFrameV::GxFrameV()
	:GxFrame(new GxListLayout(false))
{
}

}; // namespace widgets
}; // namespace guix