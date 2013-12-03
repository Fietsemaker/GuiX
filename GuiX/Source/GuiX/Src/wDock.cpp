#include <GuiX/Config.h>
#include <GuiX/Core.h>

#include <GuiX/Style.h>
#include <GuiX/Context.h>
#include <GuiX/ListLayout.h>

#include <GuiX/wDock.h>
#include <GuiX/wScrollbar.h>

namespace guix {
namespace widgets {

namespace {

static const int btnW = 20;
static const int barH = 20;
static const int binCount = 2;
static const int binRange = 32;
static const int scrollbarW = 16;

enum FocusActions
{
	FA_NONE,
	FA_RESIZE,
	FA_DRAG,
	FA_PRESS,
};

enum BinIndices
{
	BIN_L = 0,
	BIN_R = 1,
};

enum DockState
{
	DS_NONE      = 0,
	DS_FLOATING  = 1 << 0,
	DS_COLLAPSED = 1 << 1,
};

}; // anonymous namespace

// ===================================================================================
// GxDock
// ===================================================================================

GxDock::~GxDock()
{
}

GxDock::GxDock()
	:GxContainer(new GxListLayout())
	,myOwner(NULL)
	,myState(DS_FLOATING)
{
	myPolicy->min.Set(64, barH);
	myPolicy->hint.Set(128, barH);

	SetFlags(F_ALL_BUT_CLOSE);
}

void GxDock::Adjust()
{
	if(!myState[DS_COLLAPSED])
	{
		myLayout->SetMargin(GxMargini(4, 4, 4, 5));
		myLayout->Adjust();
	
		if(myPolicy->adjustFlags)
		{
			GxFlags flagsH = myLayout->GetPolicyFlagsH();
			GxFlags flagsV = myLayout->GetPolicyFlagsV();

			if(myState[DS_COLLAPSED])
				flagsV = GX_SP_FIXED;

			if(myFlags[F_HINT_AS_MIN])
			{
				flagsH.Reset(GX_SPF_SHRINK);
				flagsV.Reset(GX_SPF_SHRINK);
			}

			myPolicy->flagsH = flagsH;
			myPolicy->flagsV = flagsV;
		}

		if(myPolicy->adjustMin)
		{
			myPolicy->min = myLayout->GetMinimumSize();
			myPolicy->min.x = GxMax(64, myPolicy->min.x);
			myPolicy->min.y += barH;
		}

		if(myPolicy->adjustHint)
		{
			myPolicy->hint = myLayout->GetPreferredSize();
			myPolicy->hint.x = GxMax(128, myPolicy->hint.x);
			myPolicy->hint.y += barH;
		}
	}
	else
	{
		if(myPolicy->adjustFlags)
		{
			myPolicy->flagsH = GX_SP_RESIZE;
			myPolicy->flagsV = GX_SP_FIXED;
		}
	}

	myClampFloatSize(GxVec2i());
}

void GxDock::SetRect(const GxRecti& rect)
{
	GxRecti r = myRect = rect;
	r.Shrink(0, barH, 0, 0);
	myLayout->Arrange(r);
}

GxWidget* GxDock::FindHoverWidget(int x, int y)
{
	if(myRect.Contains(x, y))
	{
		GxWidget* w = myLayout->FindHoverWidget(x, y);
		return w ? w : this;
	}
	return NULL;
}

GxVec2i GxDock::GetMinimumSize() const
{
	GxVec2i result = myPolicy->GetMinSize();
	if(myState[DS_COLLAPSED]) result.y = barH;
	return result;
}

GxVec2i GxDock::GetPreferredSize() const
{
	GxVec2i result = myPolicy->hint;
	if(myState[DS_COLLAPSED]) result.y = barH;
	return result;
}

void GxDock::SetFloatingPos(int x, int y)
{
	myFloatRect.x = x;
	myFloatRect.y = y;
}

void GxDock::SetFloatingSize(int w, int h)
{
	myFloatRect.w = w;
	myFloatRect.h = h;
}

void GxDock::Tick(float dt)
{
	if(!myState[DS_COLLAPSED])
		myLayout->Tick(dt);
}

void GxDock::Draw()
{
	GxDraw* draw = GxDraw::Get();
	GxStyle& style = *GxStyle::Get();

	// Get the mouse over item
	GxVec2i mpos = GxInput::Get()->GetMousePos();
	Item mouseItem = myGetItemAt(mpos.x, mpos.y);

	// Draw dock frame.
	GxRecti r = myRect;
	if(IsFloating())
	{
		GxRecti f = r;
		f.Expand(16);
		style.d.dockFrame.DrawH(f.x, f.y, f.w, f.h);
	}
	else
	{
		style.d.dockBar.DrawH(r.x, r.y, r.w);
	}

	// Draw dock title
	GxRecti labelRect(r.x + 4, r.y, r.w - myButtons.Size()*btnW - 4, barH);
	style.Label(labelRect, GX_TA_LEFT, GX_TA_MIDDLE, myTitle, false);

	// Draw dock buttons
	GxColor fade = style.c.textColor.Alpha(64);
	int bx = r.x + r.w - btnW/2 - 4;
	int by = r.y + barH/2;
	for(int i=0; i<myButtons.Size(); ++i)
	{
		int type = myButtons[i];
		bool hl = (mouseItem == type);

		GxSprite* sprite = &style.d.close;
		if(type == I_COLLAPSE)
		{
			sprite = &style.d.arrow;
			if(!myState[DS_COLLAPSED])
				sprite->SetMirrorV(true);
		}

		GxColor col(255, 255, 255, hl ? 255 : 128);
		sprite->SetColor(col);
		sprite->Draw(bx, by);
		sprite->SetMirrorV(false);
		sprite->SetColor(255);

		bx -= 16;
	}

	// Draw layout
	if(!myState[DS_COLLAPSED])
		myLayout->Draw();
}

void GxDock::SetFlags(GxFlags flags)
{
	myFlags.Reset(~GxWidget::F_WMASK);
	myFlags.Set(flags.bits);

	myButtons.Clear();

	if(myFlags[F_CLOSE])
		myButtons.Append(I_CLOSE);

	if(myFlags[F_COLLAPSE])
		myButtons.Append(I_COLLAPSE);
}

void GxDock::SetTitle(GxString title)
{
	myTitle = title;
}

void GxDock::Collapse(bool collapsed)
{
	myState.Set(DS_COLLAPSED, collapsed);
}

void GxDock::MoveToTop()
{
	if(myOwner) myOwner->myMoveToTop(this);
}

void GxDock::Undock()
{
	if(myOwner) myOwner->myUndock(this);
}

void GxDock::Dock(Bin b)
{
	if(myOwner) myOwner->myDock(this, b);
}

GxString GxDock::GetTitle() const
{
	return myTitle;
}

bool GxDock::IsFloating() const
{
	return myState[DS_FLOATING];
}

void GxDock::myClampFloatSize(GxVec2i dir)
{
	GxVec2i sizeMin = GetMinimumSize();

	if(myFloatRect.w < sizeMin.x)
	{
		if(dir.x < 0)
			myFloatRect.x -= (sizeMin.x - myFloatRect.w);
		myFloatRect.w = sizeMin.x;
	}

	if(myFloatRect.h < sizeMin.y)
	{
		if(dir.y < 0)
			myFloatRect.y -= (sizeMin.y - myFloatRect.h);
		myFloatRect.h = sizeMin.y;
	}	
}

void GxDock::myClampFloatPos(GxVec2i view)
{
	int w = myFloatRect.w;
	int h = myFloatRect.h;

	myFloatRect.x = GxClamp(myFloatRect.x, 0, GxMax(0, view.x-24));
	myFloatRect.y = GxClamp(myFloatRect.y, 0, GxMax(0, view.y-24));
}

GxVec2i GxDock::myGetResizeDir(int x, int y) const
{
	GxVec2i dir(0, 0);
	GxRecti r = myRect;
	r.Shrink(6);

	if(x <     r.x) dir.x = -1;
	if(x > r.x+r.w) dir.x = +1;
	if(y <     r.y) dir.y = -1;
	if(y > r.y+r.h) dir.y = +1;

	return dir;
}

GxDock::Item GxDock::myGetItemAt(int x, int y) const
{
	if(!myRect.Contains(x, y))
		return I_NONE;

	if(y < myRect.y + barH || myState[DS_COLLAPSED])
	{
		int btnX = myRect.x + myRect.w - btnW;
		for(int i=0; i<myButtons.Size(); ++i)
		{
			if(x > btnX)
				return myButtons[i];
			btnX -= btnW;
		}
		return I_BAR;
	}

	if(IsFloating())
	{
		GxVec2i dir = myGetResizeDir(x, y);
		if(dir.x != 0 || dir.y != 0)
			return I_FRAME;
	}

	return I_CONTENTS;
}

// ===================================================================================
// GxDockBin
// ===================================================================================

class GxDockBin
{
public:
	~GxDockBin();

