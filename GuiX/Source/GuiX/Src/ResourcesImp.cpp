#include <GuiX/Config.h>
#include <GuiX/Interfaces.h>

#include <Src/ResourcesImp.h>
#include <Src/Xml.h>

namespace guix {
namespace core {

namespace {

static const char* LOG_TAG = "Resources";

static const char* XML_RESOURCES = "resources";
static const char* XML_RES_TEX   = "texture";
static const char* XML_RES_FNT   = "font";
static const char* XML_RES_TRL   = "translations";

}; // anonymous namespace

// ===================================================================================
// GxResourcesImp
// ===================================================================================

GxResourcesImp* GxResourcesImp::singleton = NULL;

GxResources::~GxResources()
{
}

GxResources* GxResources::Get()
{
	return GxResourcesImp::singleton;
}

void GxResourcesImp::Create()
{
	singleton = new GxResourcesImp();
}

void GxResourcesImp::Destroy()
{
	delete singleton;
	singleton = NULL;
}

GxResourcesImp::GxResourcesImp()
{
}

GxResourcesImp::~GxResourcesImp()
{
}

bool GxResourcesImp::Load(const char* path)
{
	// Load the XML document.
	XmlDocument doc;
	if(!doc.Load(path))
	{
		GxLog(LOG_TAG, GX_LT_ERROR, "Failed to open resources file \"%s\"", path);
		return false;
	}

	// Check if the XML sturcture has a resources element.
	XmlElem* resources = doc.ChildElem(XML_RESOURCES);
	if(!resources)
	{
		GxLog(LOG_TAG, GX_LT_ERROR, "Resources file \"%s\" does not have a <%s> node", path, XML_RESOURCES);
		return false;
	}

	// Load texture resources.
	XmlElem* n = resources->ChildElem(XML_RES_TEX);
	for(; n; n = n->NextElem(XML_RES_TEX))
	{
		TextureRes res;
		GxString id = n->GetAttribute("id");
		XmlElem* path = n->ChildElem("path");
		if(path) res.path = path->GetText();
		myTextures[id] = res;
	}

	// Load font resources.
	n = resources->ChildElem(XML_RES_FNT);
	for(; n; n = n->NextElem(XML_RES_FNT))
	{
		FontRes res;
		GxString id = n->GetAttribute("id");
		XmlElem* path = n->ChildElem("path");
		if(path) res.path = path->GetText();
		myFonts[id] = res;
	}

	// Load translations resources.
	n = resources->ChildElem(XML_RES_TRL);
	for(; n; n = n->NextElem(XML_RES_TRL))
	{
		TranslationsRes res;
		GxString id = n->GetAttribute("id");

		XmlElem* xmlpath = n->ChildElem("path");
		while(xmlpath)
		{
			res.paths.Append(xmlpath->GetText());
			xmlpath = xmlpath->NextElem("path");
		}

		myTranslations[id] = res;
	}

	// Loading succesfully completes.
	GxLog(LOG_TAG, GX_LT_INFO, "Loaded \"%s\"", path);
	return true;
}

bool GxResourcesImp::GetResource(const char* id, TextureRes& out)
{
	TexMap::const_iterator it = myTextures.find(id);
	bool valid = (it != myTextures.end());
	if(valid) out = it->second;
	return valid;
}

bool GxResourcesImp::GetResource(const char* id, FontRes& out)
{
	FntMap::const_iterator it = myFonts.find(id);
	bool valid = (it != myFonts.end());
	if(valid) out = it->second;
	return valid;
}

bool GxResourcesImp::GetResource(const char* id, TranslationsRes& out)
{
	TrlMap::const_iterator it = myTranslations.find(id);
	bool valid = (it != myTranslations.end());
	if(valid) out = it->second;
	return valid;
}

}; // namespace core
}; // namespace guix