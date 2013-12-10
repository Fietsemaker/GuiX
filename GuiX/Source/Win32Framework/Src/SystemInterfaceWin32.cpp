#include <GuiX/Config.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>

#include <string>
#include <vector>

#include <GuiX/Localize.h>
#include <GuiX/SystemInterfaceWin32.h>

namespace guix {
namespace framework {

static void AppendUnicode(std::wstring& str, int c)
{
	     if(c < 0x0000FFFF) str += (wchar_t)c;
	else if(c > 0x0010FFFF) str += (wchar_t)'~';
	else
	{
		c -= 0x0010000;
		str += (wchar_t)((c >> 10)     + 0xD800);
		str += (wchar_t)((c & 0x3FFUL) + 0xDC00);
	}
}

static int GetUnicode(const uchar*& str, const uchar* end)
{
	int c = (int)(*str);
	if(c < 0x80) ++str;
	else
	{
		int numBytes = GxUnicode::UTF8TrailingBytes()[c];
		if(str + numBytes < end)
		{
			c = 0;
			switch(numBytes)
			{
				case 5: c += *str++; c <<= 6;
				case 4: c += *str++; c <<= 6;
				case 3: c += *str++; c <<= 6;
				case 2: c += *str++; c <<= 6;
				case 1: c += *str++; c <<= 6;
				case 0: c += *str++;
			}
			c -= GxUnicode::UTF8Offsets()[numBytes];
		}
		else c = '~', str = end;
	}
	return c;
}

static void ToUTF16(std::wstring& out, const char* _str)
{
	const uchar* str = (uchar*)_str;
	const int len = GxStrLen(_str);
	const uchar* end = str + len;
	while(*str) AppendUnicode(out, GetUnicode(str, end));
}

GxList<wchar_t> GxSystemInterfaceWin32::Widen(const char* str)
{
	size_t size = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, str, -1, NULL, 0);
	GxList<wchar_t> buffer(size, (wchar_t)0);
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, str, -1, buffer.Edit(), size);
    return buffer;
}

GxList<char> GxSystemInterfaceWin32::Narrow(const wchar_t* str)
{
	size_t size = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
    GxList<char> buffer(size, (char)0);
	WideCharToMultiByte(CP_ACP, 0, str, -1, buffer.Edit(), size, NULL, NULL);
    return buffer;
}

GxSystemInterfaceWin32::GxSystemInterfaceWin32()
{
	myStartTime = GetTickCount();
}

void GxSystemInterfaceWin32::LogMessage(GxLogType type, const char* message)
{
	std::cout << message << '\n';
}

bool GxSystemInterfaceWin32::GetClipboardText(GxString& text)
{
	if(!OpenClipboard(NULL)) return false;

	bool success = false;
	HANDLE hData = GetClipboardData(CF_TEXT);
	char* source = (char*)GlobalLock(hData);
	if(source != NULL)
	{
		GxList<wchar_t> wstr = Widen(source);
		text = GxUnicode::FromWide(wstr.Data());
		success = true;
	}
	GlobalUnlock(hData);
	CloseClipboard();

	return success;
}

void GxSystemInterfaceWin32::SetClipboardText(const GxString& text)
{
	if(!OpenClipboard(NULL)) return;

	std::wstring wstr;
	ToUTF16(wstr, text.Raw());
	GxList<char> cstr = Narrow(wstr.c_str());

	EmptyClipboard();
	HGLOBAL bufferHandle = GlobalAlloc(GMEM_DDESHARE, cstr.Size());
	char* buffer = (char*)GlobalLock(bufferHandle);
	if(buffer != NULL)
	{
		memcpy(buffer, cstr.Data(), cstr.Size());
		SetClipboardData(CF_TEXT, bufferHandle);
	}
	GlobalUnlock(bufferHandle);
	CloseClipboard();
}

}; // namespace framework
}; // namespace guix