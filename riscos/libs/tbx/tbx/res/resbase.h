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

#ifndef TBX_RES_RESBASE_H_
#define TBX_RES_RESBASE_H_

#include <string>
#include <iostream>

#include "resstruct.h"

namespace tbx {

namespace res {

//! @cond INTERNAL

/**
 * Structure to represent a relocation table
 */
struct ResRelocationTable
{
	int _size;
	ResRelocation *_relocs;

	ResRelocationTable() : _size(0), _relocs(0) {};
	ResRelocationTable(const ResRelocationTable &other);
	~ResRelocationTable() {delete [] _relocs;}

	ResRelocationTable &operator=(const ResRelocationTable &other);

	void add(int offset, ResRelocation::Type type);
	void erase(int offset);
	void clear();

	void offsets_to_pointers(char *body, char *strings, char *msgs) const;
	void pointers_to_offsets(char *body, char *strings, char *msgs) const;
	void fix_text_pointers(bool string_table, char *body, const char *new_strings, const char *old_strings, const char *from, int by);
	void fix_all_pointers(char *new_body, const char *old_body, const char *new_strings, const char *old_strings, const char *new_messages, const char *old_messages);
	void fix_after_insert(char *new_body, const char *old_body, int offset, int count);
};

//Notes:
// Null string or message ref = -1

class ResImpl;

/**
 * Class to Manipulate an objects tables.
 *
 * This is a helper object and should not be used directly
 */
class ResData
{
	char *_strings;
	int _strings_size;
	char *_messages;
	int _messages_size;
	ResRelocationTable _reloc_table;

	friend class ResImpl;

public:
	ResData(ResDataHeader header, char *strings, int num_reloc, ResRelocation *relocs);
	ResData(const ResData &other);
	ResData();
	~ResData();

	int reloc_size() const {return _reloc_table._size;}
	int strings_size() const {return _strings_size;}
	const char *strings() const {return _strings;}
	int messages_size() const {return _messages_size;}
	const char *messages() const {return _messages;}

	void calculate_string_sizes(const char *body);

	const char *text(const char *body, int offset) const {return (char *)(*(int *)(body + offset));}
	int text_len(const char *body, int offset) const;
	void text(char *body, int offset, const char *new_text, int len, bool is_string);
	void text(char *body, int offset, std::string msg, bool is_string)
	{
		text(body, offset, msg.c_str(), msg.size(), is_string);
	}

	int number(const char *body, int offset) const
	{
		return *((int *)(body + offset));
	}
	void number(char *body, int offset, int value)
	{
		*((int *)(body+offset)) = value;
	}

	void init_text(char *body, int offset, const char *value, bool is_string);
	void init_sprite_area_ref(char *body, int offset);
	void init_object_ref(char *body, int offset);

	void copy_used(char *body, const char *old_body, const ResRelocationTable &copy_table);

	//TODO:	void object_offset(char *body, int offset);
	//TODO: void sprite_area(char *body, int offset);

	void pointers_to_offsets(char *body) const
	{
		_reloc_table.pointers_to_offsets(body, _strings, _messages);
	}

	void offsets_to_pointers(char *body) const
	{
		_reloc_table.offsets_to_pointers(body, _strings, _messages);
	}

	ResData *component_data(char *new_body, char *copy_body, int offset, int size) const;
	void add_data_from(char *body, int offset, const char *from_body, const ResData *from_data);
	void remove_data(char *body, int offset, int size);
	void fix_offsets(char *new_body, const char *old_body, int offset, int diff);

	static ResData *copy_from_read_only(char *new_header, char *readonly_header);
	static ResData *copy_component_from_read_only(char *new_body, char *readonly_header, int offset, int size);

	void write(std::ostream &file) const;

private:
	char *remove_chars(char *body, bool string_table, const char *where, int num);
	char *insert_chars(char *body, bool string_table, const char *where, int num);
	void copy_strings_and_messages(char *new_body, const char *copy_body);

};



/**
 * Internal Resource Memory implementation class
 *
 * Do not use this class
 *
 * TODO: Put in internal namespace or protect
 */
class ResImpl
{
	int _ref_count; // Reference count or -1 if read only object
	char *_header;
	char *_body;
	int _size;
	ResData *_data;
	ResRelocationTable *_type_reloc_table;

private:
	~ResImpl();

public:
	ResImpl(void *object_header, int body_offset, int size);
	ResImpl(void *object_header, int body_offset, int size, ResData *data);
	ResImpl(const ResImpl &other);
	ResImpl(const ResImpl &other, bool copy_read_only);

	void add_ref();
	void release();

	char *header() {return _header;}
	const char *header() const {return _header;}
	char *body() {return _body;}
	const char *body() const {return _body;}
	const ResData *data() const {return _data;}
	int size() const {return _size;}

	const char *text(int offset) const {return (char *)(*(int *)(_body + offset));}
	int text_len(int offset) const;
	void text(int offset, const char *new_text, int len, bool is_string)
	{
		if (_data == 0) _data = new ResData();
		_data->text(_body, offset, new_text, ((new_text == 0) ? 0 : len), is_string);
	}

