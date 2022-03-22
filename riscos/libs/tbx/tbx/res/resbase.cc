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

#include "resobject.h"
#include <cstring>
#include <stdexcept>
#include "../tbxexcept.h"

namespace tbx {

namespace res {

//! @cond INTERNAL

#ifdef CHECK_STRING_ALLOC

inline void check_string_table(char *table)
{
	if (table == 0) return;
	int size = *((int *)(table-8));
	fprintf(stderr, "Checking table size %d\n", size);
	if (*((unsigned int *)(table-4)) != 0xAEAEAEAE) fprintf(stderr, "*** Invalid start word %x\n", (*(int *)(table-4)));
	if (*((unsigned int *)(table + size)) != 0xEAEAEAEA) fprintf(stderr, "*** Invalid end word %x\n", (*(int *)(table+size)));
}

#endif

// Allocate string table
// Rounds up to multiple of 4 bytes
// pads end of table with char 0.
inline char *alloc_string_table(int size)
{
	int alloc_size = (size + 3) & ~3;
#ifdef CHECK_STRING_ALLOC
	char *p = new char[alloc_size+12];
	(*(int *)p)=alloc_size;
	(*(unsigned int *)(p+4))= 0xAEAEAEAE;
	p+=8;
	(*(unsigned int *)(p+alloc_size)) = 0xEAEAEAEA;
	if (size & 3) std::memset(p + size, 0, 4 - (size & 3));
	fprintf(stderr, "Allocated table %p size %d (%d)\n", p, size, alloc_size);
	if ((*(unsigned int *)(p+alloc_size)) != 0xEAEAEAEA) fprintf(stderr, "*** Padding overran end in alloc %x\n", (*(int *)(p+alloc_size)));
#else
	char *p = new char[alloc_size];
#endif
	return p;
}

/**
 * Free a string table and set pointer to 0
 */
inline void free_string_table(char *&table)
{
#ifdef CHECK_STRING_ALLOC
	if (table)
	{
		check_string_table(table);
		table -= 8;
	}
#endif
	delete [] table;
	table = 0;
}

/**
 * Copy constructor
 */
ResRelocationTable::ResRelocationTable(const ResRelocationTable &other)
{
	_size = other._size;
	if (_size)
	{
		_relocs = new ResRelocation[_size];
		std::memcpy(_relocs, other._relocs, sizeof(ResRelocation) * _size);
	} else
	{
		_relocs = 0;
	}
}

/**
 * Assigment
 */
ResRelocationTable &ResRelocationTable::operator=(const ResRelocationTable &other)
{
	delete [] _relocs;
	_size = other._size;
	if (_size)
	{
		_relocs = new ResRelocation[_size];
		std::memcpy(_relocs, other._relocs, sizeof(ResRelocation) * _size);
	} else
	{
		_relocs = 0;
	}

	return *this;
}

/**
 * Add an offset to the relocation table
 */
void ResRelocationTable::add(int offset, ResRelocation::Type type)
{
	ResRelocation *new_relocs = new ResRelocation[_size + 1];
	std::memcpy(new_relocs, _relocs, sizeof(ResRelocation) * _size);
	new_relocs[_size].offset= offset;
	new_relocs[_size].type = type;
	delete [] _relocs;
	_relocs = new_relocs;
	_size++;
}

/**
 * Erase offset from table
 */
void ResRelocationTable::erase(int offset)
{
	int index = 0;
	while (index < _size && _relocs[index].offset != offset) index++;
	if (index < _size)
	{
		if (_size == 1)
		{
			delete [] _relocs;
			_relocs = 0;
		} else
		{
			ResRelocation *new_relocs = new ResRelocation[_size -1];
			if (index > 0) std::memcpy(new_relocs, _relocs, index * sizeof(ResRelocation));
			if (index < _size - 1)
			{
				std::memcpy(new_relocs + index, _relocs + index + 1,
						(_size - index - 1) * sizeof(ResRelocation));
			}
		}
		_size--;
	}
}

/**
 * Clear table
 */
void ResRelocationTable::clear()
{
	delete [] _relocs;
	_relocs = 0;
	_size = 0;
}


/**
 * fix_pointers after string table has moved or had bytes inserted or deleted.
 */
void ResRelocationTable::fix_text_pointers(bool string_table, char *body, const char *new_strings, const char *old_strings, const char *from, int by)
{
	ResRelocation::Type fix_type = (string_table) ?
			ResRelocation::STRING_REF : ResRelocation::MESSAGE_REF;
	int base_diff = (int)(new_strings - old_strings);
	for (int j = 0; j < _size; j++)
	{
		if (_relocs[j].type == fix_type)
		{
			int *p =(int *)(body + _relocs[j].offset);
			if (*p != 0)
			{
				if (from && ((char *)(*p)) > from) (*p)+= by;
				(*p) += base_diff;
			}
		}
	}
}

/**
 * fix_pointers after string table has moved or had bytes inserted or deleted.
 */
void ResRelocationTable::fix_all_pointers(char *new_body, const char *old_body, const char *new_strings, const char *old_strings, const char *new_messages, const char *old_messages)
{
	int body_diff = (int)(new_body - old_body);
	int str_diff = (int)(new_strings - old_strings);
	int msg_diff = (int)(new_messages - old_messages);

	for (int j = 0; j < _size; j++)
	{
		switch(_relocs[j].type)
		{
		case ResRelocation::MESSAGE_REF:
			{
				int *p =(int *)(new_body + _relocs[j].offset);
				if (*p) (*p) += msg_diff;
			}
			break;
		case ResRelocation::STRING_REF:
			{
				int *p =(int *)(new_body + _relocs[j].offset);
				if (*p) (*p) += str_diff;
			}
			break;
		case  ResRelocation::OBJECT_REF:
			{
				int *p =(int *)(new_body + _relocs[j].offset);
				if (*p) (*p) += body_diff;
			}
			break;

		case ResRelocation::SPRITE_AREA_REF:
			// Nothing to do: remains unchanged
			break;
		}
	}
}

/**
 * Fix relocation table body pointers and offsets after data
 * has been inserted in the body
 */
void ResRelocationTable::fix_after_insert(char *new_body, const char *old_body, int offset, int count)
{
	int body_diff = (int)(new_body - old_body);

	for (int j = 0; j < _size; j++)
	{
		if (_relocs[j].offset >= offset) _relocs[j].offset += count;
		if (_relocs[j].type == ResRelocation::OBJECT_REF)
		{
			int *p =(int *)(new_body + _relocs[j].offset);
			if (*p) (*p) += body_diff;
		}
	}
}

/**
 * Convert all the offsets in an objects body to pointers
 */
void ResRelocationTable::offsets_to_pointers(char *body, char *strings, char *msgs) const
{
	for (int j = 0; j < _size; j++)
	{
		switch(_relocs[j].type)
		{
		case ResRelocation::MESSAGE_REF:
			{
				unsigned int *p =(unsigned int *)(body + _relocs[j].offset);
				if ((int)*p == -1) *p = 0;
				else *p = ((unsigned int)msgs) + *p;
			}
			break;
		case ResRelocation::STRING_REF:
			{
				unsigned int *p =(unsigned int *)(body + _relocs[j].offset);
				if ((int)*p == -1) *p = 0;
				else *p = ((unsigned int)strings) + *p;
			}
			break;
		case ResRelocation::OBJECT_REF:
			{
				unsigned int *p =(unsigned int *)(body + _relocs[j].offset);
				if ((int)*p == -1) *p = 0;
				else *p = ((unsigned int)body) + *p;
			}
			break;

		case ResRelocation::SPRITE_AREA_REF:
			{
				unsigned int *p =(unsigned int *)(body + _relocs[j].offset);
				if ((int)*p == -1) *p = 0;
				else *p = (unsigned int)ResObject::client_sprite_pointer();
			}
			break;
		}
	}
}

/**
 * Convert all the pointers in an objects body to offsets
 */
void ResRelocationTable::pointers_to_offsets(char *body, char *strings, char *msgs) const
{
	for (int j = 0; j < _size; j++)
	{
		switch(_relocs[j].type)
		{
		case ResRelocation::MESSAGE_REF:
			{
				unsigned int *p =(unsigned int *)(body + _relocs[j].offset);
				if (*p == 0) *p = -1;
				else *p = *p - (unsigned int)msgs;
			}
			break;
		case ResRelocation::STRING_REF:
			{
				unsigned int *p =(unsigned int *)(body + _relocs[j].offset);
				if (*p == 0) *p = -1;
				else *p = *p - (unsigned int)strings;
			}
			break;
		case ResRelocation::OBJECT_REF:
			{
				unsigned int *p =(unsigned int *)(body + _relocs[j].offset);
				if (*p) *p = *p - (unsigned int)body;
				else *p = -1;
			}
			break;
		case ResRelocation::SPRITE_AREA_REF:
			{
				unsigned int *p =(unsigned int *)(body + _relocs[j].offset);
				if (*p == 0) *p = -1;
				else *p = 0;
			}
			break;
		}
	}
}


/**
 * Construct the data handler for a Resource object.
 *
 * Takes owner ship of all pointers passed in.
 */
ResData::ResData(ResDataHeader header, char *all_strings, int num_reloc, ResRelocation *relocs)
{
	_strings = 0;
	_messages = 0;
	_strings_size = 0;
	_messages_size = 0;

	if (header.string_table_offset == -1)
	{
		if (header.messages_table_offset != -1)
		{
			_messages = all_strings;
			_messages_size = header.relocations_table_offset - header.messages_table_offset;
		}
	} else if (header.messages_table_offset == -1)
	{
		_strings = all_strings;
		_strings_size = header.relocations_table_offset - header.string_table_offset;
	} else
	{
		_messages_size = header.relocations_table_offset - header.messages_table_offset;
		_strings_size = header.messages_table_offset - header.string_table_offset;

		// Currently we just leave messages in _strings rather than reallocate
		_strings = all_strings;		
		_messages = alloc_string_table(_messages_size);
		
		std::memcpy(_messages, all_strings + _strings_size, _messages_size);
	}
	_reloc_table._size = num_reloc;
	_reloc_table._relocs = relocs;
}

/**
 * Copy constructor
 */
ResData::ResData(const ResData &other)
{
	if (other._reloc_table._size == 0)
	{
		_messages_size = _strings_size = 0;
		_strings = 0;
		_messages = 0;
	} else
	{
		_strings_size = other._strings_size;
		_messages_size = other._messages_size;
		if (_strings_size == 0) _strings = 0;
		else
		{
			_strings = alloc_string_table(_strings_size);
			std::memcpy(_strings, other._strings, _strings_size);
		}
		if (_messages_size == 0) _messages = 0;
		else
		{
			_messages = alloc_string_table(_messages_size);
			std::memcpy(_messages, other._messages, _messages_size);
		}
		_reloc_table = other._reloc_table;
	}
}

/**
 * Construct an empty resource data object.
 */
ResData::ResData()
{
	_strings = 0;
	_strings_size = 0;
	_messages = 0;
	_messages_size = 0;
}

ResData::~ResData()
{
	free_string_table(_strings);
	free_string_table(_messages);
}

/**
 * Update string and messages sizes accurately as
 * the loaded tables are padded
 */
void ResData::calculate_string_sizes(const char *body)
{
	if (_strings_size == 0 && _messages_size == 0) return;
	_strings_size = 0;
	_messages_size = 0;
	for (int idx = 0; idx < _reloc_table._size; idx++)
	{
		switch (_reloc_table._relocs[idx].type)
		{
		case ResRelocation::STRING_REF:
			_strings_size += text_len(body, _reloc_table._relocs[idx].offset);
			break;
		case ResRelocation::MESSAGE_REF:
			_messages_size += text_len(body, _reloc_table._relocs[idx].offset);
			break;
		default:
			// Other types are not strings so do nothing
			break;
		}
	}
}

/**
 * Writes data to stream.
 */
void ResData::write(std::ostream &file) const
{
	if (_strings_size)
	{
		// tables are created padded to word boundary
		int table_size= (_strings_size +3) &~3;
		file.write(_strings, table_size);
	}
	if (_messages_size)
	{
		// tables are created padded to word boundary
		int table_size= (_messages_size +3) &~3;
		file.write(_messages, table_size);
	}
	if (_reloc_table._size)
	{
		file.write((char *)&(_reloc_table._size), 4);
		file.write((char *)_reloc_table._relocs, sizeof(ResRelocation) * _reloc_table._size);
	}
}


/**
 * Returns the text length for a text pointer at the given offset.
 */
int ResData::text_len(const char *body, int offset) const
{
	int *p = (int *)(body + offset);
	if (*p == 0) return 0;
	return std::strlen((char *)*p)+1;
}

/**
 * Updates a string that is held at the specified byte offset in an object
 *
 * @param body - pointer to the base of an objects body
 * @param offset - offset into body in number of bytes
 * @param new_text - new text (or null for some fields).
 * @param new_len length of new text
 * @param is_string true if its a string, false if it's a message
 */
void ResData::text(char *body, int offset, const char *new_text, int new_len, bool is_string)
{
	const char *old_text = text(body, offset);
	int old_len = 0;
	if (old_text != 0) old_len = std::strlen(old_text) + 1;

	if (new_text == 0)
	{
		if (old_text != 0)
		{
			number(body, offset, (int)new_text);
			remove_chars(body, is_string, old_text, old_len);
		}
	} else
	{
		char *p;
		if (new_len == -1) new_len = std::strlen(new_text);
		new_len += 1; // Add space for terminating null

		if (old_len == new_len)
		{
			p = const_cast<char *>(old_text);
		} else if (old_len > new_len)
		{
			p = remove_chars(body, is_string, old_text, old_len - new_len);
		} else if (old_len == 0)
		{
			p = insert_chars(body, is_string,
					is_string ? (_strings + _strings_size) : (_messages + _messages_size),
					new_len);
			number(body, offset, (int)p);
		} else
		{
			p = insert_chars(body, is_string, old_text, new_len - old_len);
		}
		std::strcpy(p, new_text);
	}
}

/**
 * Initialise text data.
 * Usually called from a constructor
 */
void ResData::init_text(char *body, int offset, const char *value, bool is_string)
{
	int *ptext = (int *)(body + offset);

	_reloc_table.add(offset, (is_string) ? ResRelocation::STRING_REF : ResRelocation::MESSAGE_REF);

	if (value == 0)
	{
		*ptext = 0;
	} else
	{
		char *pstr = insert_chars(body, is_string,
					is_string ? (_strings + _strings_size) : (_messages + _messages_size),
					std::strlen(value)+1);
		std::strcpy(pstr, value);
		*ptext = (int)pstr;
	}	
}

void ResData::init_sprite_area_ref(char *body, int offset)
{
	_reloc_table.add(offset, ResRelocation::SPRITE_AREA_REF);
	*((int *)(body + offset)) = 0;
}

void ResData::init_object_ref(char *body, int offset)
{
	_reloc_table.add(offset, ResRelocation::OBJECT_REF);
	*((int *)(body + offset)) = 0;
}


/**
 * Remove characters from the string tables, updating the body pointers
 * if necessary.
 *
 * returns the location of the deletion in the new table
 */
char *ResData::remove_chars(char *body, bool string_table, const char *where, int num)
{
	char *old_strings = (string_table) ? _strings : _messages;
	int old_size = (string_table) ? _strings_size : _messages_size;
	int new_size = old_size - num;
	char *new_strings = 0;

	if (new_size > 0)
	{
		if ((((new_size-1) | 3) + 1) >= old_size)
		{
			// Don't need to re-allocate if size doesn't shrink to previous word
			new_strings = old_strings;
			if (where < old_strings + old_size)
			{
				std::memmove(const_cast<char *>(where),
						where + num,
						(old_size - (where - old_strings) - num));
			}			
		} else
		{
			// Tables are padded to word boundary
			new_strings = alloc_string_table(new_size);
			if (where > old_strings) std::memcpy(new_strings, old_strings, where - old_strings);
			if (where < old_strings + old_size)
			{
				std::memcpy(new_strings + (where - old_strings),
						where + num,
						(old_size - (where - old_strings) - num));
			}
		}
	}
	_reloc_table.fix_text_pointers(string_table, body, new_strings, old_strings, where, -num);

	char *new_where = new_strings + (where - old_strings);

	if (string_table)
	{
		if (new_strings != old_strings)
		{
			free_string_table(_strings);
			_strings = new_strings;
		}
		_strings_size = new_size;
	} else
	{
		if (new_strings != old_strings)
		{
			free_string_table(_messages);
			_messages = new_strings;
		}
		_messages_size = new_size;
	}

#ifdef CHECK_STRING_ALLOC
	check_string_table(new_strings);
#endif

	return new_where;
}

/**
 * Insert characters in table
 *
 * @return location characters were inserted in the new table
 */
char *ResData::insert_chars(char *body, bool string_table, const char *where, int num)
{
	char *old_strings = (string_table) ? _strings : _messages;
	int old_size = (string_table) ? _strings_size : _messages_size;
	int new_size = old_size + num;
	char *new_strings;

	if (old_size != 0 && ((old_size-1) | 3) == ((new_size -1 ) | 3))
	{
		// Don't need to allocate if size doesn't move to next word
		new_strings = old_strings;
		if (where != 0)
		{
			if (where < old_strings + old_size)
			{
				std::memmove(const_cast<char *>(where) + num,
						     where,
							 (old_size - (where - old_strings))
							 );
			}
		}
	} else
	{
		// Tables are padded to word boundary
		new_strings	= alloc_string_table(new_size);
		if (where != 0)
		{
			if (where > old_strings) std::memcpy(new_strings, old_strings, where - old_strings);
			if (where < old_strings + old_size)
			{
				std::memcpy(new_strings + (where - old_strings) + num,
						where, (old_size - (where - old_strings)));
			}
		}
	}

	_reloc_table.fix_text_pointers(string_table, body, new_strings, old_strings, where, num);

	char *new_where = new_strings + (where - old_strings);
	if (string_table)
	{
		if (new_strings != old_strings)
		{
			free_string_table(_strings);
			_strings = new_strings;
		}
		_strings_size = new_size;
	} else
	{
		if (new_strings != old_strings)
		{
			free_string_table(_messages);
			_messages = new_strings;
		}
		_messages_size = new_size;
	}
#ifdef CHECK_STRING_ALLOC
	check_string_table(new_strings);
#endif

	return new_where;
}

/**
 * Copy relocations and strings into this table updating body with new
 * string pointers.
 */
void ResData::copy_used(char *body, const char *old_body, const ResRelocationTable &copy_table)
{
	free_string_table(_strings);
	free_string_table(_messages);
	_strings_size = 0;
	_messages_size = 0;
	_reloc_table.clear();
	if (copy_table._size != 0)
	{
		int new_relocs = 0;
		int new_strings_size = 0;
		int new_messages_size = 0;
		int len;

		// Count relocs used and calculate size of string table
		for (int j = 0; j < copy_table._size; j++)
		{
			switch(copy_table._relocs[j].type)
			{
			case ResRelocation::MESSAGE_REF:
				len = text_len(body, copy_table._relocs[j].offset);
				if (len)
				{
					new_messages_size += len;
					new_relocs++;
				}
				break;
			case ResRelocation::STRING_REF:
				len = text_len(body, copy_table._relocs[j].offset);
				if (len)
				{
					new_strings_size += len;
					new_relocs++;
				}
				break;
			case ResRelocation::OBJECT_REF:
				new_relocs++;
				break;
			case ResRelocation::SPRITE_AREA_REF:
				new_relocs++;
				break;
			}
		}

		if (new_relocs)
		{
			_reloc_table._size = new_relocs;
			_reloc_table._relocs = new ResRelocation[new_relocs];
		}
		if (new_strings_size)
		{
			_strings = alloc_string_table(new_strings_size);
			_strings_size = new_strings_size;
		}

		if (new_messages_size)
		{
			_messages = alloc_string_table(new_messages_size);
			_messages_size = new_messages_size;
		}

		char *pstr = _strings;
		char *pmsg = _messages;
		int offset;
		int ridx = 0;

		for (int j = 0; j < copy_table._size; j++)
		{
			offset = copy_table._relocs[j].offset;
			switch(copy_table._relocs[j].type)
			{
			case ResRelocation::MESSAGE_REF:
				len = text_len(body, offset);
				if (len)
				{
					std::strcpy(pmsg, text(body, offset));
					number(body, offset, (int)pmsg);
					pmsg += len;
					_reloc_table._relocs[ridx].type = ResRelocation::MESSAGE_REF;
					_reloc_table._relocs[ridx].offset = offset;
					ridx++;
				}
				break;
			case ResRelocation::STRING_REF:
				len = text_len(body, offset);
				if (len)
				{
					std::strcpy(pstr, text(body, offset));
					number(body, offset, (int)pstr);
					pstr += len;
					_reloc_table._relocs[ridx].type = ResRelocation::STRING_REF;
					_reloc_table._relocs[ridx].offset = offset;
					ridx++;
				}
				break;
			case ResRelocation::OBJECT_REF:
				_reloc_table._relocs[ridx].type = ResRelocation::OBJECT_REF;
				_reloc_table._relocs[ridx].offset = offset;
				{
					int *pint = (int *)(body + offset);
					*pint = (*pint - (int)old_body) + (int)body;
				}
				ridx++;
				break;
			case ResRelocation::SPRITE_AREA_REF:
				_reloc_table._relocs[ridx].type = ResRelocation::SPRITE_AREA_REF;
				_reloc_table._relocs[ridx].offset = offset;
				ridx++;
				break;
			}
		}
	}
}

/**
 * Extract component data from a resource
 *
 * @param new_body - body of component copied
 * @param copy_body - body of ResObject copied from
 * @param offset - offset of component to copy in old_body
 * @param size - size of the component body
 */
ResData *ResData::component_data(char *new_body, char *copy_body, int offset, int size) const
{
	int end_offset = offset + size;
	ResRelocationTable comp_relocs;
	ResData *comp_data = 0;

	for (int mainRelocIdx = 0; mainRelocIdx < _reloc_table._size; mainRelocIdx++)
	{
		const ResRelocation &reloc = _reloc_table._relocs[mainRelocIdx];
		if (reloc.offset >= offset && reloc.offset < end_offset)
		{
			comp_relocs.add(reloc.offset - offset, reloc.type);
		}
	}

	if (comp_relocs._size)
	{ 
		comp_data = new ResData();
		comp_data->_reloc_table = comp_relocs;
		comp_data->copy_strings_and_messages(new_body, copy_body + offset);
	}

	return comp_data;
}

/**
 * Static function to create a copy of data from a readonly source.
 * Assumes that the readonly_header has the table pointers before it in memory
 */
ResData *ResData::copy_from_read_only(char *new_header, char *readonly_header)
{
	int reloc_offset = *(((int *)readonly_header)-1);
	if (reloc_offset == -1) return 0; // No relocations
    int *p = (int *)(readonly_header + reloc_offset);
	int num_relocs = *p;
	if (num_relocs == 0) return 0; // No relocations
	ResRelocation *rp = (ResRelocation *)(p+1);
	ResData *data = new ResData();
	data->_reloc_table._size = num_relocs;
	data->_reloc_table._relocs = new ResRelocation[num_relocs];
	memcpy(data->_reloc_table._relocs, rp, sizeof(ResRelocation) * num_relocs);
	data->copy_strings_and_messages(((ResObjectHeader *)new_header)->body, ((ResObjectHeader *)readonly_header)->body);

	return data;
}

/**
 * Copy data to a new component from a read only object
 *
 * @param new_body - component body for data
 * @param readonly_header - ResObject header to copy from
 * @param offset - offset of component in readonly_header
 * @param size of component
 */
ResData *ResData::copy_component_from_read_only(char *new_body, char *readonly_header, int offset, int size)
{
	int reloc_offset = *(((int *)readonly_header)-1);
	if (reloc_offset == -1) return 0; // No relocations
    int *p = (int *)(readonly_header + reloc_offset - 12);
	int num_relocs = *p;
	if (num_relocs == 0) return 0; // No relocations
	ResRelocation *rp = (ResRelocation *)(p+1);

	int end_offset = offset + size;
	ResRelocationTable comp_relocs;
	ResData *comp_data = 0;

	for (int mainRelocIdx = 0; mainRelocIdx < num_relocs; mainRelocIdx++)
	{
		const ResRelocation &reloc = rp[mainRelocIdx];
		if (reloc.offset >= offset && reloc.offset < end_offset)
		{
			comp_relocs.add(reloc.offset - offset, reloc.type);
		}
	}

	if (comp_relocs._size)
	{ 
		comp_data = new ResData();
		comp_data->_reloc_table = comp_relocs;
		comp_data->copy_strings_and_messages(new_body, ((ResObjectHeader *)readonly_header)->body + offset);
	}

	return comp_data;
}

/**
 * Copy the strings and messages from the old body to the new body
 * Note: the data's relocation table must be set up first to find the strings and messages
 */
void ResData::copy_strings_and_messages(char *new_body, const char *copy_body)
{
	int relocIdx;
	_strings_size = 0;
	_messages_size = 0;

	for (relocIdx = 0; relocIdx < _reloc_table._size; relocIdx++)
	{
		const ResRelocation &reloc = _reloc_table._relocs[relocIdx];
		switch(reloc.type)
		{
		case ResRelocation::STRING_REF:
			_strings_size += text_len(copy_body, reloc.offset);
			break;
		case ResRelocation::MESSAGE_REF:
			_messages_size += text_len(copy_body, reloc.offset);
			break;
		default:
			// Other types are not strings so do nothing
			break;
		}
	}


	if (_strings_size) _strings = alloc_string_table(_strings_size);
	if (_messages_size) _messages = alloc_string_table(_messages_size);
	char *pstr = _strings;
	char *pmsg = _messages;
	char **pcopy;
	char **pto;

	for (relocIdx = 0; relocIdx < _reloc_table._size; relocIdx++)
	{
		ResRelocation &reloc = _reloc_table._relocs[relocIdx];
		switch(reloc.type)
		{
		case ResRelocation::STRING_REF:
			pcopy = (char **)(copy_body + reloc.offset);
			pto = (char **)(new_body + reloc.offset);
			if (*pcopy)
			{
				*pto = pstr;
				strcpy(pstr, *pcopy);
				pstr += strlen(pstr)+1;
			} else
			{
				*pto = 0;
			}		
			break;

		case ResRelocation::MESSAGE_REF:
			pcopy = (char **)(copy_body + reloc.offset);
			pto = (char **)(new_body + reloc.offset);
			if (*pcopy)
			{
				*pto = pmsg;
				strcpy(pmsg, *pcopy);
				pmsg += strlen(pmsg)+1;
			} else
			{
				*pto = 0;
			}		
			break;

		default:
			// Other types are not strings so do nothing
			break;
		}
	}
}

/**
 * Add a copy of data from another object
 *
 * @param body body pointer for this object
 * @param offset into body of item copied
 * @param from_body body pointer of object to copy from
 * @param from_data data of other object
 */
void ResData::add_data_from(char *body, int offset, const char *from_body, const ResData *from_data)
{
	if (from_data == 0) return;

	for (int relocIdx = 0; relocIdx < from_data->_reloc_table._size; relocIdx++)
	{
		ResRelocation &from_reloc = from_data->_reloc_table._relocs[relocIdx];
		ResRelocation new_reloc(from_reloc);
		new_reloc.offset += offset;
		switch(from_reloc.type)
		{
		case ResRelocation::STRING_REF:
			init_text(body, new_reloc.offset, from_data->text(from_body, from_reloc.offset), true);
			break;
		case ResRelocation::MESSAGE_REF:
			init_text(body, new_reloc.offset, from_data->text(from_body, from_reloc.offset), false);
			break;
		case ResRelocation::OBJECT_REF:
			{
				int ref = from_data->number(from_body, from_reloc.offset);
				if (ref) ref += offset + (body - from_body);
				number(body, new_reloc.offset, ref);
				_reloc_table.add(new_reloc.offset, new_reloc.type);
			}
			break;
		case ResRelocation::SPRITE_AREA_REF:
			number(body, new_reloc.offset, from_data->number(from_body, from_reloc.offset));
			_reloc_table.add(new_reloc.offset, new_reloc.type);
			break;
		}
	}
}

/**
 * Fix offset pointers and object refs when bytes
 * have been added or removed from the body.
 */
void ResData::fix_offsets(char *new_body, const char *old_body, int offset, int diff)
{
	for (int relocIdx = 0; relocIdx < _reloc_table._size; relocIdx++)
	{
		ResRelocation &reloc = _reloc_table._relocs[relocIdx];
		if (reloc.type == ResRelocation::OBJECT_REF)
		{
			if (reloc.offset >= offset) reloc.offset += diff;
			int ref = number(old_body, reloc.offset);
			if (ref)
			{
				if ((char *)ref - old_body >= offset) ref += diff;
				ref += new_body-old_body;
			}
			number(new_body, reloc.offset, ref);
		} else if (reloc.offset >= offset) reloc.offset += diff;
	}
}

/**
 * Remove relocation refering in a given range and
 * the strings they refer to
 */
void ResData::remove_data(char *body, int offset, int size)
{
	int end_offset = offset + size;
	int deleted_count = 0;
	int num_relocs = _reloc_table._size;
	ResRelocation *reloc = _reloc_table._relocs;
	ResRelocation *copy_from = reloc;
	while (num_relocs--)
	{
		if (copy_from > reloc) *reloc = *copy_from;
		if (reloc->offset >= offset && reloc->offset < end_offset)
		{
			switch(reloc->type)
			{
			case ResRelocation::STRING_REF:
				{
					const char *old_text = text(body, offset);
					if (old_text != 0)
					{
						remove_chars(body, true, old_text, std::strlen(old_text) + 1);
					}
				}
				break;

			case ResRelocation::MESSAGE_REF:
				{
					const char *old_text = text(body, offset);
					if (old_text != 0)
					{
						remove_chars(body, false, old_text, std::strlen(old_text) + 1);
					}
				}
				break;
			default:
				// Other types are not strings so do nothing
				break;
			}
			deleted_count++;
		} else
		{
			reloc++;
		}
		copy_from++;
	}

	if (deleted_count)
	{
		_reloc_table._size -= deleted_count;
		if (deleted_count > 1)
		{
			// Only bother to realloc if with deleted more than one string
			if (_reloc_table._size)
			{
				ResRelocation *new_relocs = new ResRelocation[_reloc_table._size];
				std::memcpy(new_relocs, _reloc_table._relocs, _reloc_table._size * sizeof(ResRelocation));
				delete [] _reloc_table._relocs;
				_reloc_table._relocs = new_relocs;
			} else
			{
				delete [] _reloc_table._relocs;
				_reloc_table._relocs = 0;
			}
		}
	}
}

/**
 * Construct an implementation as a pointer to the given object
 *
 * object_header is not owned by this object so should remain accessible
 * until the implementation object is deleted
 */
ResImpl::ResImpl(void *object_header, int body_offset, int size) :
		_ref_count(-1),
		_header((char *)object_header),
		_body(_header + body_offset),
		_size(size),
		_data(0),
		_type_reloc_table(0)
{
}

/**
 * Construct implementation from an object and its data
 *
 * Takes ownership of the passed data
 */
ResImpl::ResImpl(void *object_header, int body_offset, int size, ResData *data) :
		_ref_count(1),
		_header((char *)object_header),
		_body(_header + body_offset),
		_size(size),
		_data(data),
		_type_reloc_table(0)
{

}

/**
 * Make copy of other implementation
 */
ResImpl::ResImpl(const ResImpl &other)
{
	if (other._ref_count == -1)
	{
		_ref_count = -1;
		_header = other._header;
		_body = other._body;
		_size = other._size;
		_data = 0;
	} else
	{
		_ref_count = 1;
		_header = new char[other._size];
		_size = other._size;
		std::memcpy(_header, other._header, other._size);
		_body = _header + (other._body - other._header);
		if (other._data)
		{
			_data = new ResData(*(other._data));
			_data->_reloc_table.fix_all_pointers(_body, other._body,
						_data->_strings, other._data->_strings,
						_data->_messages, other._data->_messages);
		}
		else _data = 0;
	}
	_type_reloc_table = other._type_reloc_table;
}

/**
 * Make copy of other implementation
 *
 * @param other Other item to copy
 * @param make a copy of a read-only item. Requires other to have a type_reloc_table
 * @param copy_read_only - make a copy of read-only ResObjects as well if possible
 * @throws std::invalid_argument if other doesn't have a type_reloc_table and
 * is read only
 */
ResImpl::ResImpl(const ResImpl &other, bool copy_read_only)
{
	if (other._ref_count == -1 && copy_read_only == false)
	{
		_ref_count = -1;
		_header = other._header;
		_body = other._body;
		_size = other._size;
		_data = 0;
	} else
	{
		_ref_count = 1;
		_header = new char[other._size];
		_size = other._size;
		std::memcpy(_header, other._header, other._size);
		_body = _header + (other._body - other._header);
		if (other._data)
		{
			_data = new ResData(*(other._data));
			_data->_reloc_table.fix_all_pointers(_body, other._body,
					_data->_strings, other._data->_strings,
					_data->_messages, other._data->_messages);

		} else if (other._ref_count == -1)
		{
			// If the header and body match this is a gadget or menuitem
			// and we should never get here.
			if (_header == _body) throw std::invalid_argument("Unable to copy read-only components");
			_data = ResData::copy_from_read_only(_header, other._header);

			// Copy read only body using type_reloc_table
			if (other._type_reloc_table == 0)
			{
				std::string msg("Unable to make ResObject ");
				// Can only do this if we have a type_reloc_table
				delete [] _header;
				_header = 0;
				msg += " Writeable. It is read-only without a type relocation table.";
				throw std::invalid_argument(msg);
			} else
			{
				_data = new ResData();
				_data->copy_used(_body, other._body, *(other._type_reloc_table));
			}
		} else _data = 0;
	}
	_type_reloc_table = other._type_reloc_table;
}

/**
 * Destructor deletes non read-only resources
 */
ResImpl::~ResImpl()
{
	if (_ref_count != -1) delete [] _header;
	delete _data;
}

/**
 * Add reference
 */
void ResImpl::add_ref()
{
	if (_ref_count == -1) throw std::invalid_argument("ResImpl: Adding reference to a read_only object");
	_ref_count++;
}

/**
 * Release object, deleting if no longer used by anything
 */
void ResImpl::release()
{
	if (_ref_count == -1 || --_ref_count == 0) delete this;
}

/**
 * Get a number from the specified offset
 */
int ResImpl::int_value(int offset) const
{
	return *((int *)(_body + offset));
}

/**
 * Set a number at the specified offset
 */
void ResImpl::int_value(int offset, int value)
{
	*((int *)(_body + offset)) = value;
}

/**
 * Get a number from the specified offset
 */
unsigned int ResImpl::uint_value(int offset) const
{
	return *((unsigned int *)(_body + offset));
}

/**
 * Set a number at the specified offset
 */
void ResImpl::uint_value(int offset, unsigned int value)
{
	*((unsigned int *)(_body + offset)) = value;
}

/**
 * Get a single byte at the specified offset
 */
unsigned char ResImpl::byte_value(int offset) const
{
	return (unsigned char)_body[offset];
}

/**
 * Set a single byte value
 */
void ResImpl::byte_value(int offset, unsigned char value)
{
	_body[offset] = (char)value;
}

/**
 * Get an unsigned short (2 byte number) from the specified offset
 */
unsigned short ResImpl::ushort_value(int offset) const
{
	return *((unsigned short *)(_body + offset));
}

/**
 * Set an unsigned short (2 byte number) at the specified offset
 */
void ResImpl::ushort_value(int offset, unsigned short value)
{
	*((unsigned short *)(_body + offset)) = value;
}


/**
 * Return true if any of the bits in mask are set
 */
bool ResImpl::flag(int offset, int mask) const
{
	return (*((unsigned int *)(_body + offset)) & mask) != 0;
}

/**
 * Set/clear the bits in mask
 */
void ResImpl::flag(int offset, int mask, bool on)
{
	unsigned int *p = ((unsigned int *)(_body + offset));
	if (on) *p |= mask;
	else *p &= ~mask;
}

int ResImpl::flag_value(int offset, int mask) const
{
	return (*((unsigned int *)(_body + offset)) & mask);
}

void ResImpl::flag_value(int offset, int mask, int value)
{
	unsigned int *p = ((unsigned int *)(_body + offset));
	*p = (*p & ~mask) | value;
}

/**
 * Set text where following word is maximum buffer size
 */
void ResImpl::text_with_length(int offset, const char *value, int max_length, bool is_string)
{
	int text_len = (value) ? std::strlen(value) : 0;
	text(offset, value, text_len, is_string);
	if (value) text_len++;
	if (max_length == -1) max_length = int_value(offset+4);
	if (max_length < text_len) max_length = text_len;
	int_value(offset+4, max_length);
}

/**
 * Initialise text from a constructor
 */
void ResImpl::init_text(int offset, const char *value, bool is_string)
{
	if (_data == 0) _data = new ResData();
	_data->init_text(_body, offset, value, is_string);
}

void ResImpl::init_sprite_area_ref(int offset)
{
	if (_data == 0) _data = new ResData();
	_data->init_sprite_area_ref(_body, offset);
}

void ResImpl::init_object_ref(int offset)
{
	if (_data == 0) _data = new ResData();
	_data->init_object_ref(_body, offset);
}

/**
 * Extract a copy of a components data from this object
 *
 * @param new_body - new component body to copy relocations to
 * @param offset - offset to start of components in body
 * @param size - size of component
 */
ResData *ResImpl::component_data(char *new_body, int offset, int size) const
{
	ResData *comp_data = 0;

	if (_ref_count == -1) throw std::invalid_argument("ResImpl: TODO: extract component data from a read_only object");

	if (_data != 0) comp_data = _data->component_data(new_body, _body, offset, size);

	return comp_data;
}

/**
 * Insert a copy of the given implementation into this one.
 *
 * @param offset relative to body for insert. Use current body size to insert at end.
 * @param insert_impl pointer to implementation to insert
 * @throws std::range_error if offset is outside of object
 */
void ResImpl::insert(int offset, const ResImpl *insert_impl)
{
	int body_offset = _body - _header;
	int body_size = _size - body_offset;
	if (offset < 0 || offset > body_size) throw std::range_error("Offset outside of body in ResImpl");

	int inserted_count = insert_impl->_size;
	char *new_data = new char[_size + inserted_count];
	if (body_offset) memcpy(new_data, _header, body_offset);
	if (offset > 0) memcpy(new_data + body_offset, _body, offset);
	if (offset < body_size)
	{
		memcpy(new_data + body_offset + offset + inserted_count, 
			   _body + offset, body_size - offset);
	}
	_data->_reloc_table.fix_after_insert(new_data + body_offset, _body, offset, inserted_count);

	delete [] _header;
	_header = new_data;
	_body = new_data + body_offset;
	_size += inserted_count;

	// Straight copy of main data
	memcpy(_body + offset, insert_impl->header(), inserted_count);
	_data->add_data_from(_body, offset, insert_impl->body(), insert_impl->data());
}

/**
 * Replace a component
 */
void ResImpl::replace(int offset, int old_size, const ResImpl *rep_impl)
{
	int body_offset = _body - _header;
	int body_size = _size - body_offset;
	if (offset < 0 || offset + old_size > body_size) throw std::range_error("Offset or offset + old size outside of body in ResImpl");

	// TODO: We could probably make it more efficient if relocation tables are
	// same and possibly reduce string allocation/reallocation
	_data->remove_data(_body, offset, old_size);

	int size_diff = rep_impl->size() != old_size;
	if (size_diff != 0)
	{
		char *new_data = new char[_size + size_diff];
		std::memcpy(new_data, _header, body_offset + offset);
		std::memmove(new_data + body_offset + offset + rep_impl->size(),
			        _header + body_offset + offset + old_size,
					_size - body_offset - offset - old_size);
		_data->fix_offsets(new_data + body_offset, _body, offset, size_diff);
		delete [] _header;
		_header = new_data;
		_body = new_data + body_offset;
		_size += size_diff;
	}
	std::memcpy(_body + offset, rep_impl->header(), rep_impl->size());

	_data->add_data_from(_body, offset, rep_impl->body(), rep_impl->data());
}


/**
 * Remove bytes from body and delete strings and relocations
 */
void ResImpl::erase(int offset, int size)
{
	int body_offset = _body - _header;
	int body_size = _size - body_offset;
	if (offset < 0 || offset + size > body_size) throw std::range_error("Offset or offset + size outside of body in ResImpl");

	_data->remove_data(_body, offset, size);

	char *new_data = new char[_size - size];
	memcpy(new_data, _header, body_offset + offset);
	memcpy(new_data + offset, _body + offset + size, _size - body_offset - offset - size);
	delete [] _header;
	_header = new_data;
	_body = new_data + body_offset;
}

//! @endcond

/**
 * Copy constructor
 *
 * @param other ResBase to copy
 */
ResBase::ResBase(const ResBase &other)
{
   if (other._impl->read_only())
   {
	   _impl = new ResImpl(*(other._impl));
   } else
   {
	   _impl = other._impl;
	   _impl->add_ref();
   }
}

/**
 * Destructor
 */
ResBase::~ResBase()
{
	if (_impl) _impl->release();
}

/**
 * Assignment
 *
 * @param other ResBase to copy
 */
ResBase &ResBase::operator=(const ResBase &other)
{
	ResImpl *impl = _impl;
	if (other._impl->read_only())
	{
		_impl = new ResImpl(*(other._impl));
	} else
	{
		_impl = other._impl;
		_impl->add_ref();
	}
	impl->release();

	return *this;
}

/**
 * Return a string at the given offset
 *
 * @param offset offset of pointer to string in object
 */
const char *ResBase::string(int offset) const
{
	return _impl->text(offset);
}

/**
 * Set a string at the given offset
 *
 * @param offset offset of string in the object
 * @param value new value for the string
 */
void ResBase::string(int offset, const char *value)
{
	make_writeable();
	_impl->text(offset, value, ((value == 0) ? 0 : std::strlen(value)), true);
}
/**
 * Set a string at the given offset

 * @param offset offset of string in the object
 * @param value new value for the string
 */
void ResBase::string(int offset, std::string value)
{
	make_writeable();
	_impl->text(offset, value, true);
}

/**
 * Assign a string where the length is at offset+4
 *
 * @param offset - offset into body of string
 * @param value - new value for the string
 * @param length - new length or -1 to use existing length.
 * The length is always adjusted so it is big enough for the value.
 */
void ResBase::string_with_length(int offset, const char *value, int length /*= -1*/)
{
	make_writeable();
	_impl->text_with_length(offset, value, length, true);
}

/**
 * Get the message string at the given offset
 *
 * @param offset offset of message in the object
 */
const char *ResBase::message(int offset) const
{
	return _impl->text(offset);
}

/**
 * Set a message at the given offset
 *
 * @param offset offset of message in the object
 * @param value new value for the message
 */
void ResBase::message(int offset, const char *value)
{
	make_writeable();
	_impl->text(offset, value, ((value == 0) ? 0 : std::strlen(value)), false);
}

/**
 * Set a message at the given offset
 *
 * @param offset offset of message in the object
 * @param value new value for the message
 */
void ResBase::message(int offset, std::string value)
{
	make_writeable();
	_impl->text(offset, value, false);
}

/**
 * Assign a message where the length is at offset+4
 *
 * @param offset offset into body of message
 * @param value new value for the message
 * @param length new length or -1 to use existing length.
 * The length is always adjusted so it is big enough for the value.
 */
void ResBase::message_with_length(int offset, const char *value, int length/* = -1*/)
{
	make_writeable();
	_impl->text_with_length(offset, value, length, false);
}

/**
 * Set a number at the specified offset
 *
 * @param offset offset of the number in the object
 * @param value new value
 */
void ResBase::int_value(int offset, int value)
{
	make_writeable();
	_impl->int_value(offset, value);
}

/**
 * Set an unsigned number at the specified offset
 *
 * @param offset offset of the number in the object
 * @param value new value
 */
void ResBase::uint_value(int offset, unsigned int value)
{
	make_writeable();
	_impl->uint_value(offset, value);
}

/**
 * Set an unsigned byte (8 bit value) at the specified offset
 *
 * @param offset offset of the value in the object
 * @param value new value
 */
void ResBase::byte_value(int offset, unsigned char value)
{
	make_writeable();
	_impl->byte_value(offset, value);
}

/**
 * Set an unsigned short (16 bit value) at the specified offset
 *
 * @param offset offset of the value in the object
 * @param value new value
 */
void ResBase::ushort_value(int offset, unsigned short value)
{
	make_writeable();
	_impl->ushort_value(offset, value);
}

/**
 * Set/clear the bits in mask
 *
 * @param offset offset of the value to change
 * @param mask bits to set on or off
 * @param on true to set mask bits to 1, false to set them to 0
 */
void ResBase::flag(int offset, int mask, bool on)
{
	make_writeable();
	_impl->flag(offset, mask, on);
}

/**
 * Set bits in given mask clearing others
 *
 * @param offset of value to change
 * @param mask bits to change
 * @param value new value for bits in mask
 */
void ResBase::flag_value(int offset, int mask, int value)
{
	make_writeable();
	_impl->flag_value(offset, mask, value);
}

/**
 * Initialise a string in a constructor helper
 *
 * @param offset offset of string in the object
 * @param value initial value for the string
 */
void ResBase::init_string(int offset, const char *value)
{
	_impl->init_text(offset, value, true);
}

/**
 * Initialise a message in a constructor helper
 *
 * @param offset offset of the message in the object
 * @param value initial value for the message
 */
void ResBase::init_message(int offset, const char *value)
{
	_impl->init_text(offset, value, false);
}

/**
 * Initialise a sprite area reference
 *
 * @param offset offset of the sprite area reference in the object
 */
void ResBase::init_sprite_area_ref(int offset)
{
	_impl->init_sprite_area_ref(offset);
}

/**
 * Initialise an object reference
 *
 * @param offset offset of the object reference in the object
 */
void ResBase::init_object_ref(int offset)
{
	_impl->init_object_ref(offset);
}

}
}
