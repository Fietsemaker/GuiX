/// @file
/// Contains the sprite, tile bar and tile rect class.

#pragma once

#include <GuiX/Texture.h>

namespace guix {
namespace graphics {

// ===================================================================================
// GxSprite
// ===================================================================================
/** The GxSprite class contains settings for drawing a 2D image.

 The GxSprite class contains a texture and several rendering settings such as size,
 rotation and color.

 @see GxTexture
*/
class GUIX_API GxSprite
{
public:
	// Enumeration of flags that can be set for drawing.
	enum Flags
	{
		NO_FLAGS = 0x00, ///< No flags set.
		MIRROR_H = 0x01, ///< Mirror the sprite horizontally.
		MIRROR_V = 0x02, ///< Mirror the sprite vertically.
	};

	/// Constructs a sprite with default settings.
	GxSprite();

	/// Constructs a sprite with default settings and the specified texture.
	GxSprite(const GxTexture& texture);

	/// Sets the sprite texture.
	/// The default texture is an empty texture.
	void SetTexture(const GxTexture& texture);

	/// Sets the sprite size to width and height; this affects the size of the drawn quad.
	/// The default size is set to the width and height of the texture.
	void SetSize(float width, float height);

	/// Sets the sprite origin to position (x, y) in pixels.
	/// When the sprite is drawn, it is positioned at and rotated around its origin.
	/// The default origin is (0, 0) which is the top-left corner of the sprite image.
	void SetOrigin(float x, float y);

	/// Sets the rotation of the sprite to angle degrees.
	/// The default rotation is 0 degrees.
	void SetRotation(float angle);

	/// Sets the rotation as a direction vector.
	/// The default direction is (1, 0).
	void SetRotation(float x, float y);

	/// Resets the rotation to (1, 0).
	void SetRotation();

	/// Enable one or more flags for drawing.
	void SetFlags(int flags = NO_FLAGS);

	/// Enables or disables the MIRROR_H flag.
	void SetMirrorH(bool enabled);

	/// Enables or disables the MIRROR_V flag.
	void SetMirrorV(bool enabled);

	/// Sets the sprite color to c.
	/// The default color is white.
	void SetColor(GxColor c);

	/// Draws the sprite with the origin at (x, y).
	void Draw(int x, int y);

	/// Draws the sprite with the origin at (x, y).
	void Draw(float x, float y);

	GxTexture& GetTexture();             ///< Returns the texture.
	const GxTexture& GetTexture() const; ///< Returns the texture.
	const GxVec2f& GetSize() const;      ///< Returns the size.
	const GxVec2f& GetOrigin() const;    ///< Returns the origin position.
	const GxVec2f& GetRotation() const;  ///< Returns the rotation as a direction vector.
	GxColor GetColor() const;            ///< Returns the color.
	GxFlags GetFlags() const;            ///< Returns the flags.

private:
	GxTexture myTexture;
	GxVec2f myOrigin, mySize;
	GxVec2f myRotation;
	GxColor myColor;
	GxFlags myFlags;
};

// ===================================================================================
// GxTileBar
// ===================================================================================
/** The GxTileBar class contains settings for drawing a horizontally tiling 2D image.

 The GxTileBar class contains a texture and several rendering settings for drawing an
 image that is tileable or stretchable in a horizontal direction. GxTiledBar can draw
 vertical bars as well, but the tiles in the source image are always laid out
 horizontally. The source image of a tiled bar consists of three tile regions:

 \verbatim

 +---+---+---+
 | L | C | R |
 +---+---+---+

 1. The left border   (fixed size)
 2. The center region (horizontal scaling)
 3. The right border  (fixed size)

  \endverbatim

 When drawing the tiled bar, the width of the center region is scaled to reach the
 size of the drawn rectangle. The image in the scaled region is either stretched
 or tiled, depending on the flags that are set.

 @see GxTileRect, GxTexture
*/
class GUIX_API GxTileBar
{
public:
	// Enumeration of flags that can be set for drawing.
	enum Flags
	{
		NO_FLAGS = 0x00, ///< No flags set.
		MIRROR_H = 0x01, ///< Mirror the image horizontally.
		MIRROR_V = 0x02, ///< Mirror the image vertically.
		TILING   = 0x04, ///< Repeats the image instead of stretching.
	};

	/// Constructs a tiled bar with default settings.
	GxTileBar();

	/// Constructs a tiled bar with default settings and the given texture.
	GxTileBar(const GxTexture& texture);

	/// Sets the tiled bar texture.
	/// The default texture is an empty texture.
	void SetTexture(const GxTexture& texture);

	/// Specifies, in pixels, the width of the borders in the source image.
	/// The default width for the border is 1/3 times the texture width.
	void SetBorderSize(float left, float right);

	/// Enable one or more flags for drawing.
	void SetFlags(int flags = NO_FLAGS);

	/// Enables or disables the MIRROR_H flag.
	void SetMirrorH(bool enabled);

	/// Enables or disables the MIRROR_V flag.
	void SetMirrorV(bool enabled);

	/// Enables or disables the TILING flag.
	void SetTiling(bool enabled);

	/// Sets the tiled bar color to c.
	/// The default color is white.
	void SetColor(GxColor c);

	/// Draws a horizontal bar with the top-left corner (x, y), width w.
	void DrawH(int x, int y, int w);

	/// Draws a horizontal bar with the top-left corner (x, y), width w and height h.
	void DrawH(int x, int y, int w, int h);

