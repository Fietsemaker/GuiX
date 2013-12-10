#pragma once

#include <GuiX/Texture.h>

#include <Src/ResourceMap.h>

namespace guix {
namespace graphics {

// ===================================================================================
// GxTextureDatabaseImp
// ===================================================================================

class GxTextureDatabaseImp : public GxTextureDatabase
{
public:
	static GxTextureDatabaseImp* singleton;

	static void Create();
	static void Destroy();

	GxTextureDatabaseImp();
	~GxTextureDatabaseImp();
	
	int GetLoadedTextureCount() const;
	void LogInfo() const;

	bool Load(GxTextureHandle& outHandle, GxVec2i& outSize, const char* path_or_resource);
	bool Create(GxTextureHandle& outHandle, int width, int height, const uchar* pixeldata);
	void AddReference(GxTextureHandle handle);
	void Release(GxTextureHandle handle);

private:
	bool myLoadFile(GxTextureHandle& outHandle, GxVec2i& outSize, const char* path);

	typedef ResourceMap<GxTextureHandle, GxVec2i> TexMap;

	TexMap myMap;
};

}; // namespace graphics
}; // namespace guix