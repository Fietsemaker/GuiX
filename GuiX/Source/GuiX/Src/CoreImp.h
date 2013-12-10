#pragma once

#include <GuiX/Core.h>

namespace guix {
namespace core {

// ===================================================================================
// GxCoreImp
// ===================================================================================

class GxCoreImp
{
public:
	static bool isInitialized;

	static GxRenderInterface  defaultRenderInterface;
	static GxFileInterfaceStd defaultFileInterface;
	static GxSystemInterface  defaultSystemInterface;

	static GxRenderInterface* renderInterface;
	static GxFileInterface*   fileInterface;
	static GxSystemInterface* systemInterface;
};

}; // namespace core
}; // namespace guix