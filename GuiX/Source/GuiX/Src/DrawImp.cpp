#include <GuiX/Config.h>

#include <vector>

#include <GuiX/Core.h>

#include <Src/DrawImp.h>

namespace guix {
namespace graphics {

namespace {

static const char* LOG_TAG = "Draw";

}; // anonymous namespace

// ===================================================================================
// GxDraw
// ===================================================================================

GxDrawImp* GxDrawImp::singleton = NULL;

GxDraw::~GxDraw()
{
}

GxDraw* GxDraw::Get()
{
	return GxDrawImp::singleton;
}

void GxDrawImp::Create()
{
	singleton = new GxDrawImp();
}

void GxDrawImp::Destroy()
{
	delete singleton;
	singleton = NULL;
}

GxDrawImp::GxDrawImp()
	:myCurrentTexture(NULL)
	,myNumIndices(0)
	,myNumVertices(0)
	,myCurrentOp(DRAWOP_NONE)
{
}

GxDrawImp::~GxDrawImp()
{
}

void GxDrawImp::Flush()
{
	if(myNumVertices > 0)
	{
		GxRenderInterface* renderer = GxRenderInterface::Get();
		switch(myCurrentOp)
		{
		case DRAWOP_TRIS:
			renderer->DrawTriangles(&myVertices[0], (int)myNumVertices, myCurrentTexture);
			break;
		case DRAWOP_QUADS:
			renderer->DrawTriangles(&myVertices[0], (int)myNumVertices, &myIndices[0], (int)myNumIndices, myCurrentTexture);
			break;
		};
		myNumVertices = 0;
		myNumIndices = 0;
	}
}

// ===================================================================================
// Rectangle drawing
// ===================================================================================

void GxDrawImp::Rect(int x, int y, int w, int h, GxColor c)
{
	Rect(x, y, w, h, c, c, c, c);
}

void GxDrawImp::Rect(int x, int y, int w, int h, GxColor t, GxColor b)
{
	Rect(x, y, w, h, t, t, b, b);
}

void GxDrawImp::Rect(int x, int y, int w, int h, GxColor tl, GxColor tr, GxColor bl, GxColor br)
{
	const float l = (float)(x);
	const float r = (float)(x + w);
	const float t = (float)(y);
	const float b = (float)(y + h);
	GxVertex* v = BatchQuads(1, 0);
	*v++ = GxVertex(l, t, tl);
	*v++ = GxVertex(r, t, tr);
	*v++ = GxVertex(l, b, bl);
	*v++ = GxVertex(r, b, br);
}

void GxDrawImp::Rect(int x, int y, int w, int h, GxColor c, const GxTexture& texture)
{
	const float l = (float)(x);
	const float r = (float)(x + w);
	const float t = (float)(y);
	const float b = (float)(y + h);
	const GxAreaf uvs = texture.GetUVs();
	GxVertex* v = BatchQuads(1, texture.GetHandle());
	*v++ = GxVertex(l, t, uvs.l, uvs.t, c);
	*v++ = GxVertex(r, t, uvs.r, uvs.t, c);
	*v++ = GxVertex(l, b, uvs.l, uvs.b, c);
	*v++ = GxVertex(r, b, uvs.r, uvs.b, c);
}

// ===================================================================================
// Vertex geometry drawing
// ===================================================================================

GxVertex* GxDrawImp::BatchTriangles(int triangleCount, GxTextureHandle texture)
{
	if(myCurrentOp != DRAWOP_TRIS || myCurrentTexture != texture)
	{
		Flush();
		myCurrentOp = DRAWOP_TRIS;
		myCurrentTexture = texture;
	}

	const uint baseIndex = myNumVertices;

	myNumVertices += triangleCount * 3;
	if(myNumVertices > myVertices.size())
		myVertices.resize(myNumVertices);

	return &myVertices[baseIndex];
}

GxVertex* GxDrawImp::BatchQuads(int quadCount, GxTextureHandle texture)
{
	if(myCurrentOp != DRAWOP_QUADS || myCurrentTexture != texture)
	{
		Flush();
		myCurrentOp = DRAWOP_QUADS;
		myCurrentTexture = texture;
	}
	const uint baseIndex = myNumVertices;

	myNumIndices += quadCount * 6;
	myNumVertices += quadCount * 4;
	if(myNumIndices > myIndices.size())
		myResizeIndexBuffer();
	if(myNumVertices > myVertices.size())
		myVertices.resize(myNumVertices);

	return &myVertices[baseIndex];
}

void GxDrawImp::DrawTriangles(const GxVertex* verts, int triangleCount, GxTextureHandle texture)
{
	Flush();

	GxRenderInterface* renderer = GxRenderInterface::Get();
	renderer->DrawTriangles(verts, triangleCount * 3, texture);
}

void GxDrawImp::DrawQuads(const GxVertex* verts, int quadCount, GxTextureHandle texture)
{
	Flush();

	myNumIndices = quadCount * 6;
	myResizeIndexBuffer();

	GxRenderInterface* renderer = GxRenderInterface::Get();
	renderer->DrawTriangles(verts, quadCount * 4, &myIndices[0], myNumIndices, texture);
}

// ===================================================================================
// View operations
// ===================================================================================

void GxDrawImp::PushScissorRect(int x, int y, int w, int h)
{
	Flush();

	GxRenderInterface* renderer = GxRenderInterface::Get();

	if(myScissorStack.size() < 256)
	{
		if(w < 0 || h < 0)
		{
			GxLog(LOG_TAG, GX_LT_WARNING, "PushScissorRect() was called with a negative width or height.");
		}
		if(!myScissorStack.empty())
		{
			GxRecti last = myScissorStack.back();
			int nx = GxMax(last.x, x);
			int ny = GxMax(last.y, y);
			int nw = GxMax(0, GxMin(last.x+last.w, x+w) - nx);
			int nh = GxMax(0, GxMin(last.y+last.h, y+h) - ny);
			myScissorStack.push_back(GxRecti(nx, ny, nw, nh));
			renderer->SetScissorRect(nx, ny, nw, nh);
		}
		else
		{
			myScissorStack.push_back(GxRecti(x, y, w, h));
			renderer->EnableScissorRect(true);
			renderer->SetScissorRect(x, y, w, h);
		}
	}
	else
	{
		GxLog(LOG_TAG, GX_LT_WARNING, "The ScissorRect stack is overflowing, ignoring push.");
	}
}

void GxDrawImp::PopScissorRect()
{
	Flush();

	GxRenderInterface* renderer = GxRenderInterface::Get();

	if(!myScissorStack.empty())
	{
		myScissorStack.pop_back();
		if(!myScissorStack.empty())
		{
			GxRecti& r = myScissorStack.back();
			renderer->SetScissorRect(r.x, r.y, r.w, r.h);
		}
		else
		{
			renderer->EnableScissorRect(false);
		}
	}
	else
	{
		GxLog(LOG_TAG, GX_LT_WARNING, "The ScissorRect stack is empty, ignoring pop.");
	}
}

GxRecti GxDrawImp::GetScissorRect()
{
	if(myScissorStack.empty())
	{
		GxRenderInterface* renderer = GxRenderInterface::Get();
		GxVec2i size = renderer->GetViewSize();
		return GxRecti(0, 0, size.x, size.y);
	}
	return myScissorStack.back();
}

// ===================================================================================
// Misc functionality
// ===================================================================================

void GxDrawImp::SetBlendMode(GxBlendMode blendMode)
{
	Flush();
	GxRenderInterface::Get()->SetBlendMode(blendMode);
}

void GxDrawImp::myResizeIndexBuffer()
{
	if(myNumIndices > myIndices.size())
	{
		size_t begin = myIndices.size();
		size_t end = myNumIndices;
		myIndices.resize(myNumIndices);
		for(size_t i=begin, j=begin/6*4; i<end; i+=6, j+=4)
		{
			uint* ptr = (uint*)(&myIndices[i]);
			ptr[0] = j, ptr[1] = j+1, ptr[2] = j+3;
			ptr[3] = j, ptr[4] = j+3, ptr[5] = j+2;
		}
	}
}

}; // namespace graphics
}; // namespace guix