#include <GuiX/Config.h>

#include <GuiX/Style.h>
#include <GuiX/Context.h>
#include <GuiX/GridLayout.h>
#include <GuiX/wColorPicker.h>
#include <GuiX/wSliderEdit.h>
#include <GuiX/wLabel.h>

namespace guix {
namespace widgets {

namespace {

static const char* rgbLabels[] = {"R", "G", "B"};

// ===================================================================================
// Helper functions
// ===================================================================================

static float Min3(float x, float y, float z)
{
	return (y <= z) ? ((x <= y) ? x : y) : ((x <= z) ? x : z);
}

static float Max3(float x, float y, float z)
{
	return (y >= z) ? ((x >= y) ? x : y) : ((x >= z) ? x : z);
}

static float HuetoRGB(float p, float q, float t)
{
    if(t < 0) t += 1;
    if(t > 1) t -= 1;
    if(t < (1/6.f)) return p + (q - p) * 6 * t;
    if(t < (1/2.f)) return q;
    if(t < (2/3.f)) return p + (q - p) * ((2/3.f) - t) * 6;
    return p;
}

static GxVec4f RGBtoHSL(const GxVec4f& c)
{
	float max = Max3(c.x, c.y, c.z);
	float min = Min3(c.x, c.y, c.z);
	float h = 0;
	float s = 0;
	float d = max - min;
	float l = (max + min) / 2;

    if(max != min)
    {
        s = l > 0.5 ? d / (2 - max - min) : d / (max + min);
		     if(max == c.x) {h = (c.y - c.z) / d + (c.y < c.z ? 6 : 0);}
        else if(max == c.y) {h = (c.z - c.x) / d + 2;}
        else if(max == c.z) {h = (c.x - c.y) / d + 4;}
        h /= 6;
    }
	return GxVec4f(h, s, l, c.w);
}

static GxVec4f HSLtoRGB(const GxVec4f& c)
{
	float outR, outG, outB;
    if(c.y <= 0)
	{
		outR = outG = outB = c.z;
    }
	else
	{
        float q = c.z < 0.5 ? c.z * (1 + c.y) : c.z + c.y - c.z * c.y;
        float p = 2 * c.z - q;
        outR = HuetoRGB(p, q, c.x + 1/3.f);
        outG = HuetoRGB(p, q, c.x);
        outB = HuetoRGB(p, q, c.x - 1/3.f);
    }
	return GxVec4f(outR, outG, outB, c.w);
}

enum ColorPickerItem
{
	I_NO_ITEM,
	I_GRADIENT,
	I_LUMINANCE
};

inline GxVec4f GetRGBColor(const GxSliderEdit* const* s)
{
	return GxVec4f(
		(float)s[0]->GetValue(),
		(float)s[1]->GetValue(),
		(float)s[2]->GetValue(),
		1);
}

inline void SetRGBColor(GxSliderEdit** s, const GxVec4f& c)
{
	s[0]->SetValue((double)c.x);
	s[1]->SetValue((double)c.y);
	s[2]->SetValue((double)c.z);
}

}; // anonymous namespace

// ===================================================================================
// GxColorPicker
// ===================================================================================

GxColorPicker::~GxColorPicker()
{
	delete myGrid;
}

GxColorPicker::GxColorPicker()
{
	myRGB = GxVec4f(1,1,1,1);
	myInit();
}

GxColorPicker::GxColorPicker(const char* id, GxVec4f color)
	:GxWidget(id)
{
	myRGB = color;
	myInit();
}

void GxColorPicker::myInit()
{
	myHSL = RGBtoHSL(myRGB);

	myGrid = new GxGridLayout(2);
	myGrid->SetOwner(this);

	myPolicy->min.Set(48, 96);
	myPolicy->hint.Set(96, 192);

	for(int i=0; i<3; ++i)
	{
		mySliders[i] = new GxSliderEdit;
		GxLabel* label = new GxLabel(rgbLabels[i]);

		myGrid->Add(label);
		myGrid->Add(mySliders[i]);
		mySliders[i]->SetRange(0.0, 1.0, 0.1, 2);
		mySliders[i]->SetValue(myRGB.cell[i]);
	}

	myHoverItem = I_NO_ITEM;
}

void GxColorPicker::OnMousePress(GxMouseEvent& evt)
{
	if(evt.button == GX_MC_LEFT && IsHoverWidget() && !evt.handled)
	{
		myHoverItem = myGetItemAtPos(evt.x, evt.y);
		GrabFocus();
		evt.handled = true;
		myFlags.Set(F_INTERACTED);
	}
}

void GxColorPicker::OnMouseRelease(GxMouseEvent& evt)
{
	if(evt.button == GX_MC_LEFT && IsFocusWidget())
	{
		ReleaseFocus();
	}
}

void GxColorPicker::Adjust()
{
	myGrid->Adjust();
}

void GxColorPicker::SetRect(const GxRecti& rect)
{
	myRect = rect;
	myGrid->Arrange(myGetControlsRect());
}

GxWidget* GxColorPicker::FindHoverWidget(int x, int y)
{
	GxWidget* hover = NULL;
	if(myRect.Contains(x, y))
	{
		hover = myGrid->FindHoverWidget(x, y);
		if(!hover) hover = this;
	}
	return hover;
}

void GxColorPicker::Tick(float dt)
{
	GxVec2i mpos = GxInput::Get()->GetMousePos();

	// Tick the HSL controls.
	if(IsFocusWidget())
	{
		if(myHoverItem == I_GRADIENT)
			myDragGradient(mpos.x, mpos.y);
		if(myHoverItem == I_LUMINANCE)
			myDragLuminance(mpos.x, mpos.y);
	}
	else if(IsHoverWidget())
	{
		myHoverItem = myGetItemAtPos(mpos.x, mpos.y);
	}

	// Tick the RGB controls.
	myGrid->Tick(dt);
	GxVec4f color = GetRGBColor(mySliders);
	if(memcmp(&myRGB, &color, sizeof(GxVec4f)) != 0)
	{
		myRGB = color;
		myHSL = RGBtoHSL(color);
		EmitEvent(eChanged(), myRGB);
	}
}

void GxColorPicker::Draw()
{
	GxDraw* draw = GxDraw::Get();
	GxStyle& style = *GxStyle::Get();

	// Draw the hue/saturation gradient.
	GxRecti r = myGetGradientRect();

	style.Bar(r);
	r.Shrink(2);
	draw->Rect(r.x, r.y, r.w, r.h, 255, style.d.hueMap);

	int markerX = r.x + GxInt((0+myHSL.x) * (float)r.w);
	int markerY = r.y + GxInt((1-myHSL.y) * (float)r.h);
	
	// Draw the luminance slider.
	r = myGetLuminanceRect();

	GxVec4f ch = HSLtoRGB(GxVec4f(myHSL.x, myHSL.y, 0.5f, 1));
	GxColor a = GxColor(255);
	GxColor b = GxColor(ch.x, ch.y, ch.z, 1.f);
	GxColor c = GxColor(0);

	style.Bar(r);
	r.Shrink(2);
	draw->Rect(r.x, r.y, r.w, r.h/2, a, b);
	draw->Rect(r.x, r.y+r.h/2, r.w, r.h/2, b, c);

	int sliderY = r.y + GxInt((1 - myHSL.z) * (float)r.h);

	// Draw the preview color.
	r = myGetPreviewRect();

	style.Bar(r);
	r.Shrink(2);

	GxVec4f rgb = GetRGBColor(mySliders);
	draw->Rect(r.x, r.y, r.w, r.h, GxColor(rgb.x, rgb.y, rgb.z));

	// Draw the hue/saturation indicator.
	draw->Rect(markerX-3, markerY, 6, 1, 255);
	draw->Rect(markerX, markerY-3, 1, 6, 255);

	// Draw the luminance indicator.
	draw->Rect(r.x, sliderY, r.w, 2, GxColor(255), GxColor(0));

	// Draw the RGB controls.
	myGrid->Draw();
}

void GxColorPicker::SetColor(GxVec4f color)
{
	myRGB = color;
	myHSL = RGBtoHSL(color);
	SetRGBColor(mySliders, color);
}

GxVec4f GxColorPicker::GetColorf() const
{
	return myRGB;
}

GxColor GxColorPicker::GetColor() const
{
	return GxColor(myRGB.x, myRGB.y, myRGB.z, myRGB.w);
}

GxRecti GxColorPicker::myGetGradientRect() const
{
	GxRecti r = myGetControlsRect();
	return GxRecti(myRect.x, myRect.y, myRect.w-16, r.y-myRect.y-2);
}

GxRecti GxColorPicker::myGetLuminanceRect() const
{
	GxRecti r = myGetControlsRect();
	return GxRecti(myRect.x+myRect.w-14, myRect.y, 14, r.y-myRect.y-18);
}

GxRecti GxColorPicker::myGetPreviewRect() const
{
	GxRecti r = myGetControlsRect();
	return GxRecti(myRect.x+myRect.w-14, r.y-16, 14, 14);
}

GxRecti GxColorPicker::myGetControlsRect() const
{
	GxVec2i s = myGrid->GetPreferredSize();
	return GxRecti(myRect.x, myRect.y+myRect.h-s.y, myRect.w, s.y);
}

void GxColorPicker::myDragGradient(int x, int y)
{
	GxRecti r = myGetGradientRect();
	r.Shrink(2);

	GxVec4f hsl = myHSL;
	hsl.x = 0.f + (float)(x - r.x) / (float)GxMax(1, r.w);
	hsl.y = 1.f - (float)(y - r.y) / (float)GxMax(1, r.h);
	hsl.x = GxClamp<float>(hsl.x, 0, 1);
	hsl.y = GxClamp<float>(hsl.y, 0, 1);
	myUpdateHSL(hsl);
}

void GxColorPicker::myDragLuminance(int x, int y)
{
	GxRecti r = myGetLuminanceRect();
	r.Shrink(2);

	GxVec4f hsl = myHSL;
	hsl.z = 1.f - (float)(y - r.y) / (float)GxMax(1, r.h);
	hsl.z = GxClamp<float>(hsl.z, 0, 1);
	myUpdateHSL(hsl);
}

int GxColorPicker::myGetItemAtPos(int x, int y) const
{
	if(myGetGradientRect().Contains(x, y))
		return I_GRADIENT;

	if(myGetLuminanceRect().Contains(x, y))
		return I_LUMINANCE;

	return I_NO_ITEM;
}

void GxColorPicker::myUpdateHSL(const GxVec4f& hsl)
{
	if(memcmp(&myHSL, &hsl, sizeof(GxVec4f)) != 0)
	{
		myHSL = hsl;
		myRGB = HSLtoRGB(myHSL);
		SetRGBColor(mySliders, myRGB);
		EmitEvent(eChanged(), myRGB);
	}
}

// ===================================================================================
// Widget events
// ===================================================================================

GxDefineWidgetEvent (GxColorPicker, eChanged)

}; // namespace widgets
}; // namespace guix