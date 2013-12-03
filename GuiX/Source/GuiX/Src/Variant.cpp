#include <GuiX/Config.h>

#include <string.h>

#include <GuiX/Variant.h>

namespace guix {
namespace core {

namespace {

// ===================================================================================
// Helper functions
// ===================================================================================

static bool mapInit = false;
static bool mapData[GxVariant::TYPE_COUNT * GxVariant::TYPE_COUNT];

static void SetMap(int a, int b)
{
	mapData[a * GxVariant::TYPE_COUNT + b] = true;
	mapData[b * GxVariant::TYPE_COUNT + a] = true;
}

static void InitMap()
{
	memset(mapData, 0, GxVariant::TYPE_COUNT * GxVariant::TYPE_COUNT * sizeof(bool));

	// Identity, map all types to themselves.
	for(int i=0; i<GxVariant::TYPE_COUNT; ++i) SetMap(i, i);

	// Map basic types to other basic types.
	SetMap(GxVariant::T_BOOL, GxVariant::T_INT);
	SetMap(GxVariant::T_BOOL, GxVariant::T_UINT);
	SetMap(GxVariant::T_BOOL, GxVariant::T_FLOAT);
	SetMap(GxVariant::T_BOOL, GxVariant::T_DOUBLE);
	SetMap(GxVariant::T_BOOL, GxVariant::T_STRING);
	SetMap(GxVariant::T_INT, GxVariant::T_UINT);
	SetMap(GxVariant::T_INT, GxVariant::T_FLOAT);
	SetMap(GxVariant::T_INT, GxVariant::T_DOUBLE);
	SetMap(GxVariant::T_INT, GxVariant::T_STRING);
	SetMap(GxVariant::T_UINT, GxVariant::T_FLOAT);
	SetMap(GxVariant::T_UINT, GxVariant::T_DOUBLE);
	SetMap(GxVariant::T_UINT, GxVariant::T_STRING);
	SetMap(GxVariant::T_FLOAT, GxVariant::T_DOUBLE);
	SetMap(GxVariant::T_FLOAT, GxVariant::T_STRING);
	SetMap(GxVariant::T_DOUBLE, GxVariant::T_STRING);

	// Map classes to compatible class types.
	SetMap(GxVariant::T_VEC2I, GxVariant::T_VEC2F);
	SetMap(GxVariant::T_VEC3I, GxVariant::T_VEC3F);
	SetMap(GxVariant::T_VEC4I, GxVariant::T_VEC4F);
	SetMap(GxVariant::T_RECTI, GxVariant::T_RECTF);
	SetMap(GxVariant::T_RECTI, GxVariant::T_AREAI);
	SetMap(GxVariant::T_RECTI, GxVariant::T_AREAF);
	SetMap(GxVariant::T_RECTF, GxVariant::T_AREAI);
	SetMap(GxVariant::T_RECTF, GxVariant::T_AREAF);
	SetMap(GxVariant::T_AREAI, GxVariant::T_AREAF);

	mapInit = true;
}

struct vCopy
{
	const void* src;
	void** dst;

	template <typename T>
	inline void func()
	{
		*dst = new T(*reinterpret_cast<const T*>(src));
	}
};

struct vDelete
{
	void* data;

	template <typename T>
	inline void func()
	{
		delete reinterpret_cast<T*>(data);
	}
};

struct vCompare
{
	const void* src;
	const void* dst;
	bool isEqual;

