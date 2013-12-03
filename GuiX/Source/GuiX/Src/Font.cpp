#include <GuiX/Config.h>
#include <GuiX/Interfaces.h>
#include <GuiX/Resources.h>

#include <Src/TextImp.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400
#pragma warning(disable: 4996) // fopen and sscanf warnings.
#endif

namespace guix {
namespace graphics {
namespace {

static const char* LOG_TAG      = "FontDB";

// ===================================================================================
// Utility types, structs and enums

// Contain glyph traits for ascii characters; all non-ascii characters have traits 0.
static const uchar asciiTraits[128] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0,
	0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0,
	0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0,
};

// Several constants related to the fallback font and tag bitmaps.
enum fallbackDimensions
{
	FB_CHAR_W     = 5,
	FB_CHAR_H     = 9,
	FB_CHAR_IN_W  = 12,
	FB_CHAR_IN_H  = 8,
	FB_CHAR_COUNT = FB_CHAR_IN_W * FB_CHAR_IN_H,
	FB_CHAR_BITS  = (FB_CHAR_COUNT * FB_CHAR_W * FB_CHAR_H) / 32,

	FB_TAG_W      = 16,
	FB_TAG_H      = 8,
	FB_TAG_BITS   = (FB_TAG_W * FB_TAG_H) / 32,
};

// Contains a bitmap with the fallback font glyphs.
// The image is 60 x 72 pixels, and each bit corresponds to one pixel.
static const uint charBitmap[FB_CHAR_BITS] =
{
	0xFEFF3DC0, 0xF8B9D1F7, 0x84310C63, 0x318A0910, 0x084310C6, 0x6374A091, 0x1084310C, 0xBFFB2A09,
	0x9FE9DF10, 0x0C63F1A0, 0x09188431, 0x10C63B2A, 0xA0918843, 0x310C63F4, 0x8A091884, 0x7EFF3E20,
	0x189DD170, 0x3DCF74E2, 0x218C63FF, 0x0C6318CE, 0x7618C624, 0x40C6318D, 0xD761AC62, 0x240C6318,
	0x8E6A1AC6, 0x62473E2F, 0x18E6A1AC, 0x2A24846A, 0xA18C6215, 0x52A24846, 0x7218C621, 0xF5122484,
	0xC7C17463, 0x31513C47, 0x0400107E, 0x23120400, 0xC0400104, 0x422A0041, 0xC2779CF7, 0x8222A335,
	0xC228C631, 0x1813C424, 0x44228C43, 0x31F0A0A2, 0x2472FFC4, 0x4318860A, 0x1246220C, 0xC6318861,
	0xD1246220, 0x779CFF7D, 0x828745C2, 0x00000001, 0x10200080, 0x00000000, 0x5922C008, 0xFCD73DCF,
	0x1A91288B, 0x88338C63, 0x31A90A88, 0x888218C6, 0xE31A9068, 0x8889C18B, 0x8631890A, 0x2988A01A,
	0xC8631891, 0xA26C8A01, 0x1B05D18B, 0x0000B71E, 0x2F710400, 0x00000F7C, 0x53089880, 0xF8C63108,
	0x85308141,	0x108C6310, 0x08530410, 0x0088AAB1, 0xA7BFEE21, 0x1004F12A, 0xAA8C1101, 0x0900282A,
	0x6AA8C110, 0x00900184, 0x45448C11, 0x0FFFC1F7, 0x739DF73D, 0x94E20000, 0x08C63007, 0x95512000,
	0x008C6308, 0x89551200, 0xBE0CC630, 0x8417FD20, 0x541FAF9C, 0x24239552, 0x223E09C2, 0x224153F5,
	0xD200008C, 0xC2248D14, 0x41000008, 0x739C48D1, 0xD4E20000, 0x24004403, 0x19866000, 0x04200AA0,
	0x01084210, 0x008112A0, 0x02108421, 0x1008112A, 0xC0210842, 0x27C0817E, 0x92002044, 0x42100813,
	0x45200108, 0x84210081, 0x20320010, 0x08421004, 0x2402C021, 0x1986603E, 0x04500441, 0x03800400,
	0x00450044, 0x00444840, 0x00040104, 0x80040844, 0x20004010, 0x08004102, 0x12000400, 0x00800220,
	0x02200040, 0x01100011, 0x08410004, 0x40110001, 0x00481008, 0x44001000, 0x01000110,
};

// Contains the codepoint values for each glyph in hte fallback font.
// Each character corresponds to a 5 x 9 pixel regio in the fallback font bitmap.
static const uint charCharset[FB_CHAR_COUNT] =
{
	 0 ,'A','B','C','D','E','F','G','H','I','J','K',
	'L','M','N','O','P','Q','R','S','T','U','V','W',
	'X','Y','Z','a','b','c','d','e','f','g','h','i',
	'j','k','l','m','n','o','p','q','r','s','t','u',
	'v','w','x','y','z','`','1','2','3','4','5','6',
	'7','8','9','0','-','=','~','!','@','#','$','%',
	'^','&','*','(',')','_','+','[',']','{','}',';',
  '\'','\\',':','"','|',',','.','/','<','>','?',' ',
};

// Contains the bitmap with the fallback tag image.
// The image is 16 x 8 pixels, and each bit corresponds to one pixel.
static const uint tagBitmap[FB_TAG_BITS] =
{
	0x8001ffff, 0xbffdbffd, 0xbffdbffd, 0xffff8001
};

// Contains up to four glyph indices: normal, bold, italic and bold-italic.
struct GlyphIndices
{
	GlyphIndices() { indices[0] = indices[1] = indices[2] = indices[3] = 0; }
	int indices[4];
};


// ===================================================================================
// Utility functions

template <typename T>
T* Malloc(size_t count)
{
	T* mem = GxMalloc<T>(count);
	memset(mem, 0, count * sizeof(T));
	return mem;
}

// Creates a texture from an RGBA bitmap.
static bool CreateTexture(GxTextureHandle& handle, int width, int height, const uchar* data)
{
	GxRenderInterface* renderer = GxRenderInterface::Get();
	return renderer->GenerateTexture(handle, width, height, data);
}

// Creates a texture by loading an image file.
static bool LoadTexture(GxTextureHandle& handle, int& width, int& height, const char* path)
{
	GxRenderInterface* renderer = GxRenderInterface::Get();
	return renderer->LoadTexture(handle, width, height, path);
}

// Deletes an OpenGL texture.
static void DeleteTexture(GxTextureHandle handle)
{
	GxRenderInterface* renderer = GxRenderInterface::Get();
	renderer->ReleaseTexture(handle);
}

// Returns true if the line starts with the given word.
static bool StartsWith(const char* l, const char* w)
{
	while(*l==' ' || *l=='\t') ++l;
	while(*w && *l==*w) ++l, ++w;
	return (*w==0 && *l==' ' || *l=='\t');
}

// Sorts arrays of kerning keys/values according to key.
static void SortKerningMap(uint* key, float* val, int first, int last)
{
	uint k = key[(first + last) >> 1];
	int i = first-1, j = last+1;
	do
	{
		do {j--;} while(k < key[j]);
		do {i++;} while(k > key[i]);
		if(i <= j)
		{
			GxSwap(key[i], key[j]);
			GxSwap(val[i], val[j]);
		}
	}
	while(i < j);
	if(j > first) SortKerningMap(key, val, first, j);
	if(last > j+1) SortKerningMap(key, val, j+1, last);
}

}; // anonymous namespace

// ===================================================================================
// GxFontData

GxFontData::GxFontData()
	:bmpGlyphMap(NULL)
	,mapBegin(0)
	,mapEnd(0)
	,smpGlyphKey(NULL)
	,smpGlyphVal(NULL)
	,smpGlyphCount(0)
	,kernKey(NULL)
	,kernVal(NULL)
	,kernCount(0)
	,glyphs(NULL)
	,glyphCount(0)
	,glyphPages(NULL)
	,glyphPageCount(0)
	,fontSize(DEFAULT_FONT_SIZE)
	,ellipsisW(0)
	,myStyle(0)
	,myStyleIndex(0)
	,myStyleCount(0)
{
}

GxFontData::~GxFontData()
{
	Destroy();
}

void GxFontData::Destroy()
{
	// BMP codepoint to glyph mapping.
	GxFree(bmpGlyphMap);
	bmpGlyphMap = NULL;
	mapBegin = 0;
	mapEnd = 0;

	// SMP codepoint to glyph mapping.
	GxFree(smpGlyphKey);
	GxFree(smpGlyphVal);
	smpGlyphKey = NULL;
	smpGlyphVal = NULL;
	smpGlyphCount = 0;

	// Kerning data.
	GxFree(kernKey);
	GxFree(kernVal);
	kernKey = NULL;
	kernVal = NULL;
	kernCount = 0;

	// Glyph data.
	GxFree(glyphs);
	glyphs = NULL;
	glyphCount = 0;

	// Style data.
	myStyleCount = 0;
	myStyle = 0;
	myStyleIndex = 0;

	// Release glyph page textures.
	for(int i=0; i<glyphPageCount; ++i)
		DeleteTexture(glyphPages[i].texture);

	// GlyphPage data.
	GxFree(glyphPages);
	glyphPages = NULL;
	glyphPageCount = 0;

	// Font information.
	fontSize = DEFAULT_FONT_SIZE;
	ellipsisW = 0;
}

// Creates a font from a TidyText font file.
bool GxFontData::Create(const char* path, const char* dir)
{
	Destroy();

	// Parse file contents into the buffer.
	FILE* fp = fopen(path, "rb");
	if(!fp) return false;
	char buf[256];
	std::vector<std::string> lines(1);
	for(size_t i; i=fread(buf, 1, 256, fp);)
	{
		for(size_t j=0; j<i; ++j)
		{
			if(buf[j] == C_LINE_FEED)
				lines.push_back(std::string());
			else if(buf[j] != C_CARRIAGE_RET)
				lines.back().push_back(buf[j]);
		}
	}
	fclose(fp);

	// Count the number of pages, glyphs and kerning pairs.
	for(size_t i=0; i<lines.size(); ++i)
	{
		const char* line = lines[i].c_str();
		     if(StartsWith(line, "g"   )) ++glyphCount;
		else if(StartsWith(line, "k"   )) ++kernCount;
		else if(StartsWith(line, "page")) ++glyphPageCount;
		else if(StartsWith(line, "size")) sscanf(line, "%*s %i", &fontSize);
	}

	// Make sure the file contains glyphs.
	if(glyphCount == 0 || glyphPageCount == 0)
	{
		Destroy();
		return false;
	}

	// Allocate pages, glyphs and kerning pairs.
	glyphs = Malloc<Glyph>(glyphCount);
	glyphPages = Malloc<GlyphPage>(glyphPageCount);
	if(kernCount > 0)
	{
		kernKey = Malloc<uint>(kernCount);
		kernVal = Malloc<float>(kernCount);
	}

	// Fill in page and glyph data.
	myStyleCount = 1;
	int currentStyle = GT_NORMAL;
	float rw = 1.f, rh = 1.f;
	int page = -1, gl = 0, kp = 0;
	for(size_t i=0; i<lines.size(); ++i)
	{
		const char* line = lines[i].c_str();

		// Read glyph data.
		if(StartsWith(line, "g") && page >= 0)
		{
			int a, b, c, d, e, f, g, h;
			if(sscanf(line, "%*s %i %i %i %i %i %i %i %i", &a,&b,&c,&d,&e,&f,&g,&h) == 8)
			{
				glyphs[gl].coords    = GxAreaf((float)g, (float)h, (float)(g+e), (float)(h+f));
				glyphs[gl].uvs       = GxAreaf(rw*(float)c, rh*(float)d, rw*(float)(c+e), rh*(float)(d+f));
				glyphs[gl].traits    = currentStyle;
				glyphs[gl].xAdvance  = (float)b;
				glyphs[gl].codepoint = a;
				glyphs[gl].page      = page;

				if(a < 128) glyphs[gl].traits |= asciiTraits[a] << 2;

				++gl;
			}
		}
		// Read style data.
		else if(StartsWith(line, "style") && page >= 0)
		{
			bool sb = (lines[i].find("bold") != std::string::npos);
			bool si = (lines[i].find("italic") != std::string::npos);

			if(sb && si) currentStyle = GT_BOLD_ITALIC;
			else if(sb)  currentStyle = GT_BOLD;
			else if(si)  currentStyle = GT_ITALIC;

			myStyleCount = GxMax(myStyleCount, currentStyle + 1);
		}
		// Read kerning pair data.
		else if(StartsWith(line, "k"))
		{
			int a, b, c;
			if(sscanf(line, "%*s %i %i %i", &a,&b,&c) == 3)
			{
				kernKey[kp] = (GxClamp(a,0,0xFFFF) << 16) | GxClamp(b,0,0xFFFF);
				kernVal[kp] = (float)c;
				++kp;
			}
		}
		// Read glyph page data.
		else if(StartsWith(line, "page"))
		{
			++page;
			size_t ql = lines[i].find_first_of("'\"");
			size_t qr = lines[i].find_last_of("'\"");
			if(ql < qr)
			{
				// Load the image from the path between the quotation marks.
				int imgW = 0, imgH = 0;
				std::string imgpath = dir + lines[i].substr(ql+1, qr-ql-1);
				bool result = LoadTexture(glyphPages[page].texture, imgW, imgH, imgpath.c_str());
				rw = (float)(1.0 / GxMax((double)imgW, 1.0));
				rh = (float)(1.0 / GxMax((double)imgH, 1.0));
			}
		}
	}

	myFinalize();
	return true;
}

// Creates a font from a RGBA bitmap and character set.
bool GxFontData::Create(const uchar* rgba, int glyphW, int glyphH, int glyphsInW, int glyphsInH, int _fontSize, const uint* charset)
{
	Destroy();

	fontSize = _fontSize;
	glyphCount = glyphsInW * glyphsInH;
	glyphPageCount = 1;
	myStyleCount = 1;

	// Allocate page and glyph data.
	glyphs = Malloc<Glyph>(glyphCount);
	glyphPages = Malloc<GlyphPage>(glyphPageCount);

	// Fill in glyph data.
	int bitmapW = glyphsInW * glyphW;
	int bitmapH = glyphsInH * glyphH;

	float rw = (float)(1.0 / (double)bitmapW);
	float rh = (float)(1.0 / (double)bitmapH);
	float gw = (float)glyphW;
	float gh = (float)glyphH;

	for(int y=0, gl=0; y<bitmapH; y+=glyphH)
	for(int x=0; x<bitmapW; x+=glyphW)
	{
		int c = charset[gl];

		glyphs[gl].coords    = GxAreaf(0, -gh, gw, 0);
		glyphs[gl].uvs       = GxAreaf((float)x*rw, (float)y*rh, ((float)x+gw)*rw, ((float)y+gh)*rh);
		glyphs[gl].xAdvance  = gw + 1;
		glyphs[gl].traits    = 0;
		glyphs[gl].codepoint = c;
		glyphs[gl].page      = 0;
		
		if(c < 128) glyphs[gl].traits |= asciiTraits[c] << 2;

		++gl;
	}

	// Create texture.
	CreateTexture(glyphPages[0].texture, bitmapW, bitmapH, rgba);

	myFinalize();
	return true;
}

// If enabled, the GetGlyphIndex and GetGlyph functions will return bold glyphs.
void GxFontData::SetBold(bool enabled) const
{
	myStyle = enabled ? (myStyle | GT_BOLD) : (myStyle & ~GT_BOLD);
	myStyleIndex = GxMin(myStyle, myStyleCount - 1);
}

// If enabled, the GetGlyphIndex and GetGlyph functions will return italic glyphs.
void GxFontData::SetItalic(bool enabled) const
{
	myStyle = enabled ? (myStyle | GT_ITALIC) : (myStyle & ~GT_ITALIC);
	myStyleIndex = GxMin(myStyle, myStyleCount - 1);
}

// Returns the additional advance between left and right based on kerning pairs.
float GxFontData::GetKerning(int l, int r) const
{
	if(kernCount > 0 && l < 0xFFFF && r < 0xFFFF)
	{
		uint cmp = (l << 16) | r;
		int lower = 0;
		int upper = kernCount;
		int pos = (lower + upper) >> 1;
		while(lower <= upper)
		{
			uint key = kernKey[pos];
			     if(key > cmp) upper = pos - 1;
			else if(key < cmp) lower = pos + 1;
			else return kernVal[pos];
			pos = (lower + upper) >> 1;
		}
	}
	return 0;
}

// Does a binary search in the SMP codepoint map.
int GxFontData::myGetSMPIndex(uint codepoint) const
{
	if(smpGlyphCount > 0)
	{
		int lower = 0;
		int upper = smpGlyphCount;
		int pos = (lower + upper) >> 1;
		while(lower <= upper)
		{
			uint key = smpGlyphKey[pos];
			     if(key > codepoint) upper = pos - 1;
			else if(key < codepoint) lower = pos + 1;
			else return smpGlyphVal[pos * myStyleCount + myStyleIndex];
			pos = (lower + upper) >> 1;
		}
	}
	return 0;
}

// ===================================================================================
// Charmap generation

// Helper function used by GxFontData::mySetSlot, assigns one of the style indices
// to the map slot, giving priority to styles in the order given by a, b, c, d.
static void SetSlot4(int* map, const int* s, int a, int b, int c, int d)
{
	     if(s[a]) map[a] = s[a];
	else if(s[b]) map[a] = s[b];
	else if(s[c]) map[a] = s[c];
	else if(s[d]) map[a] = s[d];
}

// Fills in glyph indices in a glyph map based on the available styles.
void GxFontData::mySetSlot(int* map, int pos, const int* indices)
{
	map += pos * myStyleCount;

	if(myStyleCount > GT_NORMAL)
		SetSlot4(map, indices, GT_NORMAL, GT_BOLD, GT_ITALIC, GT_BOLD_ITALIC);

	if(myStyleCount > GT_BOLD)
		SetSlot4(map, indices, GT_BOLD, GT_BOLD_ITALIC, GT_NORMAL, GT_ITALIC);

	if(myStyleCount > GT_ITALIC)
		SetSlot4(map, indices, GT_ITALIC, GT_BOLD_ITALIC, GT_NORMAL, GT_BOLD);

	if(myStyleCount > GT_BOLD_ITALIC)
		SetSlot4(map, indices, GT_BOLD_ITALIC, GT_BOLD, GT_ITALIC, GT_NORMAL);
}

// Inserts a glyph at the end of the glyph array if it is not already available.
void GxFontData::myInsertBlankGlyph(uint codepoint, int traits)
{
	for(int i=0; i<glyphCount; ++i)
		if(glyphs[i].codepoint == codepoint) return;

	glyphs = GxRealloc(glyphs, glyphCount + 1);
	glyphs[glyphCount].traits    = traits;
	glyphs[glyphCount].xAdvance  = (float)(fontSize * 0.5f);
	glyphs[glyphCount].codepoint = codepoint;
	glyphs[glyphCount].page      = 0;

	++glyphCount;
}

// Builds character maps based on the loaded glyphs.
void GxFontData::myFinalize()
{
	// Sort kerning pairs map for look-up.
	if(kernCount > 0)
		SortKerningMap(kernKey, kernVal, 0, kernCount-1);

	// Make sure whitespace glyphs are available.
	myInsertBlankGlyph(C_HTAB, GT_WHITESPACE);
	myInsertBlankGlyph(C_SPACE, GT_WHITESPACE);
	myInsertBlankGlyph(C_LINE_FEED, GT_WHITESPACE);

	// Divide the codepoint into BMP and SMP.
	std::map<uint, GlyphIndices> bmpMap;
	std::map<uint, GlyphIndices> smpMap;
	for(int i=0; i<glyphCount; ++i)
	{
		uint codepoint = glyphs[i].codepoint;
		uint style = glyphs[i].traits & GT_STYLE_MASK;
		if(codepoint > 0xFFFF)
			smpMap[codepoint].indices[style] = i;
		else if(codepoint > 0)
			bmpMap[codepoint].indices[style] = i;
	}

	// Create BMP codepoint to glyph map.
	if(!bmpMap.empty())
	{
		mapBegin = bmpMap.begin()->first;
		mapEnd = bmpMap.rbegin()->first + 1;

		int size = (mapEnd - mapBegin) * myStyleCount;
		bmpGlyphMap = Malloc<int>(size);

		std::map<uint, GlyphIndices>::const_iterator it;
		for(it = bmpMap.begin(); it != bmpMap.end(); ++it)
			mySetSlot(bmpGlyphMap, it->first - mapBegin, it->second.indices);
	}

	// Create SMP codepoint to glyph map.
	if(!smpMap.empty())
	{
		int size = (int)smpMap.size() * myStyleCount;

		smpGlyphCount = (int)smpMap.size();
		smpGlyphKey = Malloc<uint>(smpGlyphCount);
		smpGlyphVal = Malloc<int>(size);

		int index = 0;
		std::map<uint, GlyphIndices>::iterator it;
		for(it = smpMap.begin(); it != smpMap.end(); ++it, ++index)
		{
			smpGlyphKey[index] = it->first;
			mySetSlot(smpGlyphVal, index, it->second.indices);
		}
	}

	// Store the width of ellipsis.
	ellipsisW = glyphs[GetGlyphIndex('.')].xAdvance * 3;
}

// ===================================================================================
// GxFontDatabase
// ===================================================================================

GxFontDatabaseImp* GxFontDatabaseImp::singleton = NULL;

GxFontDatabase::~GxFontDatabase()
{
}

GxFontDatabase* GxFontDatabase::Get()
{
	return GxFontDatabaseImp::singleton;
}

void GxFontDatabaseImp::Create()
{
	singleton = new GxFontDatabaseImp();
}

void GxFontDatabaseImp::Destroy()
{
	delete singleton;
	singleton = NULL;
}

GxFontDatabaseImp::GxFontDatabaseImp()
{
	myCreateFallbacks();
}

GxFontDatabaseImp::~GxFontDatabaseImp()
{
	myDefaultFont.Destroy();
	myFallbackFont.Destroy();

	FontMap::LoadMap::iterator it = myFontMap.loaded.begin();
	for(; it != myFontMap.loaded.end(); ++it)
		delete it->second.data;
}

void GxFontDatabaseImp::AddGlyph(const char* id, const GxTexture& img, int dy, int dx, int advance)
{
	if(img.GetHandle())
	{
		int w = img.GetWidth();
		int h = img.GetHeight();

		CustomGlyph g;
		g.texture         = img;
		g.glyph.coords    = GxAreaf((float)dx, (float)(dy-h), (float)(dx+w), (float)dy);
		g.glyph.uvs       = GxAreaf(0, 0, 1, 1);
		g.glyph.traits    = 0;
		g.glyph.xAdvance  = (float)(advance ? advance : w + 1);
		g.glyph.codepoint = myGlyphMap[id] = myGlyphs.size();
		g.glyph.page      = -1;
		myGlyphs.push_back(g);
	}
}

bool GxFontDatabaseImp::Load(GxFontHandle& outHandle, const char* path_or_resource)
{
	outHandle = 0;
	if(!path_or_resource) return false;

	GxResources::FontRes res;
	if(GxResources::Get()->GetResource(path_or_resource, res))
		return myLoadFile(outHandle, res.path.Raw());

	return myLoadFile(outHandle, path_or_resource);
}

bool GxFontDatabaseImp::myLoadFile(GxFontHandle& outHandle, const char* path)
{
	FontMap::Ref* ref = myFontMap.GetData(path);
	if(ref)
	{
		outHandle = ref->handle;
		++ref->count;
	}
	else
	{
		GxFontData* font = NULL;
		if(font = myLoadFontData(path))
		{
			outHandle = (GxFontHandle)font;
			myFontMap.Insert(outHandle, font, path);
		}
		else
		{
			GxLog(LOG_TAG, GX_LT_WARNING, "Failed to load \"%s\"", path);
		}
	}
	return outHandle != 0;
}

GxFontData* GxFontDatabaseImp::myLoadFontData(const char* path)
{
	// Construct the directory string
	GxString sdir = path;
	sdir.Replace('\\', '/');
	const int i = GxMax(sdir.Last('/') + 1, 0);
	sdir.Erase(i);

	// Load the font
	GxFontData* data = new GxFontData();
	if(!data->Create(path, sdir.Raw()))
	{
		delete data;
		data = NULL;
	}

	return data;
}
void GxFontDatabaseImp::AddReference(GxFontHandle handle)
{
	myFontMap.AddReference(handle);
}

void GxFontDatabaseImp::Release(GxFontHandle handle)
{
	FontMap::Ref* ref = myFontMap.Release(handle);
	if(ref)
	{
		delete ref->data;
		myFontMap.Erase(handle);
	}
}

void GxFontDatabaseImp::MakeDefault(const GxFont& font)
{
	myDefaultFont = font;
}

GxFontData* GxFontDatabaseImp::GetFont(GxFontHandle handle)
{
	FontMap::Ref* ref = myFontMap.GetData(handle);
	if(ref) return ref->data;
	ref = myFontMap.GetData(myDefaultFont.GetHandle());
	if(ref) return ref->data;
	return &myFallbackFont;
}

const Glyph& GxFontDatabaseImp::GetCustomGlyph(const uchar* id, int len)
{
	// Look for the custom glyph in de glyph map.
	const GxString key = GxString((const char*)id, len);
	GlyphMap::iterator it = myGlyphMap.find(key);
	if(it != myGlyphMap.end())
		return myGlyphs[it->second].glyph;

	// If it was not found, we return the fallback glyph.
	return myGlyphs.front().glyph;
}

GxTextureHandle GxFontDatabaseImp::GetCustomGlyphTexture(const Glyph* g)
{
	return myGlyphs[g->codepoint].texture.GetHandle();
}

int GxFontDatabaseImp::GetLoadedFontCount() const
{
	return myFontMap.GetLoadedCount();
}

void GxFontDatabaseImp::LogInfo() const
{
	GxLog("Loaded files: %i", myFontMap.files.size());
	{
		FontMap::FileMap::const_iterator it = myFontMap.files.begin();
		for(; it != myFontMap.files.end(); ++it)
			GxLog(" path=\"%s\" handle=%i",
				it->first.Raw(),
				it->second);
		GxLog("");
	}
	GxLog("Loaded fonts: %i", myFontMap.loaded.size());
	{
		FontMap::LoadMap::const_iterator it = myFontMap.loaded.begin();
		for(; it != myFontMap.loaded.end(); ++it)
			GxLog(" handle=%i references=%i size=%i",
				it->first,
				it->second.count,
				it->second.data->fontSize);
		GxLog("");
	}
}

static uchar* CreateBitmap(const uint* src, uint bits)
{
	uint* out = Malloc<uint>(bits * 32);
	for(uint i=0, *ptr = out; i<bits; ++i)
		for(int j=0; j<32; ++j)
			*ptr++ = ((src[i] >> j) & 1) * 0xFFFFFFFF;

	return (uchar*)out;
}

void GxFontDatabaseImp::myCreateFallbacks()
{
	// Create the global fallback font.
	uchar* chr = CreateBitmap(charBitmap, FB_CHAR_BITS);
	myFallbackFont.Create(chr, FB_CHAR_W, FB_CHAR_H, FB_CHAR_IN_W, FB_CHAR_IN_H, FB_CHAR_H+4, charCharset);

	// Create the fallback glyph for custom glyphs from formatting tags.
	uchar* tag = CreateBitmap(tagBitmap, FB_TAG_BITS);

	CustomGlyph g;
	//CreateTexture(g.texture, FB_TAG_W, FB_TAG_H, tag);
	g.texture.Create(FB_TAG_W, FB_TAG_H, tag);
	g.glyph.coords    = GxAreaf(0, -FB_TAG_H, FB_TAG_W, 0);
	g.glyph.uvs       = GxAreaf(0, 0, 1, 1);
	g.glyph.traits    = 0;
	g.glyph.xAdvance  = FB_TAG_W + 1;
	g.glyph.codepoint = 0;
	g.glyph.page      = -1;
	myGlyphs.push_back(g);

	// Cleanup.
	GxFree(chr);
	GxFree(tag);
}

}; // namespace graphics
}; // namespace guix