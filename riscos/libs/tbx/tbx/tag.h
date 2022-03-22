/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2010 Alan Buckley   All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef TBX_TAG_H_
#define TBX_TAG_H_

#include <vector>
#include <string>
#include <iostream>
#include <stdexcept>
#include <cstdlib>

namespace tbx
{

class TagDoc;
class Tag;

/**
 * Class to represent a single attribute
 */
class TagAttribute
{
protected:
	TagAttribute(int id);
	TagAttribute(int id, const std::string &value);
	~TagAttribute();

public:
	/**
       * Attribute Id
       */
	int id() const	{return _id;}

	/**
	 * Get the current string value for the attribute
	 */
	const std::string &value()	{return _value;}
	/**
	 * Return the current value as an integer
	 *
	 * @return integer value or 0 if the current value is not an integer
	 */
	int integer() const		{return atoi(_value.c_str());}
	/**
	 * Set the value of the attribute
	 *
	 * @param value the new string value of the attribute
	 */
	void value(const std::string &value)	{_value = value;_has_value = true;}

	/**
	 * Set if the attribute has a value associated with it
	 *
	 * @param has true if the attribute has a value
	 */
	void has_value(bool has)	{_has_value = has;}
	/**
	 * Check if the attribute has a value
	 *
	 * @returns true if the attribute has a value
	 */
	bool has_value() const	{return _has_value;}

	/**
	 * Get the next attribute for the same tag
	 *
	 * @returns next attribute or 0 if there are no more attributes
	 */
	TagAttribute *next()	{return _next;}

protected:
	int _id; 			//!< Id of attribute
	std::string _value;	//!< Value of attribute (if any)
	bool _has_value;	//!< true if attribute has a value
	TagAttribute *_next;//!< Next attribute for the same tag

	// Tag class maintains the attributes
	friend class Tag;
};

/**
 * Base class used for adding data to a tag
 */
class TagData
{
public:
	TagData() {};
	virtual ~TagData() {};
};

/**
 * Class to represent a some data tagged with an id.
 * The tag can optionally have:
 *    text
 *    a pointer to some data derived from TagData
 *    one or more attributes.
 *
 *    Tags are created by adding them to their parent.
 *    Initially this will be a TagDoc.
 */
class Tag
{
protected:
	Tag(Tag *parent, int id);
	~Tag();

public:
	/**
	 * Return the tag id
	 *
	 * @return the ID for this type of tag
	 */
	int id() const						{return _id;}
	const std::string &name() const;

	TagDoc *doc() const;
	/**
	 * Get the parent for this tag
	 *
	 * @returns parent or 0 if it is the root of the tag tree
	 */
	Tag *parent() const				{return _parent;}

	/**
	 * Get the first child tag
	 *
	 * @returns first child tag or 0 if this tag has no children
	 */
	Tag *first_child()	const			{return _first_child;}
	/**
	 * Get the last child tag
	 *
	 * @returns last child tag or 0 if this tag has no children
	 */
	Tag *last_child()	const			{return _last_child;}
	/**
	 * Get the next sibling tag
	 *
	 * @returns the next sibling tag or 0 if there are no more
	 */
	Tag *next() const				{return _next;}

	/**
	 * Get the first attribute of this tag
	 *
	 * @returns first tag attribute or 0 if this tag has no attributes
	 */
	TagAttribute *first_attribute() const	{return _first_attribute;}

	Tag *add_child(int id);
	Tag *add_child(const std::string &name);
	void delete_child(Tag *tag);

	Tag *find_child(int id, Tag *after = NULL) const;
	Tag *find_child(const std::string &name, Tag *after = NULL) const;
	Tag *find_child(int id, int attId, Tag *after = NULL) const;
	Tag *find_child(const std::string &name, const std::string &attName, Tag *after = NULL) const;
	Tag *find_child(int id, int attId, const std::string &value, Tag *after = NULL) const;
	Tag *find_child(const std::string &name, const std::string &attName, const std::string &value, Tag *after = NULL) const;

