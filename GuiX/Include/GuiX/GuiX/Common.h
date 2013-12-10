/// @file
/// Contains several commonly used functions and classes in GuiX.

#pragma once

#include <GuiX/Config.h>

namespace guix {
namespace core {

// Forward declaration, allows GxRect to have an GxArea constructor.
template <typename T> struct GxArea;

// Utility functions.

/// Returns the lesser of a and b, or a if both are equivalent.
template <typename T> T GxMin(T a, T b);

/// Returns the greater of a and b, or a if both are equivalent.
template <typename T> T GxMax(T a, T b);

/// Returns x clamped to the range (min, max) such that x is greater than or equal to min and x is less than or equal to max.
template <typename T> T GxClamp(T x, T min, T max);

/// Returns the result of linear interpolation from begin to end with factor t, where t is a value between 0 and 1.
template <typename T> T GxLerp(T begin, T end, T t);

/// Returns the factor t, such that linear interpolation from begin to end with factor t results in x.
template <typename T> T GxLerpFactor(T begin, T end, T x);

/// Returns the result of remapping x from range (inBegin, inEnd) to range (outBegin, outEnd)
template <typename T> T GxRemap(T x, T inBegin, T inEnd, T outBegin, T outEnd);

/// Returns the value of x (a float or double) rounded to the nearest integer.
template <typename T> int GxInt(T x);

/// Allocates an array of count elements using malloc.
template <typename T> T* GxMalloc(size_t count);

/// Reallocates an array of count elements using realloc.
template <typename T> T* GxRealloc(T* ptr, size_t count);

/// Releases an array allocated with GxMalloc or GxRealloc.
template <typename T> void GxFree(T* ptr);

/// Swaps the values of a and b using a temporary variable.
template <typename T> void GxSwap(T& a, T& b);

/// Returns the number of characters in string, excluding the null-terminator.
template <typename T> int GxStrLen(const T* string);

/// Returns the difference between the values of first mismatching characters, or zero if the strings are equal.
template <typename T> int GxStrCmp(const T* a, const T* b);

// Utility structs.

/// The GxFlags struct contains a set of bits that represent generic flags.
struct GUIX_API GxFlags
{
	inline GxFlags();      ///< All bits are set to 0.
	inline GxFlags(int f); ///< All bits are copied from f.

	inline void Set(int f);         ///< Sets the bits specified by f to 1.
	inline void Reset(int f);       ///< Sets the bits specified by f to 0.
	inline void Set(int f, bool b); ///< Sets the bits specified by f to b.
	inline void Flip(int f);        ///< Flips the bits specified by f.
	inline bool Test(int f);        ///< Returns true if all bits specified by f are 1.

	inline bool operator[] (int f) const; ///< Returns true if (bits & f) is non-zero.

	int bits; ///< Contains the flag bits.
};

/// The GxMargin struct specifies the size of the margins inside a rectangular region.
template <typename T>
struct GUIX_API GxMargin
{
	GxMargin();                   ///< All components are set to 0.
	GxMargin(T size);             ///< All components are set to size.
	GxMargin(T w, T h);           ///< Components (l,r) are set to w and (t,b) are set to h.
	GxMargin(T l, T t, T r, T b); ///< Each components is set to the corresponding argument.

	// Public members.
	union {
		struct {
			T l; ///< Margin at the left side of a region.
			T t; ///< Margin at the top side of a region.
			T r; ///< Margin at the right side of a region.
			T b; ///< Margin at the bottom side of a region.
		};
		struct {
			T cell[4]; ///< Array of LTRB components.
		};
	};
};

typedef GxMargin<float> GxMarginf;
typedef GxMargin<int>   GxMargini;

/// The GxVec2 class represents a vector with two components.
template <typename T>
struct GUIX_API GxVec2
{
	GxVec2();         ///< All components are set to 0.
	GxVec2(T x, T y); ///< Each component is set to the corresponding argument.

	template <typename R>
	GxVec2(const GxVec2<R>& v); ///< Convenient casting between types.

	void Set(T x, T y); ///< Each component is set to the corresponding argument.
	void Add(T x, T y); ///< Each argument is added to the corresponding component.

	GxVec2 operator- () const;                ///< Returns the negated version of this vector.
	bool operator == (const GxVec2& v) const; ///< Returns true if all components are equal.
	bool operator != (const GxVec2& v) const; ///< Returns true if any component is not equal.

	// Public members.
	union {
		struct {
			T x; ///< The x-component.
			T y; ///< The y-component.
		};
		struct {
			T cell[2]; ///< Array of XY components.
		};
	};
};

/// Returns the result of per-component addition of vector a and b.
template <typename T> GxVec2<T> GUIX_API operator + (const GxVec2<T>& a, const GxVec2<T>& b);

/// Returns the result of per-component subtraction of vector b from a.
template <typename T> GxVec2<T> GUIX_API operator - (const GxVec2<T>& a, const GxVec2<T>& b);

// Typedefs of common vector types.
typedef GxVec2<float>  GxVec2f;
typedef GxVec2<double> GxVec2d;
typedef GxVec2<int>    GxVec2i;

/// The GxVec3 class represents a vector with three components.
template <typename T>
struct GUIX_API GxVec3
{
public:
	GxVec3();              ///< All components are set to 0.
	GxVec3(T x, T y, T z); ///< Each component is set to the corresponding argument.

