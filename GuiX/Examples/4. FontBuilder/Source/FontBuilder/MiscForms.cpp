#include <GuiX/Config.h>

#include <GuiX/Context.h>
#include <GuiX/GridLayout.h>

#include <FontBuilder/MiscForms.h>

namespace app {

using namespace guix;

// ===================================================================================
// Shared id's
// ===================================================================================

#define FB_FORM_ID(x) char* MiscForm::##x = #x;

FB_FORM_ID (idAccept)
FB_FORM_ID (idCancel)

// ===================================================================================
// CharsetForm
// ===================================================================================

void CharsetForm::Create(GxContext* context, GxCallback* callback)
{
	GxFrameV* root = new GxFrameV;
	root->SetCallback(callback);
	context->SetRoot(root);

	// Add instructions label.
	GxLabel* instructions = new GxLabel(
		"Select a set of characters from the list. "
		"The selected set will be added to the glyph page.");

	instructions->SetMultiLine(true);

	root->Add(instructions);

	// Create character set widgets.
	root->Add(charsetList = new GxSelectList);

	/// Creat dialog options widgets.
	GxFrameH* optionsFrame = new GxFrameH;
	root->Add(optionsFrame);

	optionsFrame->Add(new GxSpacerH);
	optionsFrame->Add(new GxButton(idAccept, "OK"));
	optionsFrame->Add(new GxButton(idCancel, "Cancel"));

	// Widget settings.
	charsetList->SetTextAlignH(GX_TA_LEFT);
	charsetList->SetToolTip("Select a set of characters to add.");
}

// ===================================================================================
// StringForm
// ===================================================================================

void StringForm::Create(GxContext* context, GxCallback* callback)
{
	GxFrameV* root = new GxFrameV;
	root->SetCallback(callback);
	context->SetRoot(root);

	// Add instructions label.
	GxLabel* instructions = new GxLabel(
		"Enter a string of characters. All characters in the string will be "
		"added to the glyph page. Duplicate characters are removed automatically.");

	instructions->SetMultiLine(true);

	root->Add(instructions);

	// Create character set widgets.
	root->Add(stringEdit = new GxTextEdit);

	/// Creat dialog options widgets.
	GxFrameH* optionsFrame = new GxFrameH;
	root->Add(optionsFrame);

	optionsFrame->Add(new GxSpacerH);
	optionsFrame->Add(new GxButton(idAccept, "OK"));
	optionsFrame->Add(new GxButton(idCancel, "Cancel"));

	// Widget settings.
	stringEdit->SetToolTip("Enter one or more characters to add.");
}

// ===================================================================================
// RangeForm
// ===================================================================================

void RangeForm::Create(GxContext* context, GxCallback* callback)
{
	GxFrameV* root = new GxFrameV;
	root->SetCallback(callback);
	context->SetRoot(root);

	// Add instructions label.
	GxLabel* instructions = new GxLabel(
		"Specify a range of unicode code point values. All characters inside the "
		"range will be added to the glyph page, including first and last.");

	instructions->SetMultiLine(true);

	root->Add(instructions);
	root->Add(new GxSpacerV);

	// Create character set widgets.
	GxFrame* rangeFrame = new GxFrame(new GxGridLayout(2));
	root->Add(rangeFrame);

	rangeFrame->Add(new GxLabel("First character:"));
	rangeFrame->Add(firstEdit = new GxSpinner);
	rangeFrame->Add(new GxLabel("Last character:"));
	rangeFrame->Add(lastEdit = new GxSpinner);

	/// Creat dialog options widgets.
	GxFrameH* optionsFrame = new GxFrameH;
	root->Add(new GxSpacerV);
	root->Add(optionsFrame);

	optionsFrame->Add(new GxSpacerH);
	optionsFrame->Add(new GxButton(idAccept, "OK"));
	optionsFrame->Add(new GxButton(idCancel, "Cancel"));

	// Widget settings.
	firstEdit->SetRange(0, 0x10FFFF, 1);
	firstEdit->SetToolTip("The code point of first character in the range.");

	lastEdit->SetRange(0, 0x10FFFF, 1);
	lastEdit->SetToolTip("The code point of last character in the range.");
}

// ===================================================================================
// PreviewForm
// ===================================================================================

void PreviewForm::Create(GxContext* context, GxCallback* callback, const GxTexture& tex)
{
	GxFrameV* root = new GxFrameV;
	root->SetCallback(callback);
	context->SetRoot(root);

	// Create preview widgets.
	GxScrollArea* scrollArea = new GxScrollArea;
	scrollArea->Add(sprite = new GxImageBox(tex));
	sprite->SetBackgroundColor(GxColor(0,64));
	root->Add(scrollArea);

	/// Creat dialog options widgets.
	GxFrameH* optionsFrame = new GxFrameH;
	root->Add(optionsFrame);

	optionsFrame->Add(new GxSpacerH);
	optionsFrame->Add(new GxButton(idAccept, "OK"));
}

}; // namespace app