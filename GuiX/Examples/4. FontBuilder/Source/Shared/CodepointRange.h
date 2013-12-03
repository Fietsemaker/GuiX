#pragma once

#include <Shared/Common.h>

namespace builder {

enum CodepointRangeId
{
	// Basic Multilingual Plane.
	CR_Basic_Latin = 0,							// U+0000 U+007F 128
	CR_Latin_1_Supplement,						// U+0080 U+00FF 128
	CR_Latin_Extended_A,						// U+0100 U+017F 128
	CR_Latin_Extended_B,						// U+0180 U+024F 208
	CR_IPA_Extensions,							// U+0250 U+02AF 96
	CR_Spacing_Modifier_Letters,				// U+02B0 U+02FF 80
	CR_Combining_Diacritical_Marks,				// U+0300 U+036F 112
	CR_Greek_and_Coptic,						// U+0370 U+03FF 134
	CR_Cyrillic,								// U+0400 U+04FF 256
	CR_Cyrillic_Supplement,						// U+0500 U+052F 40
	CR_Armenian,								// U+0530 U+058F 87
	CR_Hebrew,									// U+0590 U+05FF 87
	CR_Arabic,									// U+0600 U+06FF 253
	CR_Syriac,									// U+0700 U+074F 77
	CR_Arabic_Supplement,						// U+0750 U+077F 48
	CR_Thaana,									// U+0780 U+07BF 50
	CR_NKo,										// U+07C0 U+07FF 59
	CR_Samaritan,								// U+0800 U+083F 61
	CR_Mandaic,									// U+0840 U+085F 29
	CR_Arabic_Extended_A,						// U+08A0 U+08FF 39
	CR_Devanagari,								// U+0900 U+097F 127
	CR_Bengali,									// U+0980 U+09FF 92
	CR_Gurmukhi,								// U+0A00 U+0A7F 79
	CR_Gujarati,								// U+0A80 U+0AFF 84
	CR_Oriya,									// U+0B00 U+0B7F 90
	CR_Tamil,									// U+0B80 U+0BFF 72
	CR_Telugu,									// U+0C00 U+0C7F 93
	CR_Kannada,									// U+0C80 U+0CFF 86
	CR_Malayalam,								// U+0D00 U+0D7F 98
	CR_Sinhala,									// U+0D80 U+0DFF 80
	CR_Thai,									// U+0E00 U+0E7F 87
	CR_Lao,										// U+0E80 U+0EFF 67
	CR_Tibetan,									// U+0F00 U+0FFF 211
	CR_Myanmar,									// U+1000 U+109F 160
	CR_Georgian,								// U+10A0 U+10FF 88
	CR_Hangul_Jamo,								// U+1100 U+11FF 256
	CR_Ethiopic,								// U+1200 U+137F 358
	CR_Ethiopic_Supplement,						// U+1380 U+139F 26
	CR_Cherokee,								// U+13A0 U+13FF 85
	CR_Unified_Canadian_Aboriginal_Syllabics,	// U+1400 U+167F 640
	CR_Ogham,									// U+1680 U+169F 29
	CR_Runic,									// U+16A0 U+16FF 81
	CR_Tagalog,									// U+1700 U+171F 20
	CR_Hanunoo,									// U+1720 U+173F 23
	CR_Buhid,									// U+1740 U+175F 20
	CR_Tagbanwa,								// U+1760 U+177F 18
	CR_Khmer,									// U+1780 U+17FF 114
	CR_Mongolian,								// U+1800 U+18AF 156
	CR_Unified_Canadian_Aboriginal_Syllabics_Extended, // U+18B0 U+18FF 70
	CR_Limbu,									// U+1900 U+194F 66
	CR_Tai_Le,									// U+1950 U+197F 35
	CR_New_Tai_Lue,								// U+1980 U+19DF 83
	CR_Khmer_Symbols,							// U+19E0 U+19FF 32
	CR_Buginese,								// U+1A00 U+1A1F 30
	CR_Tai_Tham,								// U+1A20 U+1AAF 127
	CR_Balinese,								// U+1B00 U+1B7F 121
	CR_Sundanese,								// U+1B80 U+1BBF 64
	CR_Batak,									// U+1BC0 U+1BFF 56
	CR_Lepcha,									// U+1C00 U+1C4F 74
	CR_Ol_Chiki,								// U+1C50 U+1C7F 48
	CR_Sundanese_Supplement,					// U+1CC0 U+1CCF 8
	CR_Vedic_Extensions,						// U+1CD0 U+1CFF 39
	CR_Phonetic_Extensions,						// U+1D00 U+1D7F 128
	CR_Phonetic_Extensions_Supplement,			// U+1D80 U+1DBF 64
	CR_Combining_Diacritical_Marks_Supplement,	// U+1DC0 U+1DFF 43
	CR_Latin_Extended_Additional,				// U+1E00 U+1EFF 256
	CR_Greek_Extended,							// U+1F00 U+1FFF 233
	CR_General_Punctuation,						// U+2000 U+206F 107
	CR_Superscripts_and_Subscripts,				// U+2070 U+209F 42
	CR_Currency_Symbols,						// U+20A0 U+20CF 26
	CR_Combining_Diacritical_Marks_for_Symbols, // U+20D0 U+20FF 33
	CR_Letterlike_Symbols,						// U+2100 U+214F 80
	CR_Number_Forms,							// U+2150 U+218F 58
	CR_Arrows,									// U+2190 U+21FF 112
	CR_Mathematical_Operators,					// U+2200 U+22FF 256
	CR_Miscellaneous_Technical,					// U+2300 U+23FF 244
	CR_Control_Pictures,						// U+2400 U+243F 39
	CR_Optical_Character_Recognition,			// U+2440 U+245F 11
	CR_Enclosed_Alphanumerics,					// U+2460 U+24FF 160
	CR_Box_Drawing,								// U+2500 U+257F 128
	CR_Block_Elements,							// U+2580 U+259F 32
	CR_Geometric_Shapes,						// U+25A0 U+25FF 96
	CR_Miscellaneous_Symbols,					// U+2600 U+26FF 256
	CR_Dingbats,								// U+2700 U+27BF 191
	CR_Miscellaneous_Mathematical_Symbols_A,	// U+27C0 U+27EF 48
	CR_Supplemental_Arrows_A,					// U+27F0 U+27FF 16
	CR_Braille_Patterns,						// U+2800 U+28FF 256
	CR_Supplemental_Arrows_B,					// U+2900 U+297F 128
	CR_Miscellaneous_Mathematical_Symbols_B,	// U+2980 U+29FF 128
	CR_Supplemental_Mathematical_Operators,		// U+2A00 U+2AFF 256
	CR_Miscellaneous_Symbols_and_Arrows,		// U+2B00 U+2BFF 87
	CR_Glagolitic,								// U+2C00 U+2C5F 94
	CR_Latin_Extended_C,						// U+2C60 U+2C7F 32
	CR_Coptic,									// U+2C80 U+2CFF 123
	CR_Georgian_Supplement,						// U+2D00 U+2D2F 40
	CR_Tifinagh,								// U+2D30 U+2D7F 59
	CR_Ethiopic_Extended,						// U+2D80 U+2DDF 79
	CR_Cyrillic_Extended_A,						// U+2DE0 U+2DFF 32
	CR_Supplemental_Punctuation,				// U+2E00 U+2E7F 60
	CR_CJK_Radicals_Supplement,					// U+2E80 U+2EFF 115
	CR_Kangxi_Radicals,							// U+2F00 U+2FDF 214
	CR_Ideographic_Description_Characters,		// U+2FF0 U+2FFF 12
	CR_CJK_Symbols_and_Punctuation,				// U+3000 U+303F 64
	CR_Hiragana,								// U+3040 U+309F 93
	CR_Katakana,								// U+30A0 U+30FF 96
	CR_Bopomofo,								// U+3100 U+312F 41
	CR_Hangul_Compatibility_Jamo,				// U+3130 U+318F 94
	CR_Kanbun,									// U+3190 U+319F 16
	CR_Bopomofo_Extended,						// U+31A0 U+31BF 27
	CR_CJK_Strokes,								// U+31C0 U+31EF 36
	CR_Katakana_Phonetic_Extensions,			// U+31F0 U+31FF 16
	CR_Enclosed_CJK_Letters_and_Months, 		// U+3200 U+32FF 254
	CR_CJK_Compatibility,						// U+3300 U+33FF 256
	CR_CJK_Unified_Ideographs_Extension_A,		// U+3400 U+4DBF 6582
	CR_Yijing_Hexagram_Symbols,					// U+4DC0 U+4DFF 64
	CR_CJK_Unified_Ideographs,					// U+4E00 U+9FFF 20941
	CR_Yi_Syllables,							// U+A000 U+A48F 1165
	CR_Yi_Radicals,								// U+A490 U+A4CF 55
	CR_Lisu,									// U+A4D0 U+A4FF 48
	CR_Vai,										// U+A500 U+A63F 300
	CR_Cyrillic_Extended_B,						// U+A640 U+A69F 89
	CR_Bamum,									// U+A6A0 U+A6FF 88
	CR_Modifier_Tone_Letters,					// U+A700 U+A71F 32
	CR_Latin_Extended_D,						// U+A720 U+A7FF 134
	CR_Syloti_Nagri,							// U+A800 U+A82F 44
	CR_Common_Indic_Number_Forms,				// U+A830 U+A83F 10
	CR_Phags_pa,								// U+A840 U+A87F 56
	CR_Saurashtra,								// U+A880 U+A8DF 81
	CR_Devanagari_Extended,						// U+A8E0 U+A8FF 28
	CR_Kayah_Li,								// U+A900 U+A92F 48
	CR_Rejang,									// U+A930 U+A95F 37
	CR_Hangul_Jamo_Extended_A,					// U+A960 U+A97F 29
	CR_Javanese,								// U+A980 U+A9DF 91
	CR_Cham,									// U+AA00 U+AA5F 83
	CR_Myanmar_Extended_A,						// U+AA60 U+AA7F 28
	CR_Tai_Viet,								// U+AA80 U+AADF 72
	CR_Meetei_Mayek_Extensions,					// U+AAE0 U+AAFF 23
	CR_Ethiopic_Extended_A,						// U+AB00 U+AB2F 32
	CR_Meetei_Mayek,							// U+ABC0 U+ABFF 56
	CR_Hangul_Syllables,						// U+AC00 U+D7AF 2
	CR_Hangul_Jamo_Extended_B,					// U+D7B0 U+D7FF 72
	CR_High_Surrogates,							// U+D800 U+DB7F 2
	CR_High_Private_Use_Surrogates,				// U+DB80 U+DBFF 2
	CR_Low_Surrogates,							// U+DC00 U+DFFF 2
	CR_Private_Use_Area,						// U+E000 U+F8FF 2
	CR_CJK_Compatibility_Ideographs,			// U+F900 U+FAFF 472
	CR_Alphabetic_Presentation_Forms,			// U+FB00 U+FB4F 58
	CR_Arabic_Presentation_Forms_A,				// U+FB50 U+FDFF 611
	CR_Variation_Selectors,						// U+FE00 U+FE0F 16
	CR_Vertical_Forms,							// U+FE10 U+FE1F 10
	CR_Combining_Half_Marks,					// U+FE20 U+FE2F 7
	CR_CJK_Compatibility_Forms,					// U+FE30 U+FE4F 32
	CR_Small_Form_Variants,						// U+FE50 U+FE6F 26
	CR_Arabic_Presentation_Forms_B,				// U+FE70 U+FEFF 141
	CR_Halfwidth_and_Fullwidth_Forms,			// U+FF00 U+FFEF 225
	CR_Specials,								// U+FFF0 U+FFFF 5

