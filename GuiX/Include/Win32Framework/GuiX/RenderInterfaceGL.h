#pragma once

#include <GuiX/Interfaces.h>

namespace guix {
namespace framework {

// ===================================================================================
// GxRenderInterfaceGL
// ===================================================================================

class GUIX_API GxRenderInterfaceGL : public GxRenderInterface
{
public:
	GxRenderInterfaceGL();

	void SetViewSize(GxVec2i size);
	GxVec2i GetViewSize();

	void DrawTriangles(const GxVertex* vertices, int vertexCount, GxTextureHandle texture);
	void DrawTriangles(const GxVertex* vertices, int vertexCount, const uint* indices, int indexCount, GxTextureHandle texture);

	void SetBlendMode(GxBlendMode blendMode);
	void EnableScissorRect(bool enable);
	void SetScissorRect(int x, int y, int width, int height);

	bool LoadTexture(GxTextureHandle& outTexture, int& outWidth, int& outHeight, const char* path);
	bool GenerateTexture(GxTextureHandle& outTexture, int width, int height, const uchar* pixeldata);
	void ReleaseTexture(GxTextureHandle texture);

private:
	GxVec2i myViewSize;
};

}; // namespace framework
}; // namespace guix