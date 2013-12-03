#include <math.h>

#include <algorithm>
#include <vector>
#include <string>
#include <set>
#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H
#include <freetype/ftglyph.h>

#include <Shared/Rasterizer.h>

using namespace builder;

namespace builder {
namespace {

static const int sQBoxW = 8;
static const int sQBoxH = 11;

static uint sQBoxBitmap[(sQBoxW/4) * sQBoxH] = 
{
	0xFFFFC92D, 0x1BBAFFFF, 0x7FCFFFC6, 0xA5FFAF5F,
	0x8F0FEFF0, 0xD2BF0FAF, 0xFF00BFF0, 0xD29F1FFF,
	0xFFFFFFF0, 0xD2FF2F5F, 0x9FFFFFF0, 0xD2FFEF2F,
	0x7FFFFFF0, 0xD2FFFF7F, 0xDFFFFFF0, 0xD2FFFFDF,
	0x7FFFFFF0, 0xD2FFFF3F, 0xDFFFFFF0, 0xD2FFFFCF,
	0xFFFFC000, 0x00B1FCFF
};

inline int SizeAlign(int size)
{
	return (size + 3) & (~3);
}

}; // anonymous namespace

// ===========================================================
// RasterizerEntry
// ===========================================================

RasterizerEntry::~RasterizerEntry()
{
}

RasterizerEntry::RasterizerEntry()
	:fontSize(12)
	,hintMode(RH_NORMAL)
	,smallCaps(false)
	,italic(false)
	,bold(false)
{
}

RasterizerPage::RasterizerPage()
	:glyphPadding(0)
	,glyphAdvance(0)
	,textureWidth(0)
	,renderFallback(true)
	,kerningPairs(true)
{
}

RasterizerPage::~RasterizerPage()
{
}

// ===========================================================
// RenderData
// ===========================================================

struct RenderData
{
	RenderData();
	~RenderData();

	bool LoadFace(const char* path);
	void Release();

