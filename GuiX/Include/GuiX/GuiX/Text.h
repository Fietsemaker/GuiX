/// @file
/// Contains classes for rendering 2D text.

#pragma once

#include <GuiX/String.h>
#include <GuiX/Texture.h>

namespace guix {
namespace graphics {

/// Handle to fonts loaded by the font database.
typedef GxHandle GxFontHandle;

// Enumeration of flags that can be set for rendering text.
enum GxTextFlags
{
	GX_TF_NO_FLAGS   = 0x00, ///< No flags.
	GX_TF_WORD_WRAP  = 0x01, ///< Lines are split at word boundaries instead of character boundaries.
	GX_TF_JUSTIFIED  = 0x02, ///< Lines of text with soft breaks are justified by scaling whitespace.
	GX_TF_ELLIPSIS   = 0x04, ///< Disables line wrapping and omits text at the end of a line with ellipsis.
	GX_TF_FORMATTING = 0x08, ///< Enables the processing of formatting tags embedded in the text.
	GX_TF_SINGLELINE = 0x10, ///< All text is forced on a single line; newline characters are ignored.
	GX_TF_KERNING    = 0x20, ///< Kerning is applied to glyph advances, if available in the font.
};

/// Enumeration of horizontal alignment types.
enum GxTextAlignH 
{
	GX_TA_LEFT   = 0, ///< Left-side aligned.
	GX_TA_CENTER = 1, ///< Horizontally centered.
	GX_TA_RIGHT  = 2, ///< Right-side aligned.
};

/// Enumeration of vertical alignment types.
enum GxTextAlignV
{
	GX_TA_TOP    = 0, ///< Top-side aligned.
	GX_TA_MIDDLE = 1, ///< Vertically centered.
	GX_TA_BOTTOM = 2, ///< Bottom-side aligned.
};

// ===================================================================================
// GxFont
// ===================================================================================
/** The GxFont class contains a handle to a GuiX font.

 The GxFont class contains a handle to a GuiX font object. Empty fonts or invalid
 fonts have a handle that is zero. Loaded fonts are tracked internally by TidyText in
 a database to avoid duplicates. Instances of the GxFont class are merely references
 to a GuiX font object; it is safe and cheap to copy them or pass them as arguments.
*/
class GUIX_API GxFont
{
public:
	~GxFont();

	/// Constructs an empty font with a zero handle.
	GxFont();

	/// Loads a font from either a font file, or the ID of an entry in the font database.
	GxFont(const char* path);

	/// Copies the data of another GxFont; the font handle is implicitly shared.
	GxFont(const GxFont& other);

	/// Loads a font from either a font file, or a font ID in the font database.
	bool Create(const char* path);

	/// Releases the current font handle.
	void Destroy();

	/// Copies the data of another font; the font handle is implicitly shared.
	void Assign(const GxFont& other);

	/// Stores a copy of this font as the global default font,
	/// which is used when text is drawn with an empty or invalid font.
	void MakeDefault();

	GxFontHandle GetHandle() const; ///< Returns the font handle.
	int GetSize() const;            ///< Returns the font size.

	GxFont& operator = (const GxFont& other); ///< Calls \c Assign().

private:
	GxFontHandle myHandle;
};

// ===================================================================================
// GxFontDatabase
// ===================================================================================
/** The GxFontDatabase class stores and keeps track of loaded fonts.

 GxFont objects use the GxFontDatabase singleton to load fonts. The font database
 keeps track of which fonts are already loaded in order to avoid loading duplicate
 fonts. It also keeps a reference count on every font; fonts that are no longer used
 by any GxFont object are released automatically.

 @see GxFont
*/
class GUIX_API GxFontDatabase
{
public:
	virtual ~GxFontDatabase();

	/// Returns the font database singleton.
	static GxFontDatabase* Get();

	/// Adds a custom glyph that can be used in formatted text by using the tag {ID},
	/// where ID corresponds to the given id argument.
	virtual void AddGlyph(const char* id, const GxTexture& img, int dy=0, int dx=0, int advance=0) = 0;

	/// Returns the number currently loaded fonts.
	virtual int GetLoadedFontCount() const = 0;

	/// GxLogs info about the loaded fonts.
	virtual void LogInfo() const = 0;
};

// ===================================================================================
// GxText
// ===================================================================================
/** The GxText class contains settings and functions for 2D text rendering.

 The GxText class is a high level interface for 2D text rendering. It computes the
 text layout based on the provided text settings and then uses GxDraw to render the 
 glyphs as a series of textured quads. Besides rendering text, the class also has 
 several functions for retrieving information about the layout of text.

 @see GxFont, GxDraw
*/
class GUIX_API GxText
{
public:
	static const int npos(); ///< An arbitrary large value; used to indicate end-of-range.

	GxText(); ///< Constructs a text object with default settings, see individual members.

	/// Draws a string of text at position (x,y).
	/// Returns the area of the drawn text.
	GxRecti Draw(int x, int y, GxStringArg text) const;

	/// Draws highlight rectangles for a string of text at position (x,y).
	/// Use the \c SetRange() function to set the start and end of the highlight range.
	/// Returns the area of the drawn text.
	GxRecti DrawHighlight(int x, int y, GxStringArg text) const;

	/// Returns the area of the drawn text, if it were drawn using \c Draw().
	GxRecti GetTextRect(int x, int y, GxStringArg text) const;

	/// Returns the area of the drawn glyph for the character at index, if it were drawn using \c Draw().
	GxRecti GetCharRect(int x, int y, int index, GxStringArg text) const;

	/// Returns the index of the character that would be at position (cx,cy) if it were drawn using \c Draw().
	int GetCharIndex(int x, int y, int cx, int cy, GxStringArg text) const;

 // Utility functions for setting properties:

	/// Sets both the top and bottom text colors.
	void SetColor(GxColor c = GxColor());

	/// Sets the maximum width of the lines of text.
	void SetMaxWidth(int width = -1);

	/// Sets the range of characters that will be visible when drawing.
	void SetRange(int begin = 0, int end = npos());

	/// Sets both the horizontal and vertical text alignment.
	void SetAlign(GxTextAlignH h = GX_TA_LEFT, GxTextAlignV v = GX_TA_TOP);

	/// Sets one of the text layout flags.
	void SetFlag(GxTextFlags flag, bool state = true);

	/// Returns true if the given text layout flag is set.
	bool HasFlag(GxTextFlags flag) const;

 // Public members, all settings can be modified directly:

	GxFont font;         ///< The font used for drawing text, default is empty font.
	GxColor shadow;      ///< Color of the text shadow, default is empty (no shadow). 
	GxColor top;         ///< Color of the top vertices of every drawn glyph, default is white.
	GxColor bottom;      ///< Color of the bottom vertices of every drawn glyph, default is white.
	int flags;           ///< Flags that determine the text layout, default is TT_WORD_WRAP and TT_FORMATTING.
	int maxWidth;        ///< Maximum width of the lines of text, default is -1 (no maximum width).
	int rangeBegin;      ///< Index of the first character that will be visible when drawing, default is 0.
	int rangeEnd;        ///< Index of the first character that will no longer be visible when drawing, default is npos.
	float tabWidth;      ///< Width in pixels of each column used for tab alignment, default is 96.
	GxTextAlignH alignH; ///< Horizontal alignment of the text rectangle and the text lines, default is TT_LEFT.
	GxTextAlignV alignV; ///< Vertical alignment of the text rectangle and the lines, default is TT_TOP.
};

}; // namespace graphics
}; // namespace guix