#include <GuiX/Config.h>

#include <math.h>
#include <string.h>

#include <GuiX/Canvas.h>

namespace guix {
namespace graphics {

struct GxCanvas::DistFunc
{
	virtual float Get(float px, float py) const = 0;
};

namespace {

// ===================================================================================
// Blend functions
// ===================================================================================

typedef void(*BlendFunc)(uchar*, const uchar*, int sa);

// Linear interpolation between destination and source.
static void BlendNone(uchar* d, const uchar* s, int sa)
{
	if(sa >= 255)
	{
		d[0] = s[0];
		d[1] = s[1];
		d[2] = s[2];
		d[3] = s[3];
	}
	else if(sa > 0)
	{
		d[0] = (uchar)(d[0] + (((s[0] - d[0]) * sa) >> 8));
		d[1] = (uchar)(d[1] + (((s[1] - d[1]) * sa) >> 8));
		d[2] = (uchar)(d[2] + (((s[2] - d[2]) * sa) >> 8));
		d[3] = (uchar)(d[3] + (((s[3] - d[3]) * sa) >> 8));
	}
}

// Linear interpolation between destination and source,
// but takes source alpha into consideration as well.
static void BlendAlpha(uchar* d, const uchar* s, int sa)
{
	if(sa >= 255)
	{
		d[0] = s[0];
		d[1] = s[1];
		d[2] = s[2];
		d[3] = s[3];
	}
	else if(sa > 0)
	{
		const int a = 1 + (((sa + 1) * s[3]) >> 8);
		d[0] = (uchar)(d[0] + (((s[0] - d[0]) * a) >> 8));
		d[1] = (uchar)(d[1] + (((s[1] - d[1]) * a) >> 8));
		d[2] = (uchar)(d[2] + (((s[2] - d[2]) * a) >> 8));
		d[3] = (uchar)(a + ((d[3] * (257 - a)) >> 8) - 1);
	}
}

// Additive blending, adds source to destination.
static void BlendAdd(uchar* d, const uchar* s, int sa)
{
	if(sa >= 255)
	{
		d[0] = (uchar)GxMin(255, d[0] + s[0]);
		d[1] = (uchar)GxMin(255, d[1] + s[1]);
		d[2] = (uchar)GxMin(255, d[2] + s[2]);
		d[3] = (uchar)GxMin(255, d[3] + s[3]);
	}
	else if(sa > 0)
	{
		const int a = 1 + (((sa + 1) * s[3]) >> 8);
		d[0] = (uchar)GxMin(255, d[0] + ((s[0] * a) >> 8));
		d[1] = (uchar)GxMin(255, d[1] + ((s[1] * a) >> 8));
		d[2] = (uchar)GxMin(255, d[2] + ((s[2] * a) >> 8));
		d[3] = (uchar)(a + ((d[3] * (257 - a)) >> 8) - 1);
	}
}

// Subtractive blending, subtracts source from destination.
static void BlendSub(uchar* d, const uchar* s, int sa)
{
	if(sa >= 255)
	{
		d[0] = (uchar)GxMax(0, d[0] - s[0]);
		d[1] = (uchar)GxMax(0, d[1] - s[1]);
		d[2] = (uchar)GxMax(0, d[2] - s[2]);
		d[3] = (uchar)GxMax(0, d[3] - s[3]);
	}
	else if(sa > 0)
	{
		const int a = 1 + (((sa + 1) * s[3]) >> 8);
		d[0] = (uchar)GxMax(0, d[0] - ((s[0] * a) >> 8));
		d[1] = (uchar)GxMax(0, d[1] - ((s[1] * a) >> 8));
		d[2] = (uchar)GxMax(0, d[2] - ((s[2] * a) >> 8));
		d[3] = (uchar)(a + ((d[3] * (257 - a)) >> 8) - 1);
	}
}

// Multiplicative blending, multiplies destination by source.
static void BlendMul(uchar* d, const uchar* s, int sa)
{
	if(sa <= 0)
	{
		d[0] = d[1] = d[2] = d[3] = 0;
	}
	else
	{
		const int a = 1 + (((sa + 1) * s[3]) >> 8);
		d[0] = (uchar)((d[0] * (256 + (((s[0]-255) * a) >> 8))) >> 8);
		d[1] = (uchar)((d[1] * (256 + (((s[1]-255) * a) >> 8))) >> 8);
		d[2] = (uchar)((d[2] * (256 + (((s[2]-255) * a) >> 8))) >> 8);
		d[3] = (uchar)(a + ((d[3] * (257 - a)) >> 8) - 1);
	}
}

// ===================================================================================
// Distance functions
// ===================================================================================

// Returns the distance from a point to a line.
struct GetLineDist : public GxCanvas::DistFunc
{
	const float ax, ay, bx, by, size;