	void text(int offset, std::string msg, bool is_string)
	{
		text(offset, msg.c_str(), msg.size(), is_string);
	}

	void text_with_length(int offset, const char *new_text, int max_length, bool is_string);
	void text_with_length(int offset, const std::string &new_text, int max_length, bool is_string)
	{
		if (max_length <= (int)new_text.size()) max_length = (int)new_text.size()+1;
		text_with_length(offset, new_text.c_str(), max_length, is_string);
	}

	int int_value(int offset) const;
	void int_value(int offset, int value);
	unsigned int uint_value(int offset) const;
	void uint_value(int offset, unsigned int value);

	unsigned short ushort_value(int offset) const;
	void ushort_value(int offset, unsigned short value);

	unsigned char byte_value(int offset) const;
	void byte_value(int offset, unsigned char value);

	bool flag(int offset, int mask) const;
	void flag(int offset, int mask, bool on);

	int flag_value(int offset, int mask) const;
	void flag_value(int offset, int mask, int value);

	void init_text(int offset, const char *value, bool is_string);
	void init_sprite_area_ref(int offset);
	void init_object_ref(int offset);

	ResData *component_data(char *new_body, int offset, int size) const;

	/**
	 * Check if implentation is in a read only state
	 */
	bool read_only() const {return (_ref_count == -1);}

	/**
	 * Check if there is only one object using this object
	 */
	bool unique() const {return (_ref_count == 1);}

	void insert(int offset, const ResImpl *insert_impl);
	void replace(int offset, int old_size, const ResImpl *rep_impl);
	void erase(int offset, int size);
};

//! @endcond


/**
 * Base class for objects, gadgets and menu items from ressources.
 */
class ResBase
{
protected:
	ResImpl *_impl; //!< Internal Resource implementation

	/**
	 * Create from internal resource implementation
	 *
	 * @param impl internal implementation class
	 */
	ResBase(ResImpl *impl) { _impl = impl ;}

public:
	ResBase(const ResBase &other);
	virtual ~ResBase();

	ResBase &operator=(const ResBase &other);

protected:
	/**
	 * Make object writeable, call before any change.
	 */
	virtual void make_writeable() = 0;

	// Helpers to get and set data in record
	const char *string(int offset) const;
	void string(int offset, const char *value);
	void string(int offset, std::string value);
	void string_with_length(int offset, const char *value, int length = -1);

	/**
	 * Assign a string where the length is at offset+4
	 *
	 * @param offset - offset into body of string
	 * @param value - new value for the string
	 * @param length - new length or -1 to use existing length.
	 * The length is always adjusted so it is big enough for the value.
	 */
	void string_with_length(int offset, const std::string &value, int length = -1)
	{
		string_with_length(offset, value.c_str(), length);
	}
	const char *message(int offset) const;
	void message(int offset, const char *value);
	void message(int offset, std::string value);
	void message_with_length(int offset, const char *value, int length = -1);
	/**
	 * Assign a message where the length is at offset+4
	 *
	 * @param offset offset into body of message
	 * @param value new value for the message
	 * @param length new length or -1 to use existing length.
	 * The length is always adjusted so it is big enough for the value.
	 */
	void message_with_length(int offset, const std::string &value, int length = -1)
	{
		message_with_length(offset, value.c_str(), length);
	}
	/**
	 * Get an integer value
	 *
	 * @param offset offset to retrieve value for
	 * @returns integer value at the offset
	 */
	int int_value(int offset) const {return _impl->int_value(offset);}
	void int_value(int offset, int value);
	/**
	 * Get an unsigned integer value
	 *
	 * @param offset offset to retrieve value for
	 * @returns integer value at the offset
	 */
	unsigned int uint_value(int offset) const {return _impl->uint_value(offset);}
	void uint_value(int offset, unsigned int value);

	/**
	 * Get a byte value
	 *
	 * @param offset offset to retrieve value for
	 * @returns an unsigned 8 bit value
	 */
	unsigned char byte_value(int offset) const {return _impl->byte_value(offset);}
	void byte_value(int offset, unsigned char value);

	/**
	 * Get a unsigned short value
	 *
	 * @param offset offset to retrieve value for
	 * @returns unsigned 16 bit value at the offset
	 */
	unsigned short ushort_value(int offset) const {return _impl->ushort_value(offset);}
	void ushort_value(int offset, unsigned short value);

	/**
	 * Check if any of the bits in a mask are set
	 *
	 * @param offset offset to check bits at
	 * @param mask bits to check
	 * @returns true if one or more of the mask bits are set
	 */
	bool flag(int offset, int mask) const {return _impl->flag(offset, mask);}
	void flag(int offset, int mask, bool on);

	/**
	 * Get the bits for the given mask
	 *
	 * @param offset of value to check bits off
	 * @param mask bits to check
	 * @returns value & mask bits
	 */
	int flag_value(int offset, int mask) const {return _impl->flag_value(offset, mask);}
	void flag_value(int offset, int mask, int value);

	void init_string(int offset, const char *value);
	void init_message(int offset, const char *value);
	void init_sprite_area_ref(int offset);
	void init_object_ref(int offset);
};


}

}

#endif /* TBX_RES_RESBASE_H_ */
