#include <GuiX/Config.h>

#include <math.h>

// GuiX core

#include <GuiX/Common.h>
#include <GuiX/Core.h>
#include <GuiX/Draw.h>
#include <GuiX/Canvas.h>
#include <GuiX/Sprites.h>
#include <GuiX/Localize.h>
#include <GuiX/Resources.h>
#include <GuiX/Text.h>

// Win32 Framework

#include <GuiX/WindowWin32.h>

using namespace guix;
using namespace guix::framework;

// ===================================================================================
// The DemoWindow is derived from the window class in the Win32 Framework, and is
// going to be the main window of the application.

class DemoWindow : public GxWindowWin32, public GxInputListener
{
public:
	DemoWindow();
	~DemoWindow();

	void SwitchLanguage();
	void Tick(float dt);
	void OnKeyPress(GxKeyEvent& evt);

private:
	bool myLanguageIsEN;
	GxTileRect myFrame;
	GxFont myFontA, myFontB;
	GxSprite myBackground, mySprite, myArrow;
	float myTime, myFrameW;
};

// ===================================================================================
// In this example, we will load and create several resources such as images, fonts
// and translation strings. We will also create a texture from scratch by using the
// canvas class.

DemoWindow::DemoWindow()
{
	// Set the initial window size and window title.
	myTitle.Set("GuiX Examples - Core Demo");
	mySize.Set(1024, 768);
	
	// Add the window as an input listener, so it can receive keyboard input.
	GxInput::Get()->AddListener(this);

	// Load the resources configuration file.
	GxResources::Get()->Load("Assets/Resources.xml");

	// Load the default language.
	GxLocalize::Get()->Load("Assets/LangEN.xml");
	myLanguageIsEN = true;

	// Textures can be loaded directly from an image path...
	myBackground.SetTexture(GxTexture("Assets/Background.jpg"));

	// ...or from a texture resource...
	mySprite.SetTexture(GxTexture("Danbo"));
	mySprite.SetOrigin(0.5f, 0.5f);

	// ...or can be created by drawing on a canvas.
	GxCanvas frame(32, 32);

	frame.SetColor(GxColor(96,96,96,128), GxColor(0,0,0,128));
	frame.Rect(0, 0, 32, 32, 6);
	frame.SetColor(GxColor(0,0,0,192), GxColor(96,96,96,192));
	frame.Rect(4, 4, 28, 28, 6);

	frame.SetFill(false);
	frame.SetColor(0);
	frame.Rect(0, 0, 32, 32, 6);

	myFrame.SetTexture(frame.CreateTexture());
	myFrame.SetBorderSize(8, 8, 8, 8);

	GxCanvas arrow(24, 24);
	arrow.SetColor(GxColor(0, 128));
	arrow.Circle(12, 12, 12);
	GxVec2f arrowVerts[] = 
	{
		GxVec2f(8, 4),
		GxVec2f(8, 20),
		GxVec2f(20, 12)
	};
	arrow.SetColor(GxColor(255));
	arrow.Polygon(arrowVerts, 3);

	myArrow.SetTexture(arrow.CreateTexture());
	myArrow.SetOrigin(0.5f, 0.5f);

	// Fonts can be loaded from a GuiX font path...
	myFontA.Create("Assets/Fonts/Defused.txt");

	// ...or from a font resource.
	myFontB.Create("SegoeJP");

	// Set up some variables for the animations in the demo application.
	myTime = 0.f;
	myFrameW = 512.f;
}

DemoWindow::~DemoWindow()
{
}

void DemoWindow::SwitchLanguage()
{
	myLanguageIsEN = !myLanguageIsEN;

	// Translations can also be loaded from a resource.
	GxLocalize::Get()->Load(myLanguageIsEN ? "EN" : "NL");
}

void DemoWindow::Tick(float dt)
{
	GxInput* input = GxInput::Get();
	GxDraw* draw = GxDraw::Get();

	myTime += dt;

	int w = mySize.x;
	int h = mySize.y;
	
	// Shrink or widen the text frame based on keyboard input.
	if(input->IsKeyDown(GX_KC_LEFT))
		myFrameW = GxMax(16.f, myFrameW - dt * 256);

	if(input->IsKeyDown(GX_KC_RIGHT))
		myFrameW = GxMin(1024.f, myFrameW + dt * 256);

	// Draw the background image.
	myBackground.Draw(0, 0);

	// Create some text settings.
	GxText text;

	text.maxWidth = 512;
	text.alignH = GX_TA_CENTER;
	text.alignV = GX_TA_MIDDLE;
	text.SetFlag(GX_TF_JUSTIFIED, false);

	// Draw the top frame.
	myFrame.SetColor(GxColor(128, 128, 128));
	myFrame.DrawH(w/2 - 280, 16, 560, 144);

	// Draw the introduction text. The text is based on translation strings in the
	// translation XML file we loaded, and is retrieved by using the GxTr() functions.
	text.font = myFontA;
	text.Draw(w/2, 64, GxTr("greeting"));

	text.font = myFontB;
	text.Draw(w/2, 128, GxTr("info"));

	// We are going to the a frame around the following text. Because we don't know
	// how tall the section of text is going to be, we are going to use the
	// GetTextRect() function to compute the dimensions of the text rectangle.	
	int frameW = GxInt(myFrameW);

	text.font = myFontB;
	text.maxWidth = frameW;
	text.alignH = GX_TA_CENTER;
	text.SetFlag(GX_TF_JUSTIFIED);
	text.SetFlag(GX_TF_FORMATTING);

	GxString demoText = GxTr("text demo text");
	GxRecti textRect = text.GetTextRect(w/2, h/2, demoText);
	GxRecti frame(w/2 - frameW/2, textRect.y, frameW, textRect.h);

	frame.Expand(6, 6, 6, 6);
	myFrame.DrawH(frame.x, frame.y, frame.w, frame.h);
	//GxDraw::Get()->Rect(frame.x, frame.y, frame.w, frame.h, GxColor(0));

	// After the frame is drawn, the text is drawn on top of it.
	text.SetColor(200);
	text.shadow = GxColor(0,0,0,0);
	text.Draw(w/2, h/2, demoText);

	// Finally, we draw the resize arrows.
	myArrow.SetFlags(GxSprite::MIRROR_H);
	myArrow.Draw(frame.x - 16, h/2);
	myArrow.SetFlags();
	myArrow.Draw(frame.x + frame.w + 16, h/2);

	// Display instructions in the bar at the bottom.
	draw->Rect(0, h - 60, w, 60, GxColor(0, 0, 0, 192), GxColor(0));

	text.SetAlign();
	text.SetMaxWidth();
	text.SetColor(GxColor(160, 160, 160));

	text.Draw(4, h - 40, GxTr("instructions", "lang") + (myLanguageIsEN ? "EN" : "NL"));
	text.Draw(4, h - 20, GxTr("instructions", "area"));

	// Draw the Danbo sprite.
	mySprite.SetRotation(sin(myTime) * 10.f);
	mySprite.Draw(w - 128, h - 180);

	// Flush any left-over drawing operations.
	draw->Flush();
}

void DemoWindow::OnKeyPress(GxKeyEvent& evt) 
{
	switch(evt.key)
	{
		case GX_KC_UP:
		case GX_KC_DOWN:
			SwitchLanguage();
			break;
	};
}

// ===================================================================================
// This function is used by the Win32 Framework to create the main application window.

GxWindowWin32* guix::framework::GxCreateMainWindow()
{
	return new DemoWindow;
}