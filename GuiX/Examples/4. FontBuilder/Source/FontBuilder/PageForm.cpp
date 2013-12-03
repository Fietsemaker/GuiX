#include <GuiX/Config.h>

#include <GuiX/Context.h>
#include <GuiX/ListLayout.h>
#include <GuiX/GridLayout.h>

#include <FontBuilder/PageForm.h>

namespace app {

using namespace guix;

// ===================================================================================
// PageForm
// ===================================================================================

#define FB_FORM_ID(x) char* PageForm::##x = #x;

FB_FORM_ID (idEntryAdd)
FB_FORM_ID (idEntryRemove)
FB_FORM_ID (idEntryClear)
FB_FORM_ID (idEntryDuplicate)

FB_FORM_ID (idBrowse)
FB_FORM_ID (idClear)
FB_FORM_ID (idRemove)
FB_FORM_ID (idAddPreset)
FB_FORM_ID (idAddString)
FB_FORM_ID (idAddRange)
FB_FORM_ID (idAddBlock)
FB_FORM_ID (idPreview)
FB_FORM_ID (idAccept)
FB_FORM_ID (idCancel)
FB_FORM_ID (idEntries)

void PageForm::Create(GxContext* context, GxCallback* callback)
{
	GxFrameV* root = new GxFrameV;
	root->GetLayout().SetMargin(2);
	root->SetCallback(callback);
	context->SetRoot(root);

	GxFrameH* frameTop = new GxFrameH;
	GxFrameH* frameBtm = new GxFrameH;
	root->Add(frameTop);
	root->Add(frameBtm);

	// Create page frame.
	GxFrameV* pageFrame = new GxFrameV;
	pageFrame->GetPolicy().adjustFlags = false;
	pageFrame->GetPolicy().flagsH = GX_SP_RESIZE;
	frameTop->Add(pageFrame);

	// Create font entries widgets.
	GxGroupbox* entriesBox = new GxGroupbox;
	entriesBox->SetText("Font entries");
	pageFrame->Add(entriesBox);

	entriesBox->Add(entriesList = new GxSelectList(idEntries));
	entriesList->SetToolTip("List of entries which are going to be rendered on the glyph page image.");

	GxFrame* ebFrame = new GxFrame(new GxGridLayout(2));
	entriesBox->Add(ebFrame);
	ebFrame->Add(new GxButton(idEntryClear, "Clear"));
	ebFrame->Add(new GxButton(idEntryRemove, "Remove"));
	ebFrame->Add(new GxButton(idEntryAdd, "Add"));
	ebFrame->Add(new GxButton(idEntryDuplicate, "Duplicate"));

	// Create page settings widgets.
	GxGridLayout* pageGrid = new GxGridLayout(2);
	GxFrame* settingsFrame = new GxFrame(pageGrid);
	pageFrame->Add(settingsFrame);

	pageGrid->Add("Image width", texWidthEdit = new GxSpinner);
	texWidthEdit->SetToolTip("The width, in pixels, of the glyph page to render. If set to 0, "
		"the width is estimated to create an image that is roughly square.");

	pageGrid->Add("Glyph advance", advanceEdit = new GxSpinner);
	advanceEdit->SetToolTip("Additional spacing, in pixels, that is added to the glyph advance values. "
		"This does not affect the glyph page image, but affects the glyph values in the output file.");

	pageGrid->Add("Glyph padding", paddingEdit = new GxSpinner);
	paddingEdit->SetToolTip("The padding, in pixels, that is added on each side of the glyph images on the page image.");
	
	pageGrid->Add(fallbackBox = new GxCheckbox(NULL, "Add fallback character"), 2);
	fallbackBox->SetToolTip("If checked, a special question mark glyph is included on the glyph page that can be "
		"used as a fallback character.");

	pageGrid->Add(kerningPairs = new GxCheckbox(NULL, "Export kerning pairs"), 2);
	kerningPairs->SetToolTip("If checked, kerning pairs will be exported when available.");

	// Create entry settings widgets.
	entryBox = new GxGroupbox;
	entryBox->SetText("Entry settings");
	frameTop->Add(entryBox);

	// Font and character set widgets.
	GxGridLayout* fontGrid = new GxGridLayout(2);
	GxFrame* fontFrame = new GxFrame(fontGrid);
	entryBox->Add(fontFrame);
	
	fontGrid->Add(pathEdit = new GxLineEdit);
	pathEdit->SetToolTip("The path of a font file that is used to render the glyph images.");
	fontGrid->Add(new GxButton(idBrowse, "Browse..."));

	fontGrid->Add(charsetList = new GxSelectList);
	charsetList->SetToolTip("List of characters for which glyph images are rendered.");
	charsetList->GetPolicy().flagsH = GX_SP_EXPAND;

	GxFrameV* csbFrame = new GxFrameV;
	fontGrid->Add(csbFrame);

	csbFrame->Add(new GxButton(idAddPreset, "Add preset..."));
	csbFrame->Add(new GxButton(idAddString, "Add string..."));
	csbFrame->Add(new GxButton(idAddRange, "Add range..."));
	csbFrame->Add(new GxButton(idAddBlock, "Add block..."));
	csbFrame->Add(new GxSpacerV);
	csbFrame->Add(new GxButton(idRemove, "Remove"));
	csbFrame->Add(new GxButton(idClear, "Clear"));

	// Misc settings widgets.
	GxFrame* entryGrid = new GxFrame(new GxGridLayout(3));
	entryBox->Add(entryGrid);

	entryGrid->Add(new GxLabel("FreeType hinting mode"));
	entryGrid->Add(new GxSpacerH);
	entryGrid->Add(hintingList = new GxDroplist);
	hintingList->SetToolTip("The FreeType hinting mode that is used to render the glyph images.");

	entryGrid->Add(new GxLabel("FreeType font size"));
	entryGrid->Add(new GxSpacerH);
	entryGrid->Add(sizeEdit = new GxSpinner);
	sizeEdit->SetToolTip("The font size that is used to render the glyph images.");

	entryBox->Add(markAsBold = new GxCheckbox(NULL, "Mark glyphs as bold"));
	markAsBold->SetToolTip("If checked, the glyphs in this entry will be marked as 'bold' when the "
		"font file is exported.");

	entryBox->Add(markAsItalic = new GxCheckbox(NULL, "Mark glyphs as italic"));
	markAsItalic->SetToolTip("If checked, the glyphs in this entry will be marked as 'italic' when the "
		"font file is exported.");

	entryBox->Add(smallCapsBox = new GxCheckbox(NULL, "Render with small capitals"));
	smallCapsBox->SetToolTip("If checked, lower case characters will be rendered as scaled down upper case characters.");

	// Create form buttons.
	frameBtm->Add(new GxSpacerH);
	frameBtm->Add(new GxButton(idPreview, "Preview..."));
	frameBtm->Add(new GxButton(idAccept, "OK"));
	frameBtm->Add(new GxButton(idCancel, "Cancel"));

	// Default settings
	sizeEdit->SetRange(1, 256, 1);
	sizeEdit->SetValue(12);

	paddingEdit->SetRange(0, 64, 1);
	advanceEdit->SetRange(0, 2048, 1);
	texWidthEdit->SetRange(0, 2048, 1);

	fallbackBox->SetChecked(true);

	charsetList->SetTextAlignH(GX_TA_LEFT);
}

}; // namespace app