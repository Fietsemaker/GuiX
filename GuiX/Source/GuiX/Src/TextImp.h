#pragma once

#include <string>
#include <vector>
#include <map>

#include <GuiX/Text.h>
#include <GuiX/Draw.h>

#include <Src/ResourceMap.h>

namespace guix {
namespace graphics {

// ===================================================================================
// Utility types, structs and enums

enum MiscProperties
{
	DEFAULT_FONT_SIZE = 13,
};

enum GlyphTraits
{
	GT_NORMAL      = 0x00,
	GT_BOLD        = 0x01,
	GT_ITALIC      = 0x02,
	GT_BOLD_ITALIC = GT_BOLD  | GT_ITALIC,
	GT_STYLE_MASK  = GT_BOLD_ITALIC,

	GT_WHITESPACE  = 0x04,
	GT_ALPHA       = 0x08,
	GT_DIGIT       = 0x10,
	GT_ALNUM       = GT_ALPHA | GT_DIGIT,
};

enum SpecialChars
{
	C_TERMINATOR   = 0x0000,
	C_HTAB         = 0x0009,
	C_LINE_FEED    = 0x000A,
	C_VTAB         = 0x000B,
	C_CARRIAGE_RET = 0x000D,
	C_SPACE        = 0x0020,
};

struct GlyphPage
{
	GxTextureHandle texture;
};

struct Glyph
{
	GxAreaf coords;
	GxAreaf uvs;
	int traits;
	float xAdvance;
	uint codepoint;
	int page;
};

struct CustomGlyph
{
	GxTexture texture;
	Glyph glyph;
};

struct Pen 
{
	float x, advance, spacemul;
};

struct Line
{
	int begin, end;
	float width, spacemul;
	bool ellipsis, justified;
};

struct CGGxRecti
{
	CGGxRecti() : glyph(NULL), x(0), y(0) {}
	CGGxRecti(const Glyph* g, float _x, float _y)
		:glyph(g), x(_x), y(_y) {}

	const Glyph* glyph;
	float x, y;
};

struct BreakInfo
{
	float spaceW, lineW, trailW;
	int end;
};

struct Ellipsis
{
	float x;
	int end;
	bool add;
};

// ===================================================================================
// GxFontData

class GxFontData
{
public:
	GxFontData();
	~GxFontData();

	void Destroy();

	bool Create(const char* path, const char* dir);
	bool Create(const uchar* rgba, int glyphW, int glyphH, int glyphsInW, int glyphsInH, int fontSize, const uint* charset);

	void SetBold(bool enabled) const;
	void SetItalic(bool enabled) const;
	float GetKerning(int leftGlyphIndex, int rightGlyphIndex) const;

	inline int GetGlyphIndex(uint codepoint) const
	{
		if(codepoint >= mapBegin && codepoint < mapEnd)
			return bmpGlyphMap[(codepoint - mapBegin) * myStyleCount + myStyleIndex];
		return myGetSMPIndex(codepoint);
	}

	inline const Glyph& GetGlyph(uint codepoint) const
	{
		return glyphs[GetGlyphIndex(codepoint)];
	}

	// BMP codepoint to glyph mapping.
	int* bmpGlyphMap;
	uint mapBegin, mapEnd;

	// SMP codepoint to glyph mapping.
	uint* smpGlyphKey;
	int* smpGlyphVal;
	int smpGlyphCount;

	// Kerning data.
	uint* kernKey;
	float* kernVal;
	int kernCount;

	// Glyph data.
	Glyph* glyphs;
	int glyphCount;

	// GlyphPage data.
	GlyphPage* glyphPages;
	int glyphPageCount;

	// Font information.
	int fontSize;
	float ellipsisW;

private:
	int myGetSMPIndex(uint character) const;
	void mySetSlot(int* map, int pos, const int* indices);
	void myInsertBlankGlyph(uint codepoint, int traits);
	void myFinalize();

