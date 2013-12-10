#include <GuiX/Config.h>
#include <GuiX/ListLayout.h>

#include <Src/GuiUtils.h>

namespace guix {
namespace layouts {

// ===================================================================================
// GxListLayout
// ===================================================================================

GxListLayout::~GxListLayout()
{
}

GxListLayout::GxListLayout(bool isHorizontal)
	:myIsHorizontal(isHorizontal)
	,myStretch(0.f)
{
}

void GxListLayout::Adjust()
{
	int count = myWidgets.Size();
	bool tr = myIsHorizontal;

	GxVec2i min(0, 0);
	GxVec2i hint(0, 0);
	GxVec2i flags(GX_SP_RESIZE, GX_SP_RESIZE);

	float stretch = 0.f;

	// Sum the resize properties of each widget.
	for(int i=0; i<count; ++i)
	{
		GxWidget* w = myWidgets[i];
		w->Adjust();
		if(w->IsUnarranged()) continue;

		GxSizePolicyResult pol(w, tr);

		min.x  = GxMax(min.x, pol.min.x);
		hint.x = GxMax(hint.x, pol.hint.x);

		min.y  += pol.min.y;
		hint.y += pol.hint.y;

		flags.x |= pol.flagsH.bits;
		flags.y |= pol.flagsV.bits;

		if(pol.flagsV[GX_SPF_EXPAND])
			stretch += pol.stretchV;

		if(i != count-1)
		{
			hint.y += mySpacing;
			min.y += mySpacing;
		}
	}
	min   = VecTr(tr, min);
	hint  = VecTr(tr, hint);
	flags = VecTr(tr, flags);

	// Apply margins to the sizes.
	int mx = myMargin.l + myMargin.r;
	int my = myMargin.t + myMargin.b;

	min.Add(mx, my);
	hint.Add(mx, my);

	// Store the resulting values.
	myMinimumSize = min;
	myPreferredSize = hint;
	myPolicyFlagsH = flags.x;
	myPolicyFlagsV = flags.y;
	myStretch = stretch;
}

void GxListLayout::Arrange(GxRecti rect)
{
	int count = myWidgets.Size();
	bool tr = myIsHorizontal;

	rect = RectTr(tr, rect);

	GxVec2i min = VecTr(tr, myMinimumSize);
	GxVec2i hint = VecTr(tr, myPreferredSize);

	// If there is extra space available for widgets, calculate the expand rate.
	float expandRate = 0;
	if(rect.h > hint.y && myStretch > 0)
	{
		float extraSpace = (float)(rect.h - hint.y);
		expandRate = extraSpace / myStretch;
	}

	// If there is not enough space for the preferred size, calculate the shrink factor.
	float shrinkFactor = 0;
	if(rect.h < hint.y)
	{
		shrinkFactor = GxLerpFactor((float)hint.y, (float)min.y, (float)rect.h);
		shrinkFactor = GxClamp(shrinkFactor, 0.f, 1.f);
	}

	// Assign rectangles to all children.
	rect.Shrink(myMargin);
	float y = (float)rect.y;
	for(int i=0; i<count; ++i)
	{
		GxWidget* w = myWidgets[i];
		if(w->IsUnarranged()) continue;

		GxSizePolicyResult pol(w, tr);

		float h = (float)pol.hint.y;

		if(pol.flagsV[GX_SPF_SHRINK] && shrinkFactor > 0)
			h = GxLerp(h, (float)pol.min.y, shrinkFactor);

		if(pol.flagsV[GX_SPF_EXPAND] && expandRate > 0)
			h += expandRate * pol.stretchV;

		GxRecti r(rect.x, GxInt(y), rect.w, GxInt(h));
		r.w = GxClamp(r.w, pol.min.x, pol.max.x);
		r.h = GxClamp(r.h, pol.min.y, pol.max.y);
		myWidgets[i]->SetRect(RectTr(tr, r));

		y += h;
		y += (float)mySpacing;
	}
}

}; // namespace layouts
}; // namespace guix