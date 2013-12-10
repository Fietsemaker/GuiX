/// @file
/// Contains the style class.

#pragma once

#include <GuiX/Draw.h>
#include <GuiX/Sprites.h>
#include <GuiX/Text.h>

namespace guix {
namespace gui {

// ===================================================================================
// GxStyle
// ===================================================================================
/** The GxStyle class contains colors, images and text settings for rendering widgets.
 
 The GxStyle class is a data structure that contains colors, images and text settings
 that are used to render widgets. The standard GuiXGui widgets are rendered using the
 data from a global style object, which can be accessed using GxStyle::Get()->

 To change the color scheme of a style, the application can call the \s Create()
 function with a either a standard color scheme or a custom color scheme. The style
 will then copy the color scheme and create images and text settings based on the
 that color scheme.

 @see GxWidget, GxContainer
*/
class GUIX_API GxStyle
{
public:
	~GxStyle();

	/// Enumeration of standard color schemes.
	enum Scheme
	{
		BLACK,
		WHITE,
	};

	/// Collection of colors that determine the look of the style.
	struct GUIX_API Colors
	{
		Colors();
		Colors(Scheme scheme);

		GxColor bgPanel;
		GxColor bgField;
		GxColor bgBar;
		GxColor bgTab;

		GxColor frameOutline;
		GxColor frameOutEmboss[2];
		GxColor frameInEmboss[2];
		GxColor frameInEmbossHl[2];
		GxColor frameInColor[2];
		GxColor frameInColorHl[2];
		GxColor fieldBorder[2];

		GxColor dockBg[2];
		GxColor dockHl;

		GxColor textColor;
		GxColor textSelect;
		GxColor textShadow;
		GxColor textLocked;
	};

	// Collection of text settings and images that are used for widget rendering.
	struct GUIX_API Data
	{
		GxText text[2];
		GxTexture hueMap;
		GxTexture dirDisc;
		GxTileRect bar, field, box;
		GxTileRect button[2];
		GxTileRect tab[3];
		GxTileRect dockFrame;
		GxTileBar dockBar;
		GxTileBar hlLine;
		GxSprite radio[3];
		GxSprite checkmark;
		GxSprite arrow;
		GxSprite grab;
		GxSprite close;
	};

	/// Returns the global style that is used to render widgets.
	static GxStyle* Get();

	/// Sets the global style that is used to render widgets.
	static void Set(const GxStyle& style);

	/// Creates a style from one of the standard schemes.
	GxStyle(Scheme scheme);

	/// Creates a style from a custom color scheme.
	GxStyle(const Colors& scheme);

	/// Updates the text settings based on the given font.
	void SetFont(const GxFont& font);

	/// Draws a label within rectangle r with the given properties.
	void Label(const GxRecti& r,  GxTextAlignH h, GxTextAlignV v, const GxString& text, bool lock);

	/// Draws a button within rectangle r with the given properties.
	void Button(const GxRecti& r, float hl, bool lock);

	/// Draws a radio button within rectangle r with the given properties.
	void Radio(const GxRecti& r, float hl, bool lock, bool selected);

	/// Draws a tab image within rectangle r with the given properties.
	void Tab(const GxRecti& r, bool hl, bool active);

	/// Draws a bar frame within rectangle r.
	void Bar(const GxRecti& r);

	/// Draws a field frame within rectangle r.
	void Field(const GxRecti& r);

	/// Draws a groupbox frame within rectangle r.
	void Box(const GxRecti& r);

	Colors c; ///< Contains color data for the style.
	Data d;   ///< Contains image and text data for the style.
};	

}; // namespace style
}; // namespace guix