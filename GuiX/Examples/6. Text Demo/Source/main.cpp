#include <GuiX/Common.h>
#include <GuiX/Core.h>
#include <GuiX/List.h>
#include <GuiX/Sprites.h>
#include <GuiX/Localize.h>
#include <GuiX/Resources.h>

#include <GuiX/Context.h>
#include <GuiX/Style.h>
#include <GuiX/Widgets.h>
#include <GuiX/ListLayout.h>

#include <GuiX/WindowWin32.h>

using namespace guix;
using namespace guix::framework;

// ===================================================================================

class DragCorner
{
public:
	DragCorner(int _x, int _y)
		:x(_x), y(_y), dragging(false) {}

	void OnMousePress(GxMouseEvent& evt)
	{
		GxRecti r(x-5, y-5, 10, 10);
		if(r.Contains(evt.x, evt.y) && !evt.handled)
			x = evt.x, y = evt.y, dragging = evt.handled = true;
	}
	void OnMouseRelease(GxMouseEvent& evt)
	{
		dragging = false;
	}
	void Tick(float dt)
	{
		GxVec2i m = GxInput::Get()->GetMousePos();
		if(dragging) x = m.x, y = m.y;
	}
	void Draw()
	{
		GxVec2i m = GxInput::Get()->GetMousePos();
		bool hl = dragging || GxRecti(x-5,y-5,10, 10).Contains(m.x, m.y);
		GxDraw::Get()->Rect(x-5, y-5, 10, 10, hl ? GxColor(192) : GxColor(128));
		GxDraw::Get()->Rect(x-4, y-4, 8, 8, hl ? GxColor(128) : GxColor(64));
	}
	void Clamp(int l, int t, int r, int b)
	{
		x = GxClamp(x, l, r);
		y = GxClamp(y, t, b);
	}

	int x, y;
	bool dragging;
};

class DragRect
{
public:
	DragRect(int x, int y, int w, int h)
		:tl(x, y), br(x+w, y+h) {}

	void OnMousePress(GxMouseEvent& evt)
	{
		tl.OnMousePress(evt);
		br.OnMousePress(evt);
	}
	void OnMouseRelease(GxMouseEvent& evt)
	{
		tl.OnMouseRelease(evt);
		br.OnMouseRelease(evt);
	}
	void Tick(float dt)
	{
		tl.Tick(dt);
		br.Tick(dt);
		if(tl.dragging) tl.Clamp(tl.x, tl.y, br.x-4, br.y-4);
		if(br.dragging) br.Clamp(tl.x+4, tl.y+4, br.x, br.y);
		GxVec2i view = GxRenderInterface::Get()->GetViewSize();
		tl.Clamp(8, 8, view.x-216, view.y-16);
		br.Clamp(8, 8, view.x-216, view.y-16);
	}
	void Draw()
	{
		tl.Draw();
		br.Draw();
		GxRecti r = GxAreai(tl.x, tl.y, br.x, br.y);
		GxDraw::Get()->Rect(r.x-1, r.y-1, r.w+2, r.h+2, GxColor(192, 128));
		GxDraw::Get()->Rect(r.x, r.y, r.w, r.h, GxColor(0, 192));
	}
	GxRecti GetRect() const
	{
		return GxRecti(tl.x, tl.y, br.x-tl.x, br.y-tl.y);
	}

	DragCorner tl, br;
};

// ===================================================================================
// The DemoWindow is derived from the window class in the Win32 Framework, and is
// going to be the main window of the application.

class DemoWindow : public GxWindowWin32, public GxCallback, public GxInputListener
{
public:
	DemoWindow();
	~DemoWindow();

	void OnEvent(const GxWidgetEvent& evt);
	void Tick(float dt);

	void OnMousePress(GxMouseEvent& evt);
	void OnMouseRelease(GxMouseEvent& evt);

private:
	void CreateGui();
	void DoTextRect(float dt, DragRect& dragRect, GxString text);