	GetLineDist(float _ax, float _ay, float _bx, float _by, float _size)
		:ax(_ax), ay(_ay), bx(_bx), by(_by), size(_size) {}

	float Get(float px, float py) const
	{
		float num  = (px-ax)*(bx-ax) + (py-ay)*(by-ay);
		float den  = (bx-ax)*(bx-ax) + (by-ay)*(by-ay);
		float rden = 1.f / den;
		float r    = num * rden;
		float s    = ((ay-py)*(bx-ax) - (ax-px)*(by-ay)) * rden;

		if(r >= 0 && r <= 1)
			return abs(s * sqrt(den)) - size;

		float d1sq = (px-ax)*(px-ax) + (py-ay)*(py-ay);
		float d2sq = (px-bx)*(px-bx) + (py-by)*(py-by);
		return sqrt(d1sq < d2sq ? d1sq : d2sq) - size;
	}
};

// Returns the distance from a point to a circle.
struct GetCircleDist : public GxCanvas::DistFunc
{
	const float x, y, r;

	GetCircleDist(float _x, float _y, float _r)
		:x(_x), y(_y), r(_r) {}

	float Get(float px, float py) const
	{
		float dx = px-x, dy = py-y;
		return sqrt(dx*dx + dy*dy) - r;
	}
};

// Returns the distance from a point to a rectangle.
struct GetRectDist : public GxCanvas::DistFunc
{
	const float x1, y1, x2, y2;

	GetRectDist(float _x1, float _y1, float _x2, float _y2)
		:x1(_x1), y1(_y1), x2(_x2), y2(_y2) {}

	float Get(float px, float py) const
	{
		float x = GxClamp(px, x1, x2);
		float y = GxClamp(py, y1, y2);
		if(x == px && y == py)
		{
			float dx = GxMin(x - x1, x2 - x);
			float dy = GxMin(y - y1, y2 - y);
			return -GxMin(dx, dy);
		}
		else
		{
			float dx = abs(px - x);
			float dy = abs(py - y);
			return GxMax(dx, dy);
		}
	}
};

// Returns the distance from a point to a rectangle with rounded corners.
struct GetRoundRectDist : public GxCanvas::DistFunc
{
	const float x1, y1, x2, y2, r;

	GetRoundRectDist(float _x1, float _y1, float _x2, float _y2, float _r)
		:x1(_x1), y1(_y1), x2(_x2), y2(_y2), r(_r) {}

	float Get(float px, float py) const
	{
		float x = GxClamp(px, x1+r, x2-r);
		float y = GxClamp(py, y1+r, y2-r);
		if(x == px && y == py)
		{
			float dx = GxMin(x - x1, x2 - x);
			float dy = GxMin(y - y1, y2 - y);
			return -GxMin(dx, dy);
		}
		else
		{
			float dx = px - x;
			float dy = py - y;
			return sqrt(dx*dx + dy*dy) - r;
		}
	}
};

// Returns the distance from a point to a polygon.
struct GetPolyDist : public GxCanvas::DistFunc
{
	const GxVec2f* v;
	const int count;

	GetPolyDist(const GxVec2f* _v, int _count)
		:v(_v), count(_count) {}