	// Supplementary Multilingual Plane.
	CR_Linear_B_Syllabary,						// U+10000 U+1007F 88
	CR_Linear_B_Ideograms,						// U+10080 U+100FF 123
	CR_Aegean_Numbers,							// U+10100 U+1013F 57
	CR_Ancient_Greek_Numbers,					// U+10140 U+1018F 75
	CR_Ancient_Symbols,							// U+10190 U+101CF 12
	CR_Phaistos_Disc,							// U+101D0 U+101FF 46
	CR_Lycian,									// U+10280 U+1029F 29
	CR_Carian,									// U+102A0 U+102DF 49
	CR_Old_Italic,								// U+10300 U+1032F 35
	CR_Gothic,									// U+10330 U+1034F 27
	CR_Ugaritic,								// U+10380 U+1039F 31
	CR_Old_Persian,								// U+103A0 U+103DF 50
	CR_Deseret,									// U+10400 U+1044F 80
	CR_Shavian,									// U+10450 U+1047F 48
	CR_Osmanya,									// U+10480 U+104AF 40
	CR_Cypriot_Syllabary,						// U+10800 U+1083F 55
	CR_Imperial_Aramaic,						// U+10840 U+1085F 31
	CR_Phoenician,								// U+10900 U+1091F 29
	CR_Lydian,									// U+10920 U+1093F 27
	CR_Meroitic_Hieroglyphs,					// U+10980 U+1099F 32
	CR_Meroitic_Cursive,						// U+109A0 U+109FF 26
	CR_Kharoshthi,								// U+10A00 U+10A5F 65
	CR_Old_South_Arabian,						// U+10A60 U+10A7F 32
	CR_Avestan,									// U+10B00 U+10B3F 61
	CR_Inscriptional_Parthian,					// U+10B40 U+10B5F 30
	CR_Inscriptional_Pahlavi,					// U+10B60 U+10B7F 27
	CR_Old_Turkic,								// U+10C00 U+10C4F 73
	CR_Rumi_Numeral_Symbols,					// U+10E60 U+10E7F 31
	CR_Brahmi,									// U+11000 U+1107F 108
	CR_Kaithi,									// U+11080 U+110CF 66
	CR_Sora_Sompeng,							// U+110D0 U+110FF 35
	CR_Chakma,									// U+11100 U+1114F 67
	CR_Sharada,									// U+11180 U+111DF 83
	CR_Takri,									// U+11680 U+116CF 66
	CR_Cuneiform,								// U+12000 U+123FF 879
	CR_Cuneiform_Numbers_and_Punctuation,		// U+12400 U+1247F 103
	CR_Egyptian_Hieroglyphs,					// U+13000 U+1342F 1071
	CR_Bamum_Supplement,						// U+16800 U+16A3F 569
	CR_Miao,									// U+16F00 U+16F9F 133
	CR_Kana_Supplement,							// U+1B000 U+1B0FF 2
	CR_Byzantine_Musical_Symbols,				// U+1D000 U+1D0FF 246
	CR_Musical_Symbols,							// U+1D100 U+1D1FF 220
	CR_Ancient_Greek_Musical_Notation,			// U+1D200 U+1D24F 70
	CR_Tai_Xuan_Jing_Symbols,					// U+1D300 U+1D35F 87
	CR_Counting_Rod_Numerals,					// U+1D360 U+1D37F 18
	CR_Mathematical_Alphanumeric_Symbols,		// U+1D400 U+1D7FF 996
	CR_Arabic_Mathematical_Alphabetic_Symbols,	// U+1EE00 U+1EEFF 143
	CR_Mahjong_Tiles,							// U+1F000 U+1F02F 44
	CR_Domino_Tiles,							// U+1F030 U+1F09F 100
	CR_Playing_Cards,							// U+1F0A0 U+1F0FF 59
	CR_Enclosed_Alphanumeric_Supplement,		// U+1F100 U+1F1FF 171
	CR_Enclosed_Ideographic_Supplement,			// U+1F200 U+1F2FF 57
	CR_Miscellaneous_Symbols_And_Pictographs,	// U+1F300 U+1F5FF 533
	CR_Emoticons,								// U+1F600 U+1F64F 76
	CR_Transport_And_Map_Symbols,				// U+1F680 U+1F6FF 70
	CR_Alchemical_Symbols,						// U+1F700 U+1F77F 116

	// Supplementary Ideographic Plane.
	CR_CJK_Unified_Ideographs_Extension_B,		// U+20000 U+2A6DF 42711
	CR_CJK_Unified_Ideographs_Extension_C,		// U+2A700 U+2B73F 4149
	CR_CJK_Unified_Ideographs_Extension_D,		// U+2B740 U+2B81F 222
	CR_CJK_Compatibility_Ideographs_Supplement,	// U+2F800 U+2FA1F 542

	// Total range count.
	CR_RANGE_COUNT
};

struct CodepointRange
{
	static CodepointRange Get(CodepointRangeId id);

	codepoint32 first, last;
	const char* name;
};

}; // namespace builder