#include <GuiX/Config.h>

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>

#include <sstream>
#include <iomanip>

#include <GuiX/Common.h>
#include <GuiX/String.h>

//#define GX_STRING_DEBUG

#if defined(_MSC_VER) && _MSC_VER >= 1400
#pragma warning(disable: 4996) // vsnprintf warnings.
#endif

namespace guix {
namespace core {

namespace {

static const int _npos = INT_MAX;

}; // anonymous namespace

// ===================================================================================
// Helper functions
// ===================================================================================

namespace s {

static inline int Min(int a, int b) { return (a > b) ? b : a; }
static inline int Max(int a, int b) { return (a < b) ? b : a; }

static int Len(const char* str)
{
	const char* begin = str;
	while(*str) ++str;
	return str - begin;
}

static bool Eq(const char* a, const char* b, int len, bool caseSensitive)
{
	int i = 0;
	if(caseSensitive)
	{
		while(i < len && *a == *b)
			++a, ++b, ++i;
	}
	else
	{
		while(i < len && tolower(*a) == tolower(*b))
			++a, ++b, ++i;
	}
	return i == len;
}

static inline bool Eq(const char* a, const char* b)
{
	while(*a && *a == *b) ++a, ++b;
	return (*a == *b);
}

static inline bool iEq(const char* a, const char* b)
{
	while(*a && tolower(*a) == tolower(*b)) ++a, ++b;
	return (tolower(*a) == tolower(*b));
}

static inline int Cmp(const char* a, const char* b)
{
	while(*a && *a == *b) ++a, ++b;
	return ((int)(*a) - (int)(*b));
}

static inline int iCmp(const char* a, const char* b)
{
	while(*a && tolower(*a) == tolower(*b)) ++a, ++b;
	return ((int)tolower(*a) - (int)tolower(*b));
}

// ===================================================================================
// Reference management functions
// ===================================================================================

#ifdef GX_STRING_DEBUG
	#define GX_ASSERT_VALID				if(s::Len(myStr) != myLen) __asm { int 3 };
	#define GX_STRING_TAG(ref,a,b,c,d)	(ref->tag = (d<<24)|(c<<16)|(b<<8)|(a))
#else
	#define GX_ASSERT_VALID
	#define GX_STRING_TAG(ref,a,b,c,d)
#endif

struct Ref
{
#ifdef GX_STRING_DEBUG
	uint tag;
#endif
	size_t reserved;
	size_t count;
};

static char _null[sizeof(Ref) + 4] = {0};
static char* null = _null + sizeof(Ref);

/// Inline functions.

static inline Ref& ToRef(char* str)
{
	return reinterpret_cast<Ref*>(str)[-1];
}

static inline char* NewRef(size_t len)
{
	Ref* ref = static_cast<Ref*>(malloc(sizeof(Ref) + len + 1));
	ref->reserved = len;
	ref->count = 1;
	GX_STRING_TAG(ref, 'G','x','S','n');
	return reinterpret_cast<char*>(ref + 1);
}

static inline char* ReallocRef(Ref& src, size_t len)
{
	Ref* ref = static_cast<Ref*>(realloc(&src, sizeof(Ref) + len + 1));
	ref->reserved = len;
	GX_STRING_TAG(ref, 'G','x','S','r');
	return reinterpret_cast<char*>(ref + 1);
}

/// Utility functions.

static void Detach(char*& str, size_t len)
{
	Ref& ref = s::ToRef(str);
	if(ref.count > 1)
	{
		--ref.count;
		char* mem = s::NewRef(len);
		memcpy(mem, str, len + 1);
		str = mem;
	}
}

static void Realloc(char*& str, size_t len, size_t n)
{
	Ref& ref = s::ToRef(str);
	if(ref.count == 1)
	{
		if(ref.reserved < n)
		{
			n = s::Max(n, ref.reserved << 1);
			str = s::ReallocRef(ref, n);
		}
	}
	else if(ref.count == 0)
	{
		str = s::NewRef(n);
	}
	else
	{
		--ref.count;
		len = s::Min(len, n);
		char* mem = s::NewRef(n);
		memcpy(mem, str, len + 1);
		str = mem;
	}
}

static void AddRef(char* str)
{
	if(str != s::null)
	{
		Ref& ref = s::ToRef(str);
		++ref.count;
	}
}

static void Release(char* str)
{
	if(str != s::null)
	{
		Ref& ref = s::ToRef(str);
		if(--ref.count <= 0)
			free(&ref);
	}
}

}; // namespace s

// ===================================================================================
// StringArg
// ===================================================================================

int GxString::StringArg::_len(const char* str)
{
	const char* begin = str;
	while(*str) ++str;
	return str - begin;
}

// ===================================================================================
// Constructors and destructor
// ===================================================================================

const int GxString::npos()
{
	return _npos;
}

GxString::~GxString()
{
	GX_ASSERT_VALID;
	s::Release(myStr);
}

GxString::GxString()
	:myStr(s::null)
	,myLen(0)
{
}

GxString::GxString(int reserve)
	:myStr(s::null)
	,myLen(0)
{
	if(reserve > 0)
	{
		myStr = s::NewRef(reserve);
		myStr[0] = 0;
	}
}

GxString::GxString(char c, int n)
	:myStr(s::null)
	,myLen(0)
{
	Set(c, n);
}

GxString::GxString(const char* str)
	:myStr(s::null)
	,myLen(0)
{
	const int len = s::Len(str);
	if(len > 0)
	{
		myStr = s::NewRef(len);
		memcpy(myStr, str, len + 1);
		myLen = len;
	}
}

GxString::GxString(const char* str, int n)
	:myStr(s::null)
	,myLen(0)
{
	if(n > 0)
	{
		myStr = s::NewRef(n);
		memcpy(myStr, str, n);
		myStr[n] = 0;
		myLen = n;
	}
}

GxString::GxString(const GxString& str)
	:myStr(str.myStr)
	,myLen(str.myLen)
{
	s::AddRef(myStr);
}

GxString::GxString(const GxString& str, int pos, int n)
	:myStr(s::null)
	,myLen(0)
{
	if(pos < 0)
	{
		n += pos;
		pos = 0;
	}
	if(pos == 0 && n >= str.myLen)
	{
		Set(str);
	}
	else if(pos < str.myLen && n > 0)
	{
		const int len = s::Min(n, str.myLen - pos);
		myStr = s::NewRef(len);
		memcpy(myStr, str.myStr + pos, len);
		myStr[len] = 0;
		myLen = len;
	}
}

// ===================================================================================
// Set functions
// ===================================================================================

void GxString::Set(char c, int n)
{
	if(n > 0)
	{
		s::Realloc(myStr, myLen, n);
		memset(myStr, c, n);
		myLen = c ? n : 0;
		myStr[n] = 0;
	}
	else Clear();
}

void GxString::Set(const char* str)
{
	Set(str, s::Len(str));
}

void GxString::Set(const char* str, int n)
{
	if(myStr == str) return;

	if(n > 0)
	{
		s::Realloc(myStr, myLen, n);
		memcpy(myStr, str, n);
		myStr[n] = 0;
		myLen = n;
	}
	else Clear();
}

void GxString::Set(const GxString& str)
{
	if(myStr == str.myStr) return;

	s::Release(myStr);
	myStr = str.myStr;
	myLen = str.myLen;
	s::AddRef(myStr);	
}

// ===================================================================================
// Append functions
// ===================================================================================

void GxString::Append(char c)
{
	if(c)
	{
		const int len = myLen + 1;
		s::Realloc(myStr, myLen, len);
		myStr[myLen] = c;
		myStr[len] = 0;
		myLen = len;
	}
}

void GxString::Append(GxStringArg str)
{
	if(str.len > 0)
	{
		const int len = myLen + str.len;
		s::Realloc(myStr, myLen, len);
		memcpy(myStr + myLen, str.ptr, str.len);
		myStr[len] = 0;
		myLen = len;
	}
}

// ===================================================================================
// Prepend functions
// ===================================================================================

void GxString::Prepend(char c)
{
	if(c)
	{
		const int len = myLen + 1;
		s::Realloc(myStr, myLen, len);
		memmove(myStr + 1, myStr, myLen + 1);
		myStr[0] = c;
		myLen = len;
	}
}

void GxString::Prepend(GxStringArg str)
{
	if(str.len > 0)
	{
		const int len = myLen + str.len;
		s::Realloc(myStr, myLen, len);
		memmove(myStr + str.len, myStr, myLen + 1);
		memcpy(myStr, str.ptr, str.len);
		myLen = len;
	}
}

// ===================================================================================
// Insert functions
// ===================================================================================

void GxString::Insert(int pos, char c)
{
	if(pos >= myLen)
		Append(c);
	else if(pos <= 0)
		Prepend(c);
	else if(c)
	{
		const int len = myLen + 1;
		s::Realloc(myStr, myLen, len);
		memmove(myStr + pos + 1, myStr + pos, myLen + 1 - pos);
		myStr[pos] = c;
		myLen = len;
	}
}

void GxString::Insert(int pos, GxStringArg str)
{
	if(pos >= myLen)
		Append(str);
	else if(pos <= 0)
		Prepend(str);
	else if(str.len > 0)
	{
		const int len = myLen + str.len;
		s::Realloc(myStr, myLen, len);
		memmove(myStr + pos + str.len, myStr + pos, myLen + 1 - pos);
		memcpy(myStr + pos, str.ptr, str.len);
		myLen = len;
	}
}

// ===================================================================================
// Format functions
// ===================================================================================

void GxString::Format(const char* fmt, ...)
{
	va_list argument_list;
	va_start(argument_list, fmt);

	char buffer[256];
	const int len = vsnprintf(buffer, 255, fmt, argument_list);
	buffer[(len >= 0) ? len : 255] = 0;
	Set(buffer);

	va_end(argument_list);
}

void GxString::Format(int maxLength, const char* fmt, ...)
{
	va_list argument_list;
	va_start(argument_list, fmt);

	if(maxLength <= 255)
	{
		char buffer[256];
		const int len = vsnprintf(buffer, 255, fmt, argument_list);
		buffer[(len >= 0) ? len : 255] = 0;
		Set(buffer);
	}
	else
	{
		char* buffer = (char*)malloc(maxLength + 1);
		const int len = vsnprintf(buffer, maxLength, fmt, argument_list);
		buffer[(len >= 0) ? len : maxLength] = 0;
		Set(buffer);
		free(buffer);
	}

	va_end(argument_list);
}

// ===================================================================================
// Arg functions
// ===================================================================================

GxString& GxString::Arg(bool v, bool caps)
{
	Append(v ? (caps ? "TRUE" : "true") : (caps ? "FALSE" : "false"));
	return *this;
}

GxString& GxString::Arg(int v, bool hex)
{
	std::ostringstream ss;
	if(hex) ss << std::hex;
	ss << v;
	Append(ss.str().c_str());
	return *this;
}

GxString& GxString::Arg(uint v, bool hex)
{
	std::ostringstream ss;
	if(hex) ss << std::hex;
	ss << v;
	Append(ss.str().c_str());
	return *this;
}

GxString& GxString::Arg(float v, int precision, bool trim)
{
	std::ostringstream ss;
	std::string str;

	if(precision >= 0)
	{
		ss << std::setiosflags(std::ios::fixed);
		ss << std::setprecision(precision);
		ss << v;
		ss.str().swap(str);

		if(trim)
		{
			size_t pos = str.find_last_not_of('0');
			if(pos != std::string::npos)
			{
				size_t dec = str.find_first_of('.');
				if(dec < pos) str.resize(pos + 1);
				else if(dec == pos) str.resize(pos);
			}
		}
	}
	else
	{
		ss << v;
		ss.str().swap(str);
	}

	Append(str.c_str());
	return *this;
}

GxString& GxString::Arg(double v, int precision, bool trim)
{
	std::ostringstream ss;
	std::string str;

	if(precision >= 0)
	{
		ss << std::setiosflags(std::ios::fixed);
		ss << std::setprecision(precision);
		ss << v;
		ss.str().swap(str);

		if(trim)
		{
			size_t pos = str.find_last_not_of('0');
			if(pos != std::string::npos)
			{
				size_t dec = str.find_first_of('.');
				if(dec < pos) str.resize(pos + 1);
				else if(dec == pos) str.resize(pos);
			}
		}
	}
	else
	{
		ss << v;
		ss.str().swap(str);
	}

	Append(str.c_str());
	return *this;
}

GxString& GxString::Arg(char v)
{
	Append(v);
	return *this;
}

GxString& GxString::Arg(const char* v)
{
	Append(v);
	return *this;
}

GxString& GxString::Arg(GxStringArg v)
{
	Append(v);
	return *this;
}

// ===================================================================================
// Value conversion
// ===================================================================================

int GxString::ToInt() const
{
	std::istringstream ss(myStr);
	int num = 0;
	ss >> num;
	return num;
}

uint GxString::ToUint() const
{
	std::istringstream ss(myStr);
	uint num = 0;
	ss >> num;
	return num;
}

float GxString::ToFloat() const
{
	std::istringstream ss(myStr);
	float num = 0;
	ss >> num;
	return num;
}

double GxString::ToDouble() const
{
	std::istringstream ss(myStr);
	double num = 0;
	ss >> num;
	return num;
}

bool GxString::ToBool() const
{
	const char c = *myStr;

	if(s::iEq(myStr, "true") || ((myLen==1) && (c=='T' || c=='t' || c=='1')))
		return true;

	return false;
}

bool GxString::ToInt(int& out) const
{
	std::istringstream ss(myStr);
	ss >> out;
	return !ss.fail();
}

bool GxString::ToUint(uint& out) const
{
	std::istringstream ss(myStr);
	ss >> out;
	return !ss.fail();
}

bool GxString::ToFloat(float& out) const
{
	std::istringstream ss(myStr);
	ss >> out;
	return !ss.fail();
}

bool GxString::ToDouble(double& out) const
{
	std::istringstream ss(myStr);
	ss >> out;
	return !ss.fail();
}

bool GxString::ToBool(bool& out) const
{
	const char c = *myStr;

	if(s::iEq(myStr, "true") || ((myLen==1) && (c=='T' || c=='t' || c=='1')))
		{out = true; return true;}

	if(s::iEq(myStr, "false") || ((myLen==1) && (c=='F' || c=='f' || c=='0')))
		{out = false; return true;}

	return false;
}

// ===================================================================================
// Modifying functions
// ===================================================================================

void GxString::Clear()
{
	s::Release(myStr);
	myStr = s::null;
	myLen = 0;
}

void GxString::Chop(int n)
{
	if(n >= myLen)
		Clear();
	else if(n > 0)
	{
		s::Detach(myStr, myLen);
		myLen -= n;
		myStr[myLen] = 0;
	}
}

void GxString::Truncate(int n)
{
	if(n <= 0)
		Clear();
	else if(myLen > n)
	{
		s::Detach(myStr, myLen);
		myLen = n;
		myStr[myLen] = 0;
	}
}

void GxString::MakeUpper()
{
	s::Detach(myStr, myLen);
	for(char* p = myStr; *p; ++p)
		*p = toupper(*p);
}

void GxString::MakeLower()
{
	s::Detach(myStr, myLen);
	for(char* p = myStr; *p; ++p)
		*p = tolower(*p);
}

void GxString::Erase(int pos, int n)
{
	if(pos < 0)
		n += pos, pos = 0;

	if(pos == 0 && n >= myLen)
	{
		Clear();
	}
	else if(pos < myLen && n > 0)
	{
		s::Detach(myStr, myLen);
		n = s::Min(n, myLen - pos);
		memmove(myStr + pos, myStr + pos + n, myLen + 1 - pos - n);  
		myLen -= n;
	}
}

void GxString::Replace(char find, char replace)
{
	s::Detach(myStr, myLen);

	for(char* p = myStr; *p; ++p)
		if(*p == find)
			*p = replace;

	if(replace == 0)
		myLen = s::Len(myStr);
}

void GxString::Replace(GxStringArg find, GxStringArg replace)
{
	int offset = First(find);
	if(offset != _npos)
	{
		GxString result(myStr, offset);
		result.Append(replace);
		offset += find.len;

		while(offset < myLen)
		{
			int pos = First(find, offset);
			if(pos == _npos)
			{
				result.Append(myStr + offset);
				offset = _npos;
			}
			else
			{
				result.Append(GxStringArg(myStr + offset, pos - offset));
				result.Append(replace);
				offset = pos + find.len;
			}
		}

		Set(result);
	}
}

// ===================================================================================
// String information functions
// ===================================================================================

GxString GxString::Substr(int pos, int n) const
{
	return GxString(*this, pos, n);
}

int GxString::NextChar(int pos)
{
	if(pos < myLen)
		do {++pos;} while(pos < myLen && (myStr[pos] & 0xC0) == 0x80);
	return pos;
}

int GxString::PrevChar(int pos)
{
	if(pos > 0)
		do {--pos;} while(pos > 0 && (myStr[pos] & 0xC0) == 0x80);
	return pos;
}

int GxString::First(GxStringArg key, int pos) const
{
	if(key.len == 0 && pos <= myLen)
		return pos;

	const char* str = key.ptr;
	if(pos < myLen && pos + key.len <= myLen)
	{
		const char* end = myStr + myLen - key.len + 1;
		for(const char* ptr = myStr + pos; ptr != end; ++ptr)
		{
			if(*ptr == *str)
			{
				int i = 1;
				while(i < key.len && ptr[i] == str[i]) ++i;
				if(i == key.len) return (int)(ptr - myStr);
			}
		}
	}

	return _npos;
}

int GxString::First(char c, int pos) const
{
	pos = s::Max(pos, 0);
	while(pos < myLen && myStr[pos] != c) ++pos;
	return (pos < myLen) ? pos : _npos;
}

int GxString::Last(char c, int pos) const
{
	pos = s::Min(pos, myLen-1);
	while(pos >= 0 && myStr[pos] != c) --pos;
	return (pos >= 0) ? pos : -1;
}

int GxString::FirstAny(const char* c, int pos) const
{
	pos = s::Max(pos, 0);
	while(pos < myLen)
	{
		const char a = myStr[pos];
		const char* b = c;
		while(*b && *b != a) ++b;
		if(*b) break;
		++pos;
	}
	return (pos < myLen) ? pos : _npos;
}

int GxString::LastAny(const char* c, int pos) const
{
	pos = s::Min(pos, myLen-1);
	while(pos >= 0)
	{
		const char a = myStr[pos];
		const char* b = c;
		while(*b && *b != a) ++b;
		if(*b) break;
		--pos;
	}
	return (pos >= 0) ? pos : -1;
}

int GxString::Compare(GxStringArg str, bool caseSensitive) const
{
	if(caseSensitive)
		return s::Cmp(myStr, str.ptr);
	else
		return s::iCmp(myStr, str.ptr);
}

bool GxString::EndsWith(GxStringArg key, bool caseSensitive) const
{
	if(myLen >= key.len)
	{
		const char* src = myStr + myLen - key.len;
		return s::Eq(src, key.ptr, key.len, caseSensitive);
	}
	return false;
}

bool GxString::StartsWith(GxStringArg key, bool caseSensitive) const
{
	if(myLen >= key.len)
	{
		const char* src = myStr;
		return s::Eq(src, key.ptr, key.len, caseSensitive);
	}
	return false;
}

// ===================================================================================
// GxString operators
// ===================================================================================

// Array operator
char GxString::operator [] (int pos) const
	{ return myStr[pos]; }

// Addition operators
GxString& GxString::operator += (char chr)
	{ Append(chr); return *this; }

GxString& GxString::operator += (const char* str)
	{ Append(str); return *this; }

GxString& GxString::operator += (const GxString& str)
	{ Append(str); return *this; }

// Assignment operators
GxString& GxString::operator = (char c)
	{ Set(c, 1); return *this; }

GxString& GxString::operator = (const char* str) 
	{ Set(str); return *this; }

GxString& GxString::operator = (const GxString& str)
	{ Set(str); return *this; }

// Stream operators
GxString& GxString::operator << (bool v)
	{ return Arg(v); }

GxString& GxString::operator << (int v)
	{ return Arg(v); }

GxString& GxString::operator << (uint v)
	{ return Arg(v); }

GxString& GxString::operator << (float v)
	{ return Arg(v); }

GxString& GxString::operator << (double v)
	{ return Arg(v); }

GxString& GxString::operator << (char v)
	{ Append(v); return *this; }

GxString& GxString::operator << (const char* v)
	{ Append(v); return *this; }

GxString& GxString::operator << (const GxString& v)
	{ Append(v); return *this; }

// Addition operators
GxString operator + (const GxString& a, char b)
	{ GxString s(a); s.Append(b); return s; }

GxString operator + (char a, const GxString& b)
	{ GxString s(a, 1); s.Append(b); return s; }

GxString operator + (const GxString& a, const char* b)
	{ GxString s(a); s.Append(b); return s; }

GxString operator + (const char* a, const GxString& b)
	{ GxString s(a); s.Append(b); return s; }

GxString operator + (const GxString& a, const GxString& b)
	{ GxString s(a); s.Append(b); return s; }

// Less than operators
bool operator < (const GxString& a, const char* b)
	{ return s::Cmp(a.Raw(), b) < 0; }

bool operator < (const char* a, const GxString& b)
	{ return s::Cmp(a, b.Raw()) < 0; }

bool operator < (const GxString& a, const GxString& b)
	{ return s::Cmp(a.Raw(), b.Raw()) < 0; }

// Greater than operators
bool operator > (const GxString& a, const char* b)
	{ return s::Cmp(a.Raw(), b) > 0; }

bool operator > (const char* a, const GxString& b)
	{ return s::Cmp(a, b.Raw()) > 0; }

bool operator > (const GxString& a, const GxString& b)
	{ return s::Cmp(a.Raw(), b.Raw()) > 0; }

// Equality operators
bool operator == (const GxString& a, const char* b)
	{ return s::Eq(a.Raw(), b); }

bool operator == (const char* a, const GxString& b)
	{ return s::Eq(a, b.Raw()); }

bool operator == (const GxString& a, const GxString& b)
	{ return (a.Length() == b.Length()) && s::Eq(a.Raw(), b.Raw()); }

}; // namespace core
}; // namespace guix