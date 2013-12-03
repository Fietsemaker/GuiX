#include <GuiX/Config.h>

#include <GuiX/Common.h>

#include <GuiX/Style.h>
#include <GuiX/Context.h>
#include <GuiX/wLabel.h>

namespace guix {
namespace widgets {

// ===================================================================================
// GxLabel
// ===================================================================================

GxLabel::~GxLabel()
{
	delete mySettings;
}

GxLabel::GxLabel()
{
	myInit();
}

GxLabel::GxLabel(GxString text)
{
	myText = text;
	myInit();
}

void GxLabel::myInit()
{
	mySettings = NULL;

	myPolicy->min.Set(8, 16);
	myPolicy->hint.Set(96, 16);	
}

void GxLabel::Adjust()
{
	GxStyle& style = *GxStyle::Get();

	if(myPolicy->adjustHint || myPolicy->adjustMin)
	{
		GxText* settings = mySettings;
		if(!settings)
		{
			settings = &style.d.text[0];
			settings->alignH = GX_TA_LEFT;
			settings->alignV = GX_TA_MIDDLE;
		}

		if(myRect.w > 0 && !settings->HasFlag(GX_TF_ELLIPSIS))
			settings->maxWidth = GxMax(0, myRect.w - 4);

		GxRecti r = settings->GetTextRect(0, 0, myText);

		if(myPolicy->adjustMin)
			myPolicy->min.Set(8, 16);

		if(myPolicy->adjustHint)
		{
			myPolicy->hint.x = GxMax(8, r.w + 4);
			myPolicy->hint.y = GxMax(16, r.h + 4);
		}

		settings->SetMaxWidth();
	}
}

void GxLabel::Draw()
{
	GxDraw* draw = GxDraw::Get();
	GxStyle& style = *GxStyle::Get();

	GxRecti r = myRect;
	if(r.w > 2 && r.h > 2)
	{
		draw->PushScissorRect(r.x, r.y, r.w, r.h);

		GxText* settings = mySettings;
		if(!settings)
		{
			settings = &style.d.text[0];
			settings->alignH = GX_TA_LEFT;
			settings->alignV = GX_TA_MIDDLE;
		}

		GxTextAlignH h = settings->alignH;
		GxTextAlignV v = settings->alignV;
		int x = (h == GX_TA_LEFT) ? 2 : ((h == GX_TA_CENTER) ? r.w/2 : r.w-2);
		int y = (v == GX_TA_TOP ) ? 2 : ((v == GX_TA_MIDDLE) ? r.h/2 : r.h-2);

		settings->maxWidth = r.w - 4;
		settings->Draw(r.x + x, r.y + y - 2, myText);
		settings->SetMaxWidth();

		draw->PopScissorRect();
	}
}

void GxLabel::SetText(GxString text)
{
	myText = text;
}

void GxLabel::SetMultiLine(bool enabled)
{
	if(!mySettings)
		mySettings = new GxText(GetTextSettings());
	mySettings->SetFlag(GX_TF_ELLIPSIS, false);
}

void GxLabel::SetTextAlignH(GxTextAlignH alignH)
{
	if(!mySettings) 
		mySettings = new GxText(GetTextSettings());
	mySettings->alignH = alignH;
}

const GxString& GxLabel::GetText() const
{
	return myText;
}

void GxLabel::SetTextSettings(const GxText& settings)
{
	if(!mySettings) 
		mySettings = new GxText(settings);
	else
		*mySettings = settings;
}

GxText GxLabel::GetTextSettings() const
{
	if(mySettings) return *mySettings;

	GxText settings = GxStyle::Get()->d.text[0];
	settings.alignV = GX_TA_MIDDLE;
	settings.alignH = GX_TA_LEFT;

	return settings;
}

}; // namespace widgets
}; // namespace guix