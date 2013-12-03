#pragma once

#include <GuiX/List.h>
#include <GuiX/String.h>

namespace guix {
namespace core {

// ===================================================================================
// GxResources
// ===================================================================================

class GxResources
{
public:
	/// Texture resource, contains information for loading a texture.
	struct TextureRes
	{
		GxString path;
	};

	/// Font resource, contains information for loading a font.
	struct FontRes
	{
		GxString path;
	};

	/// Translation resource, contains information for loading translation files.
	struct TranslationsRes
	{
		GxList<GxString> paths;
	};

	virtual ~GxResources();

	/// Returns the resources singleton.
	static GxResources* Get();

	/// Loads a resource configuration file.
	virtual bool Load(const char* path) = 0;

	/// Retrieves a texture resource; returns false if the resource does not exist.
	virtual bool GetResource(const char* id, TextureRes& out) = 0;

	/// Retrieves a font resource; returns false if the resource does not exist.
	virtual bool GetResource(const char* id, FontRes& out) = 0;

	/// Retrieves a translations resource; returns false if the resource does not exist.
	virtual bool GetResource(const char* id, TranslationsRes& out) = 0;
};

}; // namespace core
}; // namespace guix