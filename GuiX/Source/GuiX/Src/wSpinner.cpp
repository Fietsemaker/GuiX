#include <GuiX/Config.h>

#include <math.h>

#include <GuiX/Core.h>
#include <GuiX/Draw.h>
#include <GuiX/Style.h>
#include <GuiX/Context.h>

#include <GuiX/wTextEdit.h>
#include <GuiX/wSpinner.h>

namespace guix {
namespace widgets {

namespace {

enum SpinnerItems
{
	I_NO_ITEM,
	I_TEXT_AREA,
	I_INC_BUTTON,
	I_DEC_BUTTON,
};

}; // anonymous namespace

// ===================================================================================
// GxSpinner
// ===================================================================================

GxSpinner::~GxSpinner()
{
}

GxSpinner::GxSpinner()
{
	myInit();
}

GxSpinner::GxSpinner(const char* id)
	:GxWidget(id)
{
	myInit();
}

void GxSpinner::myInit()
{
	GxStyle& style = *GxStyle::Get();

	myPolicy->min.Set(24, 24);
	myPolicy->hint.Set(64, 24);
	myPolicy->flagsH = GX_SP_EXPAND;

	myValue = 0;
	myMin = -1 << 24;
	myMax = 1 << 24;
	myStep = 1;
	myPrecision = 3;
	myFlags.Set(F_INTEGER);

	myMouseOverItem = I_NO_ITEM;

	myField.SetMultiLine(false);
	myField.SetNumerical(true);
	myField.SetMaxLength(32);
	myUpdateValue(myValue, false);

	myField.GetSettings() = style.d.text[0];
	myField.SetHighlightColor(style.c.textSelect);

	myButtonTimer = 0.f;
}

void GxSpinner::OnMousePress(GxMouseEvent& evt)
{
	if(IsHoverWidget())
	{
		if(evt.button == GX_MC_LEFT && !evt.handled && !IsLockedWidget())
		{
			GrabFocus();
			myFlags.Set(F_INTERACTED);

			myMouseOverItem = myGetItemAtPos(evt.x, evt.y);
			if(myMouseOverItem == I_TEXT_AREA && !IsInputWidget())
			{
				GrabInput();
				myField.Select();
			}
			else if(myMouseOverItem == I_INC_BUTTON)
			{
				myUpdateValue(myValue + myStep, true);
				myButtonTimer = 0.f;
			}
			else if(myMouseOverItem == I_DEC_BUTTON)
			{
				myUpdateValue(myValue - myStep, true);
				myButtonTimer = 0.f;
			}

			myField.SetRect(myGetTextRect());
			myField.OnMousePress(evt);
		}
		evt.handled = true;
	}
	if(evt.button == GX_MC_LEFT && IsInputWidget())
	{
		if(myMouseOverItem != I_TEXT_AREA || !IsHoverWidget())
			ReleaseInput();
	}
}

void GxSpinner::OnMouseRelease(GxMouseEvent& evt)
{
	if(evt.button == GX_MC_LEFT && IsFocusWidget())
	{
		ReleaseFocus();
	}
	if(IsInputWidget())
	{
		myField.SetRect(myGetTextRect());
		myField.OnMouseRelease(evt);
		evt.handled = true;
	}
}

void GxSpinner::OnKeyPress(GxKeyEvent& evt)
{
	if(IsInputWidget())
	{
		myField.OnKeyPress(evt);
		if(evt.handled == false && evt.key == GX_KC_RETURN)
		{
			ReleaseInput();
		}
		evt.handled = true;
	}
}

void GxSpinner::OnTextInput(GxTextEvent& evt)
{
	myField.OnTextInput(evt.text);
}

void GxSpinner::OnLoseInput()
{
	myField.Deselect();
	double newVal = 0.0;
	if(myField.GetText().ToDouble(newVal))
		myUpdateValue(newVal, true);
	else 
		myUpdateValue(myValue, true); 
}

void GxSpinner::Tick(float dt)
{
	if(myGetHoverItem() == I_TEXT_AREA || myGetFocusItem() == I_TEXT_AREA)
		GetContext()->SetCursor(GX_CI_CARET);

	GxVec2i mpos = GxInput::Get()->GetMousePos();
	myField.SetRect(myGetTextRect());
	myField.Tick(mpos.x, mpos.y, dt);

	if(!IsFocusWidget())
		myMouseOverItem = myGetItemAtPos(mpos.x, mpos.y);

	if(IsFocusWidget())
	{
		myButtonTimer += dt;
		if(myButtonTimer > 0.5f)
		{
			if(myMouseOverItem == I_INC_BUTTON)
			{
				myUpdateValue(myValue + myStep, true);
				myButtonTimer -= 0.1f;
			}
			else if(myMouseOverItem == I_DEC_BUTTON)
			{
				myUpdateValue(myValue - myStep, true);
				myButtonTimer -= 0.1f;
			}
		}
	}
}

void GxSpinner::Draw()
{
	GxDraw* draw = GxDraw::Get();
	GxStyle& style = *GxStyle::Get();

	float hl = GetHighlightValue();
	bool lock = IsLockedWidget();

	const int ht = myGetHoverItem();
	const int ft = myGetFocusItem();
	GxRecti incr = myGetIncRect(); incr.Shrink(1,1,1,0);
	GxRecti decr = myGetDecRect(); decr.Shrink(1,0,1,1);

	style.Field(myRect);
	style.Button(incr, (ht == I_INC_BUTTON) ? hl : 0, lock);
	style.Button(decr, (ht == I_DEC_BUTTON) ? hl : 0, lock);

	style.d.arrow.SetFlags(GxSprite::MIRROR_V);
	style.d.arrow.Draw(incr.x+incr.w/2, incr.y+incr.h/2);
	style.d.arrow.SetFlags();
	style.d.arrow.Draw(decr.x+decr.w/2, decr.y+decr.h/2);

	myField.Draw();
}

void GxSpinner::SetValue(int value)
{
	myUpdateValue((double)value, false);
}

void GxSpinner::SetValue(double value)
{
	myUpdateValue(value, false);
}

void GxSpinner::SetRange(int min, int max, int step)
{
	myMin = (double)min;
	myMax = (double)max;
	myStep = (double)step;
	myFlags.Set(F_INTEGER);
	myUpdateValue(myValue, false);
}

void GxSpinner::SetRange(double min, double max, double step, int precision)
{
	myMin = min;
	myMax = max;
	myStep = step;
	myPrecision = precision;
	myFlags.Reset(F_INTEGER);
	myUpdateValue(myValue, false);
}

int GxSpinner::GetIntValue() const
{
	return GxInt(myValue);
}

double GxSpinner::GetValue() const
{
	return myValue;
}

double GxSpinner::GetMin() const
{
	return myMin;
}

double GxSpinner::GetMax() const
{
	return myMax;
}

GxTextEditHelper& GxSpinner::GetField()
{
	return myField;
}

const GxTextEditHelper& GxSpinner::GetField() const
{
	return myField;
}

GxRecti GxSpinner::myGetTextRect() const
{
	GxRecti r = myRect;
	r.Shrink(2, 2, 18, 2);
	return r;
}

GxRecti GxSpinner::myGetIncRect() const
{
	return GxRecti(myRect.x + myRect.w - 16, myRect.y, 16, myRect.h/2);
}

GxRecti GxSpinner::myGetDecRect() const
{
	return GxRecti(myRect.x + myRect.w - 16, myRect.y + myRect.h/2, 16, myRect.h/2);
}

int GxSpinner::myGetItemAtPos(int x, int y) const
{
	if(myRect.Contains(x, y))
	{
		if(myGetIncRect().Contains(x, y)) return I_INC_BUTTON;
		if(myGetDecRect().Contains(x, y)) return I_DEC_BUTTON;
		return I_TEXT_AREA;
	}
	return I_NO_ITEM;
}

int GxSpinner::myGetHoverItem() const
{
	return (IsHoverWidget() ? myMouseOverItem : I_NO_ITEM);
}

int GxSpinner::myGetFocusItem() const
{
	return (IsFocusWidget() ? myMouseOverItem : I_NO_ITEM);
}

void GxSpinner::myUpdateValue(double value, bool emitEvent)
{
	double oldValue = myValue;

	// Update value
	if(myFlags[F_INTEGER])
		value = floor(value + 0.5);

	if(abs(value) < 1e-8)
		value = 0.0;

	myValue = GxClamp(value, myMin, myMax);

	// Update text box
	GxString valStr;
	if(myFlags[F_INTEGER])
		valStr.Arg((int)myValue);
	else
		valStr.Arg(myValue, myPrecision);
	myField.SetText(valStr);

	// Emit event
	if(myValue != oldValue && emitEvent)
	{
		EmitEvent(eChanged(), myValue);
		myFlags.Set(F_CHANGED);
	}
}

// ===================================================================================
// Widget events
// ===================================================================================

GxDefineWidgetEvent (GxSpinner, eChanged)

}; // namespace widgets
}; // namespace guix