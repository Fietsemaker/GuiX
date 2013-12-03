#pragma once

#include <memory.h>
#include <limits.h>

#include <vector>
#include <string>

namespace builder {

// ===================================================================================
// Common types
// ===================================================================================

typedef float          real;

typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;

typedef uint           codepoint32;

enum GlyphStyle
{
	GS_NORMAL      = 0x0,
	GS_BOLD        = 0x1,
	GS_ITALIC      = 0x2,
	GS_BOLD_ITALIC = 0x3, // GS_BOLD | GS_ITALIC

	GS_STYLE_COUNT
};

enum SpecialChars
{
	C_TERMINATOR   = 0x00,
	C_HTAB         = 0x09,
	C_LINE_FEED    = 0x0A,
	C_VTAB         = 0x0B,
	C_CARRIAGE_RET = 0x0D,
	C_SPACE        = 0x20,
};

// ===================================================================================
// Utility functions
// ===================================================================================

template <typename T>
T* AlignedAlloc(size_t size, size_t alignment)
{
	const size_t sz = sizeof(T) * size;
	void* adr = malloc((sz + alignment - 1) + sizeof(void*));
	void* ptr = (void*)(((uintptr_t)adr + sizeof(void*) + alignment - 1) & ~(alignment - 1));
	*((void**)ptr - 1) = adr;
	return (T*)ptr;
}

template <typename T>
void AlignedFree(T* ptr)
{
    if(ptr) free(*((void**)ptr - 1));
}

template <typename T>
size_t Strlen(T* ptr)
{
	size_t l = 0;
	while(*ptr++) ++l;
	return l;
}

template <typename T>
inline T Min(T a, T b)
{
	return (a < b ? a : b);
}

template <typename T>
inline T Max(T a, T b)
{
	return (a > b ? a : b);
}

// Returns the identifier string of a glyph style type.
extern const char* GetStyleString(int style);

// Returns the filename part of a path string.
extern std::string GetFilePart(const std::string& path);

// Returns the directory part of a path string.
extern std::string GetDirPart(const std::string& path);

// ===================================================================================
// Common structs
// ===================================================================================

struct Vec2
{
	Vec2()               : x(0),  y(0)  {}
	Vec2(int _x, int _y) : x(_x), y(_y) {}
	int x, y;
};

struct Rect
{
	Rect()                               : x(0),  y(0),  w(0),  h(0)  {}
	Rect(int _x, int _y, int _w, int _h) : x(_x), y(_y), w(_w), h(_h) {}
	int x, y, w, h;
};

struct Glyph
{
	codepoint32 codepoint;
	Rect bitmapRegion;
	Vec2 drawCoords;
	int advance;
};

struct KerningPair
{
	int left, right, delta;
};

typedef std::vector<KerningPair> KerningList;

// ===================================================================================
// Common classes
// ===================================================================================

class NonCopyable
{
protected:
	NonCopyable() {}
	~NonCopyable() {}
private:
	NonCopyable(const NonCopyable&);
	const NonCopyable& operator = (const NonCopyable&);
};

class GlyphPage : public NonCopyable
{
public:
	GlyphPage();
	~GlyphPage();

	int width;
	int height;
	uchar* bitmap;
};

// ===================================================================================
// Assert
// ===================================================================================

#if 0

#define BUILDER_ASSERTS

// do not call _Assert directly, use macros instead
extern bool _Assert(char* inExpression, char* inFile, int inLine, char* inFunction, const char* inFormat, ...);

#define Breakpoint																		\
	__asm { int 3 }

#define Assert(exp)																	    \
	if((!(exp)) && Lty::_Assert(#exp, __FILE__, __LINE__, __FUNCTION__, 0))			\
		Breakpoint;

#define AssertF(exp, ...)																\
	if((!(exp)) && Lty::_Assert(#exp, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__))	\
		Breakpoint;

#else

#define BreakPoint
#define Assert(exp)
#define AssertF(exp, ...)

#endif

}; // namespace builder