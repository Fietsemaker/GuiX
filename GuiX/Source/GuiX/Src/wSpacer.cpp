#include <GuiX/Config.h>

#include <GuiX/wSpacer.h>

namespace guix {
namespace widgets {

// ===================================================================================
// GxSpacer
// ===================================================================================

GxSpacer::GxSpacer()
{
	myPolicy->min.Set(0, 0);
	myPolicy->hint.Set(0, 0);

	myPolicy->flagsH = GX_SP_FIXED;
	myPolicy->flagsV = GX_SP_FIXED;
}

// ===================================================================================
// GxSpacerH
// ===================================================================================

GxSpacerH::GxSpacerH()
{
	myPolicy->min.Set(0, 0);
	myPolicy->hint.Set(0, 0);

	myPolicy->flagsH = GX_SP_EXPAND;
	myPolicy->flagsV = GX_SP_FIXED;
}

// ===================================================================================
// GxSpacerV
// ===================================================================================

GxSpacerV::GxSpacerV()
{
	myPolicy->min.Set(0, 0);
	myPolicy->hint.Set(0, 0);

	myPolicy->flagsH = GX_SP_FIXED;
	myPolicy->flagsV = GX_SP_EXPAND;
}

}; // namespace widgets
}; // namespace guix