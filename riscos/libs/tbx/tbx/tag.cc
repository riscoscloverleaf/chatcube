/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2010-2015 Alan Buckley   All Rights Reserved.
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

// Tag.cpp - implement tagged documents
//
//	Read/Write data using stream operators in TagDoc.
//
//  Each tag consists of:
//		name
//		attributes
//		attributes with values
//		One piece of text
//		Program data (not read/or written to the file).
//		A list of child tags.
//
//	This is basically a cut down version of XML.
//
//	The file format is:
//	<
//		tag [attribute]... [attribute = "value"]...
//	/> (if the tag has no further data or)
//	>
//	[text]
//	[child tag]...
//	</
//	  tag
//	>
/////////////////////////////////////////////////////////////

#include "tag.h"
#include <cstdlib>

using namespace std;
using namespace tbx;

/**
 * Construct an attribute without a value
 *
 * @param id ID of the name for this type of tag attribute
 */
TagAttribute::TagAttribute(int id)
{
	_id = id;
	_has_value = false;
	_next = NULL;
}

/**
 * Construct an attribute with the given value
 *
 * @param id ID of the name for this type of tag attribute
 * @param value value of the tag attribute
 */
TagAttribute::TagAttribute(int id, const std::string &value)
{
	_id = id;
	_has_value = true;
	_value = value;
	_next = NULL;
}

TagAttribute::~TagAttribute()
{
}


/**
 * Construct a tag with the given parent and id
 *
 * @param parent parent of the tag
 * @param id ID of name of this type of tag
 */
Tag::Tag(Tag *parent, int id)
{
	_parent = parent;
	_first_child = NULL;
	_last_child = NULL;
	_next = NULL;
	_first_attribute = NULL;

	_id  = id;
	_data = NULL;
}

/**
 * Destructor for tag deletes assosiated tag data
 */
Tag::~Tag()
{
	delete _data;
}


/**
 * Return the name of this type of tag
 */
const std::string &Tag::name() const
{
	return doc()->tag_name(_id);
}

/**
 * Return the document this tag belongs to.
 */
TagDoc *Tag::doc() const
{
	Tag *check = const_cast<Tag *>(this);
	while (check->_parent) check = check->_parent;
	return static_cast<TagDoc *>(check);
}

/**
 * Add a new tag as a child of this tag
 *
 * @param id - tag id - must have been returned from TagDoc::tag_id
 * @returns Tag pointer of new tag created
 */
Tag *Tag::add_child(int id)
{
	Tag *tag = new Tag(this, id);
	if (_first_child == NULL)
	{
		_first_child = _last_child = tag;
	} else
	{
		_last_child->_next = tag;
		_last_child = tag;
	}

	return tag;
}

/**
 * Add a new tag with the given name as a child
 * of this tag.
 *
 * @param name - name for the tag. If it the name does not exists in the document it is added to it.
 * @returns Tag pointer of new tag created
 */
Tag *Tag::add_child(const std::string &name)
{
	return add_child(doc()->tag_id(name));
}

/**
 * Deletes at child tag
 *
 * @param tag tag to delete.
 */
void Tag::delete_child(Tag *tag)
{
	if (_first_child == tag)
	{
		_first_child = tag->_next;
		if (_first_child == NULL) _last_child = NULL;
	} else
	{
		Tag *prev = _first_child;
		while (prev->_next != tag) prev = prev->_next;
		prev->_next = tag->_next;
		if (_last_child == tag) _last_child = prev;
	}

	delete tag;
}

/**
 * Find the a child tag with the given id
 *
 * @param id tag id to search for
 * @param after tag to start search after or 0 to start from the beginning
 * @return pointer to tag if found or 0
 */
Tag *Tag::find_child(int id, Tag *after /*= 0*/) const
{
	Tag *found;
	if (after) found = after->_next;
	else found = _first_child;

	while (found && found->_id != id) found = found->_next;

	return found;
}

/**
 * Find the a child tag with the given name
 *
 * @param name tag name to search for
 * @param after tag to start search after or 0 to start from the beginning
 * @return pointer to tag if found or 0
 */
Tag *Tag::find_child(const std::string &name, Tag *after /*= 0*/) const
{
	return find_child(doc()->tag_id_if_exists(name), after);
}

