#include <Shared/CodepointRange.h>

namespace builder {
namespace {

static bool sInitialized = false;
static CodepointRange sCodepoint[CR_RANGE_COUNT];

static void Set(codepoint32 first, codepoint32 last, int index, const char* name)
{
	CodepointRange r = {first, last, name};
	sCodepoint[index] = r;
}

static void InitCodepoint()
{
	if(sInitialized) return;
	sInitialized = true;

	memset(sCodepoint, 0, sizeof(CodepointRange) * CR_RANGE_COUNT);

	// Basic Multilingual Plane
	Set(0x0000, 0x007F, CR_Basic_Latin,					"Basic Latin");
	Set(0x0080, 0x00FF, CR_Latin_1_Supplement,			"Latin-1 Supplement");
	Set(0x0100, 0x017F, CR_Latin_Extended_A,			"Latin Extended-A");
	Set(0x0180, 0x024F, CR_Latin_Extended_B,			"Latin Extended-B");
	Set(0x0250, 0x02AF, CR_IPA_Extensions,				"IPA Extensions");
	Set(0x02B0, 0x02FF, CR_Spacing_Modifier_Letters,	"Spacing Modifier Letters");
	Set(0x0300, 0x036F, CR_Combining_Diacritical_Marks,	"Combining Diacritical Marks");
	Set(0x0370, 0x03FF, CR_Greek_and_Coptic,			"Greek and Coptic");
	Set(0x0400, 0x04FF, CR_Cyrillic,					"Cyrillic");
	Set(0x0500, 0x052F, CR_Cyrillic_Supplement,			"Cyrillic Supplement");
	Set(0x0530, 0x058F, CR_Armenian,					"Armenian");
	Set(0x0590, 0x05FF, CR_Hebrew,						"Hebrew");
	Set(0x0600, 0x06FF, CR_Arabic,						"Arabic");
	Set(0x0700, 0x074F, CR_Syriac,						"Syriac");
	Set(0x0750, 0x077F, CR_Arabic_Supplement,			"Arabic Supplement");
	Set(0x0780, 0x07BF, CR_Thaana,						"Thaana");
	Set(0x07C0, 0x07FF, CR_NKo,							"NKo");
	Set(0x0800, 0x083F, CR_Samaritan,					"Samaritan");
	Set(0x0840, 0x085F, CR_Mandaic,						"Mandaic");
	Set(0x08A0, 0x08FF, CR_Arabic_Extended_A,			"Arabic Extended-A");
	Set(0x0900, 0x097F, CR_Devanagari,					"Devanagari");
	Set(0x0980, 0x09FF, CR_Bengali,						"Bengali");
	Set(0x0A00, 0x0A7F, CR_Gurmukhi,					"Gurmukhi");
	Set(0x0A80, 0x0AFF, CR_Gujarati,					"Gujarati");
	Set(0x0B00, 0x0B7F, CR_Oriya,						"Oriya");
	Set(0x0B80, 0x0BFF, CR_Tamil,						"Tamil");
	Set(0x0C00, 0x0C7F, CR_Telugu,						"Telugu");
	Set(0x0C80, 0x0CFF, CR_Kannada,						"Kannada");
	Set(0x0D00, 0x0D7F, CR_Malayalam,					"Malayalam");
	Set(0x0D80, 0x0DFF, CR_Sinhala,						"Sinhala");
	Set(0x0E00, 0x0E7F, CR_Thai,						"Thai");
	Set(0x0E80, 0x0EFF, CR_Lao,							"Lao");
	Set(0x0F00, 0x0FFF, CR_Tibetan,						"Tibetan");
	Set(0x1000, 0x109F, CR_Myanmar,						"Myanmar");
	Set(0x10A0, 0x10FF, CR_Georgian,					"Georgian");
	Set(0x1100, 0x11FF, CR_Hangul_Jamo,					"Hangul Jamo");
	Set(0x1200, 0x137F, CR_Ethiopic,					"Ethiopic");
	Set(0x1380, 0x139F, CR_Ethiopic_Supplement,			"Ethiopic Supplement");
	Set(0x13A0, 0x13FF, CR_Cherokee,					"Cherokee");
	Set(0x1400, 0x167F, CR_Unified_Canadian_Aboriginal_Syllabics, "Unified Canadian Aboriginal Syllabics");
	Set(0x1680, 0x169F, CR_Ogham,						"Ogham");
	Set(0x16A0, 0x16FF, CR_Runic,						"Runic");
	Set(0x1700, 0x171F, CR_Tagalog,						"Tagalog");
	Set(0x1720, 0x173F, CR_Hanunoo,						"Hanunoo");
	Set(0x1740, 0x175F, CR_Buhid,						"Buhid");
	Set(0x1760, 0x177F, CR_Tagbanwa,					"Tagbanwa");
	Set(0x1780, 0x17FF, CR_Khmer,						"Khmer");
	Set(0x1800, 0x18AF, CR_Mongolian,					"Mongolian");
	Set(0x18B0, 0x18FF, CR_Unified_Canadian_Aboriginal_Syllabics_Extended, "Unified Canadian Aboriginal Syllabics Extended");
	Set(0x1900, 0x194F,	CR_Limbu,						"Limbu");
	Set(0x1950, 0x197F,	CR_Tai_Le,						"Tai Le");
	Set(0x1980, 0x19DF, CR_New_Tai_Lue,					"New Tai Lue");
	Set(0x19E0, 0x19FF, CR_Khmer_Symbols,				"Khmer Symbols");
	Set(0x1A00, 0x1A1F, CR_Buginese,					"Buginese");
	Set(0x1A20, 0x1AAF, CR_Tai_Tham,					"Tai Tham");
	Set(0x1B00, 0x1B7F, CR_Balinese,					"Balinese");
	Set(0x1B80, 0x1BBF, CR_Sundanese,					"Sundanese");
	Set(0x1BC0, 0x1BFF, CR_Batak,						"Batak");
	Set(0x1C00, 0x1C4F, CR_Lepcha,						"Lepcha");
	Set(0x1C50, 0x1C7F, CR_Ol_Chiki,					"Ol Chiki");
	Set(0x1CC0, 0x1CCF, CR_Sundanese_Supplement,		"Sundanese Supplement");
	Set(0x1CD0, 0x1CFF, CR_Vedic_Extensions,			"Vedic Extensions");
	Set(0x1D00, 0x1D7F, CR_Phonetic_Extensions,			"Phonetic Extensions");
	Set(0x1D80, 0x1DBF, CR_Phonetic_Extensions_Supplement, "Phonetic Extensions Supplement");
	Set(0x1DC0, 0x1DFF, CR_Combining_Diacritical_Marks_Supplement, "Combining Diacritical Marks Supplement");
	Set(0x1E00, 0x1EFF, CR_Latin_Extended_Additional,	"Latin Extended Additional");
	Set(0x1F00, 0x1FFF, CR_Greek_Extended,				"Greek Extended");
	Set(0x2000, 0x206F, CR_General_Punctuation,			"General Punctuation");
	Set(0x2070, 0x209F, CR_Superscripts_and_Subscripts,	"Superscripts and Subscripts");
	Set(0x20A0, 0x20CF, CR_Currency_Symbols,			"Currency Symbols");
	Set(0x20D0, 0x20FF, CR_Combining_Diacritical_Marks_for_Symbols, "Combining Diacritical Marks for Symbols");
	Set(0x2100, 0x214F, CR_Letterlike_Symbols,			"Letterlike Symbols");
	Set(0x2150, 0x218F, CR_Number_Forms,				"Number Forms");
	Set(0x2190, 0x21FF, CR_Arrows,						"Arrows");
	Set(0x2200, 0x22FF, CR_Mathematical_Operators,		"Mathematical Operators");
	Set(0x2300, 0x23FF, CR_Miscellaneous_Technical,		"Miscellaneous Technical");
	Set(0x2400, 0x243F, CR_Control_Pictures,			"Control Pictures");
	Set(0x2440, 0x245F, CR_Optical_Character_Recognition, "Optical Character Recognition");
	Set(0x2460, 0x24FF, CR_Enclosed_Alphanumerics,		"Enclosed Alphanumerics");
	Set(0x2500, 0x257F, CR_Box_Drawing,					"Box Drawing");
	Set(0x2580, 0x259F, CR_Block_Elements,				"Block Elements");
	Set(0x25A0, 0x25FF, CR_Geometric_Shapes,			"Geometric Shapes");
	Set(0x2600, 0x26FF, CR_Miscellaneous_Symbols,		"Miscellaneous Symbols");
	Set(0x2700, 0x27BF, CR_Dingbats,					"Dingbats");
	Set(0x27C0, 0x27EF, CR_Miscellaneous_Mathematical_Symbols_A, "Miscellaneous Mathematical Symbols-A");
	Set(0x27F0, 0x27FF, CR_Supplemental_Arrows_A,		"Supplemental Arrows-A");
	Set(0x2800, 0x28FF, CR_Braille_Patterns,			"Braille Patterns");
	Set(0x2900, 0x297F, CR_Supplemental_Arrows_B,		"Supplemental Arrows-B");
	Set(0x2980, 0x29FF, CR_Miscellaneous_Mathematical_Symbols_B, "Miscellaneous Mathematical Symbols-B");
	Set(0x2A00, 0x2AFF, CR_Supplemental_Mathematical_Operators, "Supplemental Mathematical Operators");
	Set(0x2B00, 0x2BFF, CR_Miscellaneous_Symbols_and_Arrows, "Miscellaneous Symbols and Arrows");
	Set(0x2C00, 0x2C5F, CR_Glagolitic,					"Glagolitic");
	Set(0x2C60, 0x2C7F, CR_Latin_Extended_C,			"Latin Extended-C");
	Set(0x2C80, 0x2CFF, CR_Coptic,						"Coptic");
	Set(0x2D00, 0x2D2F, CR_Georgian_Supplement,			"Georgian Supplement");
	Set(0x2D30, 0x2D7F, CR_Tifinagh,					"Tifinagh");
	Set(0x2D80, 0x2DDF, CR_Ethiopic_Extended,			"Ethiopic Extended");
	Set(0x2DE0, 0x2DFF, CR_Cyrillic_Extended_A,			"Cyrillic Extended-A");
	Set(0x2E00, 0x2E7F, CR_Supplemental_Punctuation,	"Supplemental Punctuation");
	Set(0x2E80, 0x2EFF, CR_CJK_Radicals_Supplement,		"CJK Radicals Supplement");
	Set(0x2F00, 0x2FDF, CR_Kangxi_Radicals,				"Kangxi Radicals");
	Set(0x2FF0, 0x2FFF, CR_Ideographic_Description_Characters, "Ideographic Description Characters");
	Set(0x3000, 0x303F, CR_CJK_Symbols_and_Punctuation,	"CJK Symbols and Punctuation");
	Set(0x3040, 0x309F, CR_Hiragana,					"Hiragana");
	Set(0x30A0, 0x30FF, CR_Katakana,					"Katakana");
	Set(0x3100, 0x312F, CR_Bopomofo,					"Bopomofo");
	Set(0x3130, 0x318F, CR_Hangul_Compatibility_Jamo,	"Hangul Compatibility Jamo");
	Set(0x3190, 0x319F, CR_Kanbun,						"Kanbun");
	Set(0x31A0, 0x31BF, CR_Bopomofo_Extended,			"Bopomofo Extended");
	Set(0x31C0, 0x31EF, CR_CJK_Strokes,					"CJK Strokes");
	Set(0x31F0, 0x31FF, CR_Katakana_Phonetic_Extensions, "Katakana Phonetic Extensions");
	Set(0x3200, 0x32FF, CR_Enclosed_CJK_Letters_and_Months, "Enclosed CJK Letters and Months");
	Set(0x3300, 0x33FF, CR_CJK_Compatibility,			"CJK Compatibility");
	Set(0x3400, 0x4DBF, CR_CJK_Unified_Ideographs_Extension_A, "CJK Unified Ideographs Extension A");
	Set(0x4DC0, 0x4DFF, CR_Yijing_Hexagram_Symbols,		"Yijing Hexagram Symbols");
	Set(0x4E00, 0x9FFF, CR_CJK_Unified_Ideographs,		"CJK Unified Ideographs");
	Set(0xA000, 0xA48F, CR_Yi_Syllables,				"Yi Syllables");
	Set(0xA490, 0xA4CF, CR_Yi_Radicals,					"Yi Radicals");
	Set(0xA4D0, 0xA4FF, CR_Lisu,						"Lisu");
	Set(0xA500, 0xA63F, CR_Vai,							"Vai");
	Set(0xA640, 0xA69F, CR_Cyrillic_Extended_B,			"Cyrillic Extended-B");
	Set(0xA6A0, 0xA6FF, CR_Bamum,						"Bamum");
	Set(0xA700, 0xA71F, CR_Modifier_Tone_Letters,		"Modifier Tone Letters");
	Set(0xA720, 0xA7FF, CR_Latin_Extended_D,			"Latin Extended-D");
	Set(0xA800, 0xA82F, CR_Syloti_Nagri,				"Syloti Nagri");
	Set(0xA830, 0xA83F, CR_Common_Indic_Number_Forms,	"Common Indic Number Forms");
	Set(0xA840, 0xA87F, CR_Phags_pa,					"Phags-pa");
	Set(0xA880, 0xA8DF, CR_Saurashtra,					"Saurashtra");
	Set(0xA8E0, 0xA8FF, CR_Devanagari_Extended,			"Devanagari Extended");
	Set(0xA900, 0xA92F, CR_Kayah_Li,					"Kayah Li");
	Set(0xA930, 0xA95F, CR_Rejang,						"Rejang");
	Set(0xA960, 0xA97F, CR_Hangul_Jamo_Extended_A,		"Hangul Jamo Extended-A");
	Set(0xA980, 0xA9DF, CR_Javanese,					"Javanese");
	Set(0xAA00, 0xAA5F, CR_Cham,						"Cham");
	Set(0xAA60, 0xAA7F, CR_Myanmar_Extended_A,			"Myanmar Extended-A");
	Set(0xAA80, 0xAADF, CR_Tai_Viet,					"Tai Viet");
	Set(0xAAE0, 0xAAFF, CR_Meetei_Mayek_Extensions,		"Meetei Mayek Extensions");
	Set(0xAB00, 0xAB2F, CR_Ethiopic_Extended_A,			"Ethiopic Extended-A");
	Set(0xABC0, 0xABFF, CR_Meetei_Mayek,				"Meetei Mayek");
	Set(0xAC00, 0xD7AF, CR_Hangul_Syllables,			"Hangul Syllables");
	Set(0xD7B0, 0xD7FF, CR_Hangul_Jamo_Extended_B,		"Hangul Jamo Extended-B");
	Set(0xD800, 0xDB7F, CR_High_Surrogates,				"High Surrogates");
	Set(0xDB80, 0xDBFF, CR_High_Private_Use_Surrogates,	"High Private Use Surrogates");
	Set(0xDC00, 0xDFFF, CR_Low_Surrogates,				"Low Surrogates");
	Set(0xE000, 0xF8FF, CR_Private_Use_Area,			"Private Use Area");
	Set(0xF900, 0xFAFF, CR_CJK_Compatibility_Ideographs, "CJK Compatibility Ideographs");
	Set(0xFB00, 0xFB4F, CR_Alphabetic_Presentation_Forms, "Alphabetic Presentation Forms");
	Set(0xFB50, 0xFDFF, CR_Arabic_Presentation_Forms_A,	"Arabic Presentation Forms-A");
	Set(0xFE00, 0xFE0F, CR_Variation_Selectors,			"Variation Selectors");
	Set(0xFE10, 0xFE1F, CR_Vertical_Forms,				"Vertical Forms");
	Set(0xFE20, 0xFE2F, CR_Combining_Half_Marks,		"Combining Half Marks");
	Set(0xFE30, 0xFE4F, CR_CJK_Compatibility_Forms,		"CJK Compatibility Forms");
	Set(0xFE50, 0xFE6F, CR_Small_Form_Variants,			"Small Form Variants");
	Set(0xFE70, 0xFEFF, CR_Arabic_Presentation_Forms_B,	"Arabic Presentation Forms-B");
	Set(0xFF00, 0xFFEF, CR_Halfwidth_and_Fullwidth_Forms, "Halfwidth and Fullwidth Forms");
	Set(0xFFF0, 0xFFFF, CR_Specials,					"Specials");

	// Supplementary Multilingual Plane
	Set(0x10000, 0x1007F, CR_Linear_B_Syllabary,		"Linear B Syllabary");
	Set(0x10080, 0x100FF, CR_Linear_B_Ideograms,		"Linear B Ideograms");
	Set(0x10100, 0x1013F, CR_Aegean_Numbers,			"Aegean Numbers");
	Set(0x10140, 0x1018F, CR_Ancient_Greek_Numbers,		"Ancient Greek Numbers");
	Set(0x10190, 0x101CF, CR_Ancient_Symbols,			"Ancient Symbols");
	Set(0x101D0, 0x101FF, CR_Phaistos_Disc,				"Phaistos Disc");
	Set(0x10280, 0x1029F, CR_Lycian,					"Lycian");
	Set(0x102A0, 0x102DF, CR_Carian,					"Carian");
	Set(0x10300, 0x1032F, CR_Old_Italic,				"Old Italic");
	Set(0x10330, 0x1034F, CR_Gothic,					"Gothic");
	Set(0x10380, 0x1039F, CR_Ugaritic,					"Ugaritic");
	Set(0x103A0, 0x103DF, CR_Old_Persian,				"Old Persian");
	Set(0x10400, 0x1044F, CR_Deseret,					"Deseret");
	Set(0x10450, 0x1047F, CR_Shavian,					"Shavian");
	Set(0x10480, 0x104AF, CR_Osmanya,					"Osmanya");
	Set(0x10800, 0x1083F, CR_Cypriot_Syllabary,			"Cypriot Syllabary");
	Set(0x10840, 0x1085F, CR_Imperial_Aramaic,			"Imperial Aramaic");
	Set(0x10900, 0x1091F, CR_Phoenician,				"Phoenician");
	Set(0x10920, 0x1093F, CR_Lydian,					"Lydian");
	Set(0x10980, 0x1099F, CR_Meroitic_Hieroglyphs,		"Meroitic Hieroglyphs");
	Set(0x109A0, 0x109FF, CR_Meroitic_Cursive,			"Meroitic Cursive");
	Set(0x10A00, 0x10A5F, CR_Kharoshthi,				"Kharoshthi");
	Set(0x10A60, 0x10A7F, CR_Old_South_Arabian,			"Old South Arabian");
	Set(0x10B00, 0x10B3F, CR_Avestan,					"Avestan");
	Set(0x10B40, 0x10B5F, CR_Inscriptional_Parthian,	"Inscriptional Parthian");
	Set(0x10B60, 0x10B7F, CR_Inscriptional_Pahlavi,		"Inscriptional Pahlavi");
	Set(0x10C00, 0x10C4F, CR_Old_Turkic,				"Old Turkic");
	Set(0x10E60, 0x10E7F, CR_Rumi_Numeral_Symbols,		"Rumi Numeral Symbols");
	Set(0x11000, 0x1107F, CR_Brahmi,					"Brahmi");
	Set(0x11080, 0x110CF, CR_Kaithi,					"Kaithi");
	Set(0x110D0, 0x110FF, CR_Sora_Sompeng,				"Sora Sompeng");
	Set(0x11100, 0x1114F, CR_Chakma,					"Chakma");
	Set(0x11180, 0x111DF, CR_Sharada,					"Sharada");
	Set(0x11680, 0x116CF, CR_Takri,						"Takri");
	Set(0x12000, 0x123FF, CR_Cuneiform,					"Cuneiform");
	Set(0x12400, 0x1247F, CR_Cuneiform_Numbers_and_Punctuation,	"Cuneiform Numbers and Punctuation");
	Set(0x13000, 0x1342F, CR_Egyptian_Hieroglyphs,		"Egyptian Hieroglyphs");
	Set(0x16800, 0x16A3F, CR_Bamum_Supplement,			"Bamum Supplement");
	Set(0x16F00, 0x16F9F, CR_Miao,						"Miao");
	Set(0x1B000, 0x1B0FF, CR_Kana_Supplement,			"Kana Supplement");
	Set(0x1D000, 0x1D0FF, CR_Byzantine_Musical_Symbols,	"Byzantine Musical Symbols");
	Set(0x1D100, 0x1D1FF, CR_Musical_Symbols,			"Musical Symbols");
	Set(0x1D200, 0x1D24F, CR_Ancient_Greek_Musical_Notation, "Ancient Greek Musical Notation");
	Set(0x1D300, 0x1D35F, CR_Tai_Xuan_Jing_Symbols,		"Tai Xuan Jing Symbols");
	Set(0x1D360, 0x1D37F, CR_Counting_Rod_Numerals,		"Counting Rod Numerals");
	Set(0x1D400, 0x1D7FF, CR_Mathematical_Alphanumeric_Symbols, "Mathematical Alphanumeric Symbols");
	Set(0x1EE00, 0x1EEFF, CR_Arabic_Mathematical_Alphabetic_Symbols, "Arabic Mathematical Alphabetic Symbols");
	Set(0x1F000, 0x1F02F, CR_Mahjong_Tiles,				"Mahjong Tiles");
	Set(0x1F030, 0x1F09F, CR_Domino_Tiles,				"Domino Tiles");
	Set(0x1F0A0, 0x1F0FF, CR_Playing_Cards,				"Playing Cards");
	Set(0x1F100, 0x1F1FF, CR_Enclosed_Alphanumeric_Supplement, "Enclosed Alphanumeric Supplement");
	Set(0x1F200, 0x1F2FF, CR_Enclosed_Ideographic_Supplement, "Enclosed Ideographic Supplement");
	Set(0x1F300, 0x1F5FF, CR_Miscellaneous_Symbols_And_Pictographs, "Miscellaneous Symbols And Pictographs");
	Set(0x1F600, 0x1F64F, CR_Emoticons,					"Emoticons");
	Set(0x1F680, 0x1F6FF, CR_Transport_And_Map_Symbols,	"Transport And Map Symbols");
	Set(0x1F700, 0x1F77F, CR_Alchemical_Symbols,		"Alchemical Symbols");

	// Supplementary Ideographic Plane
	Set(0x20000, 0x2A6DF, CR_CJK_Unified_Ideographs_Extension_B, "CJK Unified Ideographs Extension B");
	Set(0x2A700, 0x2B73F, CR_CJK_Unified_Ideographs_Extension_C, "CJK Unified Ideographs Extension C");
	Set(0x2B740, 0x2B81F, CR_CJK_Unified_Ideographs_Extension_D, "CJK Unified Ideographs Extension D");
	Set(0x2F800, 0x2FA1F, CR_CJK_Compatibility_Ideographs_Supplement, "CJK Compatibility Ideographs Supplement");
}

}; // anonymous namespace

CodepointRange CodepointRange::Get(CodepointRangeId id)
{
	InitCodepoint();

	CodepointRange r;
	if(id >= 0 && id < CR_RANGE_COUNT)
		r = sCodepoint[id];

	if(r.name == NULL)
		r.name = "unknown";

	return r;
}

}; // namespace Lty