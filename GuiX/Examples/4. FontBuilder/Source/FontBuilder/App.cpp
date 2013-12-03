#include <GuiX/Config.h>

#include <GuiX/Core.h>
#include <GuiX/Draw.h>
#include <GuiX/Localize.h>
#include <GuiX/Context.h>
#include <GuiX/Style.h>

#include <Shared/CodepointRange.h>
#include <Shared/CodepointPreset.h>
#include <Shared/Rasterizer.h>

#include <FontBuilder/App.h>

#include <stdio.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <Shared/stb_image_write.h>

namespace app {

// This function clears the background of the windows with the default color.
inline void ClearWindow(int w, int h)
{
	GxDraw::Get()->Rect(0, 0, w, h, GxStyle::Get()->c.bgPanel);
}

// ===================================================================================
// GenericDialog
// ===================================================================================

GenericDialog::GenericDialog(int w, int h, GxString title)
{
	myTitle   = title;
	mySize    = GxVec2i(w, h);
	myMinSize = GxVec2i(w, h);
	myContext = GxContext::New();
}

GenericDialog::GenericDialog(int w, int h, int minW, int minH, GxString title)
{
	myTitle   = title;
	mySize    = GxVec2i(w, h);
	myMinSize = GxVec2i(minW, minH);
	myContext = GxContext::New();
}

GenericDialog::~GenericDialog()
{
	delete myContext;
}

void GenericDialog::OnClose()
{
	Terminate(0);
}

void GenericDialog::OnResize(int w, int h)
{
	myContext->SetView(0, 0, w, h);
	GxWindowWin32::OnResize(w, h);
}

void GenericDialog::OnEvent(const GxWidgetEvent& e)
{
	if(e.type == GxButton::ePressed())
	{
		if(e.id == MiscForm::idAccept) Terminate(1);
		if(e.id == MiscForm::idCancel) Terminate(0);
	}
}

void GenericDialog::Tick(float dt)
{
	ClearWindow(mySize.x, mySize.y);
	myContext->Tick(dt);
	myContext->Draw();
}

// ===================================================================================
// PageEditor
// ===================================================================================

PageEditor::PageEditor(const RasterizerPage* init)
{
	myTitle   = "Page Editor";
	mySize    = GxVec2i(800, 500);
	myMinSize = GxVec2i(500, 300);
	myContext = GxContext::New();	

	myForm.Create(myContext, this);
	
	myEntryIndex = 0;

	// Copy the page entries from the existing settings.
	if(init)
	{
		myPage = *init;
	}
	else
	// Initialize the default page settings.
	{
		myPage.entries.push_back(RasterizerEntry());

		myAddRange(CR_Basic_Latin);
		myAddRange(CR_Latin_1_Supplement);

		myPage.entries[0].fontPath = "./Fonts/OpenSans-Regular.ttf";
	}

	// Add hinting options to the hinting droplist.
	myForm.hintingList->AddItem("Normal Hinting", RH_NORMAL);
	myForm.hintingList->AddItem("Light Hinting", RH_LIGHT);
	myForm.hintingList->AddItem("Mono Hinting", RH_MONO);

	// Set initial values for the widgets.
	myUpdateWidgetValues();
}

PageEditor::~PageEditor()
{
	delete myContext;
}

void PageEditor::OnClose()
{
	Terminate(0);
}

void PageEditor::OnResize(int w, int h)
{
	myContext->SetView(0, 0, w, h);
	GxWindowWin32::OnResize(w, h);
}

void PageEditor::OnEvent(const GxWidgetEvent& e)
{
	if(e.type == GxButton::ePressed())
	{
		     if(e.id == PageForm::idEntryAdd)       myEntryAdd();
		else if(e.id == PageForm::idEntryDuplicate) myEntryDup();
		else if(e.id == PageForm::idEntryRemove)    myEntryRemove();
		else if(e.id == PageForm::idEntryClear)     myEntryClear();

		else if(e.id == PageForm::idBrowse)    mySelectFile();
		else if(e.id == PageForm::idClear)     myClear();
		else if(e.id == PageForm::idRemove)    myRemove();
		else if(e.id == PageForm::idAddPreset) myAddPreset();
		else if(e.id == PageForm::idAddString) myAddString();
		else if(e.id == PageForm::idAddRange)  myAddRange();
		else if(e.id == PageForm::idAddBlock)  myAddBlock();
		else if(e.id == PageForm::idPreview)   myPreview();
		else if(e.id == PageForm::idAccept)
		{
			myUpdateEntryValues();
			Terminate(1);
		}
		else if(e.id == PageForm::idCancel)
		{
			myUpdateEntryValues();
			Terminate(0);
		}
	}

	if(e.id == PageForm::idEntries && e.type == GxSelectList::eChanged())
		myChangeEntry();
}

void PageEditor::WriteResult(RasterizerPage& out)
{
	out = myPage;
}

void PageEditor::Tick(float dt)
{
	ClearWindow(mySize.x, mySize.y);
	myContext->Tick(dt);
	myContext->Draw();
}

void PageEditor::mySelectFile()
{
	GxApplicationWin32* app = GxApplicationWin32::Get();

	GxString defaultFile = myForm.pathEdit->GetText();
	GxString result = app->FileDialog(
		"Open font file",
		defaultFile,
		"Fonts (*.ttf *.ttc *.otf *.fnt *.fon *.bdf)\n"
		"*.ttf;*.ttc;*.otf;*.fnt;*.fon;*.bdf\n"
		"All (*.*)\n"
		"*.*");

	if(!result.Empty())
		myForm.pathEdit->SetText(result);
}

void PageEditor::myEntryAdd()
{
	myUpdateEntryValues();
	myPage.entries.push_back(RasterizerEntry());
	myEntryIndex = myPage.entries.size() - 1;
	myUpdateWidgetValues();
}

void PageEditor::myEntryDup()
{
	if(myEntryIndex < 0) return;

	myUpdateEntryValues();
	myPage.entries.push_back(myPage.entries[myEntryIndex]);
	myEntryIndex = myPage.entries.size() - 1;
	myUpdateWidgetValues();
}

void PageEditor::myEntryRemove()
{
	if(myEntryIndex < 0) return;

	myPage.entries.erase(myPage.entries.begin() + myEntryIndex);
	myUpdateWidgetValues();
}

void PageEditor::myEntryClear()
{
	myPage.entries.clear();
	myUpdateWidgetValues();
}

void PageEditor::myClear()
{
	if(myEntryIndex < 0) return;
	RasterizerEntry& e = myPage.entries[myEntryIndex];

	myUpdateEntryValues();
	e.presets.clear();
	e.ranges.clear();
	myUpdateWidgetValues();
}

void PageEditor::myRemove()
{
	if(myEntryIndex < 0) return;
	RasterizerEntry& e = myPage.entries[myEntryIndex];

	int pos = myForm.charsetList->GetSelectedItem();
	if(pos >= 0)
	{
		myUpdateEntryValues();
		if(pos >= (int)e.presets.size())
		{
			pos -= (int)e.presets.size();
			e.ranges.erase(e.ranges.begin() + pos);
		}
		else
		{
			e.presets.erase(e.presets.begin() + pos);			
		}
		myUpdateWidgetValues();
	}
}

void PageEditor::myAddPreset()
{
	if(myEntryIndex < 0) return;

	GenericDialog dialog(480, 480, "Select character set");
	CharsetForm form;
	form.Create(dialog.myContext, &dialog);

	for(int i=0; i<CP_PRESET_COUNT; ++i)
		myAddToList(form.charsetList, CodepointPreset::Get((CodepointPresetId)i));

	if(dialog.Execute() && form.charsetList->GetSelectedItem() >= 0)
	{
		int i = form.charsetList->GetSelectedItem();
		CodepointPreset preset = CodepointPreset::Get((CodepointPresetId)i);
		myPage.entries[myEntryIndex].presets.push_back(preset);
		myUpdateWidgetValues();
	}
}

void PageEditor::myAddString()
{
	if(myEntryIndex < 0) return;

	GenericDialog dialog(480, 160, "Select character set");
	StringForm form;
	form.Create(dialog.myContext, &dialog);

	if(dialog.Execute())
	{
		GxString text = form.stringEdit->GetText();
		if(!text.Empty())
		{
			CodepointPreset preset = {0xFFFFFFFF, 0};
			const GxList<GxUTF32> str = GxUnicode::ToUTF32(text.Raw());
			for(int i=0; i<str.Size() - 1; ++i)
			{
				preset.chars.push_back(str[i]);
				preset.first = GxMin(preset.first, str[i]);
				preset.last  = GxMax(preset.last,  str[i]);
				if(i < 100) preset.name.push_back(str[i]);
			}

			myUpdateEntryValues();
			myPage.entries[myEntryIndex].presets.push_back(preset);
			myUpdateWidgetValues();
		}
	}
}

void PageEditor::myAddRange()
{
	if(myEntryIndex < 0) return;

	GenericDialog dialog(320, 160, "Select character range");
	RangeForm form;
	form.Create(dialog.myContext, &dialog);

	if(dialog.Execute())
	{
		int first = (int)form.firstEdit->GetValue();
		int last = (int)form.lastEdit->GetValue();
		if(last >= first)
		{
			CodepointRange range = {first, last, "Custom range"};

			myUpdateEntryValues();
			myPage.entries[myEntryIndex].ranges.push_back(range);
			myUpdateWidgetValues();
		}
	}
}

void PageEditor::myAddBlock()
{
	if(myEntryIndex < 0) return;

	GenericDialog dialog(480, 480, "Select character set");
	CharsetForm form;
	form.Create(dialog.myContext, &dialog);

	for(int i=0; i<CR_RANGE_COUNT; ++i)
		myAddToList(form.charsetList, CodepointRange::Get((CodepointRangeId)i));

	if(dialog.Execute() && form.charsetList->GetSelectedItem() >= 0)
	{
		int i = form.charsetList->GetSelectedItem();
		CodepointRange range = CodepointRange::Get((CodepointRangeId)i);

		myUpdateEntryValues();
		myPage.entries[myEntryIndex].ranges.push_back(range);
		myUpdateWidgetValues();
	}
}

void PageEditor::myPreview()
{
	myUpdateEntryValues();

	Rasterizer rasterizer;
	RasterizerOutput out;
	if(rasterizer.Render(myPage, out) == RR_SUCCESS)
	{
		GxTexture tex(out.page.width, out.page.height, out.page.bitmap);
		int dialogW = GxClamp(out.page.width + 32, 128, 1024);
		int dialogH = GxClamp(out.page.height + 64, 192, 768);

		GenericDialog dialog(dialogW, dialogH, 128, 192, "Glyph page preview");
		PreviewForm form;
		form.Create(dialog.myContext, &dialog, tex);

		dialog.Execute();
	}
	else
	{
		GxApplicationWin32* app = GxApplicationWin32::Get();
		app->MessageDialog("Failed to render glyph page", "Something went wrong...");
	}
}

void PageEditor::myChangeEntry()
{
	myUpdateEntryValues();
	myEntryIndex = myForm.entriesList->GetSelectedItem();
	myUpdateWidgetValues();
}

void PageEditor::myAddRange(int type)
{
	if(type >= 0 && type < CR_RANGE_COUNT)
	{
		CodepointRange range = CodepointRange::Get((CodepointRangeId)type);
		myPage.entries[myEntryIndex].ranges.push_back(range);
	}
}

void PageEditor::myUpdateWidgetValues()
{
	// Update entries table.
	myForm.entriesList->Clear();
	for(int i=0; i<(int)myPage.entries.size(); ++i)
	{
		const RasterizerEntry& e = myPage.entries[i];

		GxString name = e.fontPath.size() ? GetFilePart(e.fontPath).c_str() : "- no font -";

		int numChars = 0;
		for(size_t k=0; k<e.presets.size(); ++k)
			numChars += e.presets[k].chars.length();
		for(size_t k=0; k<e.ranges.size(); ++k)
			numChars += e.ranges[k].last - e.ranges[k].first + 1;
		if(numChars > 0) name.Arg(" (").Arg(numChars).Arg(')');

		myForm.entriesList->AddItem(name);
	}

	// Update entry index.
	myEntryIndex = GxClamp(myEntryIndex, -1, (int)myPage.entries.size() - 1);
	myForm.entriesList->SetSelectedItem(myEntryIndex);

	// Update page settings.
	myForm.paddingEdit->SetValue(myPage.glyphPadding);
	myForm.advanceEdit->SetValue(myPage.glyphAdvance);
	myForm.texWidthEdit->SetValue(myPage.textureWidth);
	myForm.fallbackBox->SetChecked(myPage.renderFallback);
	myForm.kerningPairs->SetChecked(myPage.kerningPairs);

	// Update selected entry.
	if(myEntryIndex >= 0)
	{
		const RasterizerEntry& e = myPage.entries[myEntryIndex];

		myForm.entryBox->SetLocked(false);
		myForm.pathEdit->SetText(e.fontPath.c_str());
		myForm.sizeEdit->SetValue(e.fontSize);
		myForm.hintingList->SetSelectedItem(e.hintMode);
		myForm.smallCapsBox->SetChecked(e.smallCaps);
		myForm.markAsItalic->SetChecked(e.italic);
		myForm.markAsBold->SetChecked(e.bold);
		myForm.charsetList->Clear();

		for(size_t i=0; i<e.presets.size(); ++i)
			myAddToList(myForm.charsetList, e.presets[i]);

		for(size_t i=0; i<e.ranges.size(); ++i)
			myAddToList(myForm.charsetList, e.ranges[i]);
	}
	else
	// No entry selected, lock everything.
	{
		myForm.entryBox->SetLocked(true);
		myForm.pathEdit->SetText(GxString());
		myForm.sizeEdit->SetValue(0);
		myForm.hintingList->SetSelectedItem(0);
		myForm.smallCapsBox->SetChecked(false);
		myForm.markAsItalic->SetChecked(false);
		myForm.markAsBold->SetChecked(false);
		myForm.charsetList->Clear();
	}
}

void PageEditor::myUpdateEntryValues()
{
	if(myEntryIndex < 0) return;

	myPage.glyphPadding   = myForm.paddingEdit->GetIntValue();
	myPage.glyphAdvance   = myForm.advanceEdit->GetIntValue();
	myPage.textureWidth   = myForm.texWidthEdit->GetIntValue();
	myPage.renderFallback = myForm.fallbackBox->IsChecked();
	myPage.kerningPairs   = myForm.kerningPairs->IsChecked();

	RasterizerEntry& e = myPage.entries[myEntryIndex];

	e.fontPath = myForm.pathEdit->GetText().Raw();
	e.fontSize = myForm.sizeEdit->GetIntValue();
	e.hintMode = (RasterizerHintmode)myForm.hintingList->GetValue().ToInt();
	e.smallCaps = myForm.smallCapsBox->IsChecked();
	e.italic = myForm.markAsItalic->IsChecked();
	e.bold = myForm.markAsBold->IsChecked();
}

static void AddToList(GxSelectList* list, codepoint32 first, codepoint32 last, int size, const char* name)
{
	GxString s("U+");
	s.Arg(first, true);
	s.Append("\tU+");
	s.Arg(last, true);
	s.MakeUpper();
	s.Append("\t");
	s.Arg(size);
	s.Append("\t");
	s.Append(name);
	list->AddItem(s);
}

void PageEditor::myAddToList(GxSelectList* list, const CodepointPreset& preset)
{
	AddToList(list, preset.first, preset.last, preset.chars.length(), preset.name.c_str());
}

void PageEditor::myAddToList(GxSelectList* list, const CodepointRange& range)
{
	AddToList(list, range.first, range.last, range.last-range.first+1, range.name);
}

// ===================================================================================
// MainWindow
// ===================================================================================

MainWindow::MainWindow()
{
	myTitle   = "FontBuilder";
	mySize    = GxVec2i(640, 480);
	myMinSize = GxVec2i(384, 384);
	myContext = GxContext::New();

	GxStyle::Get()->SetFont(GxFont("Assets/Fonts/SegoeUI.txt"));
	GxStyle::Get()->d.text[0].tabWidth = 72.f;
	GxStyle::Get()->d.text[1].tabWidth = 72.f;

	myForm.Create(myContext, this);
	myForm.folderEdit->SetText("./out");	
}

MainWindow::~MainWindow()
{
	delete myContext;
}

void MainWindow::OnClose()
{
	Terminate(0);
}

void MainWindow::OnResize(int w, int h)
{
	myContext->SetView(0, 0, w, h);
	GxWindowWin32::OnResize(w, h);
}

void MainWindow::OnEvent(const GxWidgetEvent& e)
{
	GxApplicationWin32* app = GxApplicationWin32::Get();

	if(!(e.type == GxButton::ePressed())) return;

	     if(e.id == MainForm::idFolder) mySelectFolder();
	else if(e.id == MainForm::idAdd)    myAddPage();
	else if(e.id == MainForm::idEdit)   myEditPage();
	else if(e.id == MainForm::idClear)  myClearPages();
	else if(e.id == MainForm::idRemove) myRemovePage();
	else if(e.id == MainForm::idExport)
	{
		GxString err = myExportFile();
		if(!err.Empty())
			app->MessageDialog("Failed to export the font", err);
	}
}

void MainWindow::Tick(float dt)
{
	ClearWindow(mySize.x, mySize.y);
	myContext->Tick(dt);
	myContext->Draw();
}

void MainWindow::mySelectFolder()
{
	GxApplicationWin32* app = GxApplicationWin32::Get();

	GxString defaultFolder = myForm.folderEdit->GetText();
	GxString result = app->FolderDialog("Select export folder", defaultFolder);
	if(!result.Empty())
		myForm.folderEdit->SetText(result);
}

void MainWindow::myClearPages()
{
	myPages.clear();
	myUpdateTable();
}

void MainWindow::myAddPage()
{
	PageEditor pageEditor(NULL);
	if(pageEditor.Execute())
	{
		RasterizerPage page;
		pageEditor.WriteResult(page);
		myPages.push_back(page);
		myUpdateTable();
	}

	if(myPages.size() == 1 && myPages[0].entries.size() >= 1)
		myForm.sizeEdit->SetValue(myPages[0].entries[0].fontSize);
}

void MainWindow::myEditPage()
{
	int index = myForm.pageList->GetSelectedItem();
	if(index < 0) return;

	RasterizerPage& page = myPages[index];
	PageEditor pageEditor(&page);
	if(pageEditor.Execute())
	{
		pageEditor.WriteResult(page);
		myUpdateTable();
	}

	if(myPages.size() == 1 && myPages[0].entries.size() >= 1)
		myForm.sizeEdit->SetValue(myPages[0].entries[0].fontSize);
}

void MainWindow::myRemovePage()
{
	int index = myForm.pageList->GetSelectedItem();
	if(index < 0) return;

	myPages.erase(myPages.begin() + index);
	myUpdateTable();
}

GxString MainWindow::myExportFile()
{
	GxApplicationWin32* app = GxApplicationWin32::Get();

	int fontSize = GxInt(myForm.sizeEdit->GetValue());
	GxString fontPath = myForm.folderEdit->GetText();
	GxString fontName = myForm.nameEdit->GetText();

	// Make sure the output name is non-empty.
	if(fontName.Empty())
		fontName = "nameless";

	// Make sure the output directory is non-empty.
	if(fontPath.Empty())
		return "Please specify an output directory for the font files.";

	// Make sure the output directory ends with a slash.
	if(!fontPath.EndsWith("\\") && !fontPath.EndsWith("/"))
		fontPath.Append("/");

	// Check if there is anything to export.
	if(myPages.empty())
		return "There is nothing to export, please add one or more glyph pages.";

	// Output font size.
	GxString output;
	output.Arg("size ").Arg(fontSize).Arg("\n\n");

	// Output page elements and images for every glyph page.
	int glyphIndex = 0;
	KerningList kerning;
	for(size_t i=0; i<myPages.size(); ++i)
	{
		Rasterizer rasterizer;
		RasterizerOutput out;
		if(rasterizer.Render(myPages[i], out) != RR_SUCCESS)
		{
			app->MessageDialog("Failed to render a glyph page", "Something went wrong...");
			continue;
		}

		// Save the glyph page image.
		GxString pageName = fontName + ((myPages.size() > 1) ? GxString("_").Arg(i) : "") + ".png";
		GxString imagePath = fontPath + pageName;

		int pageW = out.page.width;
		int pageH = out.page.height;
		if(!stbi_write_png(imagePath.Raw(), pageW, pageH, 4, out.page.bitmap, pageW*4))
			app->MessageDialog("Failed to save glyph image", "Failed to save the glyph image: " + imagePath);

		// Output a new glyph page.
		output.Append("page \"" + pageName + "\"\n");

		// Output a comment to clarify the glyph properties.
		output.Append("# fields: unicode codepoint, x-advance, bitmap region (x,y,w,h), draw coordinate (x,y)\n\n");

		// For every style in this entry...
		for(size_t st=0; st<GS_STYLE_COUNT; ++st)
		{
			// Translate kerning indices to global indices.
			for(size_t j=0; j<out.kerning[st].size(); ++j)
			{
				KerningPair kp = out.kerning[st][j];
				kp.left  += glyphIndex;
				kp.right += glyphIndex;
				kerning.push_back(kp);
			}

			// Output glyph properties.
			const size_t n = out.glyphs[st].size();
			if(n > 0)
			{
				output.Arg("style ").Arg(GetStyleString(st)).Append("\n");
				for(size_t j=0; j<n; ++j)
				{
					const Glyph& glyph = out.glyphs[st][j];

					output.Append("g ");
					output.Arg( glyph.codepoint      ).Append(' ');
					output.Arg( glyph.advance        ).Append(' ');
					output.Arg( glyph.bitmapRegion.x ).Append(' ');
					output.Arg( glyph.bitmapRegion.y ).Append(' ');
					output.Arg( glyph.bitmapRegion.w ).Append(' ');
					output.Arg( glyph.bitmapRegion.h ).Append(' ');
					output.Arg( glyph.drawCoords.x   ).Append(' ');
					output.Arg( glyph.drawCoords.y   );

					++glyphIndex;

					if(j+1 < out.glyphs[st].size()) output.Append('\n');
				}
				if(st+1 < GS_STYLE_COUNT && out.glyphs[st+1].size()) output.Append("\n\n");
			}
		}
		if(i+1 < myPages.size()) output.Append("\n\n");
	}

	// Append kerning pairs at the end.
	if(!kerning.empty())
	{
		output.Append("\n\n# kerning pairs: left glyph index, right glyph index, x-delta\n");
		output.Append("# indices are zero-based and refer to glyphs in the order they appear in this file.\n\n");
		for(size_t i=0; i<kerning.size(); ++i)
		{
			const KerningPair& kp = kerning[i];

			output.Append("k ");
			output.Arg( kp.left  ).Append(' ');
			output.Arg( kp.right ).Append(' ');
			output.Arg( kp.delta );

			if(i+1 < kerning.size()) output.Append('\n');
		}
	}

	// Try to open the text file for writing.
	GxString txtPath = fontPath + fontName + ".txt";
	FILE* file = fopen(txtPath.Raw(), "wb");
	if(!file) return "Failed to open the font file for writing: " + txtPath;

	// Write the output string to the text file.
	fwrite(output.Raw(), 1, output.Length(), file);
	fclose(file);

	// Report that the exporting was succesful.
	app->MessageDialog("Done exporting", "Finished exporting: " + fontName);

	return GxString();
}

void MainWindow::myUpdateTable()
{
	myForm.pageList->Clear();
	for(size_t i=0; i<myPages.size(); ++i)
	{
		const RasterizerPage& page = myPages[i];

		// Add a descriptive text to the item.
		GxString text;
		if(page.entries.size())
		{
			const RasterizerEntry& e = page.entries[0];

			int numChars = 0;
			for(size_t k=0; k<e.presets.size(); ++k)
				numChars += e.presets[k].chars.length();
			for(size_t k=0; k<e.ranges.size(); ++k)
				numChars += e.ranges[k].last - e.ranges[k].first + 1;

			text.Arg(numChars);
			text.Append("\t");
			text.Arg(e.fontSize);
			text.Append("\t: ");
			text.Append(GetFilePart(e.fontPath).c_str());
		}
		else text = "- no entries -";

		// Add the list item.
		myForm.pageList->AddItem(text);
	}
}

}; // namespace app

namespace guix {
namespace framework {

// ===================================================================================
// This function is used by the Win32 Framework to create the main application window.

GxWindowWin32* GxCreateMainWindow()
{
	return new app::MainWindow;
}

}; // namespace framework
}; // namespace guix