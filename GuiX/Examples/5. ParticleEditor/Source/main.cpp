#include <GuiX/Config.h>

#include <math.h>
#include <deque>

#include <GuiX/Common.h>
#include <GuiX/Core.h>
#include <GuiX/List.h>
#include <GuiX/Draw.h>
#include <GuiX/Resources.h>

#include <GuiX/Context.h>
#include <GuiX/Style.h>
#include <GuiX/Widgets.h>
#include <GuiX/ListLayout.h>

#include <GuiX/WindowWin32.h>
#include <GuiX/ApplicationWin32.h>

// OpenGL
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Gl/gl.h>

// Demo
#include "Matrix.h"
#include "Particles.h"

using namespace guix;
using namespace guix::framework;
using namespace guix::particles;

class DemoWindow : public GxWindowWin32, public GxCallback, public GxInputListener
{
public:
	DemoWindow();
	~DemoWindow();

	void OnEvent(const GxWidgetEvent& evt);
	void Tick(float dt);

private:
	void CreateGui();

	void Setup3D();
	void DrawGrid();
	void Setup2D();

	float myRotation;
	float mySpawnTimer;
	GxContext* myContext;
	GxParticleEmitter* myEmitter;

	// Widget elements.
	GxSliderH* wSpawnRate;
	GxVectorEdit3D* wSpawnPos;
	GxSliderH* wSpawnSpread;

	GxColorEdit* wStartColorA;
	GxColorEdit* wStartColorB;
	GxColorEdit* wEndColorA;
	GxColorEdit* wEndColorB;
	GxDroplist* wBlendMode;
	GxDroplist* wParticleModel;

	GxSliderEdit* wStartSizeA;
	GxSliderEdit* wStartSizeB;
	GxSliderEdit* wEndSizeA;
	GxSliderEdit* wEndSizeB;

	GxSliderRange* wStartVel;
	GxSliderRange* wEndVel;
	GxVectorEdit3D* wExternalForce;

	GxVectorEdit3D* wStartRotA;
	GxVectorEdit3D* wStartRotB;
	GxVectorEdit3D* wRotDeltaA;
	GxVectorEdit3D* wRotDeltaB;

	GxDock* wShapeControls;
};

DemoWindow::DemoWindow()
{
	mySize.Set(1024, 768);
	myTitle = "GuiX Examples - Particle Editor";

	// Add the demo window as an input listener, for mouse input.
	GxInput::Get()->AddListener(this);

	// Load configuration files.
	GxResources::Get()->Load("Assets/Resources.xml");

	// Set the initial gui style to black, with slightly transparent docks.
	GxStyle::Colors colors(GxStyle::BLACK);
	colors.bgPanel.a = 192;
	GxStyle::Set(GxStyle(colors));

	// Create the gui
	myContext = NULL;
	CreateGui();

	// Create the particle emitter
	myEmitter = new GxParticleEmitter;
	mySpawnTimer = 0.f;
}

DemoWindow::~DemoWindow()
{
	delete myEmitter;
	GxContext::Delete(myContext);
}

