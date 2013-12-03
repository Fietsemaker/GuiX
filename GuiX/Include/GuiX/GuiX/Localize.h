/// @file
/// Contains localization and unicode functions.

#pragma once

#include <GuiX/List.h>
#include <GuiX/String.h>

namespace guix {
namespace core {

// ===================================================================================
// GxLocalize
// ===================================================================================
/** The GxLocalize class stores translation strings for a language.

 The GxLocalize class provides an interface for loading and retrieving translations.
 In order to use GxLocalize, the following steps need to be taken:
 
 1. Create one or more translation XML files. These files contain all translation
    strings for a specific language. The GuiX example assets contain an example
	translation XML file.

 2. Use \c LoadLanguage() to load the translation files of a specific language.
    This stores all the translation strings from the language files in GxLocalize.

 3. The application can now use the \c Translate() and global \c GxTr() functions to
    retrieve translations.

 @see GxCore
*/
class GUIX_API GxLocalize
{
public:
	virtual ~GxLocalize();

	/// Returns the localize singleton.
	static GxLocalize* Get();

	/// Clears all translations that are currently loaded;
	virtual void Clear() = 0;

	/// Loads translations from a path or resource that points to a translations XML file.
	virtual bool Load(const char* path_or_resource) = 0;

	/// Returns a translation, or the name argument if no translation was found.
	virtual GxString Translate(const char* name) = 0;

	/// Returns a translation, or the name argument if no translation was found.
	virtual GxString Translate(const char* category, const char* name) = 0;

	/// GxLogs info about the loaded languages and translations.
	virtual void LogInfo() const = 0;
};

// Global translate functions for convenience.

/// Calls to \c GxTr() are forwarded to \c GxLocalize::Translate().
extern GUIX_API GxString GxTr(const char* name);

/// Calls to \c GxTr() are forwarded to \c GxLocalize::Translate().
extern GUIX_API GxString GxTr(const char* category, const char* name);

// ===================================================================================
// GxUnicode
// ===================================================================================
/** The GxUnicode class is a collection of static unicode string conversion functions.

 The GxUnicode class has functions for converting UTF-16 and UTF-32 strings to UTF-8
 strings and back. It also converts wide strings, but keep in mind that this merely
 interprets the wide string as a UTF-8, UTF-16 or UTF-32 based on the size of wchar_t
 and do not take into account the C locale.

 @see GxString
*/
class GUIX_API GxUnicode
{
public:
	/// Array of values that need to be subtracted from a multi-byte character sum to end up with the correct codepoint.
	/// Used internally by GuiX to decode UTF-8 strings to codepoint.
	static const uint* UTF8Offsets();

	/// A look-up array of trailing byte count for every leading byte value.
	/// Used internally by GuiX to decode UTF-8 strings to codepoint.
	static const int* UTF8TrailingBytes();

	/// A look-up array of leading byte values for every trailing byte count.
	/// Used internally by GuiX to encode codepoint to UTF-8 strings.
	static const uchar* UTF8LeadingByte();

	static GxString FromUTF16(const GxUTF16* str); ///< Converts UTF-16 to UTF-8.
	static GxString FromUTF32(const GxUTF32* str); ///< Converts UTF-32 to UTF-8.
	static GxString FromWide(const wchar_t* str);  ///< Converts wide string to UTF-8.

	static GxList<GxUTF16> ToUTF16(const char* str); ///< Converts UTF-8 to UTF-16.
	static GxList<GxUTF32> ToUTF32(const char* str); ///< Converts UTF-8 to UTF-32.
};

}; // namespace core
}; // namespace guix