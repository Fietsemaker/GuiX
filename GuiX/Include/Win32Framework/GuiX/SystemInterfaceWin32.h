#pragma once

#include <GuiX/List.h>
#include <GuiX/Interfaces.h>

namespace guix {
namespace framework {

// ===================================================================================
// GxSystemInterfaceWin32
// ===================================================================================

class GUIX_API GxSystemInterfaceWin32 : public GxSystemInterface
{
public:
	GxSystemInterfaceWin32();

	void LogMessage(GxLogType type, const char* message);

	bool GetClipboardText(GxString& text);
	void SetClipboardText(const GxString& text);

	static GxList<wchar_t> Widen(const char* str);
	static GxList<char> Narrow(const wchar_t* str);

private:
	int myStartTime;
};

}; // namespace framework
}; // namespace guix