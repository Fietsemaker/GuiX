#pragma once

#include <vector>
#include <string>

#include <Shared/Common.h>
#include <Shared/CodepointPreset.h>
#include <Shared/CodepointRange.h>

namespace builder {

enum RasterizerHintmode
{
	RH_NORMAL = 0,
	RH_LIGHT,
	RH_MONO,
};

enum RasterizerResult
{
	RR_SUCCESS = 0,
	RR_NO_RENDERED_GLYPHS,
	RR_RENDERING_ERROR,
};

// ===================================================================================
// Rasterizer input
// ===================================================================================

typedef std::vector<CodepointPreset> PresetVec;
typedef std::vector<CodepointRange> RangeVec;

struct RasterizerEntry
{
	RasterizerEntry();
	~RasterizerEntry();

	std::string fontPath;
	PresetVec presets;
	RangeVec ranges;
	int fontSize;
	RasterizerHintmode hintMode;
	bool italic, bold, smallCaps;
};

typedef std::vector<RasterizerEntry> RasterizerEntryVec;

struct RasterizerPage
{
	RasterizerPage();
	~RasterizerPage();

	RasterizerEntryVec entries;
	int glyphPadding;
	int glyphAdvance;
	int textureWidth;
	bool renderFallback;
	bool kerningPairs;
};

// ===================================================================================
// Rasterizer output
// ===================================================================================

typedef std::vector<Glyph> GlyphVec;

struct RasterizerOutput
{
	GlyphPage page;
	GlyphVec glyphs[GS_STYLE_COUNT];
	KerningList kerning[GS_STYLE_COUNT];
};

// ===================================================================================
// Rasterizer
// ===================================================================================

class Rasterizer : public NonCopyable
{
public:
	Rasterizer();
	~Rasterizer();

	RasterizerResult Render(const RasterizerPage& input, RasterizerOutput& output);

private:
	class Data;
	Data* myData;
};

}; // namespace builder