	FT_Library lib;
	FT_Face face;
};

RenderData::RenderData()
{
	FT_Init_FreeType(&lib);
	face = NULL;
}

RenderData::~RenderData()
{
	if(face) FT_Done_Face(face);
	if(lib) FT_Done_FreeType(lib);
}

bool RenderData::LoadFace(const char* path)
{
	if(path == NULL)
		return false;

	FT_Face newFace;
	if(FT_New_Face(lib, path, 0, &newFace) == 0)
	{
		if(face) 
			FT_Done_Face(face);
		face = newFace;
		return true;
	}
	return false;
}

// ===========================================================
// Rasterizer::Data
// ===========================================================

GlyphStyle ToStyle(bool bold, bool italic)
{
	return italic
		? (bold ? GS_BOLD_ITALIC : GS_ITALIC)
		: (bold ? GS_BOLD        : GS_NORMAL);
}

struct TempGlyph
{
	GlyphStyle style;
	int codepoint;
	int advance;
    int left, top;
    int x, y, width, height;
	int sIndex;
    uchar* buffer;
};

typedef std::vector<TempGlyph> GlyphTable;

struct HeightCompare 
{
	const TempGlyph* data;
	bool operator ()(int a, int b) const
	{
		return data[a].height > data[b].height;
	}
};

class Rasterizer::Data
{
public:
	RasterizerResult Render(const RasterizerPage& input, RasterizerOutput& output);
};

static void BuildKerningList(const RasterizerPage& input, const RenderData& data, int first, const GlyphTable& glyphs, KerningList& out)
{
	if(glyphs.empty()) return;

	const int end = (int)glyphs.size();
	const int count = end - first;
	const TempGlyph* g = &glyphs[0];

	for(int i=first; i<end && g[i].codepoint < 128; ++i)
	for(int j=first; j<end && g[j].codepoint < 128; ++j)
	{
		if(isalnum(g[i].codepoint) && isalnum(g[j].codepoint))
		{
			FT_Vector v;
			int l = FT_Get_Char_Index(data.face, g[i].codepoint);
			int r = FT_Get_Char_Index(data.face, g[j].codepoint);
			FT_Get_Kerning(data.face, l, r, FT_KERNING_DEFAULT, &v);
			if(v.x != 0)
			{
				char a = (char)g[i].codepoint;
				char b = (char)g[j].codepoint;
				KerningPair kp = {i, j, v.x >> 6};
				out.push_back(kp);
			}
		}
	}
}

static void BuildGlyphTable(const RasterizerPage& input, GlyphTable& out, KerningList& kern)
{
	RenderData data;
	bool fallbackIncluded = false;
	for(size_t i=0; i<input.entries.size(); ++i)
	{
		int first = (int)out.size();

		const RasterizerEntry& entry = input.entries[i];

		// Safety check.
		if(entry.fontSize <= 0)
			continue;

		// Try to load the font face.
		if(!data.LoadFace(entry.fontPath.c_str()))
			continue;

		// Set the font sizes.
		FT_Error error;
		error = FT_Set_Pixel_Sizes(data.face, entry.fontSize, entry.fontSize);
		if(error)
			continue;

		// Select the unicode character map.
		error = FT_Select_Charmap(data.face, FT_ENCODING_UNICODE);
		if(error)
			continue;

		// Include fallback glyph.
		if(input.renderFallback && !fallbackIncluded)
		{
			int advance  = Max(sQBoxW, entry.fontSize*3/4);

			TempGlyph glyph;
			glyph.codepoint = 0;
			glyph.advance = advance + input.glyphAdvance + 1;
			glyph.left    = advance/2 - sQBoxW/2;
			glyph.top     = Max(sQBoxH, entry.fontSize/2 + sQBoxH/2);
			glyph.x       = 0;
			glyph.y       = 0;
			glyph.width   = sQBoxW;
			glyph.height  = sQBoxH;
			glyph.buffer  = (uchar*)sQBoxBitmap;
			glyph.style   = GS_NORMAL;
			glyph.sIndex  = 0;

			out.push_back(glyph);

			fallbackIncluded = true;
		}

		// Create a charset.
		std::set<codepoint32> charset;
		for(size_t j=0; j<entry.presets.size(); ++j)
		{
			const codepoint32* str = entry.presets[j].chars.c_str();
			while(*str) charset.insert(*str++);
		}
		for(size_t j=0; j<entry.ranges.size(); ++j)
		{
			codepoint32 first = entry.ranges[j].first;
			codepoint32 last = entry.ranges[j].last;
			while(first <= last) charset.insert(first++);
		}

		// Set the correct hinting mode for rasterization.
		FT_Int32       loadflags  = FT_LOAD_TARGET_LIGHT;
		FT_Render_Mode rendermode = FT_RENDER_MODE_LIGHT;
		switch(entry.hintMode)
		{
			case RH_NORMAL: loadflags = FT_LOAD_TARGET_NORMAL; break;
			case RH_LIGHT:  loadflags = FT_LOAD_TARGET_LIGHT; break;
			case RH_MONO:   loadflags = FT_LOAD_TARGET_MONO; break;
		};

		// Render all the characters in the charset.
		bool usingSmallCaps = false;
		std::set<codepoint32>::const_iterator it;
		for(it = charset.begin(); it != charset.end(); ++it)
		{
			int c = (int)(*it);

			// Check if the character should be rendered as small capitals.
			if(entry.smallCaps && c < 128 && islower(c))
			{
				c = toupper(c);
				if(!usingSmallCaps)
				{
					usingSmallCaps = true;
					int smallSize = entry.fontSize * 8 / 10;
					FT_Set_Pixel_Sizes(data.face, smallSize, smallSize);
				}
			}
			// Set the pixel sizes back to normal.
			else if(usingSmallCaps)
			{
				usingSmallCaps = false;
				FT_Set_Pixel_Sizes(data.face, entry.fontSize, entry.fontSize);
			}

			// Load the glyph corresponding to the current character.
			int glyphIndex = FT_Get_Char_Index(data.face, c);
			error = FT_Load_Glyph(data.face, glyphIndex, loadflags);
			if(glyphIndex == 0 || error)
				continue;

			// Fetch the glyph from the font face.
			FT_Glyph ftGlyph;
			error = FT_Get_Glyph(data.face->glyph, &ftGlyph);
			if(error)
				continue;

			// Convert the glyph to a bitmap (ie. rasterize it).
			FT_Glyph_To_Bitmap(&ftGlyph, rendermode, 0, 1);
			FT_BitmapGlyph bitmapGlyph = (FT_BitmapGlyph)ftGlyph;
			if(!bitmapGlyph)
				continue;

			// Copy the bitmap glyph and store it in the glyph table.
			TempGlyph glyph;

			glyph.codepoint = c;
			glyph.advance = (bitmapGlyph->root.advance.x >> 16) + input.glyphAdvance;
			glyph.left    = bitmapGlyph->left;
			glyph.top     = bitmapGlyph->top;
			glyph.x       = 0;
			glyph.y       = 0;
			glyph.width   = bitmapGlyph->bitmap.width;
			glyph.height  = bitmapGlyph->bitmap.rows;
			glyph.buffer  = NULL;
			glyph.style   = ToStyle(entry.bold, entry.italic);
			glyph.sIndex  = 0;

			if(glyph.width > 0 && glyph.height > 0)
			{
				glyph.buffer = AlignedAlloc<uchar>(glyph.width * glyph.height, 8);
				for(int y=0; y<glyph.height; ++y)
				{
					memcpy(glyph.buffer + y*glyph.width,
						bitmapGlyph->bitmap.buffer + y*bitmapGlyph->bitmap.pitch,
						glyph.width);
				}
			}

			out.push_back(glyph);
			FT_Done_Glyph((FT_Glyph)bitmapGlyph);
		}

		// Build a list of kerning pairs with the current font.
		if(input.kerningPairs && !entry.smallCaps)
			BuildKerningList(input, data, first, out, kern);
	}
}

RasterizerResult Rasterizer::Data::Render(const RasterizerPage& input, RasterizerOutput& output)
{
	GlyphTable table;
	KerningList kerning;
	BuildGlyphTable(input, table, kerning);

	// Check if there are any rendered characters.
	if(table.empty()) return RR_NO_RENDERED_GLYPHS;

	// Calculate the total glyph area to make an estimate for a square-ish texture.
	const int pad = input.glyphPadding;
	int left = 0;
	int top  = 0;
	int texW = input.textureWidth;
	int texH = 16;
	if(texW <= 0)
	{
		int area = 0;
		GlyphTable::iterator it;
		for(it = table.begin(); it != table.end(); ++it)
			area += (it->width + pad*2) * (it->height + pad*2);
		texW = (int)(sqrt((double)Max(0, area)) * 1.2);
	}
	texW = Max(16, Min(SizeAlign(texW), 2048));

	// Sort glyphs according to height.
	std::vector<int> hsort(table.size());
	for(size_t i=0; i<table.size(); ++i) hsort[i] = i;
	HeightCompare hcmp = { &table[0] };
	std::stable_sort(hsort.begin(), hsort.end(), hcmp);

	// Assign the glyphs a position on the texture.
	std::vector<int> tops(texW, 0);
	for(size_t i=0; i<hsort.size(); ++i)
	{
		TempGlyph& g = table[hsort[i]];

		if(g.width <= 0 || g.height <= 0) continue;

		int rectW = g.width + pad*2;
		int rectH = g.height + pad*2;

		// Make sure the texture is wide enough for the character.
		if(texW < rectW) texW = rectW;

		// Make sure we don't go over the texture width.
		if(left + rectW > texW) left = 0;

		// Make sure the top array is big enough.
		if((int)tops.size() < texW)
			tops.resize(texW, 0);

		// Compute the top coordinate.
		top = 0;
		if(rectW > 0)
		{
			top = tops[left];
			for(int x=1; x<rectW; ++x)
				top = Max(top, tops[left + x]);
		}

		// Make sure we don't go over the texture height.
		if(top + rectH > texH) texH = top + rectH;

		// Update the top coordinates.
		for(int x=0; x<rectW; ++x)
			tops[left + x] = top + rectH;	

		// Store glyph position.
		g.x = left + pad;
		g.y = top + pad;

		// Advance left.
		left += rectW;	
	}

	// Create the glyph page bitmap.
	output.page.width  = texW;
	output.page.height = texH;
	output.page.bitmap = AlignedAlloc<uchar>(texW * texH * 4, 8);
	memset(output.page.bitmap, 0, texW * texH * 4);

	// Render the glyphs on the bitmap.
	for(GlyphTable::iterator it = table.begin(); it != table.end(); ++it)
	{
		// Store the glyph properties.
		Glyph glyph;

		glyph.bitmapRegion.x = it->x;
		glyph.bitmapRegion.y = it->y;
		glyph.bitmapRegion.w = it->width;
		glyph.bitmapRegion.h = it->height;
		glyph.drawCoords.x   = it->left - pad;
		glyph.drawCoords.y   = -it->top - pad;
		glyph.advance        = it->advance;
		glyph.codepoint      = it->codepoint;

		it->sIndex = output.glyphs[it->style].size();
		output.glyphs[it->style].push_back(glyph);

		// Draw the glyph on the glyph page bitmap.
		const uchar* src = it->buffer;
		uchar* dst = output.page.bitmap + it->y*texW*4 + it->x*4;
		for(int y=0; y<it->height; ++y)
		{
			for(int x=0; x<it->width; ++x)
			{
				uchar* pixel = dst + x*4;
				*pixel++ = 0xFF;
				*pixel++ = 0xFF;
				*pixel++ = 0xFF;
				*pixel++ = *src++;
			}
			dst += texW*4;
		}
	}

	// Translate the kerning indices.
	for(size_t i=0; i<kerning.size(); ++i)
	{
		const TempGlyph& l = table[kerning[i].left];
		const TempGlyph& r = table[kerning[i].right];
		if(l.style == r.style)
		{
			KerningPair kp = {l.sIndex, r.sIndex, kerning[i].delta};
			output.kerning[l.style].push_back(kp);
		}
	}

	// Cleanup, delete glyph bitmaps.
	for(GlyphTable::iterator it = table.begin(); it != table.end(); ++it)
		if(it->buffer != (uchar*)sQBoxBitmap)
			AlignedFree(it->buffer);

	return RR_SUCCESS;
}

// ===================================================================================
// FontLoader
// ===================================================================================

Rasterizer::Rasterizer()
{
	myData = new Rasterizer::Data();
}

Rasterizer::~Rasterizer()
{
	delete myData;
}

RasterizerResult Rasterizer::Render(const RasterizerPage& input, RasterizerOutput& output)
{
	return myData->Render(input, output);
}

}; // namespace builder