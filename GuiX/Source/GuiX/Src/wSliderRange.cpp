#include <GuiX/Config.h>

#include <math.h>

#include <GuiX/Draw.h>
#include <GuiX/wSpinner.h>
#include <GuiX/wSliderRange.h>

#include <Src/GuiUtils.h>

namespace guix {
namespace widgets {

// ===================================================================================
// GxSliderRange
// ===================================================================================

GxSliderRange::~GxSliderRange()
{
	delete[] mySpinners;
}

GxSliderRange::GxSliderRange(const char* id, bool addSpinners)
	:GxWidget(id)
	,myValue(0,0)
	,myRange(0,0)
	,mySpinners(NULL)
{
	myPolicy->min.Set(24, 16);
	myPolicy->hint.Set(48, 16);
	myPolicy->flagsH = GX_SP_EXPAND;

	if(addSpinners)
	{
		mySpinners = new GxSpinner[2];
		for(int i=0; i<2; ++i)
		{
			mySpinners[i].SetRange(0, 0, 1);
			mySpinners[i].SetParent(this);
			mySpinners[i].SetCallback(this);
			mySpinners[i].SetWidgetId(i ? "r" : "l");
		}

		GxVec2i min = mySpinners[0].GetMinimumSize();
		GxVec2i pref = mySpinners[0].GetPreferredSize();

		myPolicy->min.y += min.y;
		myPolicy->hint.y += pref.y;
		myPolicy->min.x = GxMax(myPolicy->min.x, min.x);
		myPolicy->hint.x = GxMax(myPolicy->hint.x, pref.x);
	}

	myUpdateValue(0, true, false);
	myUpdateValue(0, false, false);
}

void GxSliderRange::SetRect(const GxRecti& r)
{
	myRect = r;

	int spinH = mySpinners[0].GetPreferredSize().y;
	if(r.h < myPolicy->hint.y)
		spinH = mySpinners[0].GetMinimumSize().y;

	GxRecti rl = r, rr = r;
	rl.y = rr.y = r.y + r.h - spinH;
	rl.h = rr.h = spinH;
	rl.w = r.w / 2;
	rr.x = rl.x + rl.w;
	rr.w = r.w - rl.w;

	mySpinners[0].SetRect(rl);
	mySpinners[1].SetRect(rr);
}

GxWidget* GxSliderRange::FindHoverWidget(int x, int y)
{
	GxWidget* hover = NULL;
	if(myRect.Contains(x, y))
	{
		for(int i=0; mySpinners && i<2; ++i)
		{
			if(!hover) hover = mySpinners[i].FindHoverWidget(x, y);
		}
		if(!hover) hover = GxWidget::FindHoverWidget(x, y);
	}
	return hover;
}

void GxSliderRange::OnMousePress(GxMouseEvent& evt)
{
	if(IsHoverWidget())
	{
		if(evt.button == GX_MC_LEFT && !evt.handled && !IsLockedWidget())
		{
			GxVec3i vpos = myGetValuePos();
			myFlags.Set(F_LEFT_END, evt.x < (vpos.x+vpos.y)/2);

			GrabFocus();
			myFlags.Set(F_INTERACTED);
			myDragSlider(evt.x, evt.y);
		}
		evt.handled = true;
	}
}

void GxSliderRange::OnMouseRelease(GxMouseEvent& evt)
{
	if(evt.button == GX_MC_LEFT && IsFocusWidget())
	{
		myDragSlider(evt.x, evt.y);
		ReleaseFocus();
	}
}

void GxSliderRange::OnEvent(const GxWidgetEvent& event)
{
	if(event.type == GxSpinner::eChanged())
	{
		if(event.id == "l")
			myUpdateValue(mySpinners[0].GetValue(), true, true);
		else
			myUpdateValue(mySpinners[1].GetValue(), false, true);
	}
}

void GxSliderRange::Tick(float dt)
{
	for(int i=0; mySpinners && i<2; ++i)
		mySpinners[i].Tick(dt);

	if(IsFocusWidget())
	{
		const GxVec2i mpos = GxInput::Get()->GetMousePos();
		myDragSlider(mpos.x, mpos.y);
	}
}

void GxSliderRange::Draw()
{
	GxDraw* draw = GxDraw::Get();
	GxStyle& style = *GxStyle::Get();

	float hl = GetHighlightValue();
	bool lock = IsLockedWidget();

	const GxVec3i valPos = myGetValuePos();
	const GxVec2i minPos = myGetBeginPos();
	const GxVec2i maxPos = myGetEndPos();

	// Slider bar
	style.Bar(GxRecti(minPos.x-2, minPos.y-3, maxPos.x-minPos.x+4, 6));

	// Slider highlight
	const GxColor hlA = style.c.frameInColorHl[0];
	const GxColor hlB = style.c.frameInColorHl[1];
	draw->Rect(valPos.x, valPos.z-1, valPos.y-valPos.x, 2, hlB, hlA, hlB, hlA);

	// Slider box
	int mouseX = GxInput::Get()->GetMousePos().x;
	bool hlLeft = mouseX < (valPos.x+valPos.y) / 2;

	const GxVec2i dim(6, 12);
	const GxRecti boxA(valPos.x - dim.x, valPos.z - dim.y/2, dim.x, dim.y);
	const GxRecti boxB(valPos.y, valPos.z - dim.y/2, dim.x, dim.y);
	style.Button(boxA, hlLeft ? hl : 0.f, lock);
	style.Button(boxB, hlLeft ? 0.f : hl, lock);

	// Spinners
	for(int i=0; mySpinners && i<2; ++i)
		mySpinners[i].Draw();
}

void GxSliderRange::SetValue(int begin, int end)
{
	SetValue((double)begin, (double)end);
}

void GxSliderRange::SetValue(double begin, double end)
{
	if(end < begin) GxSwap(begin, end);
	myValue.x = GxClamp(begin, myRange.x, myRange.y);
	myValue.y = GxClamp(end, myRange.x, myRange.y);
	if(mySpinners)
	{
		mySpinners[0].SetValue(myValue.x);
		mySpinners[1].SetValue(myValue.y);
	}
}

void GxSliderRange::SetRange(int min, int max, int step)
{
	SetRange((double)min, (double)max, (double)step);
}

void GxSliderRange::SetRange(double min, double max, double step)
{
	if(max < min) GxSwap(min, max);
	myRange = GxVec2d(min, max);

	for(int i=0; mySpinners && i<2; ++i)
		mySpinners[i].SetRange(min, max, step);

	myUpdateValue(myValue.x, true, false);
	myUpdateValue(myValue.y, false, false);
}

GxVec2i GxSliderRange::GetIntValue() const
{
	return myValue;
}

GxVec2d GxSliderRange::GetValue() const
{
	return myValue;
}

GxVec2d GxSliderRange::GetRange() const
{
	return myRange;
}

void GxSliderRange::myUpdateValue(double value, bool left, bool emitEvent)
{
	double min = GxMin(myRange.x, myRange.y);
	double max = GxMax(myRange.x, myRange.y);
	value = GxClamp(value, min, max);

	if(left)
		value = GxMin(value, myValue.y);
	else
		value = GxMax(value, myValue.x);

	double& dst = left ? myValue.x : myValue.y;
	if(dst != value)
	{
		dst = value;
		if(emitEvent)
		{
			EmitEvent(eChanged(), GxVec2f(myValue));
			myFlags.Set(F_CHANGED);
		}
	}

	if(mySpinners)
	{
		mySpinners[0].SetValue(myValue.x);
		mySpinners[1].SetValue(myValue.y);
	}
}

void GxSliderRange::myDragSlider(int x, int y)
{
	GxRecti r = myGetBarRect();
	double xMin = (double)(r.x);
	double xMax = (double)(r.x + r.w);
	double v = GxRemap((double)x, xMin, xMax, myRange.x, myRange.y);
	myUpdateValue(v, myFlags[F_LEFT_END], true);
}

GxRecti GxSliderRange::myGetBarRect() const
{
	GxRecti r(myRect.x+6, myRect.y, myRect.w-12, myRect.h);
	if(mySpinners) r.h -= r.h / 2;
	return r;
}

GxVec2i GxSliderRange::myGetBeginPos() const
{
	GxRecti r = myGetBarRect();
	return GxVec2i(r.x, r.y + r.h/2);
}

GxVec2i GxSliderRange::myGetEndPos() const
{
	GxRecti r = myGetBarRect();
	return GxVec2i(r.x + r.w, r.y + r.h/2);
}

GxVec3i GxSliderRange::myGetValuePos() const
{
	GxRecti r = myGetBarRect();
	double xMin = (double)(r.x);
	double xMax = (double)(r.x + r.w);
	double x1 = GxRemap(myValue.x, myRange.x, myRange.y, xMin, xMax);
	double x2 = GxRemap(myValue.y, myRange.x, myRange.y, xMin, xMax);
	return GxVec3i(GxInt(x1), GxInt(x2), r.y + r.h/2);
}

// ===================================================================================
// Widget events
// ===================================================================================

GxDefineWidgetEvent (GxSliderRange, eChanged)

}; // namespace widgets
}; // namespace guix