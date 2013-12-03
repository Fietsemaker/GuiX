#include <GuiX/Config.h>

#include <math.h>

#include <GuiX/wScrollbar.h>

#include <Src/GuiUtils.h>

namespace guix {
namespace widgets {

namespace {

static const int buttonW = 16;
static const double minDragW = 4;

enum ScrollbarItem
{
	I_NO_ITEM,
	I_BAR_INC,
	I_BAR_DEC,
	I_BAR_DRAG,
	I_BTN_INC,
	I_BTN_DEC,
};

}; // anonymous namespace

// ===================================================================================
// GxScrollbarAbstract
// ===================================================================================

GxScrollbarAbstract::~GxScrollbarAbstract()
{
}

GxScrollbarAbstract::GxScrollbarAbstract(const char* id, bool vertical)
	:GxWidget(id)
	,myValue(0)
	,myBegin(0)
	,myEnd(1)
	,myPageSize(0.1)
	,myStepSize(0.1)
	,myActionValue(0)
	,myVarPtr(NULL)
	,myLastItem(I_NO_ITEM)
{
	myFlags.Set(F_BUTTONS);

	if(vertical)
	{
		myPolicy->min.Set(16, 48);
		myPolicy->hint.Set(16, 64);
		myPolicy->flagsV = GX_SP_EXPAND;
	}
	else
	{
		myPolicy->min.Set(48, 16);
		myPolicy->hint.Set(64, 16);
		myPolicy->flagsH = GX_SP_EXPAND;		
	}
}

void GxScrollbarAbstract::OnMousePress(GxMouseEvent& evt)
{
	if(IsHoverWidget())
	{
		if(evt.button == GX_MC_LEFT && !evt.handled && !IsLockedWidget())
		{
			GrabFocus();
			myFlags.Set(F_INTERACTED);

			myLastItem = myGetItemAtPos(evt.x, evt.y);
			myActionValue = myValue;

			if(myLastItem == I_BAR_DRAG)
			{
				GxRecti r = myGetDragRect();
				if(IsVertical())
					myActionValue = (double)(evt.y - r.y);
				else
					myActionValue = (double)(evt.x - r.x);

				myDragScrollbar(evt.x, evt.y);
				myFlags.Set(F_DRAGGING);
			}
		}
		evt.handled = true;
	}
}

void GxScrollbarAbstract::OnMouseRelease(GxMouseEvent& evt)
{
	if(evt.button == GX_MC_LEFT && IsFocusWidget())
	{
		double v = myActionValue;

		if(myFlags[F_DRAGGING])
			myDragScrollbar(evt.x, evt.y);

		if(myLastItem == I_BTN_INC)
			if(v + myStepSize > myValue)
				myUpdateValue(v + myStepSize, true);

		if(myLastItem == I_BTN_DEC)
			if(v - myStepSize < myValue)
				myUpdateValue(v - myStepSize, true);

		myFlags.Reset(F_DRAGGING);
		ReleaseFocus();
	}
}

void GxScrollbarAbstract::OnMouseScroll(GxScrollEvent& evt)
{
	if(IsHoverWidget() && !evt.handled)
	{
		myApplyScroll(evt.up);
		evt.handled = true;
	}
}

void GxScrollbarAbstract::Tick(float dt)
{
	GxVec2i mpos = GxInput::Get()->GetMousePos();

	if(IsFocusWidget() && myFlags[F_DRAGGING])
		myDragScrollbar(mpos.x, mpos.y);

	if(IsHoverWidget())
		myLastItem = myGetItemAtPos(mpos.x, mpos.y);

	if(IsFocusWidget())
	{
		double delta = 8.0 * (double)dt;
		GxRecti r = myGetDragRect();
		double limit = myGetValueAtPos(mpos.x - r.w/2, mpos.y - r.h/2);

		if(myLastItem == I_BTN_INC)
			myUpdateValue(myValue + myStepSize * delta, true);

		if(myLastItem == I_BTN_DEC)
			myUpdateValue(myValue - myStepSize * delta, true);

		if(myLastItem == I_BAR_INC)
			myUpdateValue(GxMin(limit, myValue + myPageSize * delta), true);

		if(myLastItem == I_BAR_DEC)
			myUpdateValue(GxMax(limit, myValue - myPageSize * delta), true);
	}
}

void GxScrollbarAbstract::Draw()
{
	GxDraw* draw = GxDraw::Get();
	GxStyle& style = *GxStyle::Get();

	float hl = GetHighlightValue();
	bool lock = IsLockedWidget();
	bool isV = IsVertical();

	// Bar ragion.
	style.Bar(myRect);

	// Increment / decrement buttons.
	if(myFlags[F_BUTTONS])
	{
		GxRecti inc = myGetIncRect();
		GxRecti dec = myGetDecRect();

		if(isV)
		{
			dec.Shrink(1, 1, 1, 0);
			inc.Shrink(1, 0, 1, 1);
		}
		else
		{
			style.d.arrow.SetRotation(0, -1);
			dec.Shrink(1, 1, 0, 1);
			inc.Shrink(0, 1, 1, 1);
		}

		style.Button(inc, (myLastItem == I_BTN_INC) ? hl : 0, lock);
		style.Button(dec, (myLastItem == I_BTN_DEC) ? hl : 0, lock);

		style.d.arrow.Draw(inc.x + inc.w/2, inc.y + inc.h/2);
		style.d.arrow.SetFlags(GxSprite::MIRROR_V);
		style.d.arrow.Draw(dec.x + inc.w/2, dec.y + dec.h/2);
		style.d.arrow.SetFlags();
		style.d.arrow.SetRotation();
	}

	// Drag button.
	GxRecti slider = myGetDragRect();
	if(isV)
		slider.Shrink(1, 0, 1, 0);
	else
		slider.Shrink(0, 1, 0, 1);
		
	style.Button(slider, (myLastItem == I_BAR_DRAG) ? hl : 0, lock);

	// Drag symbol.
	if(slider.w > 8 && slider.h > 8 && !lock)
	{
		if(!isV) style.d.grab.SetRotation(0, -1);
		style.d.grab.Draw(slider.x + slider.w/2, slider.y + slider.h/2);
		style.d.grab.SetRotation();
	}
}

void GxScrollbarAbstract::ScrollUp()
{
	myApplyScroll(true);
}

void GxScrollbarAbstract::ScrollDown()
{
	myApplyScroll(false);
}

void GxScrollbarAbstract::SetVarPtr(double* var)
{
	myVarPtr = var;
}

void GxScrollbarAbstract::SetValue(int value)
{
	myUpdateValue((double)value, false);
}

void GxScrollbarAbstract::SetValue(double value)
{
	myUpdateValue(value, false);
}

void GxScrollbarAbstract::SetRange(int begin, int end, int pageSize, int stepSize)
{
	SetRange((double)begin, (double)end, (double)pageSize, (double)stepSize);
}

void GxScrollbarAbstract::SetRange(double begin, double end, double pageSize, double stepSize)
{
	myBegin = begin;
	myEnd = end;

	myPageSize = GxMax(0.001, fabs(pageSize));
	myStepSize = GxMax(0.001, fabs(stepSize));

	if(myEnd < myBegin)
	{
		myPageSize = -myPageSize;
		myStepSize = -myStepSize;
	}

	myUpdateValue(myValue, false);
}

void GxScrollbarAbstract::SetButtons(bool enabled)
{
	myFlags.Set(F_BUTTONS, enabled);
}

void GxScrollbarAbstract::SetSnapToSteps(bool enabled)
{
	myFlags.Set(F_SNAPPING, enabled);
}

int GxScrollbarAbstract::GetIntValue() const
{
	return GxInt(myValue);
}

double GxScrollbarAbstract::GetValue() const
{
	return myValue;
}

double GxScrollbarAbstract::GetBegin() const
{
	return myBegin;
}

double GxScrollbarAbstract::GetEnd() const
{
	return myEnd;
}

void GxScrollbarAbstract::myUpdateValue(double value, bool emitEvent)
{
	double min = GxMin(myBegin, myEnd);
	double max = GxMax(myBegin, myEnd);
	if(myFlags[F_SNAPPING] && value > min && value < max)
	{
		value -= myBegin;
		value = floor(value/myStepSize + 0.5) * myStepSize;
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

void GxScrollbarAbstract::myDragScrollbar(int x, int y)
{
	int pos = GxInt(myActionValue);
	double v = myGetValueAtPos(x - pos, y - pos);
	myUpdateValue(v, true);
}

void GxScrollbarAbstract::myApplyScroll(bool up)
{
	double delta = myStepSize * 2;
	if(fabs(delta) < 0.001)
		delta = (myEnd - myBegin) / 20.0;

	if(up == false)  delta = -delta;
	if(IsVertical()) delta = -delta;

	myUpdateValue(myValue + delta, true);
}

GxRecti GxScrollbarAbstract::myGetIncRect() const
{
	bool isV = IsVertical();
	int bw = myFlags[F_BUTTONS] ? buttonW : 0;
	GxRecti r = RectTr(isV, myRect);
	r.x += r.w - bw;
	r.w = bw;
	return RectTr(isV, r);
}

GxRecti GxScrollbarAbstract::myGetDecRect() const
{
	bool isV = IsVertical();
	int bw = myFlags[F_BUTTONS] ? buttonW : 0;
	GxRecti r = RectTr(isV, myRect);
	r.w = bw;
	return RectTr(isV, r);
}

GxRecti GxScrollbarAbstract::myGetBarRect() const
{
	bool isV = IsVertical();
	int bw = myFlags[F_BUTTONS] ? buttonW : 0;
	GxRecti r = RectTr(isV, myRect);
	r.x += bw;
	r.w -= bw * 2;
	return RectTr(isV, r);
}

GxRecti GxScrollbarAbstract::myGetDragRect() const
{
	bool isV = IsVertical();
	if(myEnd != myBegin)
	{
		GxRecti r = RectTr(isV, myGetBarRect());
		double xMin = (double)(r.x);
		double xMax = (double)(r.x + r.w);
		double wBar = GxMax(minDragW, (xMax - xMin) * myPageSize / (myEnd - myBegin + myPageSize));
		double xBar = GxRemap(myValue, myBegin, myEnd, xMin, xMax - wBar);
		return RectTr(isV, GxInt(xBar), r.y, GxInt(wBar), r.h);
	}
	return myGetBarRect();
}

int GxScrollbarAbstract::myGetItemAtPos(int x, int y) const
{
	bool isV = IsVertical();
	if(myFlags[F_BUTTONS])
	{
		if(myGetIncRect().Contains(x, y))
			return I_BTN_INC;

		if(myGetDecRect().Contains(x, y))
			return I_BTN_DEC;
	}

	GxRecti r = myGetDragRect();
	if(r.Contains(x, y))
		return I_BAR_DRAG;

	bool isDec = isV ? (y <= r.y) : (x <= r.x);
	return isDec ? I_BAR_DEC : I_BAR_INC;
}

double GxScrollbarAbstract::myGetValueAtPos(int x, int y) const
{
	double result = myBegin;
	if(myEnd != myBegin)
	{
		bool isV = IsVertical();
		if(isV) GxSwap(x, y);
		GxRecti r = RectTr(isV, myGetBarRect());
		double xMin = (double)(r.x);
		double xMax = (double)(r.x + r.w);
		double wBar = GxMax(minDragW, (xMax - xMin) * myPageSize / (myEnd - myBegin + myPageSize));
		result = GxRemap((double)x, xMin, xMax - wBar, myBegin, myEnd);
	}
	return result;
}

// ===================================================================================
// GxScrollbarH
// ===================================================================================

GxScrollbarH::GxScrollbarH()
	:GxScrollbarAbstract(NULL, false)
{
	myPolicy->min.Set(48, 16);
	myPolicy->hint.Set(96, 16);
}

GxScrollbarH::GxScrollbarH(const char* id)
	:GxScrollbarAbstract(id, false)
{
	myPolicy->min.Set(48, 16);
	myPolicy->hint.Set(96, 16);
}

bool GxScrollbarH::IsVertical() const
{
	return false;
}

// ===================================================================================
// GxScrollbarV
// ===================================================================================

GxScrollbarV::GxScrollbarV()
	:GxScrollbarAbstract(NULL, true)
{
}

GxScrollbarV::GxScrollbarV(const char* id)
	:GxScrollbarAbstract(id, true)
{
}

bool GxScrollbarV::IsVertical() const
{
	return true;
}

// ===================================================================================
// Widget events
// ===================================================================================

GxDefineWidgetEvent (GxScrollbarAbstract, eChanged)

}; // namespace widgets
}; // namespace guix