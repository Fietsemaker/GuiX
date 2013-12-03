#include <GuiX/Config.h>
#include <GuiX/Interfaces.h>
#include <GuiX/Resources.h>

#include <Src/LocalizeImp.h>

namespace {

static const char* LOG_TAG = "Localize";

static const char* LOC_ROOT = "translations";
static const char* LOC_TL   = "tl";
static const char* LOC_ID   = "id";
static const char* LOC_CAT  = "category";

}; // anonymous namespace

namespace guix {
namespace core {

// ===================================================================================
// GxLocalize
// ===================================================================================

GxLocalizeImp* GxLocalizeImp::singleton = NULL;

GxLocalize::~GxLocalize()
{
}

GxLocalize* GxLocalize::Get()
{
	return GxLocalizeImp::singleton;
}

void GxLocalizeImp::Create()
{
	singleton = new GxLocalizeImp();
}

void GxLocalizeImp::Destroy()
{
	delete singleton;
	singleton = NULL;
}

GxLocalizeImp::GxLocalizeImp()
{
}

GxLocalizeImp::~GxLocalizeImp()
{
	Clear();
}

void GxLocalizeImp::Clear()
{
	myCategories.clear();
}

bool GxLocalizeImp::Load(const char* path_or_resource)
{
	GxResources::TranslationsRes res;
	if(GxResources::Get()->GetResource(path_or_resource, res))
	{
		bool anyLoaded = false;
		for(int i=0; i<res.paths.Size(); ++i)
			anyLoaded = myLoadFile(res.paths[i].Raw()) || anyLoaded;
		return anyLoaded;
	}
	return myLoadFile(path_or_resource);
}

GxString GxLocalizeImp::Translate(const char* name)
{
	return Translate("", name);
}

GxString GxLocalizeImp::Translate(const char* category, const char* name)
{
	CategoryMap::const_iterator it = myCategories.find(category);
	if(it != myCategories.end())
	{
		TranslationMap::const_iterator jt = it->second.find(name);
		if(jt != it->second.end())
		{
			return jt->second;
		}
	}
	return GxString(name);
}

bool GxLocalizeImp::myLoadFile(const char* path)
{
	XmlDocument doc;
	if(!doc.Load(path, XmlDocument::CW_LEAD_AND_TRAIL))
	{
		GxLog(LOG_TAG, GX_LT_ERROR, "Unable to parse xml \"%s\"", path);
		return false;
	}

	myLoadDocument(&doc);
	GxLog(LOG_TAG, GX_LT_INFO, "Loaded \"%s\"", path);

	return true;
}

void GxLocalizeImp::myLoadDocument(XmlDocument* doc)
{
	XmlElem* eLoc = doc->ChildElem(LOC_ROOT);
	if(eLoc)
	{
		XmlElem* eTl = eLoc->ChildElem(LOC_TL);
		for(; eTl; eTl = eTl->NextElem(LOC_TL)) 
			myParseElementTL("", eTl);

		XmlElem* eCat = eLoc->ChildElem(LOC_CAT);
		for(; eCat; eCat = eCat->NextElem(LOC_CAT))
			myParseElementCategory(eCat);
	}
}

void GxLocalizeImp::myParseElementCategory(XmlElem* eCat)
{
	const GxString category = eCat->GetAttribute(LOC_ID);
	if(!category.Empty())
	{
		XmlElem* eTl = eCat->ChildElem(LOC_TL);
		for(; eTl; eTl = eTl->NextElem(LOC_TL)) 
			myParseElementTL(category, eTl);
	}
}

void GxLocalizeImp::myParseElementTL(const GxString& category, XmlElem* eTl)
{
	const GxString name = eTl->GetAttribute(LOC_ID);
	const GxString text = eTl->GetText();
	if(!name.Empty() && !text.Empty())
		myAddTranslation(category, name, text);
}

void GxLocalizeImp::myAddTranslation(const GxString& category, const GxString& name, const GxString& text)
{
	myCategories[category][name] = text;
}

void GxLocalizeImp::LogInfo() const
{
	GxLog("Loaded categories: %i", myCategories.size());
	{
		CategoryMap::const_iterator it = myCategories.begin();
		for(int n=0; it != myCategories.end(); ++it, ++n)
			GxLog("  name=\"%s\", translations=%i",  it->first.Raw(), it->second.size());
	}
}

GxString GxTr(const char* name)
{
	return GxLocalize::Get()->Translate(name);
}

GxString GxTr(const char* category, const char* name)
{
	return GxLocalize::Get()->Translate(category, name);
}

// ===================================================================================
// GxUnicode
// ===================================================================================

static const uint utf8Offsets[6] =
{
    0x00000000, 0x00003080, 0x000E2080, 0x03C82080, 0xFA082080, 0x82082080
};

static const int utf8TrailingBytes[256] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5
};

static const uchar utf8LeadingByte[7] =
{
    0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC
};

namespace {

static void AppendUnicode(GxString& str, GxUTF32 c)
{
	     if(c < 0x00000080) str.Append((char)c);
	else if(c > 0x0010FFFF) str.Append('~');
	else
	{
		int numBytes;

		     if(c < 0x00800) numBytes = 2;
		else if(c < 0x10000) numBytes = 3;
		else                 numBytes = 4;

		uchar buffer[4];
		switch(numBytes)
		{
			case 4: buffer[3] = (c | 0x80) & 0xBF; c >>= 6;
			case 3: buffer[2] = (c | 0x80) & 0xBF; c >>= 6;
			case 2: buffer[1] = (c | 0x80) & 0xBF; c >>= 6;
		};
		buffer[0] = c | utf8LeadingByte[numBytes];
		str.Append(GxStringArg((const char*)buffer, numBytes));
	}
}

static GxUTF32 GetUnicode(const uchar*& str, const uchar* end)
{
	GxUTF32 c = *str;
	if(c < 0x80) ++str;
	else
	{
		int numBytes = utf8TrailingBytes[c];
		if(str + numBytes < end)
		{
			c = 0;
			switch(numBytes)
			{
				case 5: c += *str++; c <<= 6;
				case 4: c += *str++; c <<= 6;
				case 3: c += *str++; c <<= 6;
				case 2: c += *str++; c <<= 6;
				case 1: c += *str++; c <<= 6;
				case 0: c += *str++;
			}
			c -= utf8Offsets[numBytes];
		}
		else c = '~', str = end;
	}
	return c;
}

static int GetUnicode(const GxUTF16*& str, const GxUTF16* end)
{
	int c = (int)(*str++);
	if(c >= 0xD800 && c <= 0xDBFF && str < end)
	{
		int d = (int)(*str++);
		c = ((c - 0xD800) << 10) + (d - 0xDC00) + 0x0010000;
	}
	return c;
}

}; // anonymous namespace.

const uint* GxUnicode::UTF8Offsets()
{
	return utf8Offsets;
}

const int* GxUnicode::UTF8TrailingBytes()
{
	return utf8TrailingBytes;
}

const uchar* GxUnicode::UTF8LeadingByte()
{
	return utf8LeadingByte;
}

GxString GxUnicode::FromUTF32(const GxUTF32* str)
{
	int len = GxStrLen(str);
	GxString out(len);
	while(*str)	AppendUnicode(out, *str++);
	return out;
}

GxString GxUnicode::FromUTF16(const GxUTF16* str)
{
	int len = GxStrLen(str);
	GxString out(len);
	const GxUTF16* end = str + len;
	while(*str)	AppendUnicode(out, GetUnicode(str, end));
	return out;
}

GxString GxUnicode::FromWide(const wchar_t* str)
{
	switch(sizeof(wchar_t))
	{
		case sizeof(char):    return GxString (reinterpret_cast<const char*>(str));
		case sizeof(GxUTF16): return FromUTF16(reinterpret_cast<const GxUTF16*>(str));
		case sizeof(GxUTF32): return FromUTF32(reinterpret_cast<const GxUTF32*>(str));
	};
	return GxString();
}

GxList<GxUTF16> GxUnicode::ToUTF16(const char* str)
{
	int i = 0, len = GxStrLen(str);
	GxList<GxUTF16> out(len + 1);
	const uchar* a = (uchar*)str, *b = (uchar*)(str+len);
	while(*a)
	{
		GxUTF32 c = GetUnicode(a, b);
		     if(c < 0x0000FFFF) out << (GxUTF16)c;
		else if(c > 0x0010FFFF) out << '~';
		else
		{
			c -= 0x0010000;
			out << (GxUTF16)((c >> 10)     + 0xD800);
			out << (GxUTF16)((c & 0x3FFUL) + 0xDC00);
		}
	}
	out << 0;
	return out;
}

GxList<GxUTF32> GxUnicode::ToUTF32(const char* str)
{
	int i = 0, len = GxStrLen(str);
	GxList<GxUTF32> out(len + 1);
	const uchar* a = (uchar*)str, *b = (uchar*)(str+len);
	while(*a)
	{
		GxUTF32 c = GetUnicode(a, b);
		out << c;
	}
	out << 0;
	return out;
}

}; // namespace core
}; // namespace guix