#include <GuiX/Config.h>

#include <GuiX/ListLayout.h>
#include <GuiX/wSliderEdit.h>
#include <GuiX/wSpinner.h>
#include <GuiX/wSlider.h>

namespace guix {
namespace widgets {

// ===================================================================================
// GxSliderEdit
// ===================================================================================

GxSliderEdit::GxSliderEdit()
{
	myInit();
}

GxSliderEdit::GxSliderEdit(const char* id)
	:GxWidget(id)
{
	myInit();
}

void GxSliderEdit::myInit()
{
	myValue = 0.0;

	myLayout = new GxListLayout(true);
	myLayout->SetOwner(this);

	myLayout->Add(mySlider = new GxSliderH);
	myLayout->Add(mySpinner = new GxSpinner);

	mySpinner->GetPolicy().flagsH = GX_SP_RESIZE;
	mySpinner->SetRange(0.0, 1.0, 0.1, 2);

	myLayout->Adjust();

	myPolicy->min = myLayout->GetMinimumSize();
	myPolicy->hint = myLayout->GetPreferredSize();
	myPolicy->flagsH = GX_SP_EXPAND;
}

GxSliderEdit::~GxSliderEdit()
{
	delete myLayout;
}

void GxSliderEdit::Adjust()
{
	myLayout->Adjust();
}

void GxSliderEdit::SetRect(const GxRecti& r)
{
	myRect = r;
	myLayout->Arrange(r);
}

GxWidget* GxSliderEdit::FindHoverWidget(int x, int y)
{
	GxWidget* hover = NULL;
	if(myRect.Contains(x, y))
	{
		hover = myLayout->FindHoverWidget(x, y);
		if(!hover) hover = GxWidget::FindHoverWidget(x, y);
	}
	return hover;
}

void GxSliderEdit::Tick(float dt)
{
	myLayout->Tick(dt);

	double sliderV = mySlider->GetValue();
	double spinnerV = mySpinner->GetValue();

	if(myValue != sliderV)
	{
		myUpdateValue(sliderV, true);
	}
	else if(myValue != spinnerV)
	{
		myUpdateValue(spinnerV, true);
	}
}

void GxSliderEdit::Draw()
{
	myLayout->Draw();
}

void GxSliderEdit::SetValue(int value)
{
	myUpdateValue((double)value, false);
}

void GxSliderEdit::SetValue(double value)
{
	myUpdateValue(value, false);
}

void GxSliderEdit::SetRange(int min, int max, int step)
{
	mySlider->SetRange(min, max);
	mySpinner->SetRange(min, max, step);
}

void GxSliderEdit::SetRange(double min, double max, double step, int precision)
{
	mySlider->SetRange(min, max);
	mySpinner->SetRange(min, max, step, precision);
}

double GxSliderEdit::GetValue() const
{
	return myValue;
}

int GxSliderEdit::GetIntValue() const
{
	return GxInt(myValue);
}

GxSliderH* GxSliderEdit::GetSlider()
{
	return mySlider;
}

GxSpinner* GxSliderEdit::GetSpinner()
{
	return mySpinner;
}

void GxSliderEdit::myUpdateValue(double value, bool emitEvent)
{
	mySpinner->SetValue(value);
	value = mySpinner->GetValue();

	if(myValue != value && emitEvent)
	{
		EmitEvent(eChanged(), value);
		myFlags.Set(F_CHANGED);
	}

	mySpinner->SetValue(value);
	mySlider->SetValue(value);
	myValue = value;
}

// ===================================================================================
// Widget events
// ===================================================================================

GxDefineWidgetEvent (GxSliderEdit, eChanged)

}; // namespace widgets
}; // namespace guix