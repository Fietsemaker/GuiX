#include <GuiX/Config.h>

#include <GuiX/Style.h>
#include <GuiX/Context.h>
#include <GuiX/wDroplist.h>
#include <GuiX/wSelectList.h>

namespace guix {
namespace widgets {

// ===================================================================================
// GxDroplist
// ===================================================================================

GxDroplist::~GxDroplist()
{
}

GxDroplist::GxDroplist() 
{
	myInit();
}

GxDroplist::GxDroplist(const char* id)
	:GxWidget(id)
{
	myInit();
}

void GxDroplist::myInit()
{
	mySelectList = NULL;
	mySelectedItem = 0;

	myPolicy->min.Set(16, 16);
	myPolicy->hint.Set(96, 24);
}

void GxDroplist::OnMousePress(GxMouseEvent& evt)
{
	if(mySelectList)
	{
		if(mySelectList->IsInteracted() || !evt.handled)
		{
			int index = mySelectList->GetSelectedItem();
			
			EmitEvent(eSelected(), myValues[index]);
			if(index >= 0 && mySelectedItem != index)
			{
				mySelectedItem = index;
				EmitEvent(eChanged(), myValues[index]);
				myFlags.Set(F_CHANGED);
			}
			myFlags.Set(F_INTERACTED);

			GetContext()->PopLayer();
			mySelectList = NULL;
		}
		evt.handled = true;
	}
	else if(IsHoverWidget())
	{
		if(evt.button == GX_MC_LEFT && !evt.handled && !IsLockedWidget())
		{
			myOpenList();
		}
		evt.handled = true;
	}
}

void GxDroplist::Draw()
{
	GxDraw* draw = GxDraw::Get();
	GxStyle& style = *GxStyle::Get();

	float hl = GetHighlightValue();
	bool lock = IsLockedWidget();

	GxRecti rect(myRect);
	style.Bar(rect);
	rect.Shrink(1);
	style.Button(rect, hl, lock);
	style.d.arrow.Draw(rect.x + rect.w - 12, rect.y + rect.h/2);
	rect.w -= 12;

	if(mySelectedItem >= 0 && mySelectedItem < myItems.Size())
		style.Label(rect, GX_TA_CENTER, GX_TA_MIDDLE, myItems[mySelectedItem], lock);
}

void GxDroplist::Clear()
{
	myItems.Clear();
	myValues.Clear();
	mySelectedItem = 0;
}

void GxDroplist::AddItem(GxString text)
{
	myItems.Append(text);
	myValues.Append(myValues.Size());
}

void GxDroplist::AddItem(GxString text, const GxVariant& value)
{
	myItems.Append(text);
	myValues.Append(value);
}

void GxDroplist::SetSelectedItem(int index)
{
	mySelectedItem = index;
}

GxVariant GxDroplist::GetValue() const
{
	if(mySelectedItem >= 0 && mySelectedItem < myValues.Size())
		return myValues[mySelectedItem];
	return GxVariant();
}

int GxDroplist::GetSelectedItem() const
{
	return mySelectedItem;
}

int GxDroplist::GetItemCount() const
{
	return myItems.Size();
}

void GxDroplist::myOpenList()
{
	mySelectList = new GxSelectList;

	// Get the view dimensions.
	GxRecti view = GetContext()->GetView();
	GxRecti rect = myRect;

	// Initialize the list.
	for(int i=0; i<myItems.Size(); ++i)
		mySelectList->AddItem(myItems[i]);

	mySelectList->SetSelectedItem(mySelectedItem);

	// Determine the list rectangle.
	mySelectList->Adjust();

	int prfH = GxMax(mySelectList->GetPreferredSize().y, GxMin(myItems.Size()*16, 320));
	int topH = rect.y - view.y - 16;
	int btmH = view.y + view.h - (rect.y + rect.h) - 16;

	GxRecti r(rect.x, rect.y, rect.w, prfH);
	if(topH > btmH && btmH < prfH)
	{
		r.h = GxClamp(r.h, 32, topH);
		r.y = rect.y - r.h;
	}
	else
	{
		r.h = GxClamp(r.h, 32, btmH);
		r.y = rect.y + rect.h;
	}

	GetContext()->PushLayer(mySelectList, r);
}

// ===================================================================================
// Widget events
// ===================================================================================

GxDefineWidgetEvent (GxDroplist, eChanged)
GxDefineWidgetEvent (GxDroplist, eSelected)

}; // namespace widgets
}; // namespace guix