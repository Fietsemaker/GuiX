#include <GuiX/Config.h>

#include <GuiX/Style.h>
#include <GuiX/Context.h>
#include <GuiX/wCheckbox.h>

namespace guix {
namespace widgets {

// ===================================================================================
// GxCheckbox
// ===================================================================================

GxCheckbox::~GxCheckbox()
{
}

GxCheckbox::GxCheckbox()
	:myVarPtr(NULL)
{
	myPolicy->min.Set(16, 16);
	myPolicy->hint.Set(96, 16);
}

GxCheckbox::GxCheckbox(const char* id, GxString text, bool checked)
	:GxWidget(id)
	,myVarPtr(NULL)
{
	myPolicy->min.Set(16, 16);
	myPolicy->hint.Set(96, 16);
	
	myFlags.Set(F_CHECKED, checked);

	myText = text;
}

void GxCheckbox::OnMousePress(GxMouseEvent& evt)
{
	if(IsHoverWidget())
	{
		if(evt.button == GX_MC_LEFT && !evt.handled && !IsLockedWidget())
		{
			GrabFocus();

			myFlags.Flip(F_CHECKED);

			if(myVarPtr) 
				*myVarPtr = myFlags[F_CHECKED];

			EmitEvent(eChanged(), myFlags[F_CHECKED]);
			myFlags.Set(F_INTERACTED);
			myFlags.Set(F_CHANGED);
		}
		evt.handled = true;
	}
}

void GxCheckbox::OnMouseRelease(GxMouseEvent& evt)
{
	if(evt.button == GX_MC_LEFT && IsFocusWidget())
	{
		ReleaseFocus();
	}
}

void GxCheckbox::Adjust()
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

void GxCheckbox::Draw()
{
	GxDraw* draw = GxDraw::Get();
	GxStyle& style = *GxStyle::Get();

	float hl = GetHighlightValue();
	bool lock = IsLockedWidget();

	// Checkbox
	GxRecti box(myRect.x, myRect.y + myRect.h/2 - 8, 16, 16);
	style.Bar(box);
	box.Shrink(1);
	style.Button(box, hl, lock);
	if(myFlags[F_CHECKED])
		style.d.checkmark.Draw(box.x + box.w/2, box.y + box.h/2);

	// Label text
	GxRecti label = myRect;
	label.Shrink(box.w + 2, 0, 0, 0);
	style.Label(label, GX_TA_LEFT, GX_TA_MIDDLE, myText, lock);
}

void GxCheckbox::SetText(const GxString& text)
{
	myText = text;
}

void GxCheckbox::SetVarPtr(bool* var)
{
	myVarPtr = var;
}

void GxCheckbox::SetChecked(bool checked)
{
	myFlags.Set(F_CHECKED, checked);
}

const GxString& GxCheckbox::GetText() const
{
	return myText;
}

bool GxCheckbox::IsChecked() const
{
	return myFlags[F_CHECKED];
}

// ===================================================================================
// Widget events
// ===================================================================================

GxDefineWidgetEvent (GxCheckbox, eChanged)

}; // namespace widgets
}; // namespace guix