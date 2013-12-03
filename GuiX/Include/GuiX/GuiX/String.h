/// @file
/// Contains the string class.

#pragma once

#include <GuiX/Config.h>

namespace guix {
namespace core {

// ===================================================================================
// GxString
// ===================================================================================
/** The GxString class is a container for a null-terminated UTF-8 string.

 The GxString class has functionality similar to \c std::string. It is used to store
 a null-terminated string of UTF-8 encoded characters. The main difference compared to
 \c std::string is that GxString avoids deep copies when it is assigned or copy
 constructed from another GxString. This is achieved by sharing string data and
 keeping a reference counter.
 
 Most of the functions accept C-strings as arguments, which are reinterpreted as
 UTF-8 strings. This will work as long as the string only contains ascii characters,
 since ascii characters are a subset of UTF-8 characters. If you want to use strings
 that are not UTF-8 compliant such as wide strings, you can convert them to GxString
 objects with the GxUnicode class.

 @see GxUnicode
*/
class GUIX_API GxString
{
public:
	static const int npos(); ///< An arbitrary large value; used to indicate end-of-range.

	/// StringArg is a helper struct that allows functions to accept both C-string and GxString objects.
	struct GUIX_API StringArg
	{
		static int _len(const char* s);
		StringArg(const char* s)        :ptr(s),       len(_len(s)) {}
		StringArg(const char* s, int n) :ptr(s),       len(n)       {}
		StringArg(const GxString& s)    :ptr(s.myStr), len(s.myLen) {}
		const char* ptr;
		int len;
	};

	~GxString();

	GxString();                        ///< Constructs an empty string.
	GxString(int reserve);             ///< Constructs an empty string; reserves memory for a string of length reserve.
	GxString(char c, int n);           ///< Constructs a string with character c repeated n times.
	GxString(const char* str);         ///< Constructs a string from a C-string.
	GxString(const char* str, int n);  ///< Constructs a string from the first n characters of a C-string.
	GxString(const GxString& str);     ///< Constructs a copy of a string; the content is implicitly shared by reference.
	GxString(const GxString& str, int pos, int n); ///< Constructs a substring of a string; see \c Substr().

	void Set(char c, int n);          ///< Sets the content to character c repeated n times.
	void Set(const char* str);        ///< Sets the content to a copy of a C-string.
	void Set(const char* str, int n); ///< Sets the content to a copy of the first n characters of a C-string.
	void Set(const GxString& str);    ///< Sets the content to a copy of a string; the content is implicitly shared by reference.

	void Append(char c);                 ///< Appends character c at the end of the string.
	void Append(StringArg str);          ///< Appends another string at the end of the string.
	void Prepend(char c);                ///< Prepends character c in front of the string.
	void Prepend(StringArg str);         ///< Prepends another string in front of the string.
	void Insert(int pos, char c);        ///< Inserts character c at position pos.
	void Insert(int pos, StringArg str); ///< Inserts a string at position pos.

	GxString& Arg(bool v, bool caps = false); ///< Appends boolean v at the end of the string.
	GxString& Arg(int v, bool hex = false);   ///< Appends integer v at the end of the string.
	GxString& Arg(uint v, bool hex = false);  ///< Appends unsigned integer v at the end of the string.
	GxString& Arg(float v, int precision = -1, bool trim = true);  ///< Appends float v at the end of the string.
	GxString& Arg(double v, int precision = -1, bool trim = true); ///< Appends double v at the end of the string.
	GxString& Arg(char v);                    ///< Appends character v at the end of the string.
	GxString& Arg(const char* v);             ///< Appends string v at the end of the string.
	GxString& Arg(StringArg v);               ///< Appends string v at the end of the string.

	// Converts the string to a value. Returns zero or false if the conversion fails.
	int ToInt() const;
	uint ToUint() const;
	float ToFloat() const;
	double ToDouble() const;
	bool ToBool() const;

	// Converts the string to a value. Returns true if the conversion was successful. On failure the out value remains unchanged.
	bool ToInt(int& out) const;
	bool ToUint(uint& out) const;
	bool ToFloat(float& out) const;
	bool ToDouble(double& out) const;
	bool ToBool(bool& out) const;

	/// Formats the string using the \c vsnprintf function from the C stdio library.
	/// The function uses a 256 character buffer on the stack, and the result is truncated to 256 characters.
	void Format(const char* fmt, ...);

	/// Works like \c Format(fmt, ...) but takes an extra argument that specifies the maximum length.
	/// The character buffer is allocated on the heap, and the result is truncated to maxLength characters.
	void Format(int maxLength, const char* fmt, ...);

	/// Sets the content to an empty string.
	void Clear();

	/// Removes the last n characters from the string.
	void Chop(int n);

	/// Makes sure the string length does not exceed n characters by removing characters at the end of the string.
	void Truncate(int n);

	/// Converts the entire string to upper case.
	void MakeUpper();

	/// Converts the entire string to lower case.
	void MakeLower();

