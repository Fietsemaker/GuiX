/// @file
/// Contains classes for reading and writing xml files.

#pragma once

#include <GuiX/String.h>

namespace guix {
namespace core {

// Forward declaration, allows casting between xml nodes.
class XmlElem;
class XmlText;
class XmlComment;
class XmlDeclaration;
class XmlDocument;

// ===================================================================================
// XmlNode
// ===================================================================================
/** The XmlNode class is the base node from which other xml nodes are derived.

 When an xml document is loaded, the file is converted to a tree of xml nodes.
 All nodes except the root node have a parent, and nodes can have any number of
 children. Nodes under the same parent are also linked to each other as siblings.
 The following nodes can occur in an xml document:

 - XmlElem:        an xml element with attributes and child nodes.
 - XmlText:        a section of text or CDATA inside an element.
 - XmlComment:     a comment in the xml document.
 - XmlDeclaration: the declaration at the start of the xml document.
 - XmlDocument:    the root node of the xml document.

 Each node also has a generic 'value' property. Depending on the node type, this
 contains the following data:

 - XmlElem:        name of the element.
 - XmlText:        the string of text.
 - XmlComment:     the comment text.
 - XmlDeclaration: value is not used.
 - XmlDocument:    the path of the file.

 To load an xml file, create an XmlDocument and use the \c Load() function.

 @see XmlElem, XmlText, XmlComment, XmlDeclaration, XmlDocument
*/
class XmlNode
{
public:
	virtual ~XmlNode();

	// Types of xml nodes.
	enum Type
	{
		ELEM,        /// XmlElem.
		TEXT,        /// XmlText.
		COMMENT,     /// XmlComment.
		DECLARATION, /// XmlDeclaration.
		DOCUMENT,    /// XmlDocument.
		UNKNOWN,     /// XmlNode.
	};

	/// Constructs an empty xml node.
	XmlNode();

	/// Removes all child nodes from this node.
	void ClearChildren();

	/// Adds an xml node to this node as a child.
	/// The ownership of the child node is transferred to this node.
	void AddChild(XmlNode* child);

	/// Sets the node value, see class description for more info.
	void SetValue(GxString value);

	/// Returns the node that contains this node, or NULL if there is none.
	XmlNode* Parent();

	/// Returns the first child, or NULL if there is none.
	XmlNode* Child();

	/// Returns the first child element, or NULL if there is none.
	XmlElem* ChildElem();

	/// Returns the first child that matches value, or NULL if there is none.
	XmlNode* Child(const char* name);

	/// Returns the first child element that matches value, or NULL if there is none.
	XmlElem* ChildElem(const char* name);

	/// Returns the next sibling, or NULL if there is none.
	XmlNode* Next();

	/// Returns the next sibling element, or NULL if there is none.
	XmlElem* NextElem();

	/// Returns the next sibling that matches value, or NULL if there is none.
	XmlNode* Next(const char* value);     

	 /// Returns the next sibling element that matches value, or NULL if there is none.
	XmlElem* NextElem(const char* value);

	/// Returns the value of the node, see class description for more info.
	const GxString& Value() const;

	/// Casts the node to a more specific type. Returns NULL if the node
	/// is not of the requested type.
	/// @{
	virtual XmlDocument*    ToDocument();
	virtual XmlDeclaration* ToDeclaration();
	virtual XmlComment*     ToComment();
	virtual XmlElem*        ToElem();
	virtual XmlText*        ToText();
	/// @}

	/// Returns the type of the node.
	virtual Type GetType() const;

protected:
	GxString myValue;
	XmlNode* myNext;
	XmlNode* myParent;
	XmlNode* myChild;

private:
	XmlNode(const XmlNode&);
	void operator = (const XmlNode&);
};

// ===================================================================================
// XmlElem
// ===================================================================================
/** The XmlElem class represents an element inside an xml document.

 XmlElem nodes store an element name, attributes and child nodes. To get the name of
 the element, use \c GetValue(). To get the value of one of the attributes, use the
 \c GetAttribute() functions. Nodes that are contained within the opening and closing
 tag of the element can be accessed using the child traversal functions inherited from
 XmlNode.

 @see XmlNode
*/
class XmlElem : public XmlNode
{
public:
	~XmlElem();

	/// Used to store xml element attributes.
	struct Attribute
	{
		GxString name;  ///< The attribute name.
		GxString value; ///< The attribute value.
	};

	/// Constructs an xml element without a name.
	XmlElem();

	/// Constructs an xml element with the given name.
	XmlElem(GxString name);

