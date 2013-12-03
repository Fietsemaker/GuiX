#pragma once

#include <GuiX/Widgets.h>

namespace app {

using namespace guix;

// ===================================================================================
// Page Editor
// ===================================================================================

struct PageForm
{
	void Create(GxContext* context, GxCallback* callback);

	GxGroupbox* entryBox;
	GxLineEdit* pathEdit;
	GxSpinner* sizeEdit;
	GxSpinner* paddingEdit;
	GxSpinner* advanceEdit;
	GxDroplist* hintingList;
	GxCheckbox* fallbackBox;
	GxCheckbox* smallCapsBox;
	GxCheckbox* markAsBold;
	GxCheckbox* markAsItalic;
	GxCheckbox* kerningPairs;
	GxSpinner* texWidthEdit;
	GxSelectList* entriesList;
	GxSelectList* charsetList;

	static char* idEntryAdd;
	static char* idEntryClear;
	static char* idEntryRemove;
	static char* idEntryDuplicate;
	static char* idBrowse;
	static char* idClear;
	static char* idRemove;
	static char* idAddPreset;
	static char* idAddString;
	static char* idAddRange;
	static char* idAddBlock;
	static char* idPreview;
	static char* idAccept;
	static char* idCancel;
	static char* idEntries;
};

}; // namespace app