#pragma once

#include <GuiX/Widgets.h>

namespace app {

using namespace guix;

// ===================================================================================
// MiscForm

struct MiscForm
{
	static char* idAccept;
	static char* idCancel;
};

// ===================================================================================
// CharsetForm

struct CharsetForm : public MiscForm
{
	void Create(GxContext* context, GxCallback* callback);

	GxSelectList* charsetList;
};

// ===================================================================================
// StringForm

struct StringForm : public MiscForm
{
	void Create(GxContext* context, GxCallback* callback);

	GxTextEdit* stringEdit;
};

// ===================================================================================
// RangeForm

struct RangeForm : public MiscForm
{
	void Create(GxContext* context, GxCallback* callback);

	GxSpinner* firstEdit;
	GxSpinner* lastEdit;
};

// ===================================================================================
// PreviewForm

struct PreviewForm : public MiscForm
{
	void Create(GxContext* context, GxCallback* callback, const GxTexture& tex);

	GxImageBox* sprite;
};

}; // namespace app