void DemoWindow::CreateGui()
{
	// Context
	if(myContext) GxContext::Delete(myContext);
	myContext = GxContext::New();

	// Menu bar and dock area.
	GxFrameV* root = new GxFrameV;
	myContext->SetRoot(root);

	GxDockArea* docks = new GxDockArea;
	root->Add(docks);
	
	// Docks
	docks->SetCallback(this);

	GxGridLayout* grid = NULL;
	GxListLayout* list = NULL;

	// ===================================================================================
	// Spawn controls
	// ===================================================================================

	GxDock* spawnControls = docks->AddDock();
	spawnControls->SetTitle("Spawn controls");
	spawnControls->Dock();

	spawnControls->Add(new GxLabel("Spawn rate:"));
	spawnControls->Add(wSpawnRate = new GxSliderH);
	wSpawnRate->SetRange(0.0, 100.0, 10.0);
	wSpawnRate->SetValue(50.0);

	spawnControls->Add(new GxLabel("Spawn position:"));
	spawnControls->Add(wSpawnPos = new GxVectorEdit3D);
	wSpawnPos->SetRange(-5, 5);

	spawnControls->Add(new GxLabel("Spawn spread:"));
	spawnControls->Add(wSpawnSpread = new GxSliderH);
	wSpawnSpread->SetRange(0.0, 1.0, 0.1);
	wSpawnSpread->SetValue(0.25);

	// ===================================================================================
	// Appearance controls
	// ===================================================================================

	GxDock* colorControls = docks->AddDock();
	colorControls->SetTitle("Appearance controls");
	colorControls->Dock(GxDock::BIN_RIGHT);

	grid = new GxGridLayout(2);
	colorControls->SetLayout(grid);

	grid->Add(new GxLabel("Start color:"), 2);
	grid->Add(wStartColorA = new GxColorEdit);
	grid->Add(wStartColorB = new GxColorEdit);

	grid->Add(new GxLabel("End color:"), 2);
	grid->Add(wEndColorA = new GxColorEdit);
	grid->Add(wEndColorB = new GxColorEdit);

	grid->Add(new GxLabel("Blend mode:"), 2);
	grid->Add(wBlendMode = new GxDroplist, 2);
	wBlendMode->AddItem("None");
	wBlendMode->AddItem("Additive");
	wBlendMode->AddItem("Multiplicative");

	// ===================================================================================
	// Shape control
	// ===================================================================================

	wShapeControls = docks->AddDock();
	wShapeControls->SetTitle("Shape controls");
	wShapeControls->Dock(GxDock::BIN_RIGHT);

	list = new GxListLayout;
	wShapeControls->SetLayout(list);

	list->Add(new GxLabel("Start size:"));
	list->Add(wStartSizeA = new GxSliderEdit);
	wStartSizeA->SetRange(0, 5, 0.5, 2);
	wStartSizeA->SetValue(0.25);
	wStartSizeA->GetSlider()->SetRange(0, 5, 1);

	list->Add(wStartSizeB = new GxSliderEdit);
	wStartSizeB->SetRange(0, 5, 0.5, 2);
	wStartSizeB->SetValue(1);
	wStartSizeB->GetSlider()->SetRange(0, 5, 1);

	list->Add(new GxLabel("End size:"));
	list->Add(wEndSizeA = new GxSliderEdit);
	wEndSizeA->SetRange(0, 5, 0.5, 2);
	wEndSizeA->SetValue(0);
	wEndSizeA->GetSlider()->SetRange(0, 5, 1);

	list->Add(wEndSizeB = new GxSliderEdit);
	wEndSizeB->SetRange(0, 5, 0.5, 2);
	wEndSizeB->SetValue(0);
	wEndSizeB->GetSlider()->SetRange(0, 5, 1);

	list->Add(new GxLabel("Model type:"));
	list->Add(wParticleModel = new GxDroplist);
	wParticleModel->AddItem("Cube");
	wParticleModel->AddItem("Square");

	// ===================================================================================
	// Velocity control
	// ===================================================================================

	GxDock* velControls = docks->AddDock();
	velControls->SetTitle("Velocity controls");
	velControls->Dock();

	list = new GxListLayout;
	velControls->SetLayout(list);

	list->Add(new GxLabel("Start velocity:"));
	list->Add(wStartVel = new GxSliderRange);
	wStartVel->SetRange(0, 20);
	wStartVel->SetValue(5, 10);

	list->Add(new GxLabel("End velocity:"));
	list->Add(wEndVel = new GxSliderRange);
	wEndVel->SetRange(0, 4);
	wEndVel->SetValue(2, 2);

	list->Add(new GxLabel("External Force:"));
	list->Add(wExternalForce = new GxVectorEdit3D);
	wExternalForce->SetRange(-10, 10);
	wExternalForce->SetValue(GxVec3d(0, -2, 0));

	// ===================================================================================
	// Rotation control
	// ===================================================================================

	GxDock* rotControls = docks->AddDock();
	rotControls->SetTitle("Rotation controls");
	rotControls->Dock();

	rotControls->SetLayout(new GxGridLayout(2));

	rotControls->Add(new GxLabel("Start rotation:"));
	rotControls->Add(new GxSpacer);

	rotControls->Add(wStartRotA = new GxVectorEdit3D);
	wStartRotA->SetRange(0, 360);
	wStartRotA->SetValue(GxVec3d(0,0,0));

	rotControls->Add(wStartRotB = new GxVectorEdit3D);
	wStartRotB->SetRange(0, 360);
	wStartRotB->SetValue(GxVec3d(360,360,360));

	rotControls->Add(new GxLabel("Rotation speed:"));
	rotControls->Add(new GxSpacer);

	rotControls->Add(wRotDeltaA = new GxVectorEdit3D);
	wRotDeltaA->SetRange(-360, 360);
	wRotDeltaA->SetValue(GxVec3d(0,-180,0));

	rotControls->Add(wRotDeltaB = new GxVectorEdit3D);
	wRotDeltaB->SetRange(-360, 360);
	wRotDeltaB->SetValue(GxVec3d(0,180,0));
}

void DemoWindow::OnEvent(const GxWidgetEvent& evt)
{
}

