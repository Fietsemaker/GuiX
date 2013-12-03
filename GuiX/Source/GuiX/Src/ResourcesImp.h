#pragma once

#include <GuiX/Resources.h>

#include <map>

namespace guix {
namespace core {

// ===================================================================================
// GxResources
// ===================================================================================

class GxResourcesImp : public GxResources
{
public:
	static GxResourcesImp* singleton;

	static void Create();
	static void Destroy();

	GxResourcesImp();
	~GxResourcesImp();

	bool Load(const char* path);
	bool GetResource(const char* id, TextureRes& out);
	bool GetResource(const char* id, FontRes& out);
	bool GetResource(const char* id, TranslationsRes& out);

private:
	typedef std::map<GxString, TextureRes>      TexMap;
	typedef std::map<GxString, FontRes>         FntMap;
	typedef std::map<GxString, TranslationsRes> TrlMap;

	TexMap myTextures;
	FntMap myFonts;
	TrlMap myTranslations;
};

}; // namespace graphics
}; // namespace guix