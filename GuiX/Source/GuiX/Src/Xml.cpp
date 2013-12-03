#include <GuiX/Config.h>

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include <GuiX/Common.h>
#include <GuiX/Interfaces.h>

#include <Src/Xml.h>

namespace guix {
namespace core {

// ===================================================================================
// Helper functions
// ===================================================================================

inline bool IsWhiteSpace(char c)
{
	return ((uchar)c < 0x80) ? (isspace(c) != 0) : false;
}

inline bool IsAlpha(char c)
{
	return ((uchar)c < 0x80) ? (isalpha(c) != 0) : true;
}

inline bool IsDigit(char c)
{
	return ((uchar)c < 0x80) ? (isdigit(c) != 0) : true;
}

inline bool IsAlphaNum(char c)
{
	return ((uchar)c < 0x80) ? (isalnum(c) != 0) : true;
}

inline bool IsIdentifier(char c)
{
	return IsAlpha(c) || c == '_';
}

static bool IsTag(const char* p, const char* tag)
{
	while(*tag && *tag == *p) ++tag, ++p;
	return (*tag == 0);
}

static const char* SkipWhiteSpace(const char* p)
{
	while(IsWhiteSpace(*p)) ++p;
	return p;
}

// ===================================================================================
// XmlParser
// ===================================================================================

class XmlParser
{
public:
	enum { ENTITY_COUNT = 5 };
	struct Entity { const char* str; int chr; };

	XmlParser(XmlDocument::CondenseWhitespace cw);

	const char* Error(const char* p, const GxString& desc);
	XmlNode* Identify(const char* p);
	const char* Parse(const char* p, XmlNode* n);

	const char* ParseElem   (const char* p, XmlElem* n);
	const char* ParseText   (const char* p, XmlText* n);
	const char* ParseComment(const char* p, XmlComment* n);
	const char* ParseDecl   (const char* p, XmlDeclaration* n);
	const char* ParseDoc    (const char* p, XmlDocument* n);
	const char* ParseNode   (const char* p, XmlNode* n);