/**
 * Find a child tag with the given attribute
 *
 * @param id tag id to search for
 * @param att_id attribute id for attribute tag must have
 * @param after tag to start search after or 0 to start from the beginning
 * @return pointer to tag if found or 0
 */
Tag *Tag::find_child(int id, int att_id, Tag *after /*= 0*/) const
{
	Tag *search = after;
	Tag *found = NULL;
	bool more = true;
	
	while (more)
	{
		search = find_child(id, search);
		if (search)
		{
			if (search->find_attribute(att_id))
			{
				found = search;
				more = false;
			}
		} else
			more = false;
	}

	return found;
}
	
/**
 * Find a child tag with the given attribute
 *
 * @param name tag name to search for
 * @param att_name attribute name for attribute tag must have
 * @param after tag to start search after or 0 to start from the beginning
 * @return pointer to tag if found or 0
 */

Tag *Tag::find_child(const std::string &name, const std::string &att_name, Tag *after /*= 0*/) const
{
	TagDoc *pDoc = doc();
	return find_child(pDoc->tag_id_if_exists(name), pDoc->attribute_id_if_exists(att_name), after);
}

/**
 * Find a child tag with the given attribute value
 *
 * @param id tag id to search for
 * @param att_id attribute id for attribute tag must have
 * @param value value to search for
 * @param after tag to start search after or 0 to start from the beginning
 * @return pointer to tag if found or 0
 */
Tag *Tag::find_child(int id, int att_id, const std::string &value, Tag *after /*= 0*/) const
{
	Tag *search = after;
	Tag *found = NULL;
	TagAttribute *check;
	bool more = true;
	
	while (more)
	{
		search = find_child(id, search);
		if (search)
		{
			check = search->find_attribute(att_id);
			if (check && check->value() == value)
			{
				found = search;
				more = false;
			}
		} else
			more = false;
	}

	return found;
}

/**
 * Find a child tag with the given attribute value
 *
 * @param name tag name to search for
 * @param att_name attribute name for attribute tag must have
 * @param value value to search for
 * @param after tag to start search after or 0 to start from the beginning
 * @return pointer to tag if found or 0
 */
Tag *Tag::find_child(const std::string &name, const std::string &att_name, const std::string &value, Tag *after /*= 0*/) const
{
	TagDoc *pDoc = doc();
	return find_child(pDoc->tag_id_if_exists(name), pDoc->attribute_id_if_exists(att_name), value, after);
}

/**
 * Sets attribute with given id on the tag with no value.
 * i.e. TagAttribute::has_value() == false
 * Creates the attribute if it doesn't already exists
 *
 * @param att_id attribute id, must have been allocated with TagDoc::attribute_id
 */
void Tag::attribute(int att_id)
{
	TagAttribute *found = find_attribute(att_id);
	if (found) found->has_value(false);
	else
	{
		found = new TagAttribute(att_id);
		found->_next = _first_attribute;
		_first_attribute = found;
	}
}

/**
 * Sets attribute with given id on the tag with the given value
 *
 * Creates the attribute if it doesn't already exists
 *
 * @param att_id attribute id, must have been allocated with TagDoc::attribute_id
 * @param value string value for the attribute
 */
void Tag::attribute(int att_id, const std::string &value)
{
	TagAttribute *found = find_attribute(att_id);
	if (found) found->value(value);
	else
	{
		found = new TagAttribute(att_id, value);
		found->_next = _first_attribute;
		_first_attribute = found;
	}
}

/**
 * Delete the attribute with the given id
 *
 * @param att_id attribute id, must have been allocated with TagDoc::attribute_id
 */
void Tag::delete_attribute(int att_id)
{
	TagAttribute *prev = NULL;
	TagAttribute *found = _first_attribute;

	while(found && found->_id != att_id)
	{
		prev = found;
		found = found->_next;
	}
	if (found)
	{
		if (prev) prev->_next = found->_next;
		else _first_attribute = found->_next;

		delete found;
	}
}

/**
 * Sets attribute with given name on the tag with no value.
 * i.e. TagAttribute::has_value() == false
 *
 * Adds the attribute name to the list of attribute names if it doesn't already exists
 * Creates the attribute if it doesn't already exists
 *
 * @param name attribute name
 */