	/// Draws a horizontal bar with the top-left corner (x, y), width w and height h.
	void DrawH(float x, float y, float w, float h);

	/// Draws a vertical bar with the top-left corner (x, y), height h.
	void DrawV(int x, int y, int h);

	/// Draws a vertical bar with the top-left corner (x, y), width w and height h.
	void DrawV(int x, int y, int w, int h);

	/// Draws a vertical bar with the top-left corner (x, y), width w and height h.
	void DrawV(float x, float y, float w, float h);

	/// Draws a bar from position (x1, y1) to position (x2, y2) with thickness size.
	void DrawLine(int x1, int y1, int x2, int y2, int size);

	/// Draws a bar from position (x1, y1) to position (x2, y2) with thickness size.
	void DrawLine(float x1, float y1, float x2, float y2, float size);

	const GxTexture& GetTexture() const;    ///< Returns the texture.
	const GxVec2f& GetCenterRegion() const; ///< Returns the center tile region.
	const GxVec2f& GetSize() const;         ///< Returns the size.
	GxColor GetColor() const;               ///< Returns the color.
	GxFlags GetFlags() const;               ///< Returns the flags.

private:
	void myDraw(GxAreaf a, bool transpose);

	GxTexture myTexture;
	GxVec2f myCenterRegion;
	GxVec2f mySize;
	GxColor myColor;
	GxFlags myFlags;
};

// ===================================================================================
// GxTileRect
// ===================================================================================
/** The GxTileRect class contains settings for drawing a horizontally and vertically tiling 2D image.

 The GxTileRect very similar to the GxTileBar class, except that it is tileable or 
 stretchable in both horizontal and vertical direction. The source image of a tile
 rect consists of nine tile regions:

 \verbatim

 +----+---+----+
 | TL | T | TR |
 +----+---+----+
 | L  | C |  R |
 +----+---+----+
 | BL | B | BR |
 +----+---+----+

 1. The top-left corner     (fixed size)
 2. The top border          (horizontal scaling)
 3. The top-right corner    (fixed size)
 4. The left border         (vertical scaling)
 5. The center region       (horizontal and vertical scaling)
 6. The right border        (vertical scaling)
 7. The bottom-left corner  (fixed size)
 8. The bottom border       (horizontal scaling)
 9. The bottom-right corner (fixed size)

 \endverbatim

 All four corners are fixed size. The four borders scale horizontally or vertically to
 reach the size of the drawn rectangle. The center region is scaled both horizontally
 and vertically. The images in the scaling regions are either stretched or tiled,
 depending on the flags that are set.

 @see GxTileBar, GxTexture
*/
class GUIX_API GxTileRect
{
public:
	// Enumeration of flags that can be set for drawing.
	enum Flags
	{
		NO_FLAGS = 0x00, ///< No flags set.
		MIRROR_H = 0x01, ///< Mirror the image horizontally.
		MIRROR_V = 0x02, ///< Mirror the image vertically.
		TILING   = 0x04, ///< Repeats the image instead of stretching.
	};

	/// Constructs a tiled rect with default settings.
	GxTileRect();

	/// Constructs a tiled rect with default settings and the given texture.
	GxTileRect(const GxTexture& texture);

	/// Sets the tiled rect texture.
	/// The default texture is an empty texture.
	void SetTexture(const GxTexture& texture);

	/// Specifies, in pixels, the width or height of the borders in the source image.
	/// The default width for the left and right border is 1/3 times the texture width.
	/// The default height for the top and bottom border is 1/3 times the texture height.
	void SetBorderSize(float left, float top, float right, float bottom);

	/// Enable one or more flags for drawing.
	void SetFlags(int flags = 0);

	/// Enables or disables the MIRROR_H flag.
	void SetMirrorH(bool enabled);

	/// Enables or disables the MIRROR_V flag.
	void SetMirrorV(bool enabled);

	/// Enables or disables the TILING flag.
	void SetTiling(bool enabled);

	/// Sets the tiled bar color to c.
	/// The default color is white.
	void SetColor(GxColor c);

	/// Draws a horizontal rectangle with the top-left corner (x, y), width w and height h.
	void DrawH(int x, int y, int w, int h);
	
	/// Draws a horizontal rectangle with the top-left corner (x, y), width w and height h.
	void DrawH(float x, float y, float w, float h);

	/// Draws a vertical rectangle with the top-left corner (x, y), width w and height h.
	void DrawV(int x, int y, int w, int h);
	
	/// Draws a vertical rectangle with the top-left corner (x, y), width w and height h.
	void DrawV(float x, float y, float w, float h);

	/// Draws a rectangle from position (x1, y1) to position (x2, y2) with thickness size.
	void DrawLine(int x1, int y1, int x2, int y2, int size);
	
	/// Draws a rectangle from position (x1, y1) to position (x2, y2) with thickness size.
	void DrawLine(float x1, float y1, float x2, float y2, float size);

	const GxTexture& GetTexture() const;    ///< Returns the texture.
	const GxAreaf& GetCenterRegion() const;	///< Returns the center tile region.
	const GxVec2f& GetSize() const;			///< Returns the size.
	GxColor GetColor() const;				///< Returns the color.
	GxFlags GetFlags() const;               ///< Returns the flags.

private:
	void myDraw(GxAreaf a, bool transpose);

	GxTexture myTexture;
	GxAreaf myCenterRegion;
	GxVec2f mySize;
	GxColor myColor;
	GxFlags myFlags;
};

}; // namespace graphics
}; // namespace guix