	const char* ReadElemBody(const char* p, XmlElem* elem);
	const char* ReadAttrib  (const char* p, GxString& name, GxString& value);
	const char* ReadName    (const char* p, GxString& name);
	const char* ReadText    (const char* p, GxString& text, bool trim, const char* end);
	const char* ReadChar    (const char* p, GxString& str);
	const char* ReadEntity  (const char* p, GxString& str);

private:
	static Entity myEntities[ENTITY_COUNT];
	const char myErr;
	XmlDocument* myDoc;
	XmlDocument::CondenseWhitespace myCw;
};

XmlParser::Entity XmlParser::myEntities[XmlParser::ENTITY_COUNT] =
{
	{"&quot;", '\"'},
	{"&amp;" , '&' },
	{"&apos;", '\''},
	{"&lt;"  , '<' },
	{"&gt;"  , '>' },
};

XmlParser::XmlParser(XmlDocument::CondenseWhitespace cw)
	:myDoc(NULL)
	,myErr(0)
	,myCw(cw)
{
}

const char* XmlParser::Error(const char* p, const GxString& desc)
{
	myDoc->SetError(desc);
	return &myErr;
}

XmlNode* XmlParser::Identify(const char* p)
{
	p = SkipWhiteSpace(p);
	if(*p == 0 || *p != '<')
		return NULL;

		 if(IsTag(p, "<?xml"    )) return new XmlDeclaration();
	else if(IsTag(p, "<!--"     )) return new XmlComment();
	else if(IsTag(p, "<![CDATA[")) return new XmlText();
	else if(*p=='<' && IsIdentifier(p[1])) return new XmlElem();

	return new XmlNode();
}

const char* XmlParser::Parse(const char* p, XmlNode* node)
{
	     if(node->ToElem())        return ParseElem(p, node->ToElem());
	else if(node->ToText())        return ParseText(p, node->ToText());
	else if(node->ToComment())     return ParseComment(p, node->ToComment());
	else if(node->ToDeclaration()) return ParseDecl(p, node->ToDeclaration());
	else if(node->ToDocument())	   return ParseDoc(p, node->ToDocument());

	return ParseNode(p, node);
}

const char* XmlParser::ParseElem(const char* p, XmlElem* elem)
{
	// Read the beginning of the start tag
	p = SkipWhiteSpace(p);
	if(*p != '<')
		return Error(p, "Could not find element start");

	// Try to read the element name
	p = SkipWhiteSpace(p + 1);
	const char* err = p;
	GxString elemName;
	p = ReadName(p, elemName);
	if(*p == 0)
		return Error(err, "Could not read element name");
	elem->SetValue(elemName);

	// Create the matching end tag
	const GxString name = elem->Value();
	const GxString end = "</" + name;

	// Keep reading until the element is completed
	while(*p)
	{
		p = SkipWhiteSpace(p);

		// Check if the element is an empty element
		if(*p == '/')
		{
			if(p[1] != '>')
				return Error(p, "Invalid tag of element '" + name + '\'');
			return p + 2;
		}
		// Check if the start tag ends
		else if(*p == '>')
		{
			// Read all the content inside the element
			p = ReadElemBody(p + 1, elem);

			// Read the matching element end tag
			if(!IsTag(p, end.Raw()))
				return Error(p, "Could not find end of element '" + name + '\'');
			p = SkipWhiteSpace(p + end.Length());
			if(*p != '>')
				return Error(p, "Invalid end tag of element '" + name + '\'');
			return p + 1;
		}
		else
		{
			err = p;

			// Try to read an attribute
			GxString aName, aValue;
			p = ReadAttrib(p, aName, aValue);
			if(*p == 0)
				Error(err, "Invalid attribute of element '" + name + '\'');

			// Store the attribute
			elem->AddAttribute(aName, aValue);
		}
	}
	return p;
}

const char* XmlParser::ParseText(const char* p, XmlText* text)
{
	const char* start = "<![CDATA[";
	const char* end   = "]]>";

	if(IsTag(p, start))
	{
		// Read a CDATA section
		GxString value(8);
		const char* first = p;
		p += GxStrLen(start);
		while(*p && !IsTag(p, end)) 
			value += *p++;
		if(*p == 0)
			return Error(p, "Could not find CDATA end");
		text->SetValue(value);
		p += GxStrLen(end);
	}
	else
	{
		// Read regular text
		GxString value(8);
		p = ReadText(p, value, true, "<");
		text->SetValue(value);
		if(*p) --p;
	}
	return p;
}

const char* XmlParser::ParseComment(const char* p, XmlComment* comment)
{
	const char* start = "<!--";
	const char* end   = "-->";

	// Read the comment start
	p = SkipWhiteSpace(p);
	if(!IsTag(p, start))
		return Error(p, "Invalid comment start");
	p += GxStrLen(start);

	// Keep reading until comment ends
	GxString value(8);
	while(*p && !IsTag(p, end))
		value += *p++;
	if(*p == 0)
		return Error(p, "Could not find comment end");
	comment->SetValue(value);
	p += GxStrLen(end);

	return p;
}

const char* XmlParser::ParseDecl(const char* p, XmlDeclaration* decl)
{
	// Read declaration start
	const char* start = "<?xml";
	p = SkipWhiteSpace(p);
	if(!IsTag(p, start))
		return Error(p, "Invalid declaration start");
	p += GxStrLen(start);

	// Read declaration attributes
	GxString aValue[3], dummy;
	while(*p && *p != '>')
	{
		p = SkipWhiteSpace(p);
		     if(IsTag(p, "version"   )) p = ReadAttrib(p, dummy, aValue[0]);
		else if(IsTag(p, "encoding"  )) p = ReadAttrib(p, dummy, aValue[1]);
		else if(IsTag(p, "standalone"))	p = ReadAttrib(p, dummy, aValue[2]);
		++p;
	}
	if(*p == 0)
		return Error("Could not find declaration end", p);

	decl->Set(aValue[0], aValue[1], aValue[2]);
	return p + 1;
}

const char* XmlParser::ParseDoc(const char* p, XmlDocument* doc)
{
	myDoc = doc;
	XmlNode* node;

	// Parse all xml nodes in the document root
	p = SkipWhiteSpace(p);
	while(*p && (node = Identify(p)) != NULL)
	{
		p = Parse(p, node);
		doc->AddChild(node);
		p = SkipWhiteSpace(p);
	}
	return p;
}

const char* XmlParser::ParseNode(const char* p, XmlNode* node)
{
	// Read the start of the tag
	p = SkipWhiteSpace(p);
	if(*p == 0 || *p != '<')
		return Error("Invalid unknown data", p);
	++p;

	// Keep reading until tag ends
	GxString value(8);
	while(*p && *p != '>')
	{
		value += *p++;

		// If there is a [, read until the matching ] is encoundered
		if(*p == '[')
		while(*p && *p != ']')
			value += *p++;
	}
	if(*p == 0)
		return Error("Could not find unknown data end", p);
	node->SetValue(value);
	return p + 1;
}

const char* XmlParser::ReadElemBody(const char* p, XmlElem* elem)
{
	// Read in text and elements in any order.
	const char* pText = p;
	p = SkipWhiteSpace(p);
	while(*p)
	{
		if(*p == '<')
		{
			// Read a nested xml element
			if(IsTag(p, "</")) return p;
			XmlNode* node = Identify(p);
			if(!node) return &myErr;
			p = Parse(p, node);
			elem->AddChild(node);
		}
		else
		{
			// Read a section of text
			XmlText* text = new XmlText();
			p = ParseText(pText, text);
			if(text->Value().Empty())
				delete text;
			else
				elem->AddChild(text);
		}
		pText = p;
		p = SkipWhiteSpace(p);
	}
	return p;
}

const char* XmlParser::ReadAttrib(const char* p, GxString& name, GxString& value)
{
	// Read the attribute name and '='
	p = SkipWhiteSpace(p);
	p = ReadName(p, name);
	p = SkipWhiteSpace(p);
	if(*p != '=') return &myErr;

	// Read the attribute value
	p = SkipWhiteSpace(p + 1);
	     if(*p == '\'')	p = ReadText(p + 1, value, false, "\'");
	else if(*p == '\"')	p = ReadText(p + 1, value, false, "\"");
	else return &myErr;

	return p;
}

const char* XmlParser::ReadName(const char* p, GxString& out)
{
	out.Clear();
	if(!IsIdentifier(*p))
		return &myErr;
	while(IsAlphaNum(*p) || *p=='_' || *p=='-' || *p=='.' || *p==':')
		out += *p++;
	return p;
}

const char* XmlParser::ReadText(const char* p, GxString& out, bool trim, const char* end)
{
	out.Clear();
	if(trim == false || myCw == XmlDocument::CW_NONE)
	{
		// Keep all the white space
		while(*p && !IsTag(p, end))
			p = ReadChar(p, out);
	}
	else if(myCw == XmlDocument::CW_FULL)
	{
		// Condense all white space
		p = SkipWhiteSpace(p);
		bool ws = false;
		while(*p && !IsTag(p, end))
		{
			if(IsWhiteSpace(*p))
			{
				ws = true;
				++p;
			}
			else
			{
				if(ws)
				{
					out += ' ';
					ws = false;
				}
				p = ReadChar(p, out);
			}
		}
	}
	else if(myCw == XmlDocument::CW_LEAD_AND_TRAIL)
	{
		// Remove leading and trailing whitespace
		p = SkipWhiteSpace(p);
		while(*p && !IsTag(p, end))
			p = ReadChar(p, out);
	}
	if(IsTag(p, end))
		p += GxStrLen(end);
	return p;
}

const char* XmlParser::ReadChar(const char* p, GxString& str)
{
	if(*p == 0)	return &myErr;
	if(*p == '&') return ReadEntity(p, str);
	str.Append(*p);
	return p + 1;
}

const char* XmlParser::ReadEntity(const char* p, GxString& str)
{
	// Try to match any of the predefined entities
	for(int i=0; i<ENTITY_COUNT; ++i)
	if(IsTag(p, myEntities[i].str))
	{
		str.Append(myEntities[i].chr);
		return p + GxStrLen(myEntities[i].str);
	}
	
	// Try to read a character reference entity
	if(p[1] == '#' && p[2])
	{
		int ucs = 0;
		int delta = 0;
		if(p[2] == 'x')
		{
			// Read a hexadecimal character reference
			const char* q = strchr(p + 3, ';');
			if(*q == 0) return &myErr;
			delta = q - p; --q;

			int mul = 1;
			while(*q != 'x')
			{
				     if(*q>='0' && *q<='9') ucs += mul * (*q-'0');
				else if(*q>='a' && *q<='f') ucs += mul * (*q-'a'+10);
				else if(*q>='A' && *q<='F') ucs += mul * (*q-'A'+10);
				else return &myErr;
				mul *= 16;
				--q;
			}
		}
		else
		{
			// Read a decimal character reference
			const char* q = strchr(p + 2, ';');
			if(*q == 0) return &myErr;
			delta = q - p; --q;

			int mul = 1;
			while(*q != '#')
			{
				if(*q>='0' && *q<='9') ucs += mul * (*q-'0');
				else return &myErr;
				mul *= 10;
				--q;
			}
		}
		str.Append(ucs);
		return p + delta + 1;
	}

	// Pass through any custom entities
	if(IsIdentifier(p[1]))
	{
		const char* q = p + 1;
		while(IsAlphaNum(*q) || *q=='_' || *q=='-' || *q=='.' || *q==':') q++;
		if(*q != ';') return &myErr;
		str.Append(GxStringArg(p, (int)(q - p + 1)));
		return q + 1;
	}

	// Reading the entity failed, just append the character
	str.Append(*p);
	return p + 1;
}

// ===================================================================================
// XmlWriter
// ===================================================================================

class XmlWriter
{
public:
	XmlWriter(XmlDocument* doc);

