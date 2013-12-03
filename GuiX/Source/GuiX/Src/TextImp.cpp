#include <GuiX/Config.h>

#include <limits.h>
#include <math.h>

#include <GuiX/Common.h>
#include <GuiX/Localize.h>
#include <GuiX/Interfaces.h>

#include <Src/TextImp.h>

namespace guix {
namespace graphics {
namespace {

static const int _npos = INT_MAX;

// ===================================================================================
// Utility types, structs and enums

static const uchar utf8TrailingBytes[256] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5,
};

static const uchar utf8LeadingByte[7] =
{
    0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC
};

static const uint utf8Offsets[6] =
{
    0x00000000, 0x00003080, 0x000E2080, 0x03C82080, 0xFA082080, 0x82082080
};

struct PageQuads
{
	PageQuads() : count(0), index(0) {}

	int count, index;
};

// ===================================================================================
// Utility functions

#define IsWhitespace(glyph)		((glyph).traits & GT_WHITESPACE)
#define NonWhitespace(glyph)	(!((glyph).traits & GT_WHITESPACE))

// Returns the x-offset for the text area based on the horizontal text alignment.
static int GetOffsetX(GxTextAlignH align, int w)
{
	return (-w * align) >> 1;
}

static float GetOffsetX(GxTextAlignH align, float w)
{
	return (-w * align) * 0.5f;
}

// Returns the y-offset for the text area based on the horizontal text alignment.
static int GetOffsetY(GxTextAlignV align, int h)
{
	return (-h * align) >> 1;
}

}; // anonymous namespace

// ===================================================================================
// GxFont

GxFont::~GxFont()
{
	Destroy();
}

GxFont::GxFont()
	:myHandle(0)
{
}

GxFont::GxFont(const char* path)
	:myHandle(0)
{
	Create(path);
}

GxFont::GxFont(const GxFont& other)
	:myHandle(other.myHandle)
{
	GxFontDatabaseImp* database = GxFontDatabaseImp::singleton;
	if(database) database->AddReference(other.myHandle);
}

bool GxFont::Create(const char* path)
{
	GxFontDatabaseImp* database = GxFontDatabaseImp::singleton;
	if(database)
	{
		GxFontHandle old = myHandle;
		database->Load(myHandle, path);
		database->Release(old);
	}
	else myHandle = 0;
	return myHandle != 0;
}

void GxFont::Destroy()
{
	GxFontDatabaseImp* database = GxFontDatabaseImp::singleton;
	if(database) database->Release(myHandle);
	myHandle = 0;
}

void GxFont::Assign(const GxFont& other)
{
	GxFontDatabaseImp* database = GxFontDatabaseImp::singleton;
	if(database)
	{
		database->AddReference(other.myHandle);
		database->Release(myHandle);
	}
	myHandle = other.myHandle;
}

void GxFont::MakeDefault()
{
	GxFontDatabaseImp* database = GxFontDatabaseImp::singleton;
	if(database) database->MakeDefault(*this);
}

int GxFont::GetSize() const
{
	GxFontDatabaseImp* database = GxFontDatabaseImp::singleton;
	if(!database) return DEFAULT_FONT_SIZE;
	return database->GetFont(myHandle)->fontSize;
}

GxFontHandle GxFont::GetHandle() const
{
	return myHandle;
}

GxFont& GxFont::operator = (const GxFont& other)
{
	Assign(other);
	return *this;
}

// ===================================================================================
// GxTextRenderer

GxTextRenderer* GxTextRenderer::singleton = NULL;

void GxTextRenderer::Create()
{
	singleton = new GxTextRenderer();
}

void GxTextRenderer::Destroy()
{
	delete singleton;
	singleton = NULL;
}

GxTextRenderer::~GxTextRenderer()
{
}

GxTextRenderer::GxTextRenderer()
	:myStr(NULL)
	,myFont(NULL)
{
}

GxRecti GxTextRenderer::DrawText(int x, int y)
{
	y += GetOffsetY(myAlignV, myTextH);
	myDrawText((float)x, (float)y);
	x += GetOffsetX(myAlignH, myTextW);
	return GxRecti(x, y, myTextW, myTextH);
}

GxRecti GxTextRenderer::DrawHighlight(int x, int y)
{
	y += GetOffsetY(myAlignV, myTextH);
	myDrawHighlight((float)x, (float)y);
	x += GetOffsetX(myAlignH, myTextW);
	return GxRecti(x, y, myTextW, myTextH);
}

GxRecti GxTextRenderer::GetTextRect(int x, int y)
{
	y += GetOffsetY(myAlignV, myTextH);
	x += GetOffsetX(myAlignH, myTextW);
	return GxRecti(x, y, myTextW, myTextH);
}

GxRecti GxTextRenderer::GetCharRect(int x, int y, int charIndex)
{
	y += GetOffsetY(myAlignV, myTextH);
	return myGetCharRect((float)x, (float)y, charIndex);
}

int GxTextRenderer::GetCharIndex(int x, int y, int cx, int cy)
{
	y += GetOffsetY(myAlignV, myTextH);
	return myGetCharIndex(x, y, cx, cy);
}

// ===================================================================================
// Character reading functions

#define F_TAG1(x)		(x)
#define F_TAG2(x,y)		((y) | ((x) << 8))

void GxTextRenderer::myResetPos()
{
	myPos = myReadPos = 0;
	myKPL = myKPR = -1;
	myFont->SetBold(false);
	myFont->SetItalic(false);
	myHasUnderlines = false;
	myInUnderline = false;
}

void GxTextRenderer::mySkipTo(int pos)
{
	while(myReadPos < pos) myNextChar();
}

int GxTextRenderer::myReadMultibyte(int c)
{
	const uchar* p = myStr + myReadPos;
	int numBytes = utf8TrailingBytes[c];
	if(myReadPos + numBytes <= myLen)
	{
		switch(numBytes)
		{
			case 5: c <<= 6; c += *p++;
			case 4: c <<= 6; c += *p++;
			case 3: c <<= 6; c += *p++;
			case 2: c <<= 6; c += *p++;
			case 1: c <<= 6; c += *p++;
		};
		c -= utf8Offsets[numBytes];
		myReadPos += numBytes;
	}
	return c;
}

const Glyph& GxTextRenderer::myNextChar()
{
	myPos = myReadPos;
	myKPL = myKPR;

	// Read the current character.
	int c = myStr[myReadPos++];
	if(c >= 0x80) c = myReadMultibyte(c);

	// Check if the current character is the start of a formatting tag.
	if(c == '{' && myIsFormatted) return myReadTag();

	// Return the codepoint and glyph of the current character.
	myKPR = myFont->GetGlyphIndex(c);
	return myFont->glyphs[myKPR];
}

const Glyph& GxTextRenderer::myReadTag()
{
	// Keep reading tags until a character is returned.
	int c = 0;
	--myReadPos;
	while(myStr[myReadPos] == '{' && !c)
	{
		// Read the tag contents.
		++myReadPos;
		uint tag4 = 0;
		const uchar* tag = myStr + myReadPos, *p = tag;
		while(*p != '}' && *p) tag4 = (tag4 << 8) + *p++;
		const int len = p - tag;
		if(len > 4) tag4 = 0;
		myReadPos += len;
		if(*p) ++myReadPos;

		// Perform the action associated with the tag.
		GxFontDatabaseImp* database = GxFontDatabaseImp::singleton;
		switch(tag4)
		{
			case F_TAG1('b'    ): myFont->SetBold(true); break;
			case F_TAG1('i'    ): myFont->SetItalic(true); break;
			case F_TAG1('u'    ): myInUnderline = myHasUnderlines = true; break;
			case F_TAG2('/','b'): myFont->SetBold(false); break;
			case F_TAG2('/','i'): myFont->SetItalic(false); break;
			case F_TAG2('/','u'): myInUnderline = false; break;
			case F_TAG2('l','b'): c =  '{'; break;
			case F_TAG2('r','b'): c =  '}'; break;
			case F_TAG2('n','l'): c = '\n'; break;
			case F_TAG2('h','t'): c = '\t'; break;
			default: myKPR=-1; return database->GetCustomGlyph(tag, len);
		};
	}

	// If the tags did not return a character, we read the next character.
	if(!c)
	{
		c = myStr[myReadPos++];
		if(c >= 0x80) c = myReadMultibyte(c);
	}

	// Return the codepoint and glyph of the current character.
	myKPR = myFont->GetGlyphIndex(c);
	return myFont->glyphs[myKPR];
}

void GxTextRenderer::myAdvanceBare(Pen& pen, const Glyph& g)
{
	// Apply the current advance value.
	pen.x += pen.advance;
	if(myIsKerning) pen.x += myFont->GetKerning(myKPL, myKPR);

	// Store the next advance value.
	if(g.codepoint != C_HTAB)
		pen.advance = g.xAdvance;
	else
		pen.advance = (floor(pen.x / myTabWidth) + 1) * myTabWidth - pen.x;
}

void GxTextRenderer::myAdvance(Pen& pen, const Glyph& g)
{
	// Apply the current advance value.
	pen.x += pen.advance;
	if(myIsKerning)	pen.x += myFont->GetKerning(myKPL, myKPR);

	// Store the next advance value.
	if(NonWhitespace(g))
		pen.advance = g.xAdvance;
	else if(g.codepoint != C_HTAB)
		pen.advance = g.xAdvance * pen.spacemul;
	else
		pen.advance = (floor(pen.x / myTabWidth) + 1) * myTabWidth - pen.x;
}

// ===================================================================================
// Text pre-processing functions

// Sets the string and text settings used for rendering, and breaks the string into lines.
void GxTextRenderer::SetText(const GxText& settings, const char* text)
{
	myStr = (const uchar*)text;
	myLen = GxStrLen(text);

	GxFontDatabaseImp* database = GxFontDatabaseImp::singleton;
	myRangeBegin   = GxMax(0, settings.rangeBegin);
	myRangeEnd     = GxMin(myLen, settings.rangeEnd);
	myMaxWidth     = (float)settings.maxWidth;
	myTabWidth     = settings.tabWidth;
	myColorT       = settings.top;
	myColorB       = settings.bottom;
	myColorS       = settings.shadow;
	myFont         = database->GetFont(settings.font.GetHandle());
	myIsWordWrap   = settings.HasFlag(GX_TF_WORD_WRAP);
	myIsFormatted  = settings.HasFlag(GX_TF_FORMATTING);
	myIsEllipsis   = settings.HasFlag(GX_TF_ELLIPSIS) && myMaxWidth >= 0;
	myIsSingleLine = settings.HasFlag(GX_TF_SINGLELINE);
	myIsJustified  = settings.HasFlag(GX_TF_JUSTIFIED) && myMaxWidth >= 0 && !myIsEllipsis && !myIsSingleLine;
	myIsKerning    = settings.HasFlag(GX_TF_KERNING) && myFont->kernCount > 0;
	myAlignH       = settings.alignH;
	myAlignV       = settings.alignV;

	myTextW = myTextH = 0;
	myLines.clear();
	myResetPos();

	bool softBreaks = myMaxWidth >= 0 && !myIsEllipsis && !myIsSingleLine;
	softBreaks ? myProcessWithBreaks() : myProcessWithoutBreaks();
	myTextH = GxMax(1, (int)myLines.size()) * myFont->fontSize;
}

// Pre-processes the text and breaks it down into horizontal lines.
// This variant breaks on a newline character, or if the line width exceeds the maximum width.
void GxTextRenderer::myProcessWithBreaks()
{
	int begin = 0;
	Pen pen = { 0, 0, 1.f };
	BreakInfo cur = {0, 0, 0};
	BreakInfo brk = {0, 0, 0};
	bool inWhitespace = false;
	bool validBreak = false;
	bool newline = false;

	// Make a list of every horizontal line of text.
	while(myLines.size() < 2048) // Prevents absurd amount of lines, should be enough...
	{
		// Get the next character in the string.
		const Glyph& glyph = myNextChar();
		if(myPos == myLen) break;
		cur.end = myPos;

		// Advance to the start of this character.
		pen.x = cur.lineW;
		myAdvanceBare(pen, glyph);
		cur.lineW = pen.x;

		// Check if we have a valid break position in front of the current character.
		const bool nonWhitespace = NonWhitespace(glyph);
		if((inWhitespace && nonWhitespace) || !myIsWordWrap)
			brk = cur, validBreak = true;

		// Check if we are forced to break in front of the current character.
		const float newWidth = cur.lineW + pen.advance;
		if(newline || (nonWhitespace && newWidth > myMaxWidth && cur.lineW > 0))
		{
			// If we don't have a valid break or hit a newline, we break in front of this character.
			if(newline || !validBreak)
			{
				myAddLine(begin, cur, false);
				cur.lineW = cur.spaceW = 0.f;
				begin = cur.end;
			}
			else // Otherwise, we break at the last suitable break position (word wrap).
			{
				cur.spaceW -= brk.spaceW;
				cur.lineW -= brk.lineW;
				myAddLine(begin, brk, myIsJustified);
				begin = brk.end;
			}
			inWhitespace = validBreak = newline = false;
			cur.trailW = 0.f;
		}

		// Process the current character.
		if(nonWhitespace)
		{
			cur.trailW = 0.f;
			inWhitespace = false;
		}
		else
		{
			if(glyph.codepoint == C_LINE_FEED) newline = true;
			cur.spaceW += pen.advance;
			cur.trailW += pen.advance;
			inWhitespace = true;
		}
	}

	// Finally, add left over characters to the last line.
	cur.end = myLen;
	cur.lineW += pen.advance;
	if(begin < myLen) myAddLine(begin, cur, false);
}

// Pre-processes the text and breaks it down into horizontal lines.
// This variant only breaks on a newline character.
void GxTextRenderer::myProcessWithoutBreaks()
{
	int begin = 0;
	Pen pen = { 0, 0, 1.f };
	Ellipsis ellipsis = { 0, 0, false };

	// Make a list of every horizontal line of text.
	while(myLines.size() < 2048) // Prevents absurd amount of lines, should be enough...
	{
		// Get the next character in the string.
		const Glyph& glyph = myNextChar();
		if(myPos == myLen) break;
		myAdvanceBare(pen, glyph);

		// Keep track of the position and width of the ellipsis.
		const float newWidth = pen.x + pen.advance;
		if(myIsEllipsis && NonWhitespace(glyph))
		{
			if(newWidth + myFont->ellipsisW < myMaxWidth)
				ellipsis.end = myReadPos, ellipsis.x = newWidth;
			else if(newWidth > myMaxWidth)
				ellipsis.add = true;
		}

		// Check if we are forced to break the line and continue on a new line.
		if(glyph.codepoint == C_LINE_FEED && !myIsSingleLine)
		{
			myAddLine(begin, myReadPos, pen.x, ellipsis);
			ellipsis.x = pen.x = pen.advance = 0.f;
			ellipsis.end = begin = myReadPos;
			ellipsis.add = false;
		}
	}

	// Finally, add left over characters to the last line.
	pen.x += pen.advance;
	if(begin < myLen) myAddLine(begin, myLen, pen.x, ellipsis);
}

// This line adding function is used by myProcessWithBreaks.
void GxTextRenderer::myAddLine(int begin, const BreakInfo& b, bool justify)
{
	float lineW  = b.lineW - b.trailW;
	float spaceW = b.spaceW - b.trailW;
	Line line = { begin, b.end, lineW, 1.f, false, justify };

	// If the line is justified, we calculate the whitespace scalar.
	if(justify && spaceW > 0)
	{
		float textW = lineW - spaceW;
		line.spacemul = GxMax(0.f, myMaxWidth - textW) / spaceW;
		line.width = textW + spaceW * line.spacemul;
	}

	myTextW = GxMax(myTextW, (int)lineW);
	myLines.push_back(line);
}

// This line adding function is used by myProcessWithoutBreaks.
void GxTextRenderer::myAddLine(int begin, int end, float width, const Ellipsis& ellipsis)
{
	Line line = { begin, end, width, 1.f, ellipsis.add, false };

	// Adjust the line if ellipsis are inserted somewhere.
	if(ellipsis.add)
	{
		line.end = GxMin(line.end, ellipsis.end);
		line.width = ellipsis.x + myFont->ellipsisW;
	}

	myTextW = GxMax(myTextW, (int)width);
	myLines.push_back(line);
}

// ===================================================================================
// Rendering helper functions

#define SetVertex(p, X, Y, U, V, C)\
	(*p = GxVertex(X, Y, U, V, C))

// Used by the rendering functions to reserve vertices for rendering quads.
void GxTextRenderer::myResizeBuffers(int quadCount)
{
	if(quadCount*4 <= (int)myVertexBuffer.size()) return;

	// Resize vertex buffer.
	size_t numVertices = (size_t)(quadCount * 4);
	myVertexBuffer.resize(numVertices);

	// Resize index buffer.
	size_t numIndices = (size_t)(quadCount * 6);
	size_t begin = myIndexBuffer.size();
	myIndexBuffer.resize(numIndices);
	uint* ptr = (uint*)(&myIndexBuffer[begin]);
	for(size_t i=begin, j=begin/6*4; i<numIndices; i+=6, j+=4)
	{
		*ptr++ = j, *ptr++ = j+1, *ptr++ = j+3;
		*ptr++ = j, *ptr++ = j+3, *ptr++ = j+2;
	}
}

// Used by the rendering functions to render a single glyph quad.
void GxTextRenderer::myEmitQuad(const Glyph& glyph, int& index, float x, float y)
{
	const float a = floor(x + glyph.coords.l);
	const float b = floor(y + glyph.coords.t);
	const float c = floor(x + glyph.coords.r);
	const float d = floor(y + glyph.coords.b);

	const float e = glyph.uvs.l;
	const float f = glyph.uvs.t;
	const float g = glyph.uvs.r;
	const float h = glyph.uvs.b;

	const GxColor ct = myColorT;
	const GxColor cb = myColorB;
	const GxColor cs = myColorS;

	GxVertex* v = &myVertexBuffer.front() + (index << 2);
	if(cs.a)
	{
		SetVertex(v, a+1.f, b+1.f, e, f, cs); ++v;
		SetVertex(v, a+1.f, d+1.f, e, h, cs); ++v;
		SetVertex(v, c+1.f, b+1.f, g, f, cs); ++v;
		SetVertex(v, c+1.f, d+1.f, g, h, cs); ++v;
		++index;
	}
	SetVertex(v, a, b, e, f, ct); ++v;
	SetVertex(v, a, d, e, h, cb); ++v;
	SetVertex(v, c, b, g, f, ct); ++v;
	SetVertex(v, c, d, g, h, cb);
	++index;
}

#undef SetVertex

void GxTextRenderer::myRenderQuads(int begin, int count, GxTextureHandle tex)
{
	GxDraw::Get()->Flush();
	GxRenderInterface* renderer = GxRenderInterface::Get();
	const uint* i = &myIndexBuffer.front() + begin;
	const GxVertex* v = &myVertexBuffer.front();
	renderer->DrawTriangles(v, (int)myVertexBuffer.size(), i, count, tex);
}

// ===================================================================================
// GxText rendering

void GxTextRenderer::myDrawText(float x, float y)
{
	const int lineCount = (int)myLines.size();
	const Glyph& period = myFont->GetGlyph('.');

	// Count the total number of quads and the number of quads per glyph page.
	const int pageCount = myFont->glyphPageCount;
	std::vector<PageQuads> quads(pageCount);

	myResetPos();
	int quadTotal = 0;
	for(int l=0; l<lineCount; ++l)
	{
		// Count the number of glyph quads.
		const Line& line = myLines[l];
		mySkipTo(line.begin);
		while(myReadPos < line.end)
		{
			const Glyph& glyph = myNextChar();
			if(myInRange() && NonWhitespace(glyph))
			{
				if(glyph.page >= 0)
					++quads[glyph.page].count;
				++quadTotal;
			}
		}

		// If there are ellipsis, count three extra quads.
		if(line.ellipsis)
		{
			quads[period.page].count += 3;
			quadTotal += 3;
		}
	}
	if(quadTotal == 0) return;

	// If there is a shadow effect, each quad has an additional shadow quad.
	if(myColorS.a)
	{
		for(int i=0; i<pageCount; ++i)
			quads[i].count *= 2;
		quadTotal *= 2;
	}

	// Draw underlines before the rest of the glyphs are drawn.
	if(myHasUnderlines)	myDrawUnderlines(x, y);

	// Assign each glyph page a portion of the allocated vertices.
	myResizeBuffers(quadTotal);
	for(int i=0, j=0; i<pageCount; ++i)
	{
		quads[i].index = j;
		j += quads[i].count;
	}

	// Fill in the vertex data and make a list of custom glyphs, which are rendered afterwards.
	myResetPos();
	myCustomGlyphs.clear();
	for(int l=0; l<lineCount; ++l)
	{
		const Line& line = myLines[l];
		Pen pen = { 0, 0, line.spacemul };
		const float ox = x + GetOffsetX(myAlignH, line.width);
		const float oy = y + (float)((l+1) * myFont->fontSize);

		mySkipTo(line.begin);
		while(myReadPos < line.end)
		{
			// If this is a printable character, we will emit a quad for it.
			const Glyph& glyph = myNextChar();
			myAdvance(pen, glyph);
			if(myInRange() && NonWhitespace(glyph))
			{
				if(glyph.page >= 0)
					myEmitQuad(glyph, quads[glyph.page].index, ox + pen.x, oy);
				else
					myCustomGlyphs.push_back(CGGxRecti(&glyph, ox + pen.x, oy));
			}
		}

		// If there are ellipsis, emit quads for them.
		if(line.ellipsis)
		{
			pen.x += pen.advance;
			int& index = quads[period.page].index;
			for(int j=0; j<3; ++j, pen.x += period.xAdvance)
				myEmitQuad(period, index, ox + pen.x, oy);
		}
	}

	// Finally it's time to render the glyph vertices.
	for(int i=0, idx=0, count=0; i<pageCount; ++i)
	{
		if(count = quads[i].count*6)
			myRenderQuads(idx, count, myFont->glyphPages[i].texture);
		idx += count;
	}

	// And after that, render the custom glyphs.
	if(myCustomGlyphs.size() > 0)
	{
		myResizeBuffers(8);

		GxFontDatabaseImp* database = GxFontDatabaseImp::singleton;
		for(size_t i=0; i<myCustomGlyphs.size(); ++i)
		{
			int idx = 0;
			const CGGxRecti& r = myCustomGlyphs[i];
			myEmitQuad(*myCustomGlyphs[i].glyph, idx, r.x, r.y);
			myRenderQuads(0, idx*6, database->GetCustomGlyphTexture(r.glyph));
		}
	}
}

void GxTextRenderer::myDrawUnderlines(float x, float y)
{
	// Fill in the vertex data by rendering out the lines of rects.	
	Glyph rect = { GxAreaf(0,1,0,2), GxAreaf(), 0, 0, 0, 0 };
	myResetPos();
	int index = 0;
	for(size_t l=0; l<myLines.size(); ++l)
	{
		const Line& line = myLines[l];
		Pen pen = { 0, 0, line.spacemul };
		const float ox = x + GetOffsetX(myAlignH, line.width);
		const float oy = y + (int)((l+1) * myFont->fontSize);

		// Emit underline quads for the current line.
		bool drawing = false;
		mySkipTo(line.begin);
		while(myReadPos < line.end)
		{
			const Glyph& glyph = myNextChar();
			myAdvance(pen, glyph);
			if(myInUnderline && !drawing)
			{
				rect.coords.l = pen.x;
				drawing = true;
			}
			if(drawing)
			{
				if(!myInUnderline)
				{
					myResizeBuffers(index + 2);
					myEmitQuad(rect, index, ox, oy);
					drawing = false;
				}
				else if(NonWhitespace(glyph))
				{
					rect.coords.r = pen.x + pen.advance;
				}
			}
		}
		if(drawing)
		{
			myResizeBuffers(index + 2);
			myEmitQuad(rect, index, ox, oy);
		}
	}

	// Finally it's time to render the vertices.
	myRenderQuads(0, index*6, 0);
}

// ===================================================================================
// GxText highlight rendering

void GxTextRenderer::myDrawHighlight(float x, float y)
{
	const int lineCount = (int)myLines.size();
	myResizeBuffers(lineCount * 2);

	// Fill in the vertex data by rendering out the lines of rects.	
	Glyph rect = { GxAreaf(0,(float)-myFont->fontSize,0,0), GxAreaf(), 0, 0, 0, 0 };
	myResetPos();
	int index = 0;
	for(int l=0; l<lineCount; ++l)
	{
		const Line& line = myLines[l];
		Pen pen = { 0, 0, line.spacemul };
		const float ox = x + GetOffsetX(myAlignH, line.width);
		const float oy = y + (float)((l+1) * myFont->fontSize);

		// Find the x-position of the start and end of the highlight rectangle.
		bool started = false;
		mySkipTo(line.begin);
		while(myReadPos < line.end)
		{
			const Glyph& glyph = myNextChar();
			myAdvance(pen, glyph);
			if(myInRange())
			{
				if(!started)
					rect.coords.l = pen.x, started = true;
				rect.coords.r = pen.x + pen.advance;
			}
		}

		// Emit a quad for the highlight rectangle.
		if(started) myEmitQuad(rect, index, ox, oy);
	}

	// Finally it's time to render the vertices.
	myRenderQuads(0, index*6, 0);
}

// ===================================================================================
// Character rectangle fetching

GxRecti GxTextRenderer::myGetCharRect(float x, float y, int charIndex)
{
	// Empty string.
	if(myLines.empty())
		return GxRecti((int)x, (int)y, 0, myFont->fontSize);

	// Find the correct line number.
	int lineIndex = 0;
	for(int l=0; l<(int)myLines.size() && myLines[l].begin <= charIndex; ++l)
		lineIndex = l;

	// Process the line on which the character resides.
	const Line& line = myLines[lineIndex];
	const float ox = x + GetOffsetX(myAlignH, line.width);
	const float oy = y + (float)(lineIndex * myFont->fontSize);

	// If the index is before begin, we can return the position of the first character.
	if(charIndex < line.begin)
		return GxRecti((int)ox, (int)oy, 0, myFont->fontSize);

	// Go through the line until we reach the character at the index.
	Pen pen = { 0, 0, line.spacemul };
	myResetPos();
	mySkipTo(line.begin);
	while(myReadPos < line.end)
	{
		const Glyph& glyph = myNextChar();
		myAdvance(pen, glyph);
		if(myPos == charIndex)
			return GxRecti((int)(ox + pen.x), (int)oy, (int)pen.advance, myFont->fontSize);
	}

	// If the final line ends with a line feed, we return the start of the next line.
	if(myReadPos == myLen && myStr[myPos] == C_LINE_FEED)
		return GxRecti((int)ox, (int)oy + myFont->fontSize, 0, myFont->fontSize);

	// If not, we just return the end of the final line.
	return GxRecti((int)(ox + pen.x + pen.advance), (int)oy, 0, myFont->fontSize);
}

// ===================================================================================
// Character index fetching

int GxTextRenderer::myGetCharIndex(int x, int y, int cx, int cy)
{
	// Check if the character is before the start or after the end of the text.
	if(cy <= y || myLines.empty()) return 0;
	if(cy >= y + myTextH) return myLen;

	// Find the correct line number.
	int lineIndex = 0;
	for(int l=0, ly=y; l<(int)myLines.size() && cy>=ly; ly += myFont->fontSize, ++l)
		lineIndex = l;

	// Process the line on which the character resides.
	const Line& line = myLines[lineIndex];
	const float ox = (float)x + GetOffsetX(myAlignH, line.width);
	const float px = (float)cx;

	// Go through each character.
	Pen pen = { 0, 0, line.spacemul };
	myResetPos();
	mySkipTo(line.begin);
	while(myReadPos < line.end)
	{
		const Glyph& glyph = myNextChar();
		myAdvance(pen, glyph);
		if(px < ox + pen.x + pen.advance*0.5f) return myPos;
	}

	// Only the last line is allowed to return one beyond the last character.
	if(myReadPos == myLen && myStr[myPos] != C_LINE_FEED) return myLen;

	// Otherwise, just return the last character.
	return myPos;
}

// ===================================================================================
// GxText

const int GxText::npos()
{
	return _npos;
}

GxText::GxText()
	:shadow     (0, 0, 0, 0)
	,top        (255, 255, 255, 255)
	,bottom     (255, 255, 255, 255)
	,flags      (GX_TF_WORD_WRAP | GX_TF_FORMATTING)
	,maxWidth   (-1)
	,rangeBegin (0)
	,rangeEnd   (_npos)
	,tabWidth   (96)
	,alignH     (GX_TA_LEFT)
	,alignV     (GX_TA_TOP)
{
}

GxRecti GxText::Draw(int x, int y, GxStringArg text) const
{
	GxTextRenderer* renderer = GxTextRenderer::singleton;
	if(!renderer) return GxRecti(x, y, 0, 0);
	renderer->SetText(*this, text.ptr);
	return renderer->DrawText(x, y);
}

GxRecti GxText::DrawHighlight(int x, int y, GxStringArg text) const
{
	GxTextRenderer* renderer = GxTextRenderer::singleton;
	if(!renderer) return GxRecti(x, y, 0, 0);
	renderer->SetText(*this, text.ptr);
	return renderer->DrawHighlight(x, y);	
}

GxRecti GxText::GetTextRect(int x, int y, GxStringArg text) const
{
	GxTextRenderer* renderer = GxTextRenderer::singleton;
	if(!renderer) return GxRecti(x, y, 0, 0);
	renderer->SetText(*this, text.ptr);
	return renderer->GetTextRect(x, y);
}

GxRecti GxText::GetCharRect(int x, int y, int charIndex, GxStringArg text) const
{
	GxTextRenderer* renderer = GxTextRenderer::singleton;
	if(!renderer) return GxRecti(x, y, 0, 0);
	renderer->SetText(*this, text.ptr);
	return renderer->GetCharRect(x, y, charIndex);	
}

int GxText::GetCharIndex(int x, int y, int cx, int cy, GxStringArg text) const
{
	GxTextRenderer* renderer = GxTextRenderer::singleton;
	if(!renderer) return 0;
	renderer->SetText(*this, text.ptr);
	return renderer->GetCharIndex(x, y, cx, cy);	
}

void GxText::SetAlign(GxTextAlignH h, GxTextAlignV v)
{
	alignH = h;
	alignV = v;
}

void GxText::SetColor(GxColor c)
{
	top = bottom = c;
}

void GxText::SetMaxWidth(int w)
{
	maxWidth = w;
}

void GxText::SetRange(int begin, int end)
{
	rangeBegin = begin;
	rangeEnd = end;
}

void GxText::SetFlag(GxTextFlags f, bool b)
{
	if(b) flags |= f; else flags &= ~f;
}

bool GxText::HasFlag(GxTextFlags f) const
{
	return (flags & f) == f;
}

}; // namespace graphics
}; // namespace guix