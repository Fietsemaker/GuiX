#include <GuiX/Config.h>

#include <math.h>

#include <GuiX/Draw.h>
#include <GuiX/Sprites.h>

namespace guix {
namespace graphics {

namespace {

typedef void (*SetQuad)(GxVertex*&, float, float, float, float, const GxColor&, float, float, float, float);

void SetQuadH(GxVertex*& v, float x0, float y0, float x1, float y1, const GxColor& c, float u0, float v0, float u1, float v1)
{
	*v = GxVertex(x0, y0, u0, v0, c); ++v;
	*v = GxVertex(x1, y0, u1, v0, c); ++v;
	*v = GxVertex(x0, y1, u0, v1, c); ++v;
	*v = GxVertex(x1, y1, u1, v1, c); ++v;
}

void SetQuadV(GxVertex*& v, float x0, float y0, float x1, float y1, const GxColor& c, float u0, float v0, float u1, float v1)
{
	*v = GxVertex(y0, x0, u0, v0, c); ++v;
	*v = GxVertex(y0, x1, u1, v0, c); ++v;
	*v = GxVertex(y1, x0, u0, v1, c); ++v;
	*v = GxVertex(y1, x1, u1, v1, c); ++v;
}

}; // anonymous namespace

// ===================================================================================
// GxSprite
// ===================================================================================

GxSprite::GxSprite()
	:mySize(0,0)
	,myOrigin(0,0)
	,myRotation(1,0)
	,myColor(255)
{
}

GxSprite::GxSprite(const GxTexture& texture)
	:myTexture(texture)
	,mySize(0,0)
	,myOrigin(0,0)
	,myRotation(1,0)
	,myColor(255)
{
	mySize.x = texture.GetWidthU(); 
	mySize.y = texture.GetHeightV();
}

void GxSprite::SetTexture(const GxTexture& texture)
{
	myTexture = texture;
	mySize.x = texture.GetWidthU(); 
	mySize.y = texture.GetHeightV();
}

void GxSprite::SetSize(float width, float height)
{
	mySize.Set(width, height);
}

void GxSprite::SetOrigin(float x, float y)
{
	myOrigin.Set(x, y);
}

void GxSprite::SetRotation(float angleInDegrees)
{
	myRotation.x = cos(angleInDegrees * (float)(3.14159265359 / 180.0));
	myRotation.y = sin(angleInDegrees * (float)(3.14159265359 / 180.0));
}

void GxSprite::SetRotation(float x, float y)
{
	float sq = x*x + y*y;
	if(sq < 0.99f || sq > 1.01f)
	{
		float r = 1.f / sqrt(sq);
		x *= r, y *= r;
	}
	myRotation.Set(x, y);
}

void GxSprite::SetRotation()
{
	myRotation = GxVec2f(1, 0);
}

void GxSprite::SetFlags(int flags)
{
	myFlags = flags;
}

void GxSprite::SetMirrorH(bool enabled)
{
	myFlags.Set(MIRROR_H, enabled);
}

void GxSprite::SetMirrorV(bool enabled)
{
	myFlags.Set(MIRROR_V, enabled);
}

void GxSprite::SetColor(GxColor c)
{
	myColor = c;
}

GxTexture& GxSprite::GetTexture()
{
	return myTexture;
}

const GxTexture& GxSprite::GetTexture() const
{
	return myTexture;
}

const GxVec2f& GxSprite::GetSize() const
{
	return mySize;
}

const GxVec2f& GxSprite::GetOrigin() const
{
	return myOrigin;
}

const GxVec2f& GxSprite::GetRotation() const
{
	return myRotation;
}

GxColor GxSprite::GetColor() const
{
	return myColor;
}

GxFlags GxSprite::GetFlags() const
{
	return myFlags;
}

void GxSprite::Draw(int x, int y)
{
	Draw((float)x, (float)y);
}

void GxSprite::Draw(float x, float y)
{
	GxDraw* draw = GxDraw::Get();
	if(!draw) return;

	GxVec2f pos[4];
	if(myRotation.x > 0 && abs(myRotation.y) < 0.001f)
	{
		const float dxL = x - myOrigin.x * mySize.x;
		const float dyT = y - myOrigin.y * mySize.y;
		const float dxR = dxL + mySize.x;
		const float dyB = dyT + mySize.y;

		pos[0] = GxVec2f(dxL, dyT);
		pos[1] = GxVec2f(dxR, dyT);
		pos[2] = GxVec2f(dxL, dyB);
		pos[3] = GxVec2f(dxR, dyB);
	}
	else
	{
		const float dxL = -myOrigin.x * mySize.x;
		const float dyT = -myOrigin.y * mySize.y;
		const float dxR = dxL + mySize.x;
		const float dyB = dyT + mySize.y;

		GxVec2f in[4] =
		{
			GxVec2f(dxL, dyT),
			GxVec2f(dxR, dyT),
			GxVec2f(dxL, dyB),
			GxVec2f(dxR, dyB)
		};
		for(int i=0; i<4; ++i)
		{
			const GxVec2f v = in[i];
			pos[i] = GxVec2f(
				x + myRotation.x*v.x - myRotation.y*v.y,
				y + myRotation.y*v.x + myRotation.x*v.y);
		}
	}

	GxAreaf uvs = myTexture.GetUVs();
	if(myFlags[GxSprite::MIRROR_H]) GxSwap(uvs.l, uvs.r);
	if(myFlags[GxSprite::MIRROR_V]) GxSwap(uvs.t, uvs.b);

	GxVertex* v = draw->BatchQuads(1, myTexture.GetHandle());
	v[0] = GxVertex(pos[0].x, pos[0].y, uvs.l, uvs.t, myColor);
	v[1] = GxVertex(pos[1].x, pos[1].y, uvs.r, uvs.t, myColor);
	v[2] = GxVertex(pos[2].x, pos[2].y, uvs.l, uvs.b, myColor);
	v[3] = GxVertex(pos[3].x, pos[3].y, uvs.r, uvs.b, myColor);
}

// ===================================================================================
// GxTileBar
// ===================================================================================

GxTileBar::GxTileBar()
	:mySize(0,0)
	,myCenterRegion(1/3.f, 2/3.f)
	,myColor(255,255,255)
{
}

GxTileBar::GxTileBar(const GxTexture& texture)
	:myTexture(texture)
	,myColor(255,255,255)
{
	mySize.x = texture.GetWidthU(); 
	mySize.y = texture.GetHeightV();

	myCenterRegion.x = (1/3.f) * mySize.x;
	myCenterRegion.y = (2/3.f) * mySize.x;
}

void GxTileBar::SetTexture(const GxTexture& texture)
{
	myTexture = texture;
	mySize.x = texture.GetWidthU();
	mySize.y = texture.GetHeightV();

	myCenterRegion.x = (1/3.f) * mySize.x;
	myCenterRegion.y = (2/3.f) * mySize.x;
}

void GxTileBar::SetBorderSize(float left, float right)
{
	myCenterRegion.Set(left, mySize.x-right);
}

void GxTileBar::SetFlags(int flags)
{
	myFlags = flags;
}

void GxTileBar::SetMirrorH(bool enabled)
{
	myFlags.Set(MIRROR_H, enabled);
}

void GxTileBar::SetMirrorV(bool enabled)
{
	myFlags.Set(MIRROR_V, enabled);
}

void GxTileBar::SetTiling(bool enabled)
{
	myFlags.Set(TILING, enabled);
}

void GxTileBar::SetColor(GxColor c)
{
	myColor = c;
}

const GxTexture& GxTileBar::GetTexture() const
{
	return myTexture;
}

const GxVec2f& GxTileBar::GetCenterRegion() const
{
	return myCenterRegion;
}

const GxVec2f& GxTileBar::GetSize() const
{
	return mySize;
}

GxColor GxTileBar::GetColor() const
{
	return myColor;
}

GxFlags GxTileBar::GetFlags() const
{
	return myFlags;
}

void GxTileBar::DrawH(int x, int y, int w)
{
	myDraw(GxAreai(x, y, x+w, y+(int)mySize.y), false);
}

void GxTileBar::DrawH(int x, int y, int w, int h)
{
	myDraw(GxAreai(x, y, x+w, y+h), false);
}

void GxTileBar::DrawH(float x, float y, float w, float h)
{
	myDraw(GxAreaf(x, y, x+w, y+h), false);
}

void GxTileBar::DrawV(int x, int y, int h)
{
	myDraw(GxAreai(x, y, x+(int)mySize.y, y+h), true);
}

void GxTileBar::DrawV(int x, int y, int w, int h)
{
	myDraw(GxAreai(x, y, x+w, y+h), true);
}

void GxTileBar::DrawV(float x, float y, float w, float h)
{
	myDraw(GxAreaf(x, y, x+w, y+h), true);
}

void GxTileBar::myDraw(GxAreaf a, bool transpose)
{
	GxDraw* draw = GxDraw::Get();
	float x0 = a.l, x1 = a.r;
	float y0 = a.t, y1 = a.b;
	if(x1-x0 < 0.1f || y1-y0 < 0.1f || !draw) return;

	const SetQuad setQuad = transpose ? SetQuadV : SetQuadH;
	
	if(transpose)
	{
		GxSwap(x0, y0);
		GxSwap(x1, y1);
	}
	if(myFlags[MIRROR_H]) GxSwap(x0, x1);
	if(myFlags[MIRROR_V]) GxSwap(y0, y1);

	const bool stretch = !myFlags[GxTileBar::TILING];

	GxVec2f cr = myCenterRegion;
	float u0, u1, u2, u3, v0, v1, ud0, ud1, ud2;
	{
		const GxAreaf uv = myTexture.GetUVs();
		float rw = 1.f / myTexture.GetWidthU();
		cr.x *= rw;
		cr.y *= rw;

		u0  = uv.l;
		u1  = GxLerp(uv.l, uv.r, cr.x);
		u2  = GxLerp(uv.l, uv.r, cr.y);
		u3  = uv.r;
		v0  = uv.t;
		v1  = uv.b;

		ud0 = u1 - u0;
		ud1 = u2 - u1;
		ud2 = u3 - u2;
	}

	const float lTileW = abs((0.f + cr.x) * mySize.x);
	const float rTileW = abs((1.f - cr.y) * mySize.x);
	const float cTileW = GxMax(1.f, abs((cr.y - cr.x) * mySize.x));
	
	const float scaleX = GxClamp((x1 - x0) / (lTileW + rTileW), -1.f, 1.f);
	const float scaleU = abs(scaleX);

	const float lBarW = lTileW * scaleX;
	const float rBarW = rTileW * scaleX;
	const float cBarW = (x1 - x0) - lBarW - rBarW;
	const float cPieceW = cTileW * scaleX;
	
	// Reserve vertices for the number of quads that are going to be drawn
	int xPieces = 0;
	if(abs(cBarW) > 0.01f)
		xPieces = stretch ? 1 : (int)(ceil(abs(cBarW) / cTileW) + 0.5f);

	int numQuads = xPieces + 2;
	GxVertex* vertexPtr = draw->BatchQuads(numQuads, myTexture.GetHandle());
	GxVertex* verts = vertexPtr;
	GxColor color = myColor;

	// The left and right end of the bar
	setQuad(verts, x0, y0, x0+lBarW, y1, color, u0, v0, u0+ud0*scaleU, v1);
	setQuad(verts, x1-rBarW, y0, x1, y1, color, u3-ud2*scaleU, v0, u3, v1);

	// The center of the bar
	if(xPieces > 0)
	{
		float x = x0 + lBarW;
		for(int i=0; i<xPieces - 1; ++i)
		{
			setQuad(verts, x, y0, x+cPieceW, y1, color, u1, v0, u2, v1);
			x += cPieceW;
		}
		const float u = stretch ? u2 : u1+ud1 * (abs(x1-rBarW-x) / cTileW);
		setQuad(verts, x, y0, x1-rBarW, y1, color, u1, v0, u, v1);
	}
}

// ===================================================================================
// GxTileRect
// ===================================================================================

GxTileRect::GxTileRect()
	:mySize(0,0)
	,myCenterRegion(0, 0, 1, 1)
	,myColor(255,255,255)
{
}

GxTileRect::GxTileRect(const GxTexture& texture)
	:myTexture(texture)
	,myColor(255,255,255)
{
	mySize.x = texture.GetWidthU(); 
	mySize.y = texture.GetHeightV();

	myCenterRegion.l = (1/3.f) * mySize.x;
	myCenterRegion.t = (1/3.f) * mySize.y;
	myCenterRegion.r = (2/3.f) * mySize.x;
	myCenterRegion.b = (2/3.f) * mySize.y;
}

void GxTileRect::SetTexture(const GxTexture& texture)
{
	myTexture = texture;
	mySize.x = texture.GetWidthU();
	mySize.y = texture.GetHeightV();

	myCenterRegion.l = (1/3.f) * mySize.x;
	myCenterRegion.t = (1/3.f) * mySize.y;
	myCenterRegion.r = (2/3.f) * mySize.x;
	myCenterRegion.b = (2/3.f) * mySize.y;
}

void GxTileRect::SetBorderSize(float left, float top, float right, float bottom)
{
	myCenterRegion = GxAreaf(left, top, mySize.x-right, mySize.y-bottom);
}

void GxTileRect::SetFlags(int flags)
{
	myFlags = flags;
}

void GxTileRect::SetMirrorH(bool enabled)
{
	myFlags.Set(MIRROR_H, enabled);
}

void GxTileRect::SetMirrorV(bool enabled)
{
	myFlags.Set(MIRROR_V, enabled);
}

void GxTileRect::SetTiling(bool enabled)
{
	myFlags.Set(TILING, enabled);
}

void GxTileRect::SetColor(GxColor c)
{
	myColor = c;
}

const GxTexture& GxTileRect::GetTexture() const
{
	return myTexture;
}

const GxAreaf& GxTileRect::GetCenterRegion() const
{
	return myCenterRegion;
}

const GxVec2f& GxTileRect::GetSize() const
{
	return mySize;
}

GxColor GxTileRect::GetColor() const
{
	return myColor;
}

GxFlags GxTileRect::GetFlags() const
{
	return myFlags;
}

void GxTileRect::DrawH(int x, int y, int w, int h)
{
	myDraw(GxAreai(x, y, x+w, y+h), false);
}

void GxTileRect::DrawH(float x, float y, float w, float h)
{
	myDraw(GxAreaf(x, y, x+w, y+h), false);
}

void GxTileRect::DrawV(int x, int y, int w, int h)
{
	myDraw(GxAreai(x, y, x+w, y+h), true);
}

void GxTileRect::DrawV(float x, float y, float w, float h)
{
	myDraw(GxAreaf(x, y, x+w, y+h), true);
}

void GxTileRect::myDraw(GxAreaf a, bool transpose)
{
	GxDraw* draw = GxDraw::Get();
	float x0 = a.l, x1 = a.r;
	float y0 = a.t, y1 = a.b;
	if(x1-x0 < 0.1f || y1-y0 < 0.1f || !draw) return;

	const SetQuad setQuad = transpose ? SetQuadV : SetQuadH;
	if(transpose)
	{
		GxSwap(x0, y0);
		GxSwap(x1, y1);
	}
	if(myFlags[MIRROR_H]) GxSwap(x0, x1);
	if(myFlags[MIRROR_V]) GxSwap(y0, y1);

	const bool stretch = !myFlags[GxTileRect::TILING];
	GxAreaf cr         = myCenterRegion;

	float u0, u1, u2, u3, ud0, ud1, ud2;
	float v0, v1, v2, v3, vd0, vd1, vd2;
	{
		const GxAreaf uv = myTexture.GetUVs();
		const float rtexW = 1.f / myTexture.GetWidthU();
		const float rtexH = 1.f / myTexture.GetHeightV();
		cr.l *= rtexW;
		cr.r *= rtexW;
		cr.t *= rtexH;
		cr.b *= rtexH;

		u0  = uv.l;
		u1  = GxLerp(uv.l, uv.r, cr.l);
		u2  = GxLerp(uv.l, uv.r, cr.r);
		u3  = uv.r;

		v0  = uv.t;
		v1  = GxLerp(uv.t, uv.b, cr.t);
		v2  = GxLerp(uv.t, uv.b, cr.b);
		v3  = uv.b;

		ud0 = u1 - u0;
		ud1 = u2 - u1;
		ud2 = u3 - u2;

		vd0 = v1 - v0;
		vd1 = v2 - v1;
		vd2 = v3 - v2;
	}

	const float lTileW = abs((0.f + cr.l) * mySize.x);
	const float tTileH = abs((0.f + cr.t) * mySize.y);
	const float rTileW = abs((1.f - cr.r) * mySize.x);
	const float bTileH = abs((1.f - cr.b) * mySize.y);
	const float cTileW = GxMax(1.f, abs((cr.r - cr.l) * mySize.x));
	const float cTileH = GxMax(1.f, abs((cr.b - cr.t) * mySize.y));

	const float scaleX = GxClamp((x1 - x0) / (lTileW + rTileW), -1.f, 1.f);
	const float scaleY = GxClamp((y1 - y0) / (tTileH + bTileH), -1.f, 1.f);
	const float scaleU = abs(scaleX);
	const float scaleV = abs(scaleY);

	const float lBarW = lTileW * scaleX;
	const float tBarH = tTileH * scaleY;
	const float rBarW = rTileW * scaleX;
	const float bBarH = bTileH * scaleY;
	const float cBarW = (x1 - x0) - lBarW - rBarW;
	const float cBarH = (y1 - y0) - tBarH - bBarH;
	const float cPieceW = cTileW * scaleX;
	const float cPieceH = cTileH * scaleY;
	
	// Reserve vertices for the number of quads that are going to be drawn
	int xPieces = 0;
	int yPieces = 0;

	if(abs(cBarW) > 0.01f)
		xPieces = stretch ? 1 : (int)(ceil(abs(cBarW) / cTileW) + 0.5f);

	if(abs(cBarH) > 0.01f)
		yPieces = stretch ? 1 : (int)(ceil(abs(cBarH) / cTileH) + 0.5f);

	const int numQuads = 4 + xPieces*2 + yPieces*2 + xPieces*yPieces;
	GxVertex* vertexPtr = draw->BatchQuads(numQuads, myTexture.GetHandle());
	GxVertex* verts = vertexPtr;
	GxColor color = myColor;

	// Four corners
	setQuad(verts, x0,       y0,       x0+lBarW, y0+tBarH, color, u0, v0, u0+ud0*scaleU, v0+vd0*scaleV);
	setQuad(verts, x1-rBarW, y0,       x1,       y0+tBarH, color, u3-ud2*scaleU, v0, u3, v0+vd0*scaleV);
	setQuad(verts, x0,       y1-bBarH, x0+lBarW, y1,       color, u0, v3-vd2*scaleV, u0+ud0*scaleU, v3);
	setQuad(verts, x1-rBarW, y1-bBarH, x1,       y1,       color, u3-ud2*scaleU, v3-vd2*scaleV, u3, v3);

	// Horizontal edges
	if(xPieces > 0)
	{
		float x = x0 + lBarW;
		for(int i=0; i<xPieces-1; ++i)
		{
			setQuad(verts, x, y0, x+cPieceW, y0+tBarH, color, u1, v0, u2, v0+vd0*scaleV);
			setQuad(verts, x, y1-bBarH, x+cPieceW, y1, color, u1, v3-vd2*scaleV, u2, v3);
			x += cPieceW;
		}
		const float u = stretch ? u2 : u1+ud1 * (abs(x1-rBarW-x) / cTileW);
		setQuad(verts, x, y0, x1-rBarW, y0+tBarH, color, u1, v0, u, v0+vd0*scaleV);
		setQuad(verts, x, y1-bBarH, x1-rBarW, y1, color, u1, v3-vd2*scaleV, u, v3);
	}

	// Vertical edges
	if(yPieces > 0)
	{
		float y = y0 + tBarH;
		for(int i=0; i<yPieces-1; ++i)
		{
			setQuad(verts, x0, y, x0+lBarW, y+cPieceH, color, u0, v1, u0+ud0*scaleU, v2);
			setQuad(verts, x1-rBarW, y, x1, y+cPieceH, color, u3-ud2*scaleU, v1, u3, v2);
			y += cPieceH;
		}
		const float v = stretch ? v2 : v1+vd1 * (abs(y1-bBarH-y) / cTileH);
		setQuad(verts, x0, y, x0+lBarW, y1-bBarH, color, u0, v1, u0+ud0*scaleU, v);
		setQuad(verts, x1-rBarW, y, x1, y1-bBarH, color, u3-ud2*scaleU, v1, u3, v);
	}

	// Center pieces
	if(xPieces > 0 && yPieces > 0)
	{
		if(stretch)
		{
			setQuad(verts, x0+lBarW, y0+tBarH, x1-rBarW, y1-bBarH, color, u1, v1, u2, v2);
		}
		else
		{
			float x = x0 + lBarW;
			float y = y0 + tBarH;

			// Complete tiles
			for(int i=0; i<yPieces-1; ++i)
			{
				for(int j=0; j<xPieces-1; ++j)
				{
					setQuad(verts, x, y, x+cPieceW, y+cPieceH, color, u1, v1, u2, v2);
					x += cPieceW;
				}
				x = x0 + lBarW;
				y += cPieceH;
			}

			const float edgeX = x0 + lBarW + cPieceW * (float)(xPieces - 1);
			const float edgeY = y0 + tBarH + cPieceH * (float)(yPieces - 1);
			const float edgeW = x1 - rBarW - edgeX;
			const float edgeH = y1 - bBarH - edgeY;
			const float edgeU = u1 + ud1*(abs(edgeW) / cTileW);
			const float edgeV = v1 + vd1*(abs(edgeH) / cTileH);

			// Horizontal edge cases
			y = y0 + tBarH;
			for(int i=0; i<yPieces-1; ++i)
			{
				setQuad(verts, edgeX, y, edgeX+edgeW, y+cPieceH, color, u1, v1, edgeU, v2);
				y += cPieceH;
			}

			// Vertical edge cases
			x = x0 + lBarW;
			for(int i=0; i<xPieces-1; ++i)
			{
				setQuad(verts, x, edgeY, x+cPieceW, edgeY+edgeH, color, u1, v1, u2, edgeV);
				x += cPieceW;
			}

			// Corner case
			setQuad(verts, x, y, x+edgeW, y+edgeH, color, u1, v1, edgeU, edgeV);
		}
	}
}

}; // namespace graphics
}; // namespace guix