	DragRect myRectA, myRectB;
	GxFont myFontA, myFontB;
	GxContext* myContext;
	GxText myText;

	GxDroplist* myAlignH, *myAlignV;

	GxCheckbox* myWordWrap, *myJustify, *myEllipsis;
	GxCheckbox* myFormat, *mySingleLine, *myKerning;
	GxColorEdit* myTopC, *myBottomC, *myShadowC;
	GxSpinner* myTabWidth;
};

// ===================================================================================
// In this example, we will display a rectangle of text.

DemoWindow::DemoWindow()
	:myRectA(48, 48, 704, 64)
	,myRectB(92, 192, 704-88, 512)
{
	// Set the initial window size and window title.
	myTitle.Set("GuiX Examples - Text Demo");
	mySize.Set(1024, 768);

	// Add the window as an input listener, so it can receive mouse input.
	GxInput::Get()->AddListener(this);

	// Load the resources configuration file and translations.
	GxResources::Get()->Load("Assets/Resources.xml");
	GxLocalize::Get()->Load("EN");

	// Text settings.
	myFontA = GxFont("Assets/Fonts/Defused.txt");
	myFontB = GxFont("Assets/Fonts/SegoeJP.txt");

	GxStyle::Get()->SetFont(myFontB);

	// Create the gui context and forms.
	CreateGui();

	// Test glyph.
	GxFontDatabase::Get()->AddGlyph("GuiX", "Assets/GlyphGx.png", 5);
}

DemoWindow::~DemoWindow()
{
	GxContext::Delete(myContext);
}

void DemoWindow::CreateGui()
{
	myContext = GxContext::New();

	// Docks.
	GxDockArea* docks = new GxDockArea;
	
	// Root.
	docks->SetCallback(this);
	myContext->SetRoot(docks);

	// Text flags.
	GxDock* flagsDock = docks->AddDock();
	flagsDock->SetTitle("Flags");
	flagsDock->Dock(GxDock::BIN_RIGHT);

	flagsDock->Add(myWordWrap = new GxCheckbox(NULL, "Word wrap"));
	flagsDock->Add(myJustify = new GxCheckbox(NULL, "Justification"));
	flagsDock->Add(myEllipsis = new GxCheckbox(NULL, "Ellipsis"));
	flagsDock->Add(myFormat = new GxCheckbox(NULL, "Formatting"));
	flagsDock->Add(mySingleLine = new GxCheckbox(NULL, "Single line"));
	flagsDock->Add(myKerning = new GxCheckbox(NULL, "Kerning"));
	myWordWrap->SetChecked(true);
	myFormat->SetChecked(true);

	// Text alignment.
	GxGridLayout* alignGrid = new GxGridLayout(2);
	GxDock* alignDock = docks->AddDock();
	alignDock->SetTitle("Alignment");
	alignDock->SetLayout(alignGrid);
	alignDock->Dock(GxDock::BIN_RIGHT);

	alignGrid->Add("Horizontal:", myAlignH = new GxDroplist);
	alignGrid->Add("Vertical:", myAlignV = new GxDroplist);

	myAlignH->AddItem("Left", GX_TA_LEFT); 
	myAlignH->AddItem("Center", GX_TA_CENTER); 
	myAlignH->AddItem("Right", GX_TA_RIGHT);
	myAlignH->SetSelectedItem(1);

	myAlignV->AddItem("Top", GX_TA_TOP); 
	myAlignV->AddItem("Middle", GX_TA_MIDDLE); 
	myAlignV->AddItem("Bottom", GX_TA_BOTTOM);

	// Text color.
	GxGridLayout* colorGrid = new GxGridLayout(2);
	GxDock* colorDock = docks->AddDock();
	colorDock->SetTitle("Color");
	colorDock->SetLayout(colorGrid);
	colorDock->Dock(GxDock::BIN_RIGHT);

	colorGrid->Add("Top:", myTopC = new GxColorEdit(NULL, GxVec4f(1,1,1,1), true));
	colorGrid->Add("Bottom:", myBottomC = new GxColorEdit(NULL, GxVec4f(1,1,1,1), true));
	colorGrid->Add("Shadow:", myShadowC = new GxColorEdit(NULL, GxVec4f(0,0,0,1), true));

	// Misc properties
	GxGridLayout* miscGrid = new GxGridLayout(2);
	GxDock* miscDock = docks->AddDock();
	miscDock->SetTitle("Misc.");
	miscDock->SetLayout(miscGrid);
	miscDock->Dock(GxDock::BIN_RIGHT);

	miscGrid->Add("Tab width:", myTabWidth = new GxSpinner);
	myTabWidth->SetRange(1, 0xFFFF, 4);
	myTabWidth->SetValue(96);
}

