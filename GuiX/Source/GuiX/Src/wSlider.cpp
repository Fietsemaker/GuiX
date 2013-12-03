#include <GuiX/Config.h>

#include <math.h>

#include <GuiX/Draw.h>
#include <GuiX/wSlider.h>

#include <Src/GuiUtils.h>

namespace guix {
namespace widgets {

// ===================================================================================
// GxSliderAbstract
// ===================================================================================

GxSliderAbstract::~GxSliderAbstract()
{
}

GxSliderAbstract::GxSliderAbstract(const char* id, bool vertical)
	:GxWidget(id)
	,myValue(0)
	,myBegin(0)
	,myEnd(1)
	,myTickInterval(0)
	,myVarPtr(NULL)
{
	myUpdateValue(0, false);

	myFlags.Set(F_SHOWTICKS);

	if(vertical)
	{
		myPolicy->min.Set(16, 24);
		myPolicy->hint.Set(16, 48);
		myPolicy->flagsV = GX_SP_EXPAND;
	}
	else
	{
		myPolicy->min.Set(24, 16);
		myPolicy->hint.Set(48, 16);
		myPolicy->flagsH = GX_SP_EXPAND;
	}
}

void GxSliderAbstract::OnMousePress(GxMouseEvent& evt)
{
	if(IsHoverWidget())
	{
		if(evt.button == GX_MC_LEFT && !evt.handled && !IsLockedWidget())
		{
			GrabFocus();
			myFlags.Set(F_INTERACTED);
			myDragSlider(evt.x, evt.y);
		}
		evt.handled = true;
	}
}

void GxSliderAbstract::OnMouseRelease(GxMouseEvent& evt)
{
	if(evt.button == GX_MC_LEFT && IsFocusWidget())
	{
		myDragSlider(evt.x, evt.y);
		ReleaseFocus();
	}
}

void GxSliderAbstract::OnMouseScroll(GxScrollEvent& evt)
{
	if(IsHoverWidget() && !evt.handled)
	{
		double delta = myTickInterval;
		if(abs(delta) < 0.001)
			delta = (myEnd - myBegin) / 20.0;

		if(!evt.up) delta = -delta;

		myUpdateValue(myValue + delta, true);
		evt.handled = true;
	}
}

void GxSliderAbstract::Tick(float dt)
{
	if(IsFocusWidget())
	{
		const GxVec2i mpos = GxInput::Get()->GetMousePos();
		myDragSlider(mpos.x, mpos.y);
	}
}

void GxSliderAbstract::Draw()
{
	GxDraw* draw = GxDraw::Get();
	GxStyle& style = *GxStyle::Get();

	float hl = GetHighlightValue();
	bool lock = IsLockedWidget();
	bool isV = IsVertical();

	const GxVec2i valPos = myGetValuePos();
	const GxVec2i minPos = myGetBeginPos();
	const GxVec2i maxPos = myGetEndPos();

	// Slider bar
	if(isV)
		style.Bar(GxRecti(minPos.x-3, maxPos.y-2, 6, minPos.y-maxPos.y+4));
	else
		style.Bar(GxRecti(minPos.x-2, minPos.y-3, maxPos.x-minPos.x+4, 6));
		
	// Slider ticks
	double tickDelta = myGetTickDelta();
	if(myFlags[F_SHOWTICKS] && tickDelta > 1)
	{
		const GxColor col = style.c.frameOutline;
		if(isV)
		{
			const int w1 = -5, w2 = 4;
			const double posA = (double)minPos.y - tickDelta;
			const double posB = (double)maxPos.y + 1.0;
			for(double pos=posA; pos>posB; pos -= tickDelta)
				draw->Rect(valPos.x+w1, (int)pos, w2, 1, col);
		}
		else
		{
			const int h1 = -5, h2 = 4;
			const double posA = (double)minPos.x + tickDelta;
			const double posB = (double)maxPos.x - 1.0;
			for(double pos=posA; pos<posB; pos += tickDelta)
				draw->Rect((int)pos, valPos.y+h1, 1, h2, col);
		}
	}

	// Slider highlight
	const GxColor hlA = style.c.frameInColorHl[0];
	const GxColor hlB = style.c.frameInColorHl[1];
	if(isV)
		draw->Rect(minPos.x-1, minPos.y, 2, valPos.y-minPos.y, hlB, hlA);
	else
		draw->Rect(minPos.x, minPos.y-1, valPos.x-minPos.x, 2, hlB, hlA, hlB, hlA);

	// Slider box
	const GxVec2i dim(isV ? 12 : 6, isV ? 6 : 12);
	const GxRecti box(valPos.x - dim.x/2, valPos.y - dim.y/2, dim.x, dim.y);
	style.Button(box, hl, lock);
}

void GxSliderAbstract::SetVarPtr(double* var)
{
	myVarPtr = var;
}

void GxSliderAbstract::SetValue(int value)
{
	myUpdateValue((double)value, false);
}

void GxSliderAbstract::SetValue(double value)
{
	myUpdateValue(value, false);
}

void GxSliderAbstract::SetRange(int begin, int end, int tickInterval)
{
	SetRange((double)begin, (double)end, (double)tickInterval);
}

void GxSliderAbstract::SetRange(double begin, double end, double tickInterval)
{
	myBegin = begin;
	myEnd = end;

	myTickInterval = abs(tickInterval);
	if(myEnd < myBegin && myTickInterval > 0)
		myTickInterval = -myTickInterval;

	myUpdateValue(myValue, false);
}

void GxSliderAbstract::SetSnapToTicks(bool enabled)
{
	myFlags.Set(F_SNAPPING, enabled);
}

void GxSliderAbstract::SetShowTicks(bool enabled)
{
	myFlags.Set(F_SHOWTICKS, enabled);
}

int GxSliderAbstract::GetIntValue() const
{
	return GxInt(myValue);
}

double GxSliderAbstract::GetValue() const
{
	return myValue;
}

double GxSliderAbstract::GetBegin() const
{
	return myBegin;
}

double GxSliderAbstract::GetEnd() const
{
	return myEnd;
}

double GxSliderAbstract::GetTickInterval() const
{
	return myTickInterval;
}

void GxSliderAbstract::myUpdateValue(double value, bool emitEvent)
{
	bool snap = myFlags[F_SNAPPING];
	double min = GxMin(myBegin, myEnd);
	double max = GxMax(myBegin, myEnd);
	if(snap && abs(myTickInterval) >= 0.001 && value > min && value < max)
	{
		value -= myBegin;
		value = floor(value/myTickInterval + 0.5) * myTickInterval;
		value += myBegin;
	}
	value = GxClamp(value, min, max);

	if(myValue != value && emitEvent)
	{
		EmitEvent(eChanged(), value);
		myFlags.Set(F_CHANGED);
	}

	myValue = value;
	if(myVarPtr) *myVarPtr = value;
}

void GxSliderAbstract::myDragSlider(int x, int y)
{
	bool isV = IsVertical();
	if(isV) GxSwap(x, y);
	GxRecti r = RectTr(isV, myGetBarRect());
	double xMin = (double)(r.x);
	double xMax = (double)(r.x + r.w);
	myUpdateValue(GxRemap((double)x, xMin, xMax, myBegin, myEnd), true);
}

GxRecti GxSliderAbstract::myGetBarRect() const
{
	bool isV = IsVertical();
	if(isV) return GxRecti(myRect.x, myRect.y+myRect.h-4, myRect.w, -myRect.h+8);
	return GxRecti(myRect.x+4, myRect.y, myRect.w-8, myRect.h);
}

GxVec2i GxSliderAbstract::myGetBeginPos() const
{
	bool isV = IsVertical();
	GxRecti r = RectTr(isV, myGetBarRect());
	return VecTr(isV, r.x, r.y + r.h/2);
}

GxVec2i GxSliderAbstract::myGetEndPos() const
{
	bool isV = IsVertical();
	GxRecti r = RectTr(isV, myGetBarRect());
	return VecTr(isV, r.x + r.w, r.y + r.h/2);
}

GxVec2i GxSliderAbstract::myGetValuePos() const
{
	bool isV = IsVertical();
	GxRecti r = RectTr(isV, myGetBarRect());
	double xMin = (double)(r.x);
	double xMax = (double)(r.x + r.w);
	double x = GxRemap(myValue, myBegin, myEnd, xMin, xMax);
	return VecTr(isV, GxInt(x), r.y + r.h/2);
}

double GxSliderAbstract::myGetTickDelta() const
{
	bool isV = IsVertical();
	GxRecti r = RectTr(isV, myGetBarRect());
	return abs(r.w * GxLerpFactor(myBegin, myEnd, myBegin + myTickInterval));
}

// ===================================================================================
// GxSliderH
// ===================================================================================

GxSliderH::GxSliderH()
	:GxSliderAbstract(NULL, false)
{
}

GxSliderH::GxSliderH(const char* id)
	:GxSliderAbstract(id, false)
{
}

bool GxSliderH::IsVertical() const
{
	return false;
}

// ===================================================================================
// GxSliderV
// ===================================================================================

GxSliderV::GxSliderV()
	:GxSliderAbstract(NULL, true)
{
}

GxSliderV::GxSliderV(const char* id)
	:GxSliderAbstract(id, true)
{
}

bool GxSliderV::IsVertical() const
{
	return true;
}

// ===================================================================================
// Widget events
// ===================================================================================

GxDefineWidgetEvent (GxSliderAbstract, eChanged)

}; // namespace widgets
}; // namespace guix