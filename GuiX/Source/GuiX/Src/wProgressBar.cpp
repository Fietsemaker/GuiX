#include <GuiX/Config.h>

#include <GuiX/Draw.h>
#include <GuiX/wProgressBar.h>

#include <Src/GuiUtils.h>

namespace guix {
namespace widgets {

namespace {

enum ProgressState
{
	PS_INDEFINITE = GxWidget::F_WBIT << 0,
	PS_PERCENTAGE = GxWidget::F_WBIT << 1,
	PS_VALUE_MAX  = GxWidget::F_WBIT << 2,
};

}; // anonymous namespace

// ===================================================================================
// GxProgressBar
// ===================================================================================

GxProgressBar::~GxProgressBar()
{
}

GxProgressBar::GxProgressBar()
	:myValue(0)
	,myMaximum(0)
{
	myPolicy->min.Set(24, 20);
	myPolicy->hint.Set(48, 20);
	myPolicy->flagsH = GX_SP_EXPAND;
}

void GxProgressBar::Tick(float dt)
{
	myValue += dt;
	if(myValue > 1.0) myValue = 0.0;
}

void GxProgressBar::Draw()
{
	GxDraw* draw = GxDraw::Get();
	GxStyle& style = *GxStyle::Get();

	// Bar ragion.
	style.Bar(myRect);
	GxRecti r = myRect; r.Shrink(2);

	// Indefinite progress.
	if(myFlags[PS_INDEFINITE])
	{
		double v = GxClamp(myValue, 0.0, 1.0);
		double w = (double)r.w;
		int a = GxClamp((int)(w * 1.5 * v - w * 0.5), 0, r.w);
		int b = GxClamp((int)(w * 1.5 * v), 0, r.w);
		GxColor hlA = style.c.frameInColorHl[0];
		GxColor hlB = style.c.frameInColorHl[1];
		draw->Rect(r.x + a, r.y, b-a, r.h, hlB, hlA);
	}
	else
	{
		// Calculate progress value.
		double progress = 1.0;
		if(myFlags[PS_PERCENTAGE])
			progress = myValue / 100.0;
		else if(myMaximum > 0.0)
			progress = myValue / myMaximum;

		// Compute progress text.
		GxString text;
		if(myFlags[PS_PERCENTAGE])
			text.Arg(myValue, 1, false).Arg("%");
		else if(myMaximum > 0.0)
			text.Arg(myValue, 2).Arg("/").Arg(myMaximum, 2);

		// Display progress bar.
		int w = (int)((double)r.w * progress);
		GxColor hlA = style.c.frameInColorHl[0];
		GxColor hlB = style.c.frameInColorHl[1];
		draw->Rect(r.x, r.y, w, r.h, hlB, hlA);

		// Display progress text.
		style.Label(r, GX_TA_CENTER, GX_TA_MIDDLE, text, false);
	}
}

void GxProgressBar::SetProgressIndefinite()
{
	myFlags.Set(PS_INDEFINITE);
	myFlags.Reset(PS_PERCENTAGE);
	myFlags.Reset(PS_VALUE_MAX);
}

void GxProgressBar::SetProgress(double percentage)
{
	myFlags.Reset(PS_INDEFINITE);
	myFlags.Set(PS_PERCENTAGE);
	myFlags.Reset(PS_VALUE_MAX);

	myValue = GxClamp(percentage, 0.0, 100.0);
}

void GxProgressBar::SetProgress(double value, double maximum)
{
	myFlags.Reset(PS_INDEFINITE);
	myFlags.Reset(PS_PERCENTAGE);
	myFlags.Set(PS_VALUE_MAX);

	myMaximum = GxMax(maximum, 0.0);
	myValue = GxClamp(value, 0.0, maximum);
}

double GxProgressBar::GetProgress() const
{
	return myFlags[PS_INDEFINITE] ? 0.0 : myValue;
}

}; // namespace widgets
}; // namespace guix