#include <GuiX/Config.h>

#include <GuiX/Context.h>
#include <GuiX/GridLayout.h>

#include <FontBuilder/MainForm.h>

namespace app {

using namespace guix;

// ===================================================================================
// MainForm
// ===================================================================================

#define FB_FORM_ID(x) char* MainForm::##x = #x;

FB_FORM_ID (idFolder)
FB_FORM_ID (idClear)
FB_FORM_ID (idRemove)
FB_FORM_ID (idEdit)
FB_FORM_ID (idAdd)
FB_FORM_ID (idExport)

void MainForm::Create(GxContext* context, GxCallback* callback)
{
	GxFrameV* root = new GxFrameV;
	context->SetRoot(root);

	root->GetLayout().SetMargin(2);
	root->SetCallback(callback);

	// Create export settings widgets.
	GxGroupbox* settingsBox = new GxGroupbox;
	root->Add(settingsBox);

	settingsBox->SetText("Export settings");
	GxGridLayout* settings = new GxGridLayout(3);
	settingsBox->SetLayout(settings);

	settings->Add(new GxLabel("Output name"));
	settings->Add(nameEdit = new GxLineEdit, 2);
	nameEdit->SetToolTip("The file name that is used to create the "
		"font XML file and glyph page images.");

	settings->Add(new GxLabel("Output folder"));
	settings->Add(folderEdit = new GxLineEdit);
	settings->Add(new GxButton(idFolder, "Select folder..."));
	folderEdit->SetToolTip("The directory to which the "
		"font XML file and glyph page images are saved.");

	settings->Add(new GxLabel("Output font size"));
	settings->Add(sizeEdit = new GxSpinner, 2);
	sizeEdit->SetToolTip("The vertical font size, in pixels, that is "
		"written to the font XML file.");

	// Create pages table widgets.
	GxGroupbox* pageBox = new GxGroupbox;
	root->Add(pageBox);

	pageBox->SetText("Glyph pages");
	pageBox->Add(pageList = new GxSelectList);
	pageList->SetToolTip("List of glyph pages that are part of the "
		"current font.");

	GxFrameH* buttons = new GxFrameH;
	pageBox->Add(buttons);

	buttons->Add(new GxButton(idClear, "Remove all"));
	buttons->Add(new GxButton(idRemove, "Remove"));
	buttons->Add(new GxSpacerH);
	buttons->Add(new GxButton(idEdit, "Edit..."));
	buttons->Add(new GxButton(idAdd, "Add..."));
	buttons->GetLayout().SetMargin(0);

	/// Creat export widgets.
	GxFrameH* exportFrame = new GxFrameH;
	root->Add(exportFrame);

	exportFrame->Add(new GxSpacerH);
	exportFrame->Add(new GxButton(idExport, "Export..."));

	// Default settings
	nameEdit->SetText("myFont");

	sizeEdit->SetRange(1, 256, 1);
	sizeEdit->SetValue(12);

	pageList->SetTextAlignH(GX_TA_LEFT);
}

}; // namespace app