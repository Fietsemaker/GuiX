#pragma once

#include <GuiX/Style.h>

namespace guix {
namespace gui {

// ===================================================================================
// GxGuiImpl
// ===================================================================================

class GxStyleImp
{
public:
	static GxStyle* singleton;

	static void Create();
	static void Destroy();
};

}; // namespace gui
}; // namespace guix