void DemoWindow::Tick(float dt)
{
	GxDraw* draw = GxDraw::Get();
	GxInput* input = GxInput::Get();

	// Rotate the camera
	myRotation = (float)fmod(myRotation + dt*10, 360);

	// Tick the gui
	wShapeControls->SetDisabled(!input->IsKeyDown(GX_KC_SPACE));

	myContext->SetView(0, 0, mySize.x, mySize.y);
	myContext->Tick(dt);

	// Clear the screen
	GxColor panel = GxStyle::Get()->c.bgPanel;
	draw->Rect(0, 0, mySize.x, mySize.y, panel);
	draw->Flush();

	// Start 3D rendering, draw the grid.
	Setup3D();
	DrawGrid();

	// Update the particle properties
	GxParticleType& type = myEmitter->GetType();
	
	// Spawn controls
	{
		type.spawnSpread = (float)wSpawnSpread->GetValue();
	}
	// Color controls
	{
		GxVec4f a = wStartColorA->GetColorf();
		GxVec4f b = wStartColorB->GetColorf();
		GxVec4f c = wEndColorA->GetColorf();
		GxVec4f d = wEndColorB->GetColorf();

		type.r = GxParticleProperty(a.x, b.x, c.x, d.x);
		type.g = GxParticleProperty(a.y, b.y, c.y, d.y);
		type.b = GxParticleProperty(a.z, b.z, c.z, d.z);

		type.blendMode = wBlendMode->GetSelectedItem();
	}
	// Shape controls
	{
		type.size.a = (float)wStartSizeA->GetValue();
		type.size.b = (float)wStartSizeB->GetValue();
		type.size.c = (float)wEndSizeA->GetValue();
		type.size.d = (float)wEndSizeB->GetValue();

		type.model = wParticleModel->GetSelectedItem();
	}
	// Velocity controls
	{
		type.vel.a = (float)wStartVel->GetValue().x;
		type.vel.b = (float)wStartVel->GetValue().y;
		type.vel.c = (float)wEndVel->GetValue().x;
		type.vel.d = (float)wEndVel->GetValue().y;

		type.externalForce = wExternalForce->GetValue();		
	}
	// Rotation controls
	{
		GxVec3f a = wStartRotA->GetValue();
		GxVec3f b = wStartRotB->GetValue();
		GxVec3f c = wRotDeltaA->GetValue();
		GxVec3f d = wRotDeltaB->GetValue();

		type.rotX = GxParticleProperty(a.x, b.x, c.x, d.x);
		type.rotY = GxParticleProperty(a.y, b.y, c.y, d.y);
		type.rotZ = GxParticleProperty(a.z, b.z, c.z, d.z);

		type.rotX.delta = PD_RANDOM;
		type.rotY.delta = PD_RANDOM;
		type.rotZ.delta = PD_RANDOM;
	}

	// Spawn new particles
	mySpawnTimer = GxMin(10.f, mySpawnTimer + dt * (float)wSpawnRate->GetValue());
	int spawnCount = (int)mySpawnTimer;
	if(spawnCount > 0)
	{
		GxVec3f spawnPos = wSpawnPos->GetValue();
		GxVec3f spawnDir = GxVec3f(0, 1, 0);
		myEmitter->Spawn(spawnPos, spawnDir, spawnCount);
		mySpawnTimer -= (float)spawnCount;
	}

	// Tick and render particles
	myEmitter->Tick(dt);
	myEmitter->Render();

	// End 3D rendering.
	Setup2D();
	
	// Draw the gui
	myContext->Draw();

	// Finish drawing
	draw->Flush();
}

void DemoWindow::Setup3D()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Matrices
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	Matrix4f proj;
	proj.SetPerspective(45.0f, (float)mySize.x / (float)mySize.y, 0.1f, 400.0f);
	glMultMatrixf(proj.cell);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	Matrix4f view;
	view.SetTranslation(0, 10, -50);
	view.SetRotationY(myRotation);

	view.InvertOrtho();
	view.SetScale(1, 1, -1);
	glMultMatrixf(view.cell);
}

void DemoWindow::DrawGrid()
{
	glColor3f(0,0,0);
	glBegin(GL_LINES);
	for(int i=-5; i<=5; ++i)
	{
		glVertex3f(-5.f, 0.f, (float)i);
		glVertex3f(+5.f, 0.f, (float)i);
		glVertex3f((float)i, 0.f, -5.f);
		glVertex3f((float)i, 0.f, +5.f);
	}
	glEnd();
	glColor3f(0,1,0);
}

void DemoWindow::Setup2D()
{
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	// Matrices
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, mySize.x, mySize.y, 0, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

GxWindowWin32* guix::framework::GxCreateMainWindow()
{
	return new DemoWindow;
}