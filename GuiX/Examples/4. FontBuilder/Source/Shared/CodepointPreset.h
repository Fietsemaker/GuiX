#pragma once

#include <Shared/Common.h>

namespace builder {

enum CodepointPresetId
{
	CP_Numeric,      // Numbers (0-9).
	CP_Alphabetic,   // Letters (a-z, A-Z).
	CP_Alphanumeric, // Numbers & letters.
	CP_Jouyou_Kanji, // All 2136 Jouyou Kanji.

	// Total preset count.
	CP_PRESET_COUNT
};

struct CodepointPreset
{
	static CodepointPreset Get(CodepointPresetId id);

	codepoint32 first, last;
	std::basic_string<codepoint32> chars;
	std::string name;
};

}; // namespace builder