void Tag::attribute(const std::string &name)
{
	attribute(doc()->attribute_id(name));
}

/**
 * Sets attribute with given name on the tag with the given value.
 *
 * Adds the attribute name to the list of attribute names if it doesn't already exists
 * Creates the attribute if it doesn't already exists
 *
 * @param name attribute name
 * @param value string value for attribute
 */
void Tag::attribute(const std::string &name, const std::string &value)
{
	attribute(doc()->attribute_id(name), value);
}

/**
 * Delete the named attribute
 *
 * @param name attribute name
 */
void Tag::delete_attribute(const std::string &name)
{
	delete_attribute(doc()->attribute_id_if_exists(name));
}

/**
 * Find the attribute with the given id
 *
 * @param att_id attribute id, must have been allocated with TagDoc::attribute_id
 * @return TagAttribute pointer or 0 if not found
 */
TagAttribute *Tag::find_attribute(int att_id) const
{
	TagAttribute *found = _first_attribute;
	while (found && found->_id != att_id) found = found->_next;
	return found;
}

/**
 * Find the attribute with the given id
 *
 * @param name attribute name
 * @return TagAttribute pointer or 0 if not found
 */
TagAttribute *Tag::find_attribute(const std::string &name) const
{
	return find_attribute(doc()->attribute_id_if_exists(name));
}


/**
 * Get the value of an attribute
 *
 * @param att_id id of the attribute
 * @return attribute value or empty string if attribute doesn't exist on this tag
 */
std::string Tag::attribute_value(int att_id) const
{
	TagAttribute *att = find_attribute(att_id);
	return (att ? att->value() : "");
}

/**
 * Get the value of an attribute
 *
 * @param name name of the attribute
 * @return attribute value or empty string if attribute doesn't exist on this tag
 */
std::string Tag::attribute_value(const std::string &name) const
{
	TagAttribute *att = find_attribute(name);
	return (att ? att->value() : "");
}

/**
 * Constructor for a new empty tag document.
 */
TagDoc::TagDoc() : Tag(NULL, -2)
{
}

/**
 * Destructor
 */
TagDoc::~TagDoc()
{
}

/**
 * Returns the tag id for a name or allocates a new one if the name
 * has not already been used.
 *
 * @param name name for the tag
 */
int TagDoc::tag_id(const std::string &name)
{
	int id = -1;
	int j, size = _tag_names.size();

	for (j = 0; j < size && id < 0; j++)
	{
		if (_tag_names[j] == name) id = j;
	}

	if (id < 0)
	{
		id = _tag_names.size();
		_tag_names.push_back(name);
	}

	return id;
}

/**
 * Gets a tag id if it has already been defined
 *
 * @param name tag name to get id for
 * @returns id for name or -1 if tag is not in this document.
 */
int TagDoc::tag_id_if_exists(const std::string &name) const
{
	int id = -1;
	int j, size = _tag_names.size();

	for (j = 0; j < size && id < 0; j++)
	{
		if (_tag_names[j] == name) id = j;
	}

	return id;
}

/**
 * Get the name of the tag given its id.
 *
 * @param id tag id to get name for
 */
const std::string &TagDoc::tag_name(int id) const
{
	return _tag_names[id];
}

/**
 * Returns the attribute id for a name or allocates a new one if the name
 * has not already been used.
 *
 * @param name name for the attribute
 */
int TagDoc::attribute_id(const std::string &name)
{
	int id = -1;
	int j, size = _attribute_names.size();

	for (j = 0; j < size && id < 0; j++)
	{
		if (_attribute_names[j] == name) id = j;
	}

	if (id < 0)
	{
		id = _attribute_names.size();
		_attribute_names.push_back(name);
	}

	return id;
}

/**
 * Gets a attribute id if it has already been defined
 *
 * @param name attribute name to get id for
 * @returns id for name or -1 if attribute is not in this document.
 */
int TagDoc::attribute_id_if_exists(const std::string &name) const
{
	int id = -1;
	int j, size = _attribute_names.size();

	for (j = 0; j < size && id < 0; j++)
	{
		if (_attribute_names[j] == name) id = j;
	}

	return id;
}

/**
 * Get the name of the attribute given its id.
 *
 * @param id attribute id to get name for
 */