	template <typename R>
	GxVec3(const GxVec3<R>& v); ///< Convenient casting between types.

	// Public members.
	union {
		struct {
			T x; ///< The x-component.
			T y; ///< The y-component.
			T z; ///< The z-component.
		};
		struct {
			T cell[3]; ///< Array of XYZ components.
		};
	};
};

// Typedefs of common vector types.
typedef GxVec3<float>  GxVec3f;
typedef GxVec3<double> GxVec3d;
typedef GxVec3<int>    GxVec3i;

/// The GxVec4 class represents a vector with four components.
template <typename T>
struct GUIX_API GxVec4
{
	GxVec4();                   ///< All components are set to 0.
	GxVec4(T x, T y, T z, T w); ///< Each component is set to the corresponding argument.

	template <typename R>
	GxVec4(const GxVec4<R>& v); ///< Convenient casting between types.

	// Public members.
	union {
		struct {
			T x; ///< The x-component.
			T y; ///< The y-component.
			T z; ///< The z-component.
			T w; ///< The w-component.
		};
		struct {
			T cell[4]; ///< Array of XYZW components.
		};
	};
};

// Typedefs of common vector types.
typedef GxVec4<float>  GxVec4f;
typedef GxVec4<double> GxVec4d;
typedef GxVec4<int>    GxVec4i;

/// The GxColor class represents an RGBA color stored as four 8-bit values.
class GUIX_API GxColor
{
public:
	GxColor();                                     ///< All channels are set to 255.
	GxColor(int i);                                ///< The RGB channels are set to i, alpha is set to 255.
	GxColor(int i, int a);                         ///< The RGB channels are set to i, alpha is set to a.
	GxColor(int r, int g, int b, int a=255);       ///< Each channel is set to the corresponding argument. 
	GxColor(float r, float g, float b, float a=1); ///< Each channel is set to the corresponding argument and multiplied by 255.

	GxColor Alpha(int a) const;   ///< Returns the RGB channels of this color and alpha a.
	GxColor Alpha(float a) const; ///< Returns the RGB channels of this color and alpha a. Alpha is multiplied by 255.

	// Public members.
	union {
		struct {
			uchar r; ///< The red channel.
			uchar g; ///< The green channel.
			uchar b; ///< The blue channel.
			uchar a; ///< The alpha channel.
		};
		struct {
			uchar channels[4]; ///< Array of RGBA channels.
		};
	};
};

/// The GxRect class represents a 2D rectangular region stored as a position and size.
template <typename T>
struct GUIX_API GxRect
{
public:
	GxRect();                      ///< All components are set to 0.
	GxRect(T x, T y, T w, T h);    ///< Each component is set to the corresponding argument.
	GxRect(const GxArea<T>& area); ///< Constructs a rect from an area.

	template <typename R>
	GxRect(const GxRect<R>& rect); ///< Convenient casting between types.

	void Shrink(T m);                  ///< Sets the components to (x+m, y+m, w-m*2, h-m*2).
	void Shrink(T l, T t, T r, T b);   ///< Sets the components to (x+l, y+t, w-l-r, h-t-b).
	void Shrink(const GxMargin<T>& m); ///< Subtracts the given margins from each side.

	void Expand(T m);                  ///< Sets the components to (x-m, y-m, w+m*2, h+m*2).
	void Expand(T l, T t, T r, T b);   ///< Sets the components to (x-l, y-t, w+l+r, h+t+b).
	void Expand(const GxMargin<T>& m); ///< Adds the given margins to each side.

	bool Contains(T x, T y) const;     ///< Returns true if point (x,y) lies within this rect.

	// Public members.
	union {
		struct {
			T x; ///< top-left x position.
			T y; ///< top-left y position.
			T w; ///< rectangle width.
			T h; ///< rectangle height.
		};
		struct {
			T cell[4]; ///< Array of XYWH components.
		};
	};
};

typedef GxRect<float> GxRectf;
typedef GxRect<int>   GxRecti;

/// The GxArea struct represents a 2D rectangular region stored as two corner positions.
template <typename T>
struct GUIX_API GxArea
{
	GxArea();                      ///< All components are set to 0.
	GxArea(T l, T t, T r, T b);    ///< Each component is set to the corresponding argument.
	GxArea(const GxRect<T>& rect); ///< Constructs an area from a rect.

	template <typename R>
	GxArea(const GxArea<R>& area); ///< Convenient casting between types.

	// Public members.
	union {
		struct {
			T l; ///< top-left x position.
			T t; ///< top-left y position.
			T r; ///< bottom-right x position.
			T b; ///< bottom-right y position.
		};
		struct {
			T cell[4]; ///< Array of LTRB components.
		};
	};
};

typedef GxArea<float> GxAreaf;
typedef GxArea<int>   GxAreai;

}; // namespace core
}; // namespace guix

#include <GuiX/Common.inl>