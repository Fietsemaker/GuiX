#pragma once

#include <GuiX/Config.h>

#include <vector>

#include <GuiX/Draw.h>

namespace guix {
namespace graphics {

class GxDrawImp : public GxDraw
{
public:
	static GxDrawImp* singleton;

	static void Create();
	static void Destroy();

	enum GxDrawOp 
	{
		DRAWOP_NONE,
		DRAWOP_TRIS,
		DRAWOP_QUADS,
	};

	GxDrawImp();
	~GxDrawImp();

	// Rectangle drawing
	void Rect(int x, int y, int w, int h, GxColor c);
	void Rect(int x, int y, int w, int h, GxColor t, GxColor b);
	void Rect(int x, int y, int w, int h, GxColor tl, GxColor tr, GxColor bl, GxColor br);
	void Rect(int x, int y, int w, int h, GxColor c, const GxTexture& texture);

	// GxVertex geometry
	GxVertex* BatchTriangles(int triangleCount, GxTextureHandle texture);
	GxVertex* BatchQuads(int quadCount, GxTextureHandle texture);
	
	void DrawTriangles(const GxVertex* vertices, int triangleCount, GxTextureHandle texture);
	void DrawQuads(const GxVertex* vertices, int quadCount, GxTextureHandle texture);

	// View operations
	void PushScissorRect(int x, int y, int width, int height);
	void PopScissorRect();
	GxRecti GetScissorRect();

	// Blend mode
	void SetBlendMode(GxBlendMode blendMode);

	// Misc
	void Flush();

private:
	void myResizeIndexBuffer();
	GxTextureHandle myCurrentTexture;

	std::vector<uint> myIndices;
	std::vector<GxVertex> myVertices;
	std::vector<GxRecti> myScissorStack;
	uint myNumIndices, myNumVertices;

	GxDrawOp myCurrentOp;
};

}; // namespace graphics
}; // namespace guix