	template <typename T>
	inline void func()
	{
		const T* a = reinterpret_cast<const T*>(src);
		const T* b = reinterpret_cast<const T*>(dst);
		isEqual = (*a == *b);
	}
};

template <typename Func>
static void vFunc(GxVariant::ValueType type, Func& f)
{
	switch(type)
	{
		case GxVariant::T_DOUBLE: f.func<double>(); break;
		case GxVariant::T_STRING: f.func<GxString>(); break;
		case GxVariant::T_VEC2I:  f.func<GxVec2i>(); break;
		case GxVariant::T_VEC2F:  f.func<GxVec2f>(); break;
		case GxVariant::T_VEC3I:  f.func<GxVec3i>(); break;
		case GxVariant::T_VEC3F:  f.func<GxVec3f>(); break;
		case GxVariant::T_VEC4I:  f.func<GxVec4i>(); break;
		case GxVariant::T_VEC4F:  f.func<GxVec4f>(); break;
		case GxVariant::T_RECTI:  f.func<GxRecti>(); break;
		case GxVariant::T_RECTF:  f.func<GxRectf>(); break;
		case GxVariant::T_AREAI:  f.func<GxAreai>(); break;
		case GxVariant::T_AREAF:  f.func<GxAreaf>(); break;
		case GxVariant::T_COLOR:  f.func<GxColor>(); break;
	};
}

}; // anonymous namespace

template <typename T>
static inline T* vNew(const T& value)
{
	return new T(value);
}

template <typename T>
static inline const T& vCast(void* data)
{
	return *reinterpret_cast<const T*>(data);
}

// ===================================================================================
// GxVariant
// ===================================================================================

GxVariant::~GxVariant()
{
	if(myType > TYPE_BY_PTR)
	{
		vDelete del = {myData};
		vFunc(myType, del);
	}
}

GxVariant::GxVariant()
	:myType(T_EMPTY)
	,myData(NULL)
{
}

GxVariant::GxVariant(const GxVariant& other)
	:myType(T_EMPTY)
	,myData(NULL)
{
	Assign(other);
}

GxVariant::GxVariant(bool v)			:myType(T_BOOL)     {myInt = (v ? 1 : 0);}
GxVariant::GxVariant(int v)				:myType(T_INT)      {myInt = v;}
GxVariant::GxVariant(uint v)			:myType(T_UINT)     {myUint = v;}
GxVariant::GxVariant(float v)			:myType(T_FLOAT)    {myFloat = v;}
GxVariant::GxVariant(double v)			:myType(T_DOUBLE)   {myData = vNew(v);}
GxVariant::GxVariant(const GxVec2i& v)	:myType(T_VEC2I)    {myData = vNew(v);}
GxVariant::GxVariant(const GxVec2f& v)	:myType(T_VEC2F)    {myData = vNew(v);}
GxVariant::GxVariant(const GxVec3i& v)	:myType(T_VEC3I)    {myData = vNew(v);}
GxVariant::GxVariant(const GxVec3f& v)	:myType(T_VEC3F)    {myData = vNew(v);}
GxVariant::GxVariant(const GxVec4i& v)	:myType(T_VEC4I)    {myData = vNew(v);}
GxVariant::GxVariant(const GxVec4f& v)	:myType(T_VEC4F)    {myData = vNew(v);}
GxVariant::GxVariant(const GxRecti& v)	:myType(T_RECTI)    {myData = vNew(v);}
GxVariant::GxVariant(const GxRectf& v)	:myType(T_RECTF)    {myData = vNew(v);}
GxVariant::GxVariant(const GxAreai& v)	:myType(T_AREAI)    {myData = vNew(v);}
GxVariant::GxVariant(const GxAreaf& v)	:myType(T_AREAF)    {myData = vNew(v);}
GxVariant::GxVariant(const GxColor& v)	:myType(T_COLOR)    {myData = vNew(v);}
GxVariant::GxVariant(const GxString& v)	:myType(T_STRING)   {myData = vNew(v);}
GxVariant::GxVariant(const char* str)   :myType(T_STRING)   {myData = vNew(GxString(str));}

bool GxVariant::ToBool() const
{
	switch(myType)
	{
		case T_BOOL:   return myInt != 0;
		case T_INT:    return myInt != 0;
		case T_UINT:   return myUint != 0;
		case T_FLOAT:  return myFloat != 0;
		case T_DOUBLE: return vCast<double>(myData) != 0;
		case T_STRING: return vCast<GxString>(myData).ToBool();
	};
	return false;
}

int GxVariant::ToInt() const
{
	switch(myType)
	{
		case T_INT:    return myInt;
		case T_UINT:   return (int)myUint;
		case T_FLOAT:  return (int)(myFloat + 0.5f);
		case T_DOUBLE: return (int)(vCast<double>(myData) + 0.5);
		case T_STRING: return vCast<GxString>(myData).ToInt();
		case T_BOOL:   return myInt;
	};
	return 0;
}

uint GxVariant::ToUint() const
{
	switch(myType)
	{
		case T_UINT:   return myUint;
		case T_INT:    return (uint)myInt;
		case T_FLOAT:  return (uint)(myFloat + 0.5f);
		case T_DOUBLE: return (uint)(vCast<double>(myData) + 0.5);
		case T_STRING: return vCast<GxString>(myData).ToUint();
		case T_BOOL:   return (uint)myInt;
	};
	return 0;
}

float GxVariant::ToFloat() const
{
	switch(myType)
	{
		case T_FLOAT:  return myFloat;
		case T_DOUBLE: return (float)vCast<double>(myData);
		case T_INT:    return (float)myInt;
		case T_UINT:   return (float)myUint;
		case T_STRING: return vCast<GxString>(myData).ToFloat();
		case T_BOOL:   return (float)myInt;
	};
	return 0.f;
}

double GxVariant::ToDouble() const
{
	switch(myType)
	{
		case T_DOUBLE: return vCast<double>(myData);
		case T_FLOAT:  return (double)myFloat;
		case T_INT:    return (double)myInt;
		case T_UINT:   return (double)myUint;
		case T_STRING: return vCast<GxString>(myData).ToDouble();
		case T_BOOL:   return (float)myInt;
	};
	return 0.0;
}

GxVec2i GxVariant::ToVec2i() const
{
	switch(myType)
	{
		case T_VEC2I: return vCast<GxVec2i>(myData);
		case T_VEC2F: return vCast<GxVec2f>(myData);
	};
	return GxVec2i();
}

GxVec2f GxVariant::ToVec2f() const
{
	switch(myType)
	{
		case T_VEC2F: return vCast<GxVec2f>(myData);
		case T_VEC2I: return vCast<GxVec2i>(myData);
	};
	return GxVec2f();
}

GxVec3i GxVariant::ToVec3i() const
{
	switch(myType)
	{
		case T_VEC3I: return vCast<GxVec3i>(myData);
		case T_VEC3F: return vCast<GxVec3f>(myData);
	};
	return GxVec3i();
}

GxVec3f GxVariant::ToVec3f() const
{
	switch(myType)
	{
		case T_VEC3F: return vCast<GxVec3f>(myData);
		case T_VEC3I: return vCast<GxVec3i>(myData);
	};
	return GxVec3f();
}

GxVec4i GxVariant::ToVec4i() const
{
	switch(myType)
	{
		case T_VEC4I: return vCast<GxVec4i>(myData);
		case T_VEC4F: return vCast<GxVec4f>(myData);
	};
	return GxVec4i();
}

GxVec4f GxVariant::ToVec4f() const
{
	switch(myType)
	{
		case T_VEC4F: return vCast<GxVec4f>(myData);
		case T_VEC4I: return vCast<GxVec4i>(myData);
	};
	return GxVec4f();
}

GxRecti GxVariant::ToRecti() const
{
	switch(myType)
	{
		case T_RECTI: return vCast<GxRecti>(myData);
		case T_RECTF: return vCast<GxRectf>(myData);
		case T_AREAI: return vCast<GxAreai>(myData);
		case T_AREAF: return vCast<GxAreaf>(myData);
	};
	return GxRecti();
}

GxRectf GxVariant::ToRectf() const
{
	switch(myType)
	{
		case T_RECTF: return vCast<GxRectf>(myData);
		case T_RECTI: return vCast<GxRecti>(myData);
		case T_AREAF: return vCast<GxAreaf>(myData);
		case T_AREAI: return vCast<GxAreai>(myData);
	};
	return GxRectf();
}

GxAreai GxVariant::ToAreai() const
{
	switch(myType)
	{
		case T_AREAI: return vCast<GxAreai>(myData);
		case T_AREAF: return vCast<GxAreaf>(myData);
		case T_RECTI: return vCast<GxRecti>(myData);
		case T_RECTF: return vCast<GxRectf>(myData);
	};
	return GxAreai();
}

GxAreaf GxVariant::ToAreaf() const
{
	switch(myType)
	{
		case T_AREAF: return vCast<GxAreaf>(myData);
		case T_AREAI: return vCast<GxAreai>(myData);
		case T_RECTF: return vCast<GxRectf>(myData);
		case T_RECTI: return vCast<GxRecti>(myData);
	};
	return GxAreaf();
}

GxColor GxVariant::ToColor() const
{
	switch(myType)
	{
		case T_COLOR:  return vCast<GxColor>(myData);
	};
	return GxColor();
}

GxString GxVariant::ToString() const
{
	switch(myType)
	{
		case T_STRING: return vCast<GxString>(myData);
		case T_BOOL:   return GxString().Arg(myInt != 0);
		case T_INT:    return GxString().Arg(myInt);
		case T_UINT:   return GxString().Arg(myUint);
		case T_FLOAT:  return GxString().Arg(myFloat);
		case T_DOUBLE: return GxString().Arg(vCast<double>(myData));
	};
	return GxString();
}

GxVariant::ValueType GxVariant::Type() const
{
	return myType;
}

bool GxVariant::Is(ValueType type) const
{
	if(!mapInit)
		InitMap();

	if(myType >= TYPE_COUNT || type >= TYPE_COUNT)
		return false;

	return mapData[myType * TYPE_COUNT + type];
}

bool GxVariant::Empty() const
{
	return myType == T_EMPTY;
}

void GxVariant::Assign(const GxVariant& other)
{
	if(this == &other) return;

	// Delete old data.
	if(myType > TYPE_BY_PTR)
	{
		vDelete del = {myData};
		vFunc(myType, del);
	}

	// Copy new data.
	if(other.myType > TYPE_BY_PTR)
	{
		vCopy copy = {other.myData, &myData};
		vFunc(other.myType, copy);
		myType = other.myType;
	}
	else
	{
		memcpy(this, &other, sizeof(GxVariant));		
	}
}

bool GxVariant::Equals(const GxVariant& other) const
{
	if(myType != other.myType)
		return false;

	if(myType < TYPE_BY_PTR)
		return myInt == other.myInt;

	vCompare comp = {other.myData, myData};
	vFunc(myType, comp);
	return comp.isEqual;	
}

// Assignment operators
GxVariant& GxVariant::operator = (const GxVariant& other)
	{ Assign(other); return *this; }

// Equality operators
bool operator == (const GxVariant& a, int b)
	{ return (a.ToInt() == b); }

bool operator == (const GxVariant& a, const char* b)
	{ return (a.ToString() == b); }

bool operator == (const GxVariant& a, const GxString& b)
	{ return (a.ToString() == b); }

bool operator == (const GxVariant& a, const GxVariant& b)
	{ return a.Equals(b); }

}; // namespace core
}; // namespace guix