/// @file
/// Contains several common graphics classes.

#pragma once

#include <GuiX/Texture.h>

namespace guix {
namespace graphics {

// ===================================================================================
// GxDraw
// ===================================================================================
/** The GxDraw class contains functions for different kinds of 2D drawing operations.

 The GxDraw class is a high level interface for 2D drawing operations. Internally the
 drawing operations generate vertices that are sent to the GxRenderInterface for
 rendering.
 
 GxDraw batches successive calls to functions that use the same render operation, such
 as drawing multiple lines or multiple textured quads with the same texture. Because
 of this, make sure \c Flush() is called at the end of every frame to complete drawing
 operations that might not have executed yet due to batching.

 @see GxRenderInterface, GxSprite, GxTileBar, GxTileRect
*/
class GUIX_API GxDraw
{
public:
	virtual ~GxDraw();

	/// Returns the draw singleton.
	static GxDraw* Get();

	/// Draws a filled rectangle with the top-left corner (x, y), width w and height h.
	virtual void Rect(int x, int y, int w, int h, GxColor c) = 0;

	/// Draws a filled rectangle with the top-left corner (x, y), width w and height h.
	/// The top vertices are set to color t, the bottom vertices are set to color b.
	virtual void Rect(int x, int y, int w, int h, GxColor t, GxColor b) = 0;

	/// Draws a filled rectangle with the top-left corner (x, y), width w and height h.
	/// The top-left, top-right, bottom-left and bottom-right vertices are set to the corresponding colors.
	virtual void Rect(int x, int y, int w, int h, GxColor tl, GxColor tr, GxColor bl, GxColor br) = 0;

	/// Draws a textured rectangle with the top-left corner (x, y), width w and height h.
	virtual void Rect(int x, int y, int w, int h, GxColor c, const GxTexture& texture) = 0;

	/// Allocates quadCount * 4 vertices for drawing textured or colored quads.
	/// The returned memory is guaranteed to be valid until GxDraw is flushed (manually 
	/// or by another drawing operation), which will draw and discard the vertices.
	virtual GxVertex* BatchQuads(int quadCount, GxTextureHandle texture) = 0;

	/// Allocates triangleCount * 3 vertices for drawing textured or colored triangles. 
	/// The returned memory is guaranteed to be valid until GxDraw is flushed (manually 
	/// or by another drawing operation), which will draw and discard the vertices.
	virtual GxVertex* BatchTriangles(int triangleCount, GxTextureHandle texture) = 0;

	/// Uses quadCount * 4 vertices to draw textured or colored quads.
	/// This drawing operation is always executed immediately.
	virtual void DrawQuads(const GxVertex* vertices, int quadCount, GxTextureHandle texture) = 0;

	/// Uses triangleCount * 3 vertices to draw textured or colored triangles.
	/// This drawing operation is always executed immediately.
	virtual void DrawTriangles(const GxVertex* vertices, int triangleCount, GxTextureHandle texture) = 0;

	/// Pushes a new scissor region on the scissor stack. The new scissor region is set to the
	/// intersection of the current scissor region and the given region.
	virtual void PushScissorRect(int x, int y, int width, int height) = 0;

	/// Pops a scissor region from the scissor stack. The active scissor region is restored to
	/// the state it was at the previous stack position.
	virtual void PopScissorRect() = 0;

	/// Returns the current scissor region, which is at the top of the scissor stack.
	virtual GxRecti GetScissorRect() = 0;

	/// Sets a new blend mode for drawing operations.
	/// available blend modes depend on the implementation of the GxRenderInterface.
	virtual void SetBlendMode(GxBlendMode blendMode = GX_BM_ALPHA) = 0;

	/// Completes any drawing operations that have not yet been executed due to batching.
	virtual void Flush() = 0;
};

}; // namespace graphics
}; // namespace guix