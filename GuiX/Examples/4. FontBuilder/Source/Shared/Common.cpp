#include <Shared/Common.h>

#ifdef BUILDER_ASSERTS

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#define MAX_CONSOLE_LINES		256
#define LOG_BUFFER_SIZE			8192

// if your assert message is longer then MAX_MESSAGE_LENGTH, you are screwed
#define MAX_MESSAGE_LENGTH		8192

#endif // BUILDER_ASSERTS

namespace builder {

const char* GetStyleString(int style)
{
	switch(style)
	{
		case GS_NORMAL:      return "normal";
		case GS_BOLD:        return "bold";
		case GS_ITALIC:      return "italic";
		case GS_BOLD_ITALIC: return "bold italic";
	};
	return "unknown";
}

std::string GetFilePart(const std::string& path)
{
	size_t i = path.find_last_of("\\/");
	if(i == std::string::npos) return path;
	return path.substr(i+1);
}

std::string GetDirPart(const std::string& path)
{
	size_t i = path.find_last_of("\\/");
	if(i == std::string::npos) return path + '/';
	return path.substr(0,i) + '/';
}

// ===================================================================================
// GlyphPage
// ===================================================================================

GlyphPage::GlyphPage()
{
	width  = 0;
	height = 0;
	bitmap = NULL; 
}

GlyphPage::~GlyphPage()
{
	AlignedFree(bitmap);
}

// ===================================================================================
// Assert
// ===================================================================================

#ifdef BUILDER_ASSERTS

namespace
{
	static HHOOK hhk;
	static std::vector<std::string> sIgnoredAsserts;
};

LRESULT CALLBACK CBTProc(INT nCode, WPARAM wParam, LPARAM lParam)
{
	HWND hWndChild;
	if(nCode == HCBT_ACTIVATE)
	{
		hWndChild = (HWND)wParam;
		UINT result;
		if(GetDlgItem(hWndChild, IDYES) != NULL)
			result = SetDlgItemText(hWndChild, IDYES, "Debug");
		if(GetDlgItem(hWndChild, IDYES) != NULL)
			result = SetDlgItemText(hWndChild, IDNO, "Ignore Once");
		if(GetDlgItem(hWndChild, IDYES) != NULL)
			result = SetDlgItemText(hWndChild, IDCANCEL, "Ignore All");
		UnhookWindowsHookEx(hhk);
	}
	else
	{
		CallNextHookEx(hhk, nCode, wParam, lParam);
	}
	return 0;
}

static int sMessageBox(HWND hWnd, LPSTR lpText, LPSTR lpCaption, UINT uType)
{
	hhk = SetWindowsHookEx(WH_CBT, &CBTProc, 0, GetCurrentThreadId());
	return MessageBox(hWnd, lpText, lpCaption, uType);
}

bool _Assert(char* inExpression, char* inFile, int inLine, char* inFunction, const char* inFormat, ...)
{
	char assertID[MAX_MESSAGE_LENGTH];
	sprintf(assertID, "%s%i", inFile, inLine);

	bool ignoreThisAssert = false;
	unsigned int ignoredSize = sIgnoredAsserts.size();
	for(unsigned int i=0; i<ignoredSize; ++i)
	{
		if(strcmp(sIgnoredAsserts[i].c_str(), assertID) == 0)
			ignoreThisAssert = true;
	}
	if(!ignoreThisAssert)
	{
		char* message = Malloc<char>(MAX_MESSAGE_LENGTH);
		char* buffer  = Malloc<char>(MAX_MESSAGE_LENGTH * 2);

		if(inFormat)
		{
			va_list args;
			va_start(args, inFormat);
			vsnprintf(message, MAX_MESSAGE_LENGTH-1, inFormat, args);
			va_end(args);
		}
		else
		{
			strcpy(message, inExpression);
		}

		double elapsedTime = 0.0;//gApplication->GetElapsedTime();
		const double hours = floor(elapsedTime / 3600);
		elapsedTime -= hours * 3600;
		const double minutes = floor(elapsedTime / 60); 
		elapsedTime -= minutes * 60;
		const double seconds = elapsedTime;

		char* fileName = strrchr(inFile, '\\');
		fileName = fileName ? (fileName + 1) : inFile;
		sprintf(buffer, "ASSERT (%02.0f:%02.0f:%05.2f) in %s (%i):\n%s", hours, minutes, seconds, fileName, inLine, message);
		//gLog->Print(buffer);
	
		int answer = sMessageBox(0, buffer, "ASSERT", MB_ICONERROR | MB_YESNOCANCEL);
		if(answer == IDYES)
		{
			return true;
		}
		else if(answer == IDCANCEL)
		{
			sIgnoredAsserts.push_back(assertID);
		}
	}
	return false;
}

#endif

}; // namespace builder