	GxDockBin(GxDockArea* owner);

	void Adjust();
	void SetRect(const GxRecti& rect);
	GxWidget* FindHoverWidget(int x, int y);

	void Tick(float dt);
	void Draw();

	void Add(GxDock* dock);
	void Remove(GxDock* dock);
	bool Contains(GxDock* dock);
	bool Empty();

	const GxRecti& GetRect() {return myRect;}

private:
	friend class GxDockArea;

	GxListLayout myLayout;
	GxScrollbarV* myScrollbar;
	GxRecti myRect;
	int myWidth;
};

GxDockBin::~GxDockBin()
{
	myLayout.RemoveAll();
	delete myScrollbar;
}

GxDockBin::GxDockBin(GxDockArea* owner)
{
	myLayout.SetMargin(0);
	myLayout.SetOwner(owner);

	myScrollbar = new GxScrollbarV;
	myScrollbar->SetParent(owner);
}

void GxDockBin::Adjust()
{
	myLayout.Adjust();

	GxVec2i size = myLayout.GetPreferredSize();
	myWidth = GxMax(32, size.x);

	if(size.y > myRect.h)
	{
		myScrollbar->SetDisabled(false);
		myWidth += scrollbarW;
	}
	else
	{
		myScrollbar->SetDisabled(true);
		myScrollbar->SetValue(0);
	}
}

void GxDockBin::SetRect(const GxRecti& rect)
{
	GxRecti r = myRect = rect;

	if(!myScrollbar->IsHidden())
	{
		GxVec2i size = myLayout.GetPreferredSize();
		int end = GxMax(0, size.y - myRect.h);
		myScrollbar->SetRange(0, end, myRect.h, 32);

		GxRecti bar(r);
		bar.x += bar.w - scrollbarW;
		bar.w = scrollbarW;
		myScrollbar->SetRect(bar);

		r.y -= GxInt(myScrollbar->GetValue());
		r.w -= scrollbarW;
	}

	myLayout.Arrange(r);	
}

GxWidget* GxDockBin::FindHoverWidget(int x, int y)
{
	if(!myScrollbar->IsHidden())
	{
		GxWidget* w = myScrollbar->FindHoverWidget(x, y);
		if(w) return w;
	}
	return myLayout.FindHoverWidget(x, y);
}

void GxDockBin::Tick(float dt)
{
	myLayout.Tick(dt);

	if(!myScrollbar->IsHidden())
		myScrollbar->Tick(dt);
}

void GxDockBin::Draw()
{
	GxDraw* draw = GxDraw::Get();
	GxStyle& style = *GxStyle::Get();

	bool empty = true;
	const GxWidget* const* docks = myLayout.GetWidgets().Data();
	for(int i=0; empty && i<myLayout.GetWidgets().Size(); ++i)
		if(!docks[i]->IsHidden()) empty = false;

	if(!empty)
	{
		GxRecti r(myRect);

		draw->Rect(r.x, r.y, r.w, r.h, style.c.frameOutline);
		r.Shrink(1);
		draw->Rect(r.x, r.y, r.w, r.h, style.c.bgPanel);

		if(!myScrollbar->IsHidden())
			myScrollbar->Draw();

		draw->PushScissorRect(r.x, r.y, r.w, r.h);
		myLayout.Draw();
		draw->PopScissorRect();
	}	
}

void GxDockBin::Add(GxDock* dock)
{
	dock->myState.Reset(DS_FLOATING);
	myLayout.Add(dock);
}

void GxDockBin::Remove(GxDock* dock)
{
	if(myLayout.GetWidgets().Contains(dock))
	{
		dock->myState.Set(DS_FLOATING);
		myLayout.Remove(dock);
	}
}

bool GxDockBin::Contains(GxDock* dock)
{
	return myLayout.GetWidgets().Contains(dock);
}

bool GxDockBin::Empty()
{
	return myLayout.GetWidgets().Empty();
}

// ===================================================================================
// GxDockArea
// ===================================================================================

GxDockArea::~GxDockArea()
{
	for(int i=0; i<binCount; ++i)
		delete myBins[i];

	for(int i=0; i<myDocks.Size(); ++i)
		delete myDocks[i];
}

GxDockArea::GxDockArea()
	:myHoverDock(NULL)
	,myFocusDock(NULL)
	,myDragHl(0.f)
{
	myPolicy->min.Set(64, 64);
	myPolicy->hint.Set(64, 64);

	myPolicy->flagsH = GX_SP_EXPAND;
	myPolicy->flagsV = GX_SP_EXPAND;

	myBins[0] = myBins[1] = NULL;

	SetDockBins(DOCK_LR);
}

void GxDockArea::OnMousePress(GxMouseEvent& evt)
{
	GxDock* hover = myHoverDock;
	GxDock* focus = myFocusDock;

	if(hover)
	{
		if(evt.button == GX_MC_LEFT && !evt.handled)
		{
			GxDock::Item item = hover->myGetItemAt(evt.x, evt.y);
			switch(item)
			{
			case GxDock::I_CLOSE:
				myStartPress(hover);
				hover->EmitEvent(GxDock::eClose());
				break;

			case GxDock::I_COLLAPSE:
				myStartPress(hover);
				hover->myState.Flip(DS_COLLAPSED);
				break;

			case GxDock::I_BAR:
				if(hover->myFlags[GxDock::F_DRAG])
					myStartDrag(hover, evt.x, evt.y);
				break;

			case GxDock::I_FRAME:
				if(hover->myFlags[GxDock::F_RESIZE])
					myStartResize(hover, evt.x, evt.y);
				break;
			};
		}
		evt.handled = true;
	}
}

void GxDockArea::OnMouseRelease(GxMouseEvent& evt)
{
	if(myFocusDock && myActionType == FA_DRAG)
	{
		for(int i=0; i<binCount; ++i)
		{
			if(myBins[i] && myBins[i]->myRect.Contains(evt.x, evt.y))
				myBins[i]->Add(myFocusDock);
		}
	}

	myActionType = FA_NONE;
	myFocusDock = NULL;
}

void GxDockArea::Adjust()
{
	// Adjust the bins.
	for(int i=0; i<binCount; ++i)
		if(myBins[i])
			myBins[i]->Adjust();

	// Adjust the floating docks
	for(int i=0; i<myDocks.Size(); ++i)
		myDocks[i]->Adjust();
}

void GxDockArea::SetRect(const GxRecti& rect)
{
	myRect = rect;

	// Position dock bins.
	for(int i=0; i<binCount; ++i)
	{
		if(myBins[i])
		{
			int w = myBins[i]->myWidth;
			GxRecti r = rect;
			switch(i)
			{
			case BIN_L:	r.w = w;
				break;
			case BIN_R:	r.x += r.w - w, r.w = w;
				break;
			};
			myBins[i]->SetRect(r);
		}
	}

	// Position floating docks.
	for(int i=0; i<myDocks.Size(); ++i)
	{
		GxDock* dock = myDocks[i];
		if(dock->IsFloating())
		{
			GxRecti r = dock->myFloatRect;
			r.x += myRect.x;
			r.y += myRect.y;
			if(dock->myState[DS_COLLAPSED]) r.h = barH;
			dock->SetRect(r);
		}
	}
}

GxWidget* GxDockArea::FindHoverWidget(int x, int y)
{
	GxWidget* hover = NULL;
	GxDockBin* bin = NULL;
	myHoverDock = NULL;

	if(!myRect.Contains(x, y))
		return NULL;

	if(!myFocusDock)
	{
		// Try all floating docks first.
		for(int i=myDocks.Size()-1; i>=0 && !hover; --i)
		{
			if(myDocks[i]->IsFloating())
			{
				hover = myDocks[i]->FindHoverWidget(x, y);
				GxDock* dock = static_cast<GxDock*>(hover);
				if(dock && myDocks.Contains(dock))
					myHoverDock = dock;
			}
		}

		// Try all dock bins if no hover dock was found
		for(int i=0; i<binCount && !hover; ++i)
		{
			if(myBins[i])
			{
				hover = myBins[i]->FindHoverWidget(x, y);
				GxDock* dock = static_cast<GxDock*>(hover);
				if(dock && myDocks.Contains(dock))
					myHoverDock = dock;
				if(myBins[i]->GetRect().Contains(x, y))
					bin = myBins[i];
			}
		}
	}

	return hover ? hover : (bin ? this : NULL);
}

void GxDockArea::Tick(float dt)
{
	GxVec2i mpos = GxInput::Get()->GetMousePos();
	mpos.x = GxClamp(mpos.x, 0, myRect.w);
	mpos.y = GxClamp(mpos.y, 0, myRect.h);

	GxVec2i resizeDir = GxVec2i(0, 0);
	bool draggingAction = false;

	if(myHoverDock)
	{
		GxDock::Item item = myHoverDock->myGetItemAt(mpos.x, mpos.y);

		if(item == GxDock::I_BAR)
			draggingAction = true;

		if(item == GxDock::I_FRAME)
			resizeDir = myHoverDock->myGetResizeDir(mpos.x, mpos.y);
	}

	// Handle floating dock dragging action.
	if(myActionType == FA_DRAG)
	{
		draggingAction = true;

		GxRecti r = myActionDims;
		r.x += mpos.x - myActionPos.x;
		r.y += mpos.y - myActionPos.y;

		myFocusDock->myFloatRect = r;
		myFocusDock->myClampFloatSize(GxVec2i());
		myFocusDock->myClampFloatPos(GxVec2i(myRect.w, myRect.h));

		myDragHl = GxMin(1.f, myDragHl + dt * 2);
	}
	else 
	{
		myDragHl = GxMax(0.f, myDragHl - dt * 4);
	}

	// Handle floating dock resize action.
	if(myActionType == FA_RESIZE)
	{
		resizeDir = myResizeDir;
		GxRecti r = myActionDims;
		GxVec2i d = mpos - myActionPos;

		if(myResizeDir.x < 0) r.Expand(-d.x, 0, 0, 0);
		if(myResizeDir.y < 0) r.Expand(0, -d.y, 0, 0);
		if(myResizeDir.x > 0) r.Expand(0, 0, +d.x, 0);
		if(myResizeDir.y > 0) r.Expand(0, 0, 0, +d.y);

		myFocusDock->myFloatRect = r;
		myFocusDock->myClampFloatSize(myResizeDir);
		myFocusDock->myClampFloatPos(GxVec2i(myRect.w, myRect.h));
	}

	// Change the mouse cursor image when resizing.
	if(resizeDir.x != 0 || resizeDir.y != 0)
	{
		GxVec2i d = resizeDir;
		GxCursorImage c = GX_CI_DRAG;

		if(d.x != 0 && d.y == 0) c = GX_CI_SIZE_WE;
		if(d.x == 0 && d.y != 0) c = GX_CI_SIZE_NS;
		if(d.x <  0 && d.y <  0) c = GX_CI_SIZE_NWSE;
		if(d.x >  0 && d.y >  0) c = GX_CI_SIZE_NWSE;
		if(d.x <  0 && d.y >  0) c = GX_CI_SIZE_NESW;
		if(d.x >  0 && d.y <  0) c = GX_CI_SIZE_NESW;

		GetContext()->SetCursor(c);
	}

	// Change the mouse cursor image when dragging.
	if(draggingAction)
	{
		GetContext()->SetCursor(GX_CI_DRAG);
	}

	// Tick the dock bars.
	for(int i=0; i<binCount; ++i)
		if(myBins[i])
			myBins[i]->Tick(dt);

	// Tick the floating docks.
	for(int i=0; i<myDocks.Size(); ++i)
		if(myDocks[i]->IsFloating() && !myDocks[i]->IsHidden())
			myDocks[i]->Tick(dt);
}

void GxDockArea::Draw()
{
	GxDraw* draw = GxDraw::Get();

	// Draw the dock bars.
	for(int i=0; i<binCount; ++i)
	if(myBins[i])
		myBins[i]->Draw();

	// Draw the floating docks.
	for(int i=0; i<myDocks.Size(); ++i)
		if(myDocks[i]->IsFloating() && !myDocks[i]->IsHidden())
			myDocks[i]->Draw();

	// Draw docking area highlights.
	if(myDragHl > 0.01f)
	{
		GxVec2i mpos = GxInput::Get()->GetMousePos();
		int hl = myGetBinAt(mpos.x, mpos.y);
		if(myActionType != FA_DRAG) hl = -1;

		for(int i=0; i<binCount; ++i)
		if(myBins[i])
		{
			GxRecti r = myBins[i]->myRect;
			GxColor c(128, 128, (hl == i) ? 160 : 128, 0);
			draw->Rect(r.x, r.y, r.w, r.h, c.Alpha(myDragHl * 0.5f));
			r.Shrink(1);
			draw->Rect(r.x, r.y, r.w, r.h, c.Alpha(myDragHl * 0.2f));
		}
	}
}

GxDock* GxDockArea::AddDock()
{
	GxDock* dock = new GxDock;

	dock->myOwner = this;
	dock->SetParent(this);
	dock->SetTitle("Test Dock");

	int ofs = 32 + myDocks.Size() * 32;
	dock->SetFloatingPos(ofs, ofs);

	myDocks.Append(dock);

	return dock;
}

void GxDockArea::DeleteDock(GxDock* dock)
{
	myDocks.EraseValue(dock);

	for(int i=0; i<binCount; ++i)
		myBins[i]->Remove(dock);

	delete dock;
}

void GxDockArea::SetDockBins(DockBins bins)
{
	for(int i=0; i<binCount; ++i)
	{
		delete myBins[i];
		myBins[i] = NULL;
	}

	if(bins & DOCK_L)
		myBins[BIN_L] = new GxDockBin(this);

	if(bins & DOCK_R)
		myBins[BIN_R] = new GxDockBin(this);
}

void GxDockArea::myStartPress(GxDock* dock)
{
	myFocusDock = dock;
	myActionType = FA_PRESS;
}

void GxDockArea::myStartDrag(GxDock* dock, int x, int y)
{
	if(!dock->IsFloating())
	{
		GxRecti r = dock->myRect;
		r.x -= myRect.x;
		r.y -= myRect.y;

		GxRecti f = dock->myFloatRect;
		int dx = (x - r.x) - (x - r.x) * f.w / GxMax(r.w, 1);
		dock->SetFloatingPos(r.x + dx, r.y);
	}

	myUndock(dock);
	myMoveToTop(dock);

	myFocusDock = dock;
	myActionDims = dock->myFloatRect;
	myActionPos = GxVec2i(x, y);
	myActionType = FA_DRAG;
}

void GxDockArea::myStartResize(GxDock* dock, int x, int y)
{
	myUndock(dock);
	myMoveToTop(dock);

	myFocusDock = dock;
	myResizeDir = dock->myGetResizeDir(x, y);
	myActionDims = dock->myFloatRect;
	myActionPos = GxVec2i(x, y);
	myActionType = FA_RESIZE;
}

void GxDockArea::myMoveToTop(GxDock* dock)
{
	myDocks.EraseValue(dock);
	myDocks.Append(dock);
}

void GxDockArea::myUndock(GxDock* dock)
{
	for(int i=0; i<binCount; ++i)
		if(myBins[i] && myBins[i]->Contains(dock))
			myBins[i]->Remove(dock);
}

void GxDockArea::myDock(GxDock* d, int bin)
{
	myUndock(d);

	if(myBins[BIN_L] && bin == GxDock::BIN_LEFT)
		myBins[BIN_L]->Add(d);
	else if(myBins[BIN_R])
		myBins[BIN_R]->Add(d);
}

int GxDockArea::myGetBinAt(int x, int y) const
{
	for(int i=0; i<binCount; ++i)
		if(myBins[i] && myBins[i]->myRect.Contains(x,y))
			return i;

	return -1;
}

// ===================================================================================
// Widget events
// ===================================================================================

GxDefineWidgetEvent (GxDock, eClose)

}; // namespace widgets
}; // namespace guix