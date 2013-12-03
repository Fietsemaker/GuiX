/// @file
/// Contains the canvas class.

#pragma once

#include <GuiX/Texture.h>

namespace guix {
namespace graphics {

// ===================================================================================
// GxCanvas
// ===================================================================================
/** The GxCanvas class contains a bitmap canvas which can be used to create a texture.

 The GxCanvas class provides functionality to draw high quality anti-aliased shapes on
 a bitmap canvas. After all drawing operations are completed, the \c CreateTexture()
 function is used to create a texture from the bitmap.

 @see GxTexture
*/
class GUIX_API GxCanvas
{
public:
	struct DistFunc;

	~GxCanvas();

	/// Enumeration of color blending modes for drawing on a canvas.
	/// These blending modes are not related to the blending modes of the render interface, and will always work.
	enum BlendMode
	{
		BM_NONE,  ///< No blending.
		BM_ALPHA, ///< Alpha blending.
		BM_ADD,   ///< Additive blending.
		BM_SUB,   ///< Subtractive blending.
		BM_MUL,   ///< Multiplicative blending.
	};

	/// Constructs a canvas with w horizontal pixels and h vertical pixels.
	GxCanvas(int w, int h);

	/// Sets the entire bitmap to color c.
	void Clear(GxColor c);

	/// Sets the line thickness in pixels that is used for drawing outlined shapes.
	void SetOutline(float pixels);

	/// Setting glow to a value greater than zero will give shapes an outer glow.
	void SetOuterGlow(float pixels);

	/// Setting glow to a value greater than zero will give outline shapes an inner glow.
	void SetInnerGlow(float pixels);

	/// Enables or disables fill; when fill is disabled, shapes are drawn as outlines.
	void SetFill(bool enabled);

	/// Set the color blending mode that is used in drawing operations on the canvas.
	void SetBlendMode(BlendMode bm);

	/// Sets the drawing color to c.
	void SetColor(GxColor c);

	/// Sets the drawing color to the given RGBA values.
	void SetColor(float r, float g, float b, float a);

	/// Sets the drawing color to a vertical gradient that interpolates from top to bottom.
	void SetColor(GxColor top, GxColor bottom);

	/// Sets the drawing color to a gradient that interpolates between four corner colors.
	void SetColor(GxColor topLeft, GxColor topRight, GxColor bottomLeft, GxColor bottomRight);

	/// Draws a line from point (x1, y1) to point (x2, y2) of thickness width.
	void Line(float x1, float y1, float x2, float y2, float width);

	/// Draws a circle at point (x, y) with the specified radius.
	void Circle(float x, float y, float radius);

	/// Draws a rectangle with (x1, y1) as top-left and (x2, y2) as bottom-right coordinate.
	void Rect(float x1, float y1, float x2, float y2);

	/// Draws a rectangle with (x1, y1) as top-left and (x2, y2) as bottom-right coordinate.
	/// Setting radius to a value greater than zero will round the corners of the rectangle.
	void Rect(float x1, float y1, float x2, float y2, float radius);

	/// Draws a polygon with the given vertices.
	void Polygon(GxVec2f* vertices, int vertexCount);

	/// Uses the current bitmap to generate a texture. After creation, the texture no
	/// longer connected to the canvas and the canvas can be destroyed or drawn on again.
	GxTexture CreateTexture();

	int GetWidth() const;           ///< Returns the number of horizontal pixels.
	int GetHeight() const;          ///< Returns the number of vertical pixels.
	uchar* GetBitmap();             ///< Returns the bitmap as an array of 8-bit RGBA values.
	const uchar* GetBitmap() const; ///< Returns the bitmap as an array of 8-bit RGBA values.

private:
	void myDrawOp(const GxAreaf& area, DistFunc* func);

	int myW, myH;
	uchar* myBitmap;
	GxColor myCTL, myCTR;
	GxColor myCBL, myCBR;
	float myOuterGlow;
	float myInnerGlow;
	float myLineW;
	BlendMode myBlendMode;
	bool myHasFill;
};

}; // namespace graphics
}; // namespace guix