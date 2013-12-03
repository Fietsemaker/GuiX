#include <GuiX/Config.h>

#include <GuiX/Style.h>
#include <GuiX/GridLayout.h>
#include <GuiX/wVectorEdit.h>
#include <GuiX/wSpinner.h>
#include <GuiX/wLabel.h>

namespace guix {
namespace widgets {

namespace {

static const char* labels[] = {"x", "y", "z", "w"};

}; // anonymous namespace

// ===================================================================================
// GxVectorEdit
// ===================================================================================

GxVectorEdit::~GxVectorEdit()
{
	GxFree(mySpinners);
}

GxVectorEdit::GxVectorEdit(int numComponents)
	:GxExpandingButton(new GxGridLayout(2))
{
	myComponentCount = GxClamp(numComponents, 1, 64);
	mySpinners = GxMalloc<GxSpinner*>(myComponentCount);

	for(int i=0; i<myComponentCount; ++i)
	{
		if(i < 4)
			myLayout->Add(new GxLabel(labels[i]));
		else
			myLayout->Add(new GxLabel);

		myLayout->Add(mySpinners[i] = new GxSpinner);
		mySpinners[i]->SetRange(-1<<24, 1<<24, 1.0, -1);
	}
}

void GxVectorEdit::Draw()
{
	GxExpandingButton::Draw();

	// Format button text.
	GxString text('(', 1);
	for(int i=0; i<myComponentCount; ++i)
	{
		text.Arg(mySpinners[i]->GetValue(), 2);
		if(i != myComponentCount-1)
			text.Append(", ");
	}
	text.Append(')');

	// Display button text.
	GxRecti rect = myRect;
	rect.w -= myButtonW;
	rect.h = myButtonH;
	rect.Shrink(1);
	bool lock = IsLockedWidget();
	GxStyle::Get()->Label(rect, GX_TA_CENTER, GX_TA_MIDDLE, text, lock);
}

void GxVectorEdit::SetRange(double min, double max)
{
	for(int i=0; i<myComponentCount; ++i)
		mySpinners[i]->SetRange(min, max, 1.0, -1);
}

// ===================================================================================
// GxVectorEdit2D
// ===================================================================================

GxVectorEdit2D::GxVectorEdit2D()
	:GxVectorEdit(2)
{
}

void GxVectorEdit2D::SetValue(GxVec2d value)
{
	mySpinners[0]->SetValue(value.x);
	mySpinners[1]->SetValue(value.y);
}

GxVec2d GxVectorEdit2D::GetValue() const
{
	return GxVec2d(
		mySpinners[0]->GetValue(),
		mySpinners[1]->GetValue());
}

// ===================================================================================
// GxVectorEdit3D
// ===================================================================================

GxVectorEdit3D::GxVectorEdit3D()
	:GxVectorEdit(3)
{
}

void GxVectorEdit3D::SetValue(GxVec3d value)
{
	mySpinners[0]->SetValue(value.x);
	mySpinners[1]->SetValue(value.y);
	mySpinners[2]->SetValue(value.z);
}

GxVec3d GxVectorEdit3D::GetValue() const
{
	return GxVec3d(
		mySpinners[0]->GetValue(),
		mySpinners[1]->GetValue(),
		mySpinners[2]->GetValue());
}

// ===================================================================================
// GxVectorEdit4D
// ===================================================================================

GxVectorEdit4D::GxVectorEdit4D()
	:GxVectorEdit(4)
{
}

void GxVectorEdit4D::SetValue(GxVec4d value)
{
	mySpinners[0]->SetValue(value.x);
	mySpinners[1]->SetValue(value.y);
	mySpinners[2]->SetValue(value.z);
	mySpinners[3]->SetValue(value.w);
}

GxVec4d GxVectorEdit4D::GetValue() const
{
	return GxVec4d(
		mySpinners[0]->GetValue(),
		mySpinners[1]->GetValue(),
		mySpinners[2]->GetValue(),
		mySpinners[3]->GetValue());
}

}; // namespace widgets
}; // namespace guix