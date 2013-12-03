#pragma once

#include <GuiX/String.h>
#include <GuiX/Core.h>
#include <GuiX/WindowWin32.h>
#include <GuiX/ApplicationWin32.h>

#include <Shared/Rasterizer.h>

#include <FontBuilder/MainForm.h>
#include <FontBuilder/PageForm.h>
#include <FontBuilder/MiscForms.h>

namespace app {

using namespace guix;
using namespace framework;
using namespace builder;

// ===================================================================================
// GenericDialog
// ===================================================================================

class GenericDialog : public GxWindowWin32, public GxCallback
{
public:
	GenericDialog(int w, int h, GxString title);
	GenericDialog(int w, int h, int minW, int minH, GxString title);
	~GenericDialog();

	void Tick(float dt);

	void OnClose();
	void OnResize(int w, int h);
	void OnEvent(const GxWidgetEvent& e);

	GxContext* myContext;
};

// ===================================================================================
// PageEditor
// ===================================================================================

class PageEditor : public GxWindowWin32, public GxCallback
{
public:
	PageEditor(const RasterizerPage* init);
	~PageEditor();

	void Tick(float dt);

	void OnClose();
	void OnResize(int w, int h);
	void OnEvent(const GxWidgetEvent& e);
	void WriteResult(RasterizerPage& out);

private:
	void myEntryAdd();
	void myEntryDup();
	void myEntryRemove();
	void myEntryClear();
	void mySelectFile();
	void myClear();
	void myRemove();
	void myAddPreset();
	void myAddString();
	void myAddRange();
	void myAddBlock();
	void myPreview();
	void myChangeEntry();

	void myAddRange(int type);
	void myAddPreset(int type);
	void myUpdateWidgetValues();
	void myUpdateEntryValues();

	void myAddToList(GxSelectList* list, const CodepointPreset& preset);
	void myAddToList(GxSelectList* list, const CodepointRange& range);
	void myAddToList(GxSelectList* list, int first, int last, int size, const char* text);

	PageForm myForm;
	GxContext* myContext;
	RasterizerPage myPage;
	int myEntryIndex;
};

// ===================================================================================
// MainWindow
// ===================================================================================

typedef std::vector<RasterizerPage> PageVector;

class MainWindow : public GxWindowWin32, public GxCallback
{
public:
	MainWindow();
	~MainWindow();

	void Tick(float dt);

	void OnClose();
	void OnResize(int w, int h);
	void OnEvent(const GxWidgetEvent& e);

private:
	void mySelectFolder();
	void myClearPages();
	void myAddPage();
	void myEditPage();
	void myRemovePage();
	GxString myExportFile();
	void myUpdateTable();

	MainForm myForm;
	GxContext* myContext;
	PageVector myPages;
};

}; // namespace app