void DemoWindow::OnEvent(const GxWidgetEvent& evt)
{
}

void DemoWindow::Tick(float dt)
{
	GxDraw* draw = GxDraw::Get();
	GxInput* input = GxInput::Get();

	// Tick the gui.
	myContext->SetView(0, 0, mySize.x, mySize.y);
	myContext->Tick(dt);

	// Set text properties according to the widgets.
	myText.alignH = (GxTextAlignH)myAlignH->GetValue().ToInt();
	myText.alignV = (GxTextAlignV)myAlignV->GetValue().ToInt();

	myText.SetFlag(GX_TF_WORD_WRAP, myWordWrap->IsChecked());
	myText.SetFlag(GX_TF_JUSTIFIED, myJustify->IsChecked());
	myText.SetFlag(GX_TF_ELLIPSIS, myEllipsis->IsChecked());
	myText.SetFlag(GX_TF_FORMATTING, myFormat->IsChecked());
	myText.SetFlag(GX_TF_SINGLELINE, mySingleLine->IsChecked());
	myText.SetFlag(GX_TF_KERNING, myKerning->IsChecked());

	myText.top = myTopC->GetColor();
	myText.bottom = myBottomC->GetColor();
	myText.shadow = myShadowC->GetColor();

	myText.tabWidth = (float)myTabWidth->GetValue();

	// Tick and draw the title rectangle.
	myText.font = myFontB;
	DoTextRect(dt, myRectB, GxTr("text demo text"));

	// Tick and draw text rectangle.
	myText.font = myFontA;
	myText.SetFlag(GX_TF_FORMATTING, false);
	DoTextRect(dt, myRectA, GxTr("text demo title"));

	// Draw the gui.
	myContext->Draw();

	// Finish drawing.
	draw->Flush();
}

void DemoWindow::OnMousePress(GxMouseEvent& evt)
{
	myRectA.OnMousePress(evt);
	myRectB.OnMousePress(evt);
}

void DemoWindow::OnMouseRelease(GxMouseEvent& evt)
{
	myRectA.OnMouseRelease(evt);
	myRectB.OnMouseRelease(evt);
}

void DemoWindow::DoTextRect(float dt, DragRect& dragRect, GxString text)
{
	dragRect.Tick(dt);
	dragRect.Draw();

	GxRecti r = dragRect.GetRect();
	GxDraw::Get()->PushScissorRect(r.x, r.y, r.w, r.h);
	r.Shrink(2);

	myText.maxWidth = GxMax(0, r.w);

	if(myText.alignH == GX_TA_CENTER) r.x += r.w / 2;
	if(myText.alignV == GX_TA_MIDDLE) r.y += r.h / 2;
	if(myText.alignH == GX_TA_RIGHT ) r.x += r.w;
	if(myText.alignV == GX_TA_BOTTOM) r.y += r.h;
	myText.Draw(r.x, r.y, text);

	GxDraw::Get()->PopScissorRect();
}

// ===================================================================================
// This function is used by the Win32 Framework to create the main application window.

GxWindowWin32* guix::framework::GxCreateMainWindow()
{
	return new DemoWindow;
}