#include <GuiX/Config.h>

#include <GuiX/Style.h>
#include <GuiX/Context.h>
#include <GuiX/wButton.h>

namespace guix {
namespace widgets {

// ===================================================================================
// GxButton
// ===================================================================================

GxButton::~GxButton()
{
}

GxButton::GxButton()
{
	myPolicy->min.Set(16, 16);
	myPolicy->hint.Set(96, 24);
}

GxButton::GxButton(const char* id, GxString text)
	:GxWidget(id)
{
	myPolicy->min.Set(16, 16);
	myPolicy->hint.Set(96, 24);

	myText = text;
}

void GxButton::OnMousePress(GxMouseEvent& evt)
{
	if(IsHoverWidget())
	{
		if(evt.button == GX_MC_LEFT && !evt.handled && !IsLockedWidget())
		{
			GrabFocus();
			EmitEvent(ePressed());
			myFlags.Set(F_INTERACTED);
		}	
		evt.handled = true;
	}
}

void GxButton::OnMouseRelease(GxMouseEvent& evt)
{
	if(evt.button == GX_MC_LEFT && IsFocusWidget())
	{
		ReleaseFocus();
		EmitEvent(eReleased());
	}
}

void GxButton::Adjust()
{
	GxStyle& style = *GxStyle::Get();

	if(myPolicy->adjustHint || myPolicy->adjustMin)
	{
		GxRecti r = style.d.text[0].GetTextRect(0, 0, myText);

		if(myPolicy->adjustMin)
			myPolicy->min.x = GxMax(16, r.w + 16);

		if(myPolicy->adjustHint)
			myPolicy->hint.x = GxMax(96, r.w + 16);
	}
}

void GxButton::Draw()
{
	GxDraw* draw = GxDraw::Get();
	GxStyle& style = *GxStyle::Get();

	float hl = GetHighlightValue();
	bool lock = IsLockedWidget();

	style.Bar(myRect);
	GxRecti rect = myRect;
	rect.Shrink(1);
	style.Button(rect, hl, lock);
	style.Label(rect, GX_TA_CENTER, GX_TA_MIDDLE, myText, lock);
}

void GxButton::SetText(GxString text)
{
	myText = text;
}

const GxString& GxButton::GetText() const
{
	return myText;
}

bool GxButton::IsDown() const
{
	return IsFocusWidget();
}

// ===================================================================================
// Widget events
// ===================================================================================

GxDefineWidgetEvent (GxButton, ePressed)
GxDefineWidgetEvent (GxButton, eReleased)

}; // namespace widgets
}; // namespace guix