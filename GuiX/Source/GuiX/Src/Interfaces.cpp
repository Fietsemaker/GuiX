#include <GuiX/Config.h>

#include <stdarg.h>
#include <stdio.h>

#include <GuiX/Interfaces.h>

#include <Src/CoreImp.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400
#pragma warning(disable: 4996) // vsnprintf and fopen warnings.
#endif

namespace guix {
namespace core {

// ===================================================================================
// GxLog
// ===================================================================================

namespace {

static const size_t LOG_BUFFER_SIZE = 512;

inline void SprintV(char* buffer, const char* fmt, va_list va)
{
	int len = vsnprintf(buffer, LOG_BUFFER_SIZE-1, fmt, va);	
	if(len < 0 || len > LOG_BUFFER_SIZE-1) len = LOG_BUFFER_SIZE-1;
	buffer[len] = 0;
}

inline void SprintF(char* buffer, const char* fmt, ...)
{
	va_list argument_list;
	va_start(argument_list, fmt);
	SprintV(buffer, fmt, argument_list);
	va_end(argument_list);
}

}; // anonymous namespace

void GxLog(const char* tag, GxLogType type, const char* fmt, ...)
{
	char buffer[LOG_BUFFER_SIZE];

	va_list argument_list;
	va_start(argument_list, fmt);
	SprintV(buffer, fmt, argument_list);
	va_end(argument_list);

	char composite[LOG_BUFFER_SIZE];

	SprintF(composite, "%-13s: %s", tag, buffer);
	GxSystemInterface::Get()->LogMessage(type, composite);
}

void GxLog(const char* fmt, ...)
{
	char buffer[LOG_BUFFER_SIZE];

	va_list argument_list;
	va_start(argument_list, fmt);
	SprintV(buffer, fmt, argument_list);
	va_end(argument_list);

	GxSystemInterface::Get()->LogMessage(GX_LT_INFO, buffer);
}

// ===================================================================================
// GxFileInterface
// ===================================================================================

int GxFileInterface::SeekSet() { return SEEK_SET; }
int GxFileInterface::SeekEnd() { return SEEK_END; }
int GxFileInterface::SeekCur() { return SEEK_CUR; }

GxFileInterface::~GxFileInterface()
{
}

GxFileInterface* GxFileInterface::Get()
{
	return GxCoreImp::fileInterface;
}

// ===================================================================================
// GxFileInterfaceStd
// ===================================================================================

GxFileHandle GxFileInterfaceStd::Open(const char* path)
{
	return reinterpret_cast<GxFileHandle>(fopen(path, "rb"));
}

void GxFileInterfaceStd::Close(GxFileHandle file)
{
	fclose(reinterpret_cast<FILE*>(file));
}

size_t GxFileInterfaceStd::Read(GxFileHandle file, void* buffer, size_t size)
{
	return fread(buffer, 1, size, reinterpret_cast<FILE*>(file));
}

bool GxFileInterfaceStd::Seek(GxFileHandle file, long offset, int origin)
{
	return fseek(reinterpret_cast<FILE*>(file), offset, origin) == 0;
}

size_t GxFileInterfaceStd::Tell(GxFileHandle file)
{
	return ftell(reinterpret_cast<FILE*>(file));
}

bool GxFileInterfaceStd::EndOfFile(GxFileHandle file)
{
	return (feof(reinterpret_cast<FILE*>(file)) != 0);
}

// ===================================================================================
// GxSystemInterface
// ===================================================================================

GxSystemInterface::~GxSystemInterface()
{
}

GxSystemInterface* GxSystemInterface::Get()
{
	return GxCoreImp::systemInterface;
}

void GxSystemInterface::LogMessage(GxLogType logtype, const char* message)
{
}

bool GxSystemInterface::GetClipboardText(GxString& text)
{
	return false;
}

void GxSystemInterface::SetClipboardText(const GxString& text)
{
}

// ===================================================================================
// GxRenderInterface
// ===================================================================================

GxRenderInterface::~GxRenderInterface()
{
}

GxRenderInterface* GxRenderInterface::Get()
{
	return GxCoreImp::renderInterface;
}

GxVec2i GxRenderInterface::GetViewSize()
{
	return GxVec2i(640, 480);
}

void GxRenderInterface::DrawTriangles(const GxVertex* vertices, int vertexCount, GxTextureHandle texture)
{
}

void GxRenderInterface::DrawTriangles(const GxVertex* vertices, int vertexCount, const uint* indices, int indexCount, GxTextureHandle texture)
{
}

void GxRenderInterface::SetBlendMode(GxBlendMode blendMode)
{
}

void GxRenderInterface::EnableScissorRect(bool enable)
{
}

void GxRenderInterface::SetScissorRect(int x, int y, int width, int height)
{
}

bool GxRenderInterface::LoadTexture(GxTextureHandle& outTexture, int& outWidth, int& outHeight, const char* path)
{
	outTexture = 0;
	outWidth = 0;
	outHeight = 0;
	return false;
}

bool GxRenderInterface::GenerateTexture(GxTextureHandle& outTexture, int width, int height, const uchar* pixeldata)
{
	outTexture = 0;
	return false;
}

void GxRenderInterface::ReleaseTexture(GxTextureHandle texture)
{
}

}; // namespace core
}; // namespace guix
