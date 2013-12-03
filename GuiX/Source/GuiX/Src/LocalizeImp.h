#pragma once

#include <GuiX/Config.h>

#include <map>

#include <GuiX/Localize.h>

#include <Src/Xml.h>

namespace guix {
namespace core {

// ===================================================================================
// GxLocalize
// ===================================================================================

class GxLocalizeImp : public GxLocalize
{
public:
	static GxLocalizeImp* singleton;

	static void Create();
	static void Destroy();

	GxLocalizeImp();
	~GxLocalizeImp();

	void Clear();
	bool Load(const char* path_or_resource);

	GxString Translate(const char* name);
	GxString Translate(const char* category, const char* name);

	void LogInfo() const;

private:
	typedef std::map<GxString, GxString> TranslationMap;
	typedef std::map<GxString, TranslationMap> CategoryMap;
	
	bool myLoadFile(const char* path);
	void myLoadDocument(XmlDocument* doc);
	void myParseElementCategory(XmlElem* element);
	void myParseElementTL(const GxString& category, XmlElem* element);
	void myAddTranslation(const GxString& category, const GxString& name, const GxString& text);

	CategoryMap myCategories;
};

}; // namespace core
}; // namespace guix