	/**
	 * Get the text from the tag
	 *
	 * @returns text for this tag or "" if none
	 */
	const std::string &text() const		{return _text;}
	/**
	 * Set the text for this tag
	 *
	 * @param text new text for the tag
	 */
	void text(const std::string &text)	{_text = text;}

	/**
	 * Get the user data for this tag
	 *
	 * @returns data for this tag or 0 if there is none
	 */
	TagData *data() const			{return _data;}
	/**
	 * Set the data for this tag
	 *
	 * @param data new data for the tag
	 */
	void data(TagData *data)		{_data = data;}

	void attribute(int att_id);
	void attribute(int att_id, const std::string &value);
	void delete_attribute(int attId);

	void attribute(const std::string &name);
	void attribute(const std::string &name, const std::string &value);
	void delete_attribute(const std::string &name);

	TagAttribute *find_attribute(int att_id) const;
	TagAttribute *find_attribute(const std::string &name) const;

	std::string attribute_value(int att_id) const;
	std::string attribute_value(const std::string &name) const;

protected:
	Tag *_parent;		//!< parent tag or 0 for no parent
	Tag *_first_child;	//!< first child tag or 0 for no children
	Tag *_last_child;	//!< last child tag or 0 for no children
	Tag *_next;			//!< next sibling tag or 0 if no more siblings
	TagAttribute *_first_attribute; //!< first attribute or 0 if no attributes

	int _id;			//!< ID for this type of tag
	std::string _text;	//!< text for this type of tag or "" if not set
	TagData *_data;		//!< user data or 0 if none
};

/**
 * Document that stores Tags with optional attributes in a hierarchy
 *
 * The document is the root of the tree and allocates the ids used
 * for the tag and attribute names.
 *
 * It also allows loading and saving of the tags using a simple XML
 * like text file format.
 */
class TagDoc : public Tag
{
public:
	TagDoc();
	~TagDoc();

	int tag_id(const std::string &name);
	const std::string &tag_name(int id) const;
	int tag_id_if_exists(const std::string &name) const;

	int attribute_id(const std::string &name);
	int attribute_id_if_exists(const std::string &name) const;
	const std::string &attribute_name(int id) const;

	void write_tag(std::ostream &os, Tag *tag, int indent = 0);
	void read_tag(std::istream &is);

protected:
	// Helper functions
	void write_string(std::ostream &os, const std::string &text);
	char read_string(std::istream &is, std::string &text);
	char read_name(std::istream &is, std::string &name);

protected:
	/**
	 * List of all tag names known to this document
	 */
	std::vector<std::string> _tag_names;
	/**
	 * List of all attribute names known to this document
	 */
	std::vector<std::string> _attribute_names;
};


/**
 * Exception thrown when reading a saved tag fails
 */
class TagException : public std::runtime_error
{
public:
	/**
	 * Enumeration describing the reason for a tag exception
	 */
	enum Cause {None, EmptyFile, ErrorReading, EndTagNotMatch,
		InvalidTagStartChar, InvalidTagEndChar,
		InvalidNameEnd, MissingTagName,
		InvalidStringStart, InvalidStringEnd, InvalidEntityEnd, InvalidEntity
	};
	TagException(Cause cause, const std::string &item);
	TagException(Cause cause, char c);
	~TagException() throw() {}

    /**
     * Return enumeration value specifying the cause of the exception
     */
     Cause cause() const {return _cause;}

     /**
      * Return the item or character the exception happened on
      */
     const std::string &item() const {return _item;}

private:
     static std::string error_text(Cause cause, std::string item);
// Member variables
private:
	Cause _cause;
	std::string _item;
};

};

std::istream &operator>>(std::istream &is, tbx::TagDoc &doc);
std::ostream &operator<<(std::ostream &os, tbx::TagDoc &doc);


#endif
