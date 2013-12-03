#include <GuiX/Config.h>

#include <GuiX/Style.h>
#include <GuiX/Context.h>
#include <GuiX/GridLayout.h>
#include <GuiX/wColorEdit.h>
#include <GuiX/wSliderEdit.h>
#include <GuiX/wLabel.h>

namespace guix {
namespace widgets {

namespace {

static const char* labels[] = {"R", "G", "B", "A"};

inline GxVec4f GetSliderColor(const GxSliderEdit* const* s)
{
	double a = s[3] ? s[3]->GetValue() : 1.0;
	return GxVec4f(
		(float)s[0]->GetValue(),
		(float)s[1]->GetValue(),
		(float)s[2]->GetValue(),
		(float)a);
}

inline void SetSliderColor(GxSliderEdit** s, const GxVec4f& c)
{
	s[0]->SetValue((double)c.x);
	s[1]->SetValue((double)c.y);
	s[2]->SetValue((double)c.z);
	if(s[3]) s[3]->SetValue((double)c.w);
}

}; // anonymous namespace

// ===================================================================================
// GxColorEdit
// ===================================================================================

GxColorEdit::~GxColorEdit()
{
}

GxColorEdit::GxColorEdit()
	:GxExpandingButton(new GxGridLayout(2))
{
	myInit(GxVec4f(1,1,1,1), false);
}

GxColorEdit::GxColorEdit(const char* id, GxVec4f col, bool alpha)
	:GxExpandingButton(new GxGridLayout(2))
{
	SetWidgetId(id);
	myInit(col, alpha);
}

void GxColorEdit::myInit(const GxVec4f& col, bool alpha)
{
	mySliders[3] = NULL;
	for(int i=0; i<4; ++i)
	{
		if(i < 3 || alpha)
		{
			myLayout->Add(new GxLabel(labels[i]));
			myLayout->Add(mySliders[i] = new GxSliderEdit);
			mySliders[i]->SetRange(0.0, 1.0, 0.1, 2);
			mySliders[i]->SetValue(col.cell[i]);
		}
	}
}

void GxColorEdit::Draw()
{
	GxExpandingButton::Draw();

	GxDraw* draw = GxDraw::Get();
	GxStyle& style = *GxStyle::Get();

	GxRecti r = myRect;

	// Draw the preview color.
	r.Shrink(3, 3, myButtonW + 3, r.h - myButtonH + 3);
	draw->Rect(r.x, r.y, r.w, r.h, style.c.frameOutline);

	r.Shrink(1);
	GxVec4f colf = GetSliderColor(mySliders);
	GxColor col(colf.x, colf.y, colf.z, colf.w);
	draw->Rect(r.x, r.y, r.w, r.h, col);
	if(col.a < 255)
		draw->Rect(r.x, r.y, r.w/2, r.h, col.Alpha(255));
}

void GxColorEdit::SetColor(GxVec4f color)
{
	SetSliderColor(mySliders, color);
}

GxVec4f GxColorEdit::GetColorf() const
{
	return GetSliderColor(mySliders);
}

GxColor GxColorEdit::GetColor() const
{
	GxVec4f c = GetSliderColor(mySliders);
	return GxColor(c.x, c.y, c.z, c.w);
}

// ===================================================================================
// Widget events
// ===================================================================================

GxDefineWidgetEvent (GxColorEdit, eChanged)

}; // namespace widgets
}; // namespace guix