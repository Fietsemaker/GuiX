#include <GuiX/Config.h>

#include <GuiX/Common.h>

#include <GuiX/Style.h>
#include <GuiX/Context.h>
#include <GuiX/wSelectList.h>

namespace guix {
namespace widgets {

// ===================================================================================
// GxSelectList
// ===================================================================================

GxSelectList::~GxSelectList()
{
	delete myScrollbar;
}

GxSelectList::GxSelectList() 
	:myScrollbar(NULL)
	,mySelectedItem(-1)
{
	myAlignH = GX_TA_CENTER;

	myPolicy->min.Set(48, 48);
	myPolicy->hint.Set(96, 96);
	myPolicy->flagsV = GX_SP_EXPAND;

	myScrollbar = new GxScrollbarV();
	myScrollbar->SetParent(this);
}

GxSelectList::GxSelectList(const char* id)
	:GxWidget(id)
	,myScrollbar(NULL)
	,mySelectedItem(-1)
{
	myAlignH = GX_TA_CENTER;

	myPolicy->min.Set(48, 48);
	myPolicy->hint.Set(96, 96);
	myPolicy->flagsV = GX_SP_EXPAND;

	myScrollbar = new GxScrollbarV();
	myScrollbar->SetParent(this);	
}

void GxSelectList::OnMousePress(GxMouseEvent& evt)
{
	if(IsHoverWidget())
	{
		if(evt.button == GX_MC_LEFT && !evt.handled && !IsLockedWidget())
		{
			GrabFocus();

			GxRecti r = myGetListRect();
			int index = myGetItemAtPos(evt.x, evt.y);

			if(r.Contains(evt.x, evt.y))
			{
				EmitEvent(eSelected(), index);
				if(mySelectedItem != index)
				{
					mySelectedItem = index;
					EmitEvent(eChanged(), index);
					myFlags.Set(F_CHANGED);
				}
				myFlags.Set(F_INTERACTED);
			}
		}
		evt.handled = true;
	}
}

void GxSelectList::OnMouseRelease(GxMouseEvent& evt)
{
	if(evt.button == GX_MC_LEFT && IsFocusWidget())
	{
		ReleaseFocus();
	}
}

void GxSelectList::OnMouseScroll(GxScrollEvent& evt)
{
	if(IsHoverWidget() && !evt.handled)
	{
		if(evt.up)
			myScrollbar->ScrollUp();
		else
			myScrollbar->ScrollDown();
		evt.handled = true;
	}
}

void GxSelectList::Adjust()
{
	if(myPolicy->adjustHint)
	{
		int itemH = myItems.Size() * 16 + 4;
		myPolicy->hint.y = GxClamp(itemH, 20, 128);
	}
}

void GxSelectList::SetRect(const GxRecti& rect)
{
	GxRecti r = myRect = rect;

	r.Shrink(r.w - 16, 0, 0, 0);
	int itemH = myItems.Size() * 16 + 4;

	myScrollbar->SetRect(r);
	myScrollbar->SetDisabled(itemH <= r.h);
	myScrollbar->SetRange(0, itemH - r.h, r.h, 16);
}

GxWidget* GxSelectList::FindHoverWidget(int x, int y)
{
	GxWidget* w = myScrollbar->FindHoverWidget(x, y);
	if(!w) w = GxWidget::FindHoverWidget(x, y);
	return w;
}

void GxSelectList::Tick(float dt)
{
	if(!myScrollbar->IsHidden())
		myScrollbar->Tick(dt);

	if(IsHoverWidget())
	{
		GxVec2i pos = GxInput::Get()->GetMousePos();
		myMouseOverItem = myGetItemAtPos(pos.x, pos.y);
	}
}

void GxSelectList::Draw()
{
	GxDraw* draw = GxDraw::Get();
	GxStyle& style = *GxStyle::Get();

	float hl = GetHighlightValue();
	bool lock = IsLockedWidget();

	// Draw the list rectangle
	GxRecti bar(myRect);
	if(!myScrollbar->IsHidden())
		bar.w -= 16;
	style.Field(bar);

	// Draw the scrollbar
	if(!myScrollbar->IsHidden())
		myScrollbar->Draw();

	// Draw the list of items
	draw->PushScissorRect(bar.x+2, bar.y+2, bar.w-4, bar.h-4);

	GxRecti viewRect = draw->GetScissorRect();
	GxRecti listR = myGetListRect();

	int ox = 2;
	GxText& settings = style.d.text[0];
	settings.maxWidth = listR.w - ox*2;
	settings.SetAlign(myAlignH, GX_TA_MIDDLE);
	if(myAlignH == GX_TA_CENTER) ox = listR.w/2;
	else if(myAlignH == GX_TA_RIGHT) ox = listR.w - ox;

	for(int i=0; i<myItems.Size(); ++i)
	{
		GxRecti r(listR.x, listR.y + i*16, listR.w, 16);
		if(r.y < viewRect.y+viewRect.h && r.y+r.h > viewRect.y)
		{
			if(i == mySelectedItem)
				draw->Rect(r.x, r.y, r.w, r.h, style.c.textSelect);
			else if(i == myMouseOverItem)
				draw->Rect(r.x, r.y, r.w, r.h, style.c.textSelect.Alpha(hl * 0.5f));
			settings.Draw(r.x + ox, r.y+r.h/2-2, myItems[i]);
		}
	}

	settings.SetMaxWidth();

	draw->PopScissorRect();
}

void GxSelectList::Clear()
{
	myItems.Clear();
	mySelectedItem = -1;
}

void GxSelectList::AddItem(GxString text)
{
	myItems.Append(text);
}

void GxSelectList::SetSelectedItem(int index)
{
	mySelectedItem = index;
}

void GxSelectList::SetTextAlignH(GxTextAlignH alignH)
{
	myAlignH = alignH;
}

GxString GxSelectList::GetItem(int index) const
{
	if(index >= 0 && index < myItems.Size())
		return myItems[index];

	return GxString();
}

int GxSelectList::GetSelectedItem() const
{
	return mySelectedItem;
}

int GxSelectList::GetItemCount() const
{
	return myItems.Size();
}

GxRecti GxSelectList::myGetListRect() const
{
	if(myScrollbar->IsHidden())
	{
		GxRecti r = myRect; r.Shrink(2);
		return r;
	}

	const int ofs = 2 - GxInt(myScrollbar->GetValue());
	return GxRecti(myRect.x + 2, myRect.y + ofs, myRect.w - 20, myItems.Size() * 16);
}

int GxSelectList::myGetItemAtPos(int x, int y) const
{
	GxRecti r = myGetListRect();
	for(int i=0; i<myItems.Size(); ++i)
	{
		if(y < r.y + 16)
			return i;
		r.y += 16;
	}
	return -1;
}

// ===================================================================================
// Widget events
// ===================================================================================

GxDefineWidgetEvent (GxSelectList, eChanged)
GxDefineWidgetEvent (GxSelectList, eSelected)

}; // namespace widgets
}; // namespace guix