	/// Erases a sequence of n characters starting at position pos.
	void Erase(int pos = 0, int n = npos());

	/// Replaces every occurence of the find character with the replace character.
	/// Replacing with a null terminator truncates the string to the first ocurrence of find.
	void Replace(char find, char replace);

	/// Replaces every occurence of the find string with the replace string.
	void Replace(StringArg find, StringArg replace);

	/// Returns the substring of n characters starting at position pos.
	GxString Substr(int pos = 0, int n = npos()) const;

	/// Returns the position of the first character after pos,
	/// which may be an offset of more than one when dealing with multi-byte characters. 
	int NextChar(int pos);

	/// Returns the position of the first character before pos,
	/// which may be an offset of more than one when dealing with multi-byte characters.
	int PrevChar(int pos);

	/// Returns the position of the first occurence of substring str on or after position pos, or npos if no match is found.
	int First(StringArg str, int pos = 0) const;

	/// Returns the position of the first occurence of character c on or after position pos, or npos if no match is found.
	int First(char c, int pos = 0) const;

	/// Returns the position of the last occurence of character c on or before position pos, or -1 if no match is found.
	int Last(char c, int pos = npos()) const;

	/// Returns the position of the first occurence of any of the characters in c on or after position pos, or npos if no match is found.
	int FirstAny(const char* c, int pos = 0) const;

	/// Returns the position of the last occurence of any of the characters in c on or before position pos, or -1 if no match is found.
	int LastAny(const char* c, int pos = 0) const;

	/// Returns the difference between the first mismatching character of the string and str, or 0 if they are equivalent.
	int Compare(StringArg str, bool caseSensitive = true) const;

	/// Returns true if the string ends with the character sequence specified by str.
	bool EndsWith(StringArg str, bool caseSensitive = true) const;

	/// Returns true if the string starts with the character sequence specified by str.
	bool StartsWith(StringArg str, bool caseSensitive = true) const;

	const char* Raw() const { return myStr;  } ///< Returns a pointer to the raw UTF-8 string content.
	int Length() const      { return myLen;  } ///< Returns the number of characters in the string, excluding the null-terminator.
	bool Empty() const      { return !myLen; } ///< Returns true if the string has a length of zero.

	/// Returns the character at position pos; does not perform an out-of-bounds check.
	char operator [] (int pos) const;

	GxString& operator += (char c);              ///< Calls \c Append().
	GxString& operator += (const char* str);     ///< Calls \c Append().
	GxString& operator += (const GxString& str); ///< Calls \c Append().

	GxString& operator = (char c);               ///< Calls \c Set().
	GxString& operator = (const char* str);      ///< Calls \c Set().
	GxString& operator = (const GxString& str);  ///< Calls \c Set().

	GxString& operator << (bool v);              ///< Calls \c Arg().
	GxString& operator << (int v);               ///< Calls \c Arg().
	GxString& operator << (uint v);              ///< Calls \c Arg().
	GxString& operator << (float v);             ///< Calls \c Arg().
	GxString& operator << (double v);            ///< Calls \c Arg().

	GxString& operator << (char c);              ///< Calls \c Append().
	GxString& operator << (const char* str);     ///< Calls \c Append().
	GxString& operator << (const GxString& str); ///< Calls \c Append().

private:
	char* myStr;
	int myLen;
};

// Shorthand for string arguments.
typedef GxString::StringArg GxStringArg;

// Global string operators.
GxString GUIX_API operator + (const GxString& a, char b);            ///< Returns the result of concatenating string a and character b.
GxString GUIX_API operator + (char a, const GxString& b);            ///< Returns the result of concatenating character a and string b.
GxString GUIX_API operator + (const GxString& a, const char* b);     ///< Returns the result of concatenating string a and b.
GxString GUIX_API operator + (const char* a, const GxString& b);     ///< Returns the result of concatenating string a and b.
GxString GUIX_API operator + (const GxString& a, const GxString& b); ///< Returns the result of concatenating string a and b.

bool GUIX_API operator < (const GxString& a, const char* b);      ///< Returns true if string a comes before b lexicographically.
bool GUIX_API operator < (const char* a, const GxString& b);      ///< Returns true if string a comes before b lexicographically.
bool GUIX_API operator < (const GxString& a, const GxString& b);  ///< Returns true if string a comes before b lexicographically.

bool GUIX_API operator > (const GxString& a, const char* b);      ///< Returns true if string a comes after b lexicographically.
bool GUIX_API operator > (const char* a, const GxString& b);      ///< Returns true if string a comes after b lexicographically.
bool GUIX_API operator > (const GxString& a, const GxString& b);  ///< Returns true if string a comes after b lexicographically.

bool GUIX_API operator == (const GxString& a, const char* b);     ///< Returns true if the content of string a and b are equivalent.
bool GUIX_API operator == (const char* a, const GxString& b);     ///< Returns true if the content of string a and b are equivalent.
bool GUIX_API operator == (const GxString& a, const GxString& b); ///< Returns true if the content of string a and b are equivalent.

}; // namespace core
}; // namespace guix