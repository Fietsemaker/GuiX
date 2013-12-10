namespace guix {
namespace core {

// Utility functions

template <typename T>
T GxMin(T a, T b)
{
	return (a > b) ? b : a;
}

template <typename T>
T GxMax(T a, T b)
{
	return (a < b) ? b : a;
}

template <typename T>
T GxClamp(T x, T min, T max)
{
	return (x < min) ? min : ((x > max) ? max : x);
}

template <typename T>
T GxLerp(T begin, T end, T t)
{
	return begin + (end - begin) * t;
}

template <typename T>
T GxLerpFactor(T begin, T end, T x)
{
	return (x - begin) / (end - begin);
}

template <typename T>
T GxRemap(T x, T inBegin, T inEnd, T outBegin, T outEnd)
{
	return GxLerp(outBegin, outEnd, GxLerpFactor(inBegin, inEnd, x));
}

template <typename T>
int GxInt(T x)
{
	return static_cast<int>(x + (T)0.5);
}

template <typename T>
T* GxMalloc(size_t count)
{
	return static_cast<T*>(malloc(sizeof(T) * count));
}

template <typename T>
T* GxRealloc(T* ptr, size_t count)
{
	return static_cast<T*>(realloc(ptr, sizeof(T) * count));
}

template <typename T>
void GxFree(T* ptr)
{
	if(ptr) free(ptr);
}

template <typename T>
void GxSwap(T& a, T& b)
{
	const T t = a;
	a = b;
	b = t;
}

template <typename T>
int GxStrLen(const T* s)
{
	const T* o = s;
	while(*s) ++s;
	return (s - o);
}

template <typename T>
int GxStrCmp(const T* a, const T* b)
{
	while(*a && *a == *b) ++a, ++b;
	return ((int)(*a) - (int)(*b));
}

// GxFlags

GxFlags::GxFlags()
	:bits(0)
{
}

GxFlags::GxFlags(int f)
	:bits(f)
{
}

void GxFlags::Set(int f)
{
	bits |= f;
}

void GxFlags::Set(int f, bool b)
{
	if(b) bits |= f; else bits &= ~f;
}

void GxFlags::Reset(int f)
{
	bits &= ~f;
}

void GxFlags::Flip(int f)
{
	bits ^= f;
}

bool GxFlags::Test(int f)
{
	return (bits & f) == f;
}

bool GxFlags::operator[] (int f) const
{
	return (bits & f) != 0;
}

// GxMargin

template <typename T>
GxMargin<T>::GxMargin()
	:l(0), t(0), r(0), b(0) 
{
}

template <typename T>
GxMargin<T>::GxMargin(T s)
	:l(s), t(s), r(s), b(s)
{
}

template <typename T>
GxMargin<T>::GxMargin(T w, T h)
	:l(w), t(w), r(h), b(h)
{
}

template <typename T> 
GxMargin<T>::GxMargin(T _l, T _t, T _r, T _b)
	:l(_l), t(_t), r(_r), b(_b)
{
}

// GxVec2

template <typename T>
GxVec2<T>::GxVec2()
	:x(0), y(0)
{
}

template <typename T>
GxVec2<T>::GxVec2(T _x, T _y)
	:x(_x), y(_y)
{
}

template <typename T>
template <typename R>
GxVec2<T>::GxVec2(const GxVec2<R>& v)
	:x((T)v.x), y((T)v.y)
{
}

template <typename T>
void GxVec2<T>::Set(T _x, T _y)
{
	x = _x, y = _y;
}

template <typename T>
void GxVec2<T>::Add(T dx, T dy)
{
	x += dx, y += dy;
}

template <typename T> GxVec2<T>
GxVec2<T>::operator- () const
{
	return GxVec2<T>(-x, -y);
}

template <typename T>
bool GxVec2<T>::operator == (const GxVec2<T>& v) const
{
	return (x == v.x && y == v.y);
}

template <typename T>
bool GxVec2<T>::operator != (const GxVec2<T>& v) const
{
	return (x != v.x || y != v.y);
}

template <typename T>
GxVec2<T> operator + (const GxVec2<T>& a, const GxVec2<T>& b)
{
	return GxVec2<T>(a.x+b.x, a.y+b.y);
}

template <typename T>
GxVec2<T> operator - (const GxVec2<T>& a, const GxVec2<T>& b)
{
	return GxVec2<T>(a.x-b.x, a.y-b.y);
}

// GxVec3

template <typename T>
GxVec3<T>::GxVec3()
	:x(0), y(0), z(0)
{
}

template <typename T>
GxVec3<T>::GxVec3(T _x, T _y, T _z)
	:x(_x), y(_y), z(_z)
{
}

template <typename T>
template <typename R>
GxVec3<T>::GxVec3(const GxVec3<R>& v)
	:x((T)v.x), y((T)v.y), z((T)v.z)
{
}

// GxVec4

template <typename T>
GxVec4<T>::GxVec4()
	:x(0), y(0), z(0), w(0)
{
}

template <typename T>
GxVec4<T>::GxVec4(T _x, T _y, T _z, T _w)
	:x(_x), y(_y), z(_z), w(_w)
{
}

template <typename T>
template <typename R>
GxVec4<T>::GxVec4(const GxVec4<R>& v)
	:x((T)v.x), y((T)v.y), z((T)v.z), w((T)v.w)
{
}

// GxColor

inline GxColor::GxColor()
	:r(255), g(255), b(255), a(255)
{
}

inline GxColor::GxColor(int i)
	:r((uchar)i), g((uchar)i), b((uchar)i), a(255)
{
}

inline GxColor::GxColor(int i, int _a)
	:r((uchar)i), g((uchar)i), b((uchar)i), a((uchar)_a)
{
}

inline GxColor::GxColor(int _r, int _g, int _b, int _a)
	:r((uchar)_r), g((uchar)_g), b((uchar)_b), a((uchar)_a)
{
}

inline GxColor::GxColor(float _r, float _g, float _b, float _a)
	:r((uchar)GxClamp((int)(_r*255.f+0.5f), 0, 255))
	,g((uchar)GxClamp((int)(_g*255.f+0.5f), 0, 255))
	,b((uchar)GxClamp((int)(_b*255.f+0.5f), 0, 255))
	,a((uchar)GxClamp((int)(_a*255.f+0.5f), 0, 255))
{
}

inline GxColor GxColor::Alpha(int _a) const
{
	return GxColor(r, g, b, _a);
}

inline GxColor GxColor::Alpha(float _a) const
{
	return GxColor(r, g, b, GxClamp((int)(_a*255.f+0.5f), 0, 255));
}

// GxRect

template <typename T>
GxRect<T>::GxRect()
	:x(0), y(0), w(0), h(0)
{
}

template <typename T>
GxRect<T>::GxRect(T _x, T _y, T _w, T _h)
	:x(_x), y(_y), w(_w), h(_h)
{
}

template <typename T>
GxRect<T>::GxRect(const GxArea<T>& a)
	:x(a.l), y(a.t), w(a.r-a.l), h(a.b-a.t)
{
}

template <typename T>
template <typename R>
GxRect<T>::GxRect(const GxRect<R>& r)
	:x((T)r.x), y((T)r.y), w((T)r.w), h((T)r.h) {}

template <typename T>
void GxRect<T>::Shrink(T m)
{
	x+=m, y+=m, w-=m*2, h-=m*2;
}

template <typename T>
void GxRect<T>::Shrink(T l, T t, T r, T b)
{
	x+=l, y+=t, w-=l+r, h-=t+b;
}

template <typename T>
void GxRect<T>::Shrink(const GxMargin<T>& m)
{
	x+=m.l, y+=m.t, w-=m.l+m.r, h-=m.t+m.b;
}

template <typename T> 
void GxRect<T>::Expand(T m)
{
	x-=m, y-=m, w+=m*2, h+=m*2;
}

template <typename T>
void GxRect<T>::Expand(T l, T t, T r, T b)
{
	x-=l, y-=t, w+=l+r, h+=t+b;
}

template <typename T>
void GxRect<T>::Expand(const GxMargin<T>& m)
{
	x-=m.l, y-=m.t, w+=m.l+m.r, h+=m.t+m.b;
}

template <typename T>
bool GxRect<T>::Contains(T _x, T _y) const
{
	return (_x>=x && _x<x+w && _y>=y && _y<y+h);
}

// GxArea

template <typename T>
GxArea<T>::GxArea()
	:l(0), t(0), r(0), b(0)
{
}

template <typename T>
GxArea<T>::GxArea(T _l, T _t, T _r, T _b)
	:l(_l), t(_t), r(_r), b(_b)
{
}

template <typename T>
GxArea<T>::GxArea(const GxRect<T>& r)
	:l(r.x), t(r.y), r(r.x+r.w), b(r.y+r.h)
{
}

template <typename T>
template <typename R>
GxArea<T>::GxArea(const GxArea<R>& a)
	:l((T)a.l), t((T)a.t), r((T)a.r), b((T)a.b)
{
}

}; // namespace core
}; // namespace guix