	void Write(int tab, GxString& s, XmlNode* n);

	void WriteElem   (int tab, GxString& s, XmlElem* n);
	void WriteText   (int tab, GxString& s, XmlText* n);
	void WriteComment(int tab, GxString& s, XmlComment* n);
	void WriteDecl   (int tab, GxString& s, XmlDeclaration* n);
	void WriteDoc    (int tab, GxString& s, XmlDocument* n);
	void WriteNode   (int tab, GxString& s, XmlNode* n);

	void Encode(GxString& out, const GxString& str);

private:
	XmlDocument* myDoc;
};

XmlWriter::XmlWriter(XmlDocument* doc)
	:myDoc(doc)
{
}

void XmlWriter::Write(int tab, GxString& s, XmlNode* node)
{
	     if(node->ToElem())        return WriteElem(tab, s, node->ToElem());
	else if(node->ToText())        return WriteText(tab, s, node->ToText());
	else if(node->ToComment())     return WriteComment(tab, s, node->ToComment());
	else if(node->ToDeclaration()) return WriteDecl(tab, s, node->ToDeclaration());
	else if(node->ToDocument())	   return WriteDoc(tab, s, node->ToDocument());

	return WriteNode(tab, s, node);
}

void XmlWriter::WriteElem(int tab, GxString& s, XmlElem* elem)
{
	// Element name
	s << GxString(' ', tab) << '<' << elem->Value();

	// Attributes
	for(int i=0; i<elem->GetAttributeCount(); ++i)
	{
		XmlElem::Attribute* attrib = elem->GetAttribute(i);
		s += ' '  ; Encode(s, attrib->name);
		s += "=\""; Encode(s, attrib->value);
		s += '\"' ;
	}

	// Child elements
	XmlNode* node = elem->Child();
	if(!node)
	{
		// No children
		s += " />";
	}
	else if(node->ToText() && !node->Next())
	{
		// Only text
		s += '>';
		Write(tab, s, node);
		s << "</" << elem->Value() << '>';
	}
	else
	{
		// Multiple and/or non-text children
		s += '>';
		for(; node; node = node->Next())
		{
			if(!node->ToText()) s += '\n';
			Write(tab + 2, s, node);
		}
		s << '\n' << GxString(' ', tab);
		s << "</" << elem->Value() << ">";
	}
}

void XmlWriter::WriteText(int tab, GxString& s, XmlText* text)
{
	Encode(s, text->Value());
}

void XmlWriter::WriteComment(int tab, GxString& s, XmlComment* comment)
{
	s << "<!--" << comment->Value() << "-->";
}

void XmlWriter::WriteDecl(int tab, GxString& s, XmlDeclaration* decl)
{
	s += "<?xml";
	if(!decl->GetVersion().Empty())
		s << " version=\"" << decl->GetVersion() << '\"';
	if(!decl->GetEncoding().Empty())
		s << " encoding=\"" << decl->GetEncoding() << '\"';
	if(!decl->GetStandalone().Empty())
		s << " standalone=\"" << decl->GetStandalone() << '\"';
	s += "?>";
}

void XmlWriter::WriteDoc(int tab, GxString& s, XmlDocument* doc)
{
	for(XmlNode* n = doc->Child(); n; n = n->Next())
	{
		Write(tab, s, n);
		if(n->Next()) s += '\n';
	}
}

void XmlWriter::WriteNode(int tab, GxString& s, XmlNode* node)
{
	s << '<' << node->Value() << '>';
}

void XmlWriter::Encode(GxString& out, const GxString& str)
{
	const char* p = str.Raw();
	while(*p)
	{
		if(p[0] == '&' && IsIdentifier(p[1]))
		{
			const char* q = p + 2;
			while(IsAlphaNum(*q) || *q=='_' || *q=='-' || *q=='.' || *q==':') q++;
			if(*q == ';')
			{
				out.Append(GxStringArg(p, (int)(q - p + 1)));
				p = q + 1;
			}
		}

		if(p[0] == '&' && p[1] == '#')
		{
			const char* q = p + 2;
			while(IsDigit(*q) || *q=='x') ++q;
			if(*q == ';')
			{
				out.Append(GxStringArg(p, (int)(q - p + 1)));
				p = q + 1;
			}
		}

		if(*p)
		{
				 if(*p == '&' ) out += "&amp;";
			else if(*p == '<' ) out += "&lt;";
			else if(*p == '>' ) out += "&gt;";
			else if(*p == '\"') out += "&quot;";
			else if(*p == '\'') out += "&apos;";
			else if(*p!='\n' && *p!=' ' && ((unsigned int)*p) < 32)
			{
				out << "&#" << ((unsigned int)*p) << ';';
			}
			else out += *p;
			++p;
		}
	}
}

// ===================================================================================
// XmlNode
// ===================================================================================

XmlNode::XmlNode()
	:myNext(NULL)
	,myParent(NULL)
	,myChild(NULL)
{
}

XmlNode::~XmlNode()
{
	ClearChildren();
}

void XmlNode::ClearChildren()
{
	XmlNode* node = myChild;
	while(node)
	{
		XmlNode* temp = node;
		node = node->myNext;
		delete temp;
	}
	myChild = NULL;
}

void XmlNode::AddChild(XmlNode* child)
{
	child->myParent = this;
	if(myChild)
	{
		XmlNode* last = myChild;
		while(last->myNext)
			last = last->myNext;
		last->myNext = child;
	}
	else myChild = child;
}

void XmlNode::SetValue(GxString value)
{
	myValue = value;
}

XmlElem* XmlNode::ChildElem()
{
	for(XmlNode* n = myChild; n; n = n->myNext)
		if(n->ToElem())
			return n->ToElem();
	return NULL;
}

XmlNode* XmlNode::Child(const char* name)
{
	for(XmlNode* n = myChild; n; n = n->myNext)
		if(n->myValue == name)
			return n;
	return NULL;
}

XmlElem* XmlNode::ChildElem(const char* name)
{
	for(XmlNode* n = myChild; n; n = n->myNext)
		if(n->ToElem() && n->myValue == name)
			return n->ToElem();
	return NULL;
}

XmlElem* XmlNode::NextElem()
{
	for(XmlNode* n = myNext; n; n = n->myNext)
		if(n->ToElem())
			return n->ToElem();
	return NULL;
}

XmlNode* XmlNode::Next(const char* name)
{
	for(XmlNode* n = myNext; n; n = n->myNext)
		if(n->myValue == name)
			return n;
	return NULL;
}

XmlElem* XmlNode::NextElem(const char* name)
{
	for(XmlNode* n = myNext; n; n = n->myNext)
		if(n->ToElem() && n->myValue == name)
			return n->ToElem();
	return NULL;
}

XmlNode* XmlNode::Parent()                {return myParent;}
XmlNode* XmlNode::Child()                 {return myChild;}
XmlNode* XmlNode::Next()                  {return myNext;}

const GxString& XmlNode::Value() const    {return myValue;}

XmlDocument*    XmlNode::ToDocument()     {return NULL;}
XmlDeclaration* XmlNode::ToDeclaration()  {return NULL;}
XmlComment*     XmlNode::ToComment()      {return NULL;}
XmlElem*        XmlNode::ToElem()         {return NULL;}
XmlText*        XmlNode::ToText()         {return NULL;}

XmlNode::Type   XmlNode::GetType() const  {return UNKNOWN;}

// ===================================================================================
// XmlElem
// ===================================================================================

XmlElem::XmlElem()
	:myAttributes(NULL)
	,myAttributeCount(0)
{
}

XmlElem::XmlElem(GxString name)
	:myAttributes(NULL)
	,myAttributeCount(0)
{
	myValue = name;
}

XmlElem::~XmlElem()
{
	ClearAttributes();
}

void XmlElem::ClearAttributes()
{
	for(int i=0; i<myAttributeCount; ++i)
		delete myAttributes[i];
	GxFree(myAttributes);

	myAttributeCount = 0;
	myAttributes = NULL;
}

void XmlElem::AddAttribute(GxString name, GxString value)
{
	Attribute a = {name, value};
	int count = ++myAttributeCount;
	myAttributes = GxRealloc(myAttributes, count);
	myAttributes[count - 1] = new Attribute(a);
}

int XmlElem::GetAttributeCount() const
{
	return myAttributeCount;
}

XmlElem::Attribute* XmlElem::GetAttribute(int index)
{
	return (index >= 0 && index < myAttributeCount) ? 
		myAttributes[index] : NULL;
}

GxString XmlElem::GetAttribute(const char* name) const
{
	for(int i=0; i<myAttributeCount; ++i)
		if(myAttributes[i]->name == name)
			return myAttributes[i]->value;
	return GxString();
}

bool XmlElem::GetAttribute(const char* name, int& v) const
{
	for(int i=0; i<myAttributeCount; ++i)
		if(myAttributes[i]->name == name)
			return myAttributes[i]->value.ToInt(v);
	return false;
}

bool XmlElem::GetAttribute(const char* name, bool& v) const
{
	for(int i=0; i<myAttributeCount; ++i)
		if(myAttributes[i]->name == name)
			return myAttributes[i]->value.ToBool(v);
	return false;
}

bool XmlElem::GetAttribute(const char* name, float& v) const
{
	for(int i=0; i<myAttributeCount; ++i)
		if(myAttributes[i]->name == name)
			return myAttributes[i]->value.ToFloat(v);
	return false;
}

bool XmlElem::GetAttribute(const char* name, GxString& v) const
{
	for(int i=0; i<myAttributeCount; ++i)
		if(myAttributes[i]->name == name)
			{v = myAttributes[i]->value; return true;}
	return false;
}

GxString XmlElem::GetText()
{
	XmlNode* node = myChild;
	if(node && node->ToText())
		return node->Value();
	return GxString();
}

XmlElem*      XmlElem::ToElem()        {return this;}
XmlNode::Type XmlElem::GetType() const {return ELEM;}

// ===================================================================================
// XmlText
// ===================================================================================

XmlText::XmlText()
{
}

XmlText::XmlText(GxString text)
{
	myValue = text;
}

XmlText*      XmlText::ToText()        {return this;}
XmlNode::Type XmlText::GetType() const {return TEXT;}

// ===================================================================================
// XmlComment
// ===================================================================================

XmlComment::XmlComment()
{
}

XmlComment::XmlComment(GxString text)
{
	myValue = text;
}

XmlComment*   XmlComment::ToComment()     {return this;}
XmlNode::Type XmlComment::GetType() const {return COMMENT;}

// ===================================================================================
// XmlDeclaration
// ===================================================================================

void XmlDeclaration::Set(GxString version, GxString encoding, GxString standalone)
{
	if(!version.Empty()) myVersion = version;
	if(!encoding.Empty()) myEncoding = encoding;
	if(!standalone.Empty()) myStandalone = standalone;
}

const GxString& XmlDeclaration::GetVersion() const     {return myVersion;}
const GxString& XmlDeclaration::GetEncoding() const    {return myEncoding;}
const GxString& XmlDeclaration::GetStandalone() const  {return myStandalone;}

XmlDeclaration* XmlDeclaration::ToDeclaration()      {return this;}
XmlNode::Type   XmlDeclaration::GetType() const      {return DECLARATION;}

// ===================================================================================
// XmlDocument
// ===================================================================================

XmlDocument::XmlDocument()
{
	ClearError();
}

bool XmlDocument::Load(const char* path, CondenseWhitespace cw)
{
	ClearError();

	// Open the file
	GxFileInterface* file = GxFileInterface::Get();
	GxFileHandle fp = file->Open(path);
	if(!fp)
	{
		myError.description = "Unable to open xml file";
		return false;
	}
	myValue = path;

	// Read the fp contents into a buffer
	file->Seek(fp, 0, SEEK_END);
	size_t size = file->Tell(fp);
	file->Seek(fp, 0, SEEK_SET);

	char* buffer = GxMalloc<char>(size + 1);
	size = file->Read(fp, buffer, size);
	buffer[size] = 0;

	// Parse the string of text
	bool result = LoadString(buffer, cw);

	GxFree(buffer);
	return result;
}

bool XmlDocument::LoadString(const char* text, CondenseWhitespace cw)
{
	int len = GxStrLen(text);
	char* buffer = GxMalloc<char>(len + 1);

	// Convert all carriage returns to line feeds
	const char* read = text;
	char* write = buffer;
	while(*read)
	{
		if(*read == 0xD) // CR
		{
			*write = 0xA; // LF
			++write, ++read;
			if(*read == 0xA) ++read; // LF
		}
		else 
		{
			*write++ = *read++;
		}
	}
	*write = 0;

	// Skip BOM character
	const char* p = buffer;
	const uchar bom[] = {0xEF, 0xBB, 0xBF};
	if(memcmp(p, bom, 3) == 0) p += 3;

	// Parse the document
	XmlParser parser(cw);
	parser.ParseDoc(p, this);

	GxFree(buffer);
	return !myError.isError;
}

GxString XmlDocument::Save()
{
	GxString result;
	XmlWriter writer(this);
	writer.WriteDoc(0, result, this);
	return result;
}

void XmlDocument::ClearError()
{
	myError.description = "No error";
	myError.isError = false;
}

void XmlDocument::SetError(const GxString& description)
{
	myError.description = description;
	myError.isError = true;
}

const XmlDocument::Error& XmlDocument::GetError() const {return myError;}
XmlDocument*              XmlDocument::ToDocument()     {return this;}
XmlNode::Type             XmlDocument::GetType() const  {return DOCUMENT;}

}; // namespace core
}; // namespace guix