#include <GuiX/Config.h>

#include <Src/CoreImp.h>
#include <Src/DrawImp.h>
#include <Src/InputImp.h>
#include <Src/TextImp.h>
#include <Src/TextureImp.h>
#include <Src/LocalizeImp.h>
#include <Src/ResourcesImp.h>

#include <Src/StyleImp.h>
#include <Src/WidgetDatabase.h>

namespace guix {
namespace core {

namespace {

static const char* LOG_TAG = "Core";

}; // anonymous namespace

// ===================================================================================
// GxCore
// ===================================================================================

void GxCore::Initialize()
{
	if(GxCoreImp::isInitialized)
		return;

	GxLog(LOG_TAG, GX_LT_INFO, "Initializing...");

	GxInputImp::Create();
	GxResourcesImp::Create();
	GxTextureDatabaseImp::Create();
	GxFontDatabaseImp::Create();
	GxLocalizeImp::Create();
	GxDrawImp::Create();
	GxTextRenderer::Create();
	GxWidgetDatabase::Create();
	GxStyleImp::Create();

	GxCoreImp::isInitialized = true;
}

void GxCore::Shutdown()
{
	if(!GxCoreImp::isInitialized)
		return;

	GxLog(LOG_TAG, GX_LT_INFO, "Shutting down...");

	GxStyleImp::Destroy();
	GxWidgetDatabase::Destroy();
	GxTextRenderer::Destroy();
	GxDrawImp::Destroy();
	GxLocalizeImp::Destroy();
	GxFontDatabaseImp::Destroy();
	GxTextureDatabaseImp::Destroy();
	GxResourcesImp::Destroy();
	GxInputImp::Destroy();

	SetRenderInterface();
	SetFileInterface();
	SetSystemInterface();

	GxCoreImp::isInitialized = false;
}

void GxCore::SetRenderInterface(GxRenderInterface* renderInterface)
{
	if(!renderInterface)
		renderInterface = &GxCoreImp::defaultRenderInterface;

	GxCoreImp::renderInterface = renderInterface;
}

void GxCore::SetFileInterface(GxFileInterface* fileInterface)
{
	if(!fileInterface)
		fileInterface = &GxCoreImp::defaultFileInterface;

	GxCoreImp::fileInterface = fileInterface;
}

void GxCore::SetSystemInterface(GxSystemInterface* systemInterface)
{
	if(!systemInterface)
		systemInterface = &GxCoreImp::defaultSystemInterface;

	GxCoreImp::systemInterface = systemInterface;
}

// ===================================================================================
// GxCoreImp
// ===================================================================================

bool GxCoreImp::isInitialized = false;

GxRenderInterface     GxCoreImp::defaultRenderInterface;
GxFileInterfaceStd    GxCoreImp::defaultFileInterface;
GxSystemInterface     GxCoreImp::defaultSystemInterface;

GxRenderInterface*    GxCoreImp::renderInterface = &GxCoreImp::defaultRenderInterface;
GxFileInterface*      GxCoreImp::fileInterface   = &GxCoreImp::defaultFileInterface;
GxSystemInterface*    GxCoreImp::systemInterface = &GxCoreImp::defaultSystemInterface;

}; // namespace core
}; // namespace guix