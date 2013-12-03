#pragma once

#include <GuiX/Widgets.h>

namespace app {

using namespace guix;

// ===================================================================================
// Main Window
// ===================================================================================

struct MainForm
{
	void Create(GxContext* context, GxCallback* callback);

	GxLineEdit* nameEdit;
	GxLineEdit* folderEdit;
	GxSpinner* sizeEdit;
	GxSelectList* pageList;

	static char* idFolder;
	static char* idClear;
	static char* idRemove;
	static char* idEdit;
	static char* idAdd;
	static char* idExport;
};

}; // namespace app