	/// Removes all attributes from the element.
	void ClearAttributes();

	/// Adds an attribute to the element.
	void AddAttribute(GxString name, GxString value);

	/// Returns the number of attributes that the element has.
	int GetAttributeCount() const;

	/// Returns the attribute of the element at position index.
	Attribute* GetAttribute(int index);

	/// Returns the value of the attribute with name, or an empty string if it does not exist.
	GxString GetAttribute(const char* name) const;

	/// Finds and stores the value of the attribute with name in v. Returns true if 
	/// the attribute was found and could be converted. On failure v is unchanged.
	/// @{
	bool GetAttribute(const char* name, int& v) const;
	bool GetAttribute(const char* name, bool& v) const;
	bool GetAttribute(const char* name, float& v) const;
	bool GetAttribute(const char* name, GxString& v) const;
	/// @}

	/// A convenience function that returns the value of the first text node inside
	/// the element. If the element does not have a child text node, the function
	/// returns an empty string.
	GxString GetText();

	XmlElem* ToElem();
	Type GetType() const;

protected:
	Attribute** myAttributes;
	int myAttributeCount;
};

// ===================================================================================
// XmlText
// ===================================================================================
/** The XmlText class represents a section of text inside an xml document.

 XmlText contains a section of raw text or text obtained from reading CDATA. To access
 the text, use \c GetValue().

 @see XmlNode
*/
class XmlText : public XmlNode
{
public:
	// Constructs an empty xml text.
	XmlText();

	// Constructs an xml text that contains the given text.
	XmlText(GxString text);

	XmlText* ToText();
	Type GetType() const;
};

// ===================================================================================
// XmlComment
// ===================================================================================
/** The XmlComment class represents a comment inside an xml document.

 To access the comment text, use \c GetValue().

 @see XmlNode
*/
class XmlComment : public XmlNode
{
public:
	// Constructs an empty xml comment.
	XmlComment();

	// Constructs an xml comment node that contains the given text.
	XmlComment(GxString text);

	XmlComment* ToComment();
	Type GetType() const;
};

// ===================================================================================
// XmlDeclaration
// ===================================================================================
/** The XmlDeclaration class represents the declaration at the start of an xml document.

 The xml declaration is a special node that is only found at the start of a document.
 It can contain several attributes that identify the document as xml and provide
 information to the xml parser.

 @see XmlNode
*/
class XmlDeclaration : public XmlNode
{
public:
	/// Sets one or more attributes in the xml declaration.
	void Set(GxString version = GxString(), GxString encoding = GxString(), GxString standalone = GxString()); 

	const GxString& GetVersion() const;    ///< Returns the 'version' attribute.
	const GxString& GetEncoding() const;   ///< Returns the 'encoding' attribute.
	const GxString& GetStandalone() const; ///< Returns the 'standalone' attribute.

	XmlDeclaration* ToDeclaration();
	Type GetType() const;

private:
	GxString myVersion;
	GxString myEncoding;
	GxString myStandalone;
};

// ===================================================================================
// XmlDocument
// ===================================================================================
/** The XmlDocument class is used to load and save xml documents.

 The xml document is the root node of an xml node tree. It is also the owner of the
 document and can be used to load new documents from an xml file, or to save the
 document to a string.

 @see XmlNode
*/
class XmlDocument : public XmlNode
{
public:
	// Different settings for condensing whitespace when parsing an xml document.
	enum CondenseWhitespace
	{
		CW_FULL,           ///< Condense all whitespace in text to a single space.
		CW_LEAD_AND_TRAIL, ///< Only condense leading and trailing whitespace.
		CW_NONE,           ///< Leave all whitespace untouched.
	};

	// Contains error information when the parsing of an xml file has failed.
	struct Error
	{
		GxString description; // A short description of the error that occured.
		bool isError;       // True if there is an error.
	};

	// Constructs an empty xml document.
	XmlDocument();

	// Loads an xml document from an xml file.
	bool Load(const char* path, CondenseWhitespace cw = CW_FULL);

	// Loads an xml document from a string of text.
	bool LoadString(const char* text, CondenseWhitespace cw = CW_FULL);

	// Writes the xml document to a string, which can be saved as an xml file.
	GxString Save();

	// Resets the error flag and error description.
	void ClearError();

	// Sets the error flag and error description. Mainly used by \c Load().
	void SetError(const GxString& description);
	
	// Returns error information when loading an xml file has failed. 
	const Error& GetError() const;

	XmlDocument* ToDocument();
	Type GetType() const;

private:
	Error myError;
};

}; // namespace core
}; // namespace guix