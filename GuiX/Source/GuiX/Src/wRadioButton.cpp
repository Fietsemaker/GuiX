#include <GuiX/Config.h>

#include <GuiX/Style.h>
#include <GuiX/Context.h>
#include <GuiX/wRadioButton.h>

namespace guix {
namespace widgets {

// ===================================================================================
// GxRadioButton
// ===================================================================================

GxRadioButton::~GxRadioButton()
{
}

GxRadioButton::GxRadioButton()
	:myValue(0)
{
	myPolicy->min.Set(16, 16);
	myPolicy->hint.Set(96, 16);
}

GxRadioButton::GxRadioButton(const char* id, const char* group, GxString text)
	:GxWidget(id)
	,myValue(0)
{
	myPolicy->min.Set(16, 16);
	myPolicy->hint.Set(96, 16);

	myText = text;

	if(group) SetGroupId(group);
}

void GxRadioButton::OnMousePress(GxMouseEvent& evt)
{
	if(IsHoverWidget())
	{
		if(evt.button == GX_MC_LEFT && !evt.handled && !IsLockedWidget())
		{
			GrabFocus();
			if(!myFlags[F_SELECTED])
			{
				SetSelected();
				EmitEvent(eSelected(), myValue);
				myFlags.Set(F_CHANGED);
			}
			myFlags.Set(F_INTERACTED);
		}
		evt.handled = true;
	}
}

void GxRadioButton::OnMouseRelease(GxMouseEvent& evt)
{
	if(evt.button == GX_MC_LEFT && IsFocusWidget())
	{
		ReleaseFocus();
	}
}

void GxRadioButton::OnGroupEvent(GxWidget* sender)
{
	if(sender->HasType<GxRadioButton>())
		myFlags.Reset(F_SELECTED);
}

void GxRadioButton::Adjust()
{
	GxStyle& style = *GxStyle::Get();

	if(myPolicy->adjustHint || myPolicy->adjustMin)
	{
		GxRecti r = style.d.text[0].GetTextRect(0, 0, myText);

		if(myPolicy->adjustMin)
			myPolicy->min.x = GxMax(20, r.w + 20);

		if(myPolicy->adjustHint)
			myPolicy->hint.x = GxMax(20, r.w + 20);
	}
}

void GxRadioButton::Draw()
{
	GxDraw* draw = GxDraw::Get();
	GxStyle& style = *GxStyle::Get();

	float hl = GetHighlightValue();
	bool lock = IsLockedWidget();

	// Radio button
	GxRecti box(myRect.x, myRect.y + myRect.h/2 - 8, 16, 16);
	style.Radio(box, hl, lock, myFlags[F_SELECTED]);

	// Label text
	GxRecti r = myRect; r.Shrink(box.w + 2, 0, 0, 0);
	style.Label(r, GX_TA_LEFT, GX_TA_MIDDLE, myText, lock);
}

void GxRadioButton::SetText(GxString text)
{
	myText = text;
}

void GxRadioButton::SetValue(int value)
{
	myValue = value;
}

void GxRadioButton::SetSelected()
{
	myFlags.Set(F_SELECTED, true);
	EmitGroupEvent();
}

const GxString& GxRadioButton::GetText() const
{
	return myText;
}

bool GxRadioButton::IsSelected() const
{
	return myFlags[F_SELECTED];
}

// ===================================================================================
// Widget events
// ===================================================================================

GxDefineWidgetEvent (GxRadioButton, eSelected)

}; // namespace widgets
}; // namespace guix