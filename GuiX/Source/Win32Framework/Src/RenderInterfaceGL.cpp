#include <GuiX/Config.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Gl/gl.h>
#include <stdio.h>

#include <GuiX/Core.h>
#include <GuiX/RenderInterfaceGl.h>

#include "stb_image.h"

namespace guix {
namespace framework {

// ===================================================================================
// stb_image callbacks
// ===================================================================================

namespace {

static int fileRead(void* user, char* data, int size)
{
	GxFileInterface* io = GxFileInterface::Get();
	GxFileHandle* handle = (GxFileHandle*)user;

	return io->Read(*handle, data, size); 
}

static void fileSkip(void* user, unsigned int n)
{
	GxFileInterface* io = GxFileInterface::Get();
	GxFileHandle* handle = (GxFileHandle*)user;

	io->Seek(*handle, n, GxFileInterface::SeekCur());
}

static int fileEof(void* user)
{
	GxFileInterface* io = GxFileInterface::Get();
	GxFileHandle* handle = (GxFileHandle*)user;

	return (io->EndOfFile(*handle) ? 1 : 0);
}

}; // anonymous namespace.

// ===================================================================================
// GxRenderInterfaceGL
// ===================================================================================

GxRenderInterfaceGL::GxRenderInterfaceGL()
	:myViewSize(640, 480)
{
}

void GxRenderInterfaceGL::SetViewSize(GxVec2i size)
{
	myViewSize = size;
}

GxVec2i GxRenderInterfaceGL::GetViewSize()
{
	return myViewSize;
}

void GxRenderInterfaceGL::DrawTriangles(const GxVertex* vertices, int vertexCount, GxTextureHandle texture)
{
	glVertexPointer(2, GL_FLOAT, sizeof(GxVertex), &vertices[0].pos);
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(GxVertex), &vertices[0].color);

	if(!texture)
	{
		glDisable(GL_TEXTURE_2D);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
	else
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, (GLuint)texture);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, sizeof(GxVertex), &vertices[0].uvs);
	}

	glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}

void GxRenderInterfaceGL::DrawTriangles(const GxVertex* vertices, int vertexCount, const uint* indices, int indexCount, GxTextureHandle texture)
{
	glVertexPointer(2, GL_FLOAT, sizeof(GxVertex), &vertices[0].pos);
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(GxVertex), &vertices[0].color);

	if(!texture)
	{
		glDisable(GL_TEXTURE_2D);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
	else
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, (GLuint)texture);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, sizeof(GxVertex), &vertices[0].uvs);
	}

	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, indices);
}
	
void GxRenderInterfaceGL::EnableScissorRect(bool enable)
{
	if (enable)
		glEnable(GL_SCISSOR_TEST);
	else
		glDisable(GL_SCISSOR_TEST);
}

void GxRenderInterfaceGL::SetScissorRect(int x, int y, int width, int height)
{
	glScissor(x, myViewSize.y - (y + height), width, height);
}

bool GxRenderInterfaceGL::LoadTexture(GxTextureHandle& outTexture, int& outWidth, int& outHeight, const char* path)
{
	outWidth = outHeight = 0;

	// Try to open the image file.
	GxFileInterface* io = GxFileInterface::Get();
	GxFileHandle file = io->Open(path);
	if(!file) return false;

	// Process the file using stb_image.
	stbi_io_callbacks callbacks;
	callbacks.read = fileRead;
	callbacks.skip = fileSkip;
	callbacks.eof = fileEof;

	int cfmt, w, h;
	uchar* bits = (uchar*)stbi_load_from_callbacks(&callbacks, &file, &w, &h, &cfmt, STBI_rgb_alpha);
	const char* reason = stbi_failure_reason();
	io->Close(file);
	if(!bits) return false;

	// If the loading succeeded, we fill in the output data.
	bool result = GenerateTexture(outTexture, w, h, bits);
	if(result)
	{
		outWidth = w;
		outHeight = h;
	}
	stbi_image_free(bits);
	return result;
}

bool GxRenderInterfaceGL::GenerateTexture(GxTextureHandle& outTexture, int width, int height, const uchar* pixeldata)
{
	GLuint texture_id = 0;
	glGenTextures(1, &texture_id);
	if(!texture_id)	return false;

	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixeldata);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	outTexture = (GxTextureHandle)texture_id;

	return true;
}
		
void GxRenderInterfaceGL::ReleaseTexture(GxTextureHandle texture_handle)
{
	glDeleteTextures(1, (GLuint*) &texture_handle);
}

static void SetBlendFunc(uint src, uint dst)
{
	glEnable(GL_BLEND);
	glBlendFunc(src, dst);
}

void GxRenderInterfaceGL::SetBlendMode(GxBlendMode blendMode)
{
	switch(blendMode)
	{
		case GX_BM_NONE:     glDisable(GL_BLEND); break;
		case GX_BM_ALPHA:    SetBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); break;
		case GX_BM_ADD:      SetBlendFunc(GL_SRC_ALPHA, GL_ONE                ); break;
		case GX_BM_MULTIPLY: SetBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA); break;
		default:          SetBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); break; 
	};
}

}; // namespace framework
}; // namespace guix