const std::string &TagDoc::attribute_name(int id) const
{
	return _attribute_names[id];
}

//////////////////////////////////////////////////////////////////////
//@{
//	Read a tagged document from an input stream.
//
//	This will read only between the next start tag and its
//  associated end tag. If multiple top level tags this will
//	have to be repeated called for each tag.
//
//@param is The stream containing the tag data.
//@returns A reference to the passed stream.
//@}
//////////////////////////////////////////////////////////////////////

std::istream &operator>>(std::istream &is, TagDoc &doc)
{
	doc.read_tag(is);
	return is;
}

/**
 * Reads a tag and all its children from a stream
 *
 * @param is input stream to read data from
 * @throws TagException if read fails or there is a syntax error
 */
void TagDoc::read_tag(std::istream &is)
{
	Tag *tag = this;
	string tag_name;
	string att_name, next_att_name;
	string att_value;
	string text;
	bool start_tag = false;
	char c;

	is.setf(ios::skipws);

	do
	{
		if (start_tag)
		{
			c = read_string(is, text);
			tag->text(text);
			start_tag = false;
		} else
		{
			is >> c;
		}
		if (!is)
		{
			if (tag == this) throw TagException(TagException::EmptyFile,"");
			else throw TagException(TagException::ErrorReading, "");
		}

		if (c != '<') throw TagException(TagException::InvalidTagStartChar, c);
		c = read_name(is, tag_name);

		if (c == '/')
		{
			// End tag
			c = read_name(is, tag_name);
			if (tag->name() != tag_name) throw TagException(TagException::EndTagNotMatch, tag_name);
			if (c == ' ') is >> c;
			if (c != '>') throw TagException(TagException::InvalidTagEndChar, c);
			tag = tag->parent();
		} else if (c != '>' && c != ' ') throw TagException(TagException::InvalidNameEnd, c);
		else
		{
			if (tag_name.empty()) throw TagException(TagException::MissingTagName,"");
			// Add new tag and move focus to it.
			tag = tag->add_child(tag_name);
			start_tag = true;

			// Parse attributes
			next_att_name.erase();

			while (c != '>' && c != '/' && is)
			{
				if (!next_att_name.empty())
				{
					att_name = next_att_name;
					next_att_name.erase();
				} else c = read_name(is, att_name);

				if (c == ' ') c = read_name(is, next_att_name);

				if (c == '=')
				{
					is >> c;
					if (c != '\"') throw TagException(TagException::InvalidStringStart,c);
					c = read_string(is, att_value);
					if (c != '\"') throw TagException(TagException::InvalidStringEnd,c);
					tag->attribute(att_name, att_value);
				} else if (!att_name.empty())
					tag->attribute(att_name);
			}

			if (c == '/')
			{
				// Tag with no text or children
				is >> c;
				tag = tag->parent();
			}
			if (c != '>') throw TagException(TagException::InvalidTagEndChar, c);
		}

	} while (tag != this); // Exit when we get back up to this level

}

/**
 * Write Tag document to an output stream
 *
 * @param os stream to write to
 * @param doc Tag document to write
 */
std::ostream &operator<<(std::ostream &os, TagDoc &doc)
{
	Tag *child = doc.first_child();
	while (child)
	{
		doc.write_tag(os, child);
		child = child->next();
	}

	return os;
}

/**
 * Write a tag and all its children to an output stream
 *
 * @param os stream to write to
 * @param tag to write
 * @param indent number of spaces to indent the tag (default to 0)
 */
void TagDoc::write_tag(std::ostream &os, Tag *tag, int indent /* = 0*/)
{
	int j;
	for (j = 0; j < indent; j++) os << ' ';

	os << '<' << tag->name();
	TagAttribute *att = tag->first_attribute();
	while (att)
	{
		os << ' ';
		os << _attribute_names[att->id()];
		if (att->has_value())
		{
			os << " = \"";
			write_string(os, att->value());
			os << "\"";
		}
		att = att->next();
	}

	Tag *child = tag->first_child();
	if (child == NULL && tag->text().empty())
	{
		// Imediately close tag
		os << "/>";
	} else
	{
		os << ">";
		if (!tag->text().empty()) write_string(os, tag->text());
		if (child)
		{
			os << endl;
			while (child)
			{
				write_tag(os, child, indent+3);
				child = child->next();
			}
		}

		for (j = 0; j < indent; j++) os << ' ';
		os << "</" << tag->name() << '>';
	}

	os << endl;
}