	int myStyleCount;
	mutable int myStyle, myStyleIndex;
};

// ===================================================================================
// GxFontDatabaseImp

class GxFontDatabaseImp : public GxFontDatabase
{
public:
	static GxFontDatabaseImp* singleton;

	static void Create();
	static void Destroy();

	GxFontDatabaseImp();
	~GxFontDatabaseImp();

	void AddGlyph(const char* id, const GxTexture& img, int dy, int dx, int advance);
	int GetLoadedFontCount() const;
	void LogInfo() const;

	bool Load(GxFontHandle& outHandle, const char* path_or_resource);
	void AddReference(GxFontHandle handle);
	void Release(GxFontHandle handle);
	void MakeDefault(const GxFont& font);

	GxFontData* GetFont(GxFontHandle handle);

	const Glyph& GetCustomGlyph(const uchar* id, int len);
	GxTextureHandle GetCustomGlyphTexture(const Glyph* g);

private:
	void myCreateFallbacks();
	bool myLoadFile(GxFontHandle& outHandle, const char* path);
	GxFontData* myLoadFontData(const char* path);

	typedef ResourceMap<GxTextureHandle, GxFontData*> FontMap;
	typedef std::vector<CustomGlyph> GlyphVec;
	typedef std::map<GxString, int> GlyphMap;

	FontMap myFontMap;
	GlyphVec myGlyphs;
	GlyphMap myGlyphMap;
	GxFontData myFallbackFont;
	GxFont myDefaultFont;
};

// ===================================================================================
// GxTextRenderer

class GxTextRenderer
{
public:
	static GxTextRenderer* singleton;

	static void Create();
	static void Destroy();

	GxTextRenderer();
	~GxTextRenderer();

	void SetText(const GxText& settings, const char* string);
	GxRecti DrawText(int x, int y);
	GxRecti DrawHighlight(int x, int y);
	GxRecti GetTextRect(int x, int y);
	GxRecti GetCharRect(int x, int y, int charIndex);
	int GetCharIndex(int x, int y, int cx, int cy);

private:
	void myResetPos();
	void mySkipTo(int pos);
	int myReadMultibyte(int c);
	const Glyph& myNextChar();
	const Glyph& myReadTag();

	void myAdvanceBare(Pen& pen, const Glyph& g);
	void myAdvance(Pen& pen, const Glyph& g);

	void myProcessWithBreaks();
	void myProcessWithoutBreaks();
	void myAddLine(int, int, float, const Ellipsis&);
	void myAddLine(int, const BreakInfo&, bool);

	void myResizeBuffers(int quadCount);
	void myEmitQuad(const Glyph& glyph, int& index, float x, float y);
	void myRenderQuads(int begin, int count, GxTextureHandle tex);

	void myDrawText(float x, float y);
	void myDrawUnderlines(float x, float y);
	void myDrawHighlight(float x, float y);
	GxRecti myGetCharRect(float x, float y, int charIndex);
	int myGetCharIndex(int x, int y, int cx, int cy);

private:
	inline bool myInRange() const
		{ return myPos >= myRangeBegin && myPos < myRangeEnd; }

	const GxFontData* myFont;
	const uchar* myStr;

	int myPos, myReadPos, myLen;
	int myKPL, myKPR;

	int myTextW, myTextH;
	int myRangeBegin, myRangeEnd;
	float myTabWidth, myMaxWidth;
	GxColor myColorT, myColorB, myColorS;
	bool myIsWordWrap, myIsJustified;
	bool myIsFormatted, myIsEllipsis;
	bool myIsSingleLine, myIsKerning;
	bool myInUnderline, myHasUnderlines;
	GxTextAlignH myAlignH;
	GxTextAlignV myAlignV;

	std::vector<uint> myIndexBuffer;
	std::vector<GxVertex> myVertexBuffer;
	std::vector<CGGxRecti> myCustomGlyphs;
	std::vector<Line> myLines;
};

}; // namespace graphics
}; // namespace guix