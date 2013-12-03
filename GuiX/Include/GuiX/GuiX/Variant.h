/// @file
/// Contains the variant class.

#pragma once

#include <GuiX/Common.h>
#include <GuiX/String.h>

namespace guix {
namespace core {

// ===================================================================================
// GxVariant
// ===================================================================================
/** The GxVariant class acts as a union of common types and GuiX classes.
 
 The GxVariant class can store a single value, that can be one of several types. The
 following value types can be stored: bool, int, uint, float, double, GxVec2, GxVec3,
 GxVec4, GxRect, GxArea, GxColor and GxString.

 Value types that fit within the size of a pointer are stored directly. These are:
 bool, int, uint and float. Doubles and classes are allocated on the heap and stored
 by pointer instead.

 The value can be extracted from a variant through the \c ToType() functions, which
 will either return the value directly if the types match, or try to convert the
 value if the types don't match. If the value can not be converted the function
 returns zero or a default constructed object. The following types can be converted:

 \verbatim

 - bool:     to all C-types and GxString.
 - int:      to all C-types and GxString.
 - uint:     to all C-types and GxString.
 - float:    to all C-types and GxString.
 - double:   to all C-types and GxString.
 - GxVec2:   to any other GxVec2 type.
 - GxVec3:   to any other GxVec3 type.
 - GxVec4:   to any other GxVec4 type.
 - GxRect:   to any other GxRect or GxArea type.
 - GxArea:   to any other GxRect or GxArea type.
 - GxString: to all C-types.

 \endverbatim
*/
class GUIX_API GxVariant
{
public:
	~GxVariant();

	/// Enumeration of the value types that can be stored inside a GxVariant.
	enum ValueType
	{
		T_EMPTY,    ///< Empty variant.
		T_BOOL,     ///< Boolean value.
		T_INT,      ///< Signed integer.
		T_UINT,     ///< Unsigned integer.
		T_FLOAT,    ///< Floating point value.

		/// Not a value type, but indicates the start of types stored by pointer.
		TYPE_BY_PTR,

		T_DOUBLE, ///< Double precision floating point value.
		T_STRING, ///< GxString class.
		T_VEC2I,  ///< GxVec2i class.
		T_VEC2F,  ///< GxVec2f class.
		T_VEC3I,  ///< GxVec3i class.
		T_VEC3F,  ///< GxVec3f class.
		T_VEC4I,  ///< GxVec4i class.
		T_VEC4F,  ///< GxVec4f class.
		T_RECTI,  ///< GxRecti class.
		T_RECTF,  ///< GxRectf class.
		T_AREAI,  ///< GxAreai class.
		T_AREAF,  ///< GxAreaf class.
		T_COLOR,  ///< GxColor class.

		TYPE_COUNT, ///< The number of enumeration values.
	};

	/// Constructs an empty variant.
	GxVariant();

	/// Constructs a variant by copying the content of another variant.
	GxVariant(const GxVariant& other);

	GxVariant(bool v);   ///< Construct a variant that contains a bool.
	GxVariant(int v);    ///< Construct a variant that contains an int.
	GxVariant(uint v);   ///< Construct a variant that contains a uint.
	GxVariant(float v);  ///< Construct a variant that contains a float.
	GxVariant(double v); ///< Construct a variant that contains a double.
	
	GxVariant(const GxVec2i& v);  ///< Construct a variant that contains a GxVec2i.
	GxVariant(const GxVec2f& v);  ///< Construct a variant that contains a GxVec2f.
	GxVariant(const GxVec3i& v);  ///< Construct a variant that contains a GxVec3i.
	GxVariant(const GxVec3f& v);  ///< Construct a variant that contains a GxVec3f.
	GxVariant(const GxVec4i& v);  ///< Construct a variant that contains a GxVec4i.
	GxVariant(const GxVec4f& v);  ///< Construct a variant that contains a GxVec4f.
	GxVariant(const GxRecti& v);  ///< Construct a variant that contains a GxRecti.
	GxVariant(const GxRectf& v);  ///< Construct a variant that contains a GxRectf. 
	GxVariant(const GxAreai& v);  ///< Construct a variant that contains a GxAreai.
	GxVariant(const GxAreaf& v);  ///< Construct a variant that contains a GxAreaf.
	GxVariant(const GxColor& v);  ///< Construct a variant that contains a GxColor.
	GxVariant(const GxString& v); ///< Construct a variant that contains a GxString.
	GxVariant(const char* v);     ///< Construct a variant that contains a GxString.

	bool ToBool() const;       ///< Returns the value, or false if conversion fails.
	int ToInt() const;         ///< Returns the value, or 0 if conversion fails.
	uint ToUint() const;       ///< Returns the value, or 0 if conversion fails.
	float ToFloat() const;     ///< Returns the value, or 0.0 if conversion fails.
	double ToDouble() const;   ///< Returns the value, or 0.0 if conversion fails.

	GxVec2i ToVec2i() const;   ///< Returns the value, or GxVec2i() if conversion fails.
	GxVec2f ToVec2f() const;   ///< Returns the value, or GxVec2f() if conversion fails.
	GxVec3i ToVec3i() const;   ///< Returns the value, or GxVec3i() if conversion fails.
	GxVec3f ToVec3f() const;   ///< Returns the value, or GxVec3f() if conversion fails.
	GxVec4i ToVec4i() const;   ///< Returns the value, or GxVec4i() if conversion fails.
	GxVec4f ToVec4f() const;   ///< Returns the value, or GxVec4f() if conversion fails.
	GxRecti ToRecti() const;   ///< Returns the value, or GxRecti() if conversion fails.
	GxRectf ToRectf() const;   ///< Returns the value, or GxRectf() if conversion fails.
	GxAreai ToAreai() const;   ///< Returns the value, or GxAreai() if conversion fails.
	GxAreaf ToAreaf() const;   ///< Returns the value, or GxAreaf() if conversion fails.
	GxColor ToColor() const;   ///< Returns the value, or GxColor() if conversion fails.
	GxString ToString() const; ///< Returns the value, or GxString() if conversion fails.

	/// Returns the type of the current value that is stored inside the variant.
	ValueType Type() const;

	/// Returns true if the value that is currently stored is of type, or can be converted to type.
	bool Is(ValueType type) const;

	/// Returns true if the variant does not contain a value, which is when type is T_EMPTY.
	bool Empty() const;

	/// Copies the contents of another variant.
	void Assign(const GxVariant& other);

	/// Returns true if the types and values of both variants are equivalent.
	bool Equals(const GxVariant& other) const;

	/// Calls \c Assign().
	GxVariant& operator = (const GxVariant& other);

private:
	union
	{
		int myInt;
		uint myUint;
		float myFloat;
		void* myData;
	};
	ValueType myType;
};

// Global operators for comparing variants to common types, for convenience.
bool operator == (const GxVariant& a, int b);              ///< Returns true if the contents of variant a and int b are equivalent.
bool operator == (const GxVariant& a, const char* b);      ///< Returns true if the contents of variant a and string b are equivalent.
bool operator == (const GxVariant& a, const GxString& b);  ///< Returns true if the contents of variant a and string b are equivalent.
bool operator == (const GxVariant& a, const GxVariant& b); ///< Returns true if the contents of variant a and b are equivalent.

}; // namespace core
}; // namespace guix