/**
 * Write a string to the output stream converting characters
 * used in the syntax to special entities.
 *
 * @param os stream to write to
 * @param text text to write.
 */
void TagDoc::write_string(std::ostream &os, const std::string &text)
{
	unsigned int pos = 0;
	const char *entities = "&<>\"\'";
	unsigned int epos = text.find_first_of(entities);

	while (epos != string::npos)
	{
		if (epos > pos) os << text.substr(pos, epos-pos);
		switch(text[epos])
		{
		case '&': os << "&amp;"; break;
		case '<': os << "&lt;"; break;
		case '>': os << "&gt;"; break;
		case '\"': os << "&quot;"; break;
		case '\'': os << "&apos;"; break;
		}
		pos = epos+1;
		epos = text.find_first_of(entities, pos);
	}
	
	if (pos < text.length()) os << text.substr(pos);
}

/**
 * Read a string from an input stream converting entities used
 * back to characters.
 *
 * @param is input string to read from
 * @param text text to read
 * @returns char that finished string read
 * @throws TagException if an invalid entity is found in the string
 */
char TagDoc::read_string(std::istream &is, std::string &text)
{
	char c;
	
	text.erase();

	is >> c;
	is.unsetf(ios::skipws);

	while (is && c != '\"' && c != '<')
	{
		if (c == '&')
		{
			std::string entity;
			is >> c;
			while (entity.length() < 5 && c != ';' && is)
			{
				entity += c;
				is >> c;
			}

			if (c != ';') throw TagException(TagException::InvalidEntityEnd, entity);

			if (entity == "amp")	   text += '&';
			else if (entity == "lt")   text += '<';
			else if (entity == "gt")   text += '>';
			else if (entity == "apos") text += '\'';
			else if (entity == "quot") text += '\"';
			else throw TagException(TagException::InvalidEntity, entity);
		} else
			text += c;

		is >> c;
	}

	is.setf(ios::skipws);

	return c;
}

/**
 * Read a name from and input stream.
 *
 * @param is input string to read from
 * @param name name to read
 * @returns char that finished string read
 * @throws TagException if an invalid entity is found in the string
 */
char TagDoc::read_name(std::istream &is, std::string &name)
{
	char c;
	std::string delims(" >/=");
	
	name.erase();

	is >> c;
	is.unsetf(ios::skipws);

	while (is && delims.find(c) == std::string::npos)
	{
		name += c;
		is >> c;
	}
	is.setf(ios::skipws);

	return c;
}

/**
 * Constructor for error with an item name
 */
TagException::TagException(Cause cause, const std::string &item) :
		std::runtime_error(error_text(cause, item)),
		_cause(cause),
		_item(item)
{
}

/**
 * Contructor for error due to invalid character
 */
TagException::TagException(Cause cause, char c)  :
		std::runtime_error(error_text(cause, std::string(1,c))),
		_cause(cause),
		_item(1,c)
{
}

/**
 * Helper function to get the full error text
 */
std::string TagException::error_text(Cause cause, std::string item)
{
	std::string text;
	switch(cause)
	{
	case None: text = "No errors"; break;
	case EmptyFile: text = "Empty file"; break;
	case ErrorReading: text = "Error while reading file"; break;
	case EndTagNotMatch: text.assign("End tag does not match start tag ").append(item); break;
	case InvalidTagStartChar: text.assign("Start tag character (<) not found, char found = ").append(item); break;
	case InvalidTagEndChar: text.assign("End tag character (<) not found, char found = ").append(item); break;
	case InvalidNameEnd: text.assign("Invalid character at the end of a name, char found = ").append(item); break;
	case MissingTagName: text.assign("Missing tag name between < and >"); break;
	case InvalidStringStart: text.assign("Missing quote (\") at start of string, char found = ").append(item); break;
	case InvalidStringEnd: text.assign("Missing quote (\") at end of string, char found = ").append(item); break;
	case InvalidEntityEnd: text.assign("Missing end to entity, starts ").append(item); break;
	case InvalidEntity: text.assign("Invalid character entity ").append(item); break;
	}

	return text;
}