	bool InPoly(float px, float py) const
	{
		bool c = false;
		for(int i=0, j=count-1; i<count; j=i, ++i)
			if(((v[i].y >= py) != (v[j].y >= py)) &&
				(px <= (v[j].x - v[i].x) * (py - v[i].y) / (v[j].y - v[i].y) + v[i].x))
					c = !c;
		return c;
	}

	float Get(float px, float py) const
	{
		float d = 1e10f;
		for(int i=0, j=count-1; i<count; j=i, ++i)
		{
			GetLineDist ld(v[i].x, v[i].y, v[j].x, v[j].y, 0);
			d = GxMin(d, ld.Get(px, py));
		}
		return InPoly(px, py) ? -d : d;
	}
};

}; // anonymous namespace

// ===================================================================================
// GxCanvas
// ===================================================================================

GxCanvas::GxCanvas(int w, int h)
	:myW(GxMax(w, 0))
	,myH(GxMax(h, 0))
	,myBitmap(NULL)
	,myCTL(255)
	,myCTR(255)
	,myCBL(255)
	,myCBR(255)
	,myOuterGlow(0.f)
	,myInnerGlow(0.f)
	,myLineW(1.f)
	,myBlendMode(BM_ALPHA)
	,myHasFill(true)
{
	int size = w * h;
	if(size > 0)
	{
		myBitmap = GxMalloc<uchar>(size * 4);
		memset(myBitmap, 0, size * 4);
	}
}

GxCanvas::~GxCanvas()
{
	GxFree(myBitmap);
}

void GxCanvas::Clear(GxColor c)
{
	const uchar c4[4] = {c.r, c.g, c.b, c.a};
	const uint src = *reinterpret_cast<const uint*>(c4);
	uint* dst = reinterpret_cast<uint*>(myBitmap);
	for(int i=0; i<myW * myH; ++i) dst[i] = src;
}

void GxCanvas::SetOutline(float width)
{
	myLineW = width;
}

void GxCanvas::SetOuterGlow(float pixels)
{
	myOuterGlow = pixels;
}

void GxCanvas::SetInnerGlow(float pixels)
{
	myInnerGlow = pixels;
}

void GxCanvas::SetFill(bool enabled)
{
	myHasFill = enabled;
}

void GxCanvas::SetBlendMode(BlendMode bm)
{
	myBlendMode = bm;
}

void GxCanvas::SetColor(GxColor c)
{
	myCTL = myCTR = myCBL = myCBR = c;
}

void GxCanvas::SetColor(float r, float g, float b, float a)
{
	myCTL = myCTR = myCBL = myCBR = GxColor(r,g,b,a);
}

void GxCanvas::SetColor(GxColor top, GxColor bottom)
{
	myCTL = myCTR = top;
	myCBL = myCBR = bottom;
}

void GxCanvas::SetColor(GxColor topLeft, GxColor topRight, GxColor bottomLeft, GxColor bottomRight)
{
	myCTL = topLeft;
	myCTR = topRight;
	myCBL = bottomLeft;
	myCBR = bottomRight;
}

void GxCanvas::Line(float x1, float y1, float x2, float y2, float width)
{
	float xMin = x1, xMax = x2;
	float yMin = y1, yMax = y2;
	if(xMin > xMax) GxSwap(xMin, xMax);
	if(yMin > yMax) GxSwap(yMin, yMax);

	float r = width * 0.5f;
	GetLineDist func(x1, y1, x2, y2, r);
	myDrawOp(GxAreaf(xMin-r, yMin-r, xMax+r, yMax+r), &func);
}

void GxCanvas::Circle(float x, float y, float r)
{
	GetCircleDist func(x, y, r);
	myDrawOp(GxAreaf(x-r, y-r, x+r, y+r), &func);
}

void GxCanvas::Rect(float x1, float y1, float x2, float y2)
{
	if(x1 > x2) GxSwap(x1, x2);
	if(y1 > y2) GxSwap(y1, y2);
	GetRectDist func(x1, y1, x2, y2);
	myDrawOp(GxAreaf(x1, y1, x2, y2), &func);
}

void GxCanvas::Rect(float x1, float y1, float x2, float y2, float radius)
{
	if(x1 > x2) GxSwap(x1, x2);
	if(y1 > y2) GxSwap(y1, y2);
	radius = GxMin(GxMin(x2-x1, y2-y1)*0.5f, radius);
	GetRoundRectDist func(x1, y1, x2, y2, radius);
	myDrawOp(GxAreaf(x1, y1, x2, y2), &func);
}

void GxCanvas::Polygon(GxVec2f* vertices, int vertexCount)
{
	if(vertexCount < 3) return;
	GetPolyDist func(vertices, vertexCount);
	myDrawOp(GxAreaf(0, 0, (float)myW, (float)myH), &func);
}

GxTexture GxCanvas::CreateTexture()
{
	return GxTexture(myW, myH, myBitmap);
}

int GxCanvas::GetWidth() const
{
	return myW;
}

int GxCanvas::GetHeight() const
{
	return myH;
}

const uchar* GxCanvas::GetBitmap() const
{
	return myBitmap;
}

uchar* GxCanvas::GetBitmap()
{
	return myBitmap;
}

void GxCanvas::myDrawOp(const GxAreaf& area, DistFunc* func)
{
	BlendFunc blendfunc = BlendAlpha;
	switch(myBlendMode)
	{
		case BM_NONE:  blendfunc = BlendNone;  break;
		case BM_ALPHA: blendfunc = BlendAlpha; break;
		case BM_ADD:   blendfunc = BlendAdd;   break;
		case BM_SUB:   blendfunc = BlendSub;   break;
		case BM_MUL:   blendfunc = BlendMul;   break;
	};

	float rig = 255.f / (myInnerGlow + 1.f);
	float rog = 255.f / (myOuterGlow + 1.f);

	const int x1 = GxMax(0,   (int)(area.l - myOuterGlow - 1 + 0.5f));
	const int y1 = GxMax(0,   (int)(area.t - myOuterGlow - 1 + 0.5f));
	const int x2 = GxMin(myW, (int)(area.r + myOuterGlow + 1 + 0.5f));
	const int y2 = GxMin(myH, (int)(area.b + myOuterGlow + 1 + 0.5f));

	float oy = 0.5f + (float)y1;
	float ox = 0.5f + (float)x1;

	const int rH = 0xFFFF / GxMax(1, y2 - y1);
	const int rW = 0xFFFF / GxMax(1, x2 - x1);
	
	float yf = oy;
	for(int y=0; y<y2 - y1; ++y)
	{
		uchar* dst = myBitmap + ((y1 + y)*myW + x1) * 4;

		GxColor cl = myCTL, cr = myCTR;
		BlendAlpha(cl.channels, myCBL.channels, (y * rH) >> 8);
		BlendAlpha(cr.channels, myCBR.channels, (y * rH) >> 8);

		float xf = ox;
		for(int x=0; x<x2 - x1; ++x)
		{
			GxColor col = cl;
			BlendAlpha(col.channels, cr.channels, (x * rW) >> 8);

			float dist = func->Get(xf, yf);
			if(dist < myOuterGlow + 1)
			{
				if(myHasFill)
				{
					const int alpha = 255 - (int)(rog * (dist + 0.5f));
					blendfunc(dst, col.channels, alpha);
				}
				else
				{
					if(dist < -0.5f)
					{
						float absdist = -dist - myLineW;
						const int alpha = 255 - (int)(rig * (absdist + 0.5f));
						blendfunc(dst, col.channels, alpha);
					}
					else
					{
						const int alpha = 255 - (int)(rog * (dist + 0.5f));
						blendfunc(dst, col.channels, alpha);
					}
				}
			}
			xf += 1.f;
			dst += 4;
		}
		yf += 1.f;
	}
}

}; // namespace graphics
}; // namespace guix