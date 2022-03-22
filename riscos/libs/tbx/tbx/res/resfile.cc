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

#include "resfile.h"
#include <fstream>
#include "resexcept.h"

using namespace std;

namespace tbx
{
namespace res
{

ResFile::ResFile(void)
{
	_res = 0;
	_length = 0;
}

ResFile::~ResFile(void)
{
	delete [] _res;
}

/**
 * Load a resource file
 */
bool ResFile::load(const std::string &fname)
{
	ifstream res_file(fname.c_str(), ios::binary);
	if (!res_file) return false;

	res_file.seekg (0, ios::end);
	_length = res_file.tellg();
	res_file.seekg (0, ios::beg);
	if (_length == 0) return false;

	_res = new char[_length];
	res_file.read(_res, _length);

	char *p = _res;
	char *end = _res + _length;
	ResFileHeader *rh = (ResFileHeader *)p;

	if (_length < 12 || rh->file_id != RESF_MARKER) return false;

	p += rh->object_offset;
	while (p < end)
	{
		ResDataHeader *rdh = (ResDataHeader *)p;
		ResObjectHeader *obj = (ResObjectHeader *)(p + 12);
		obj->body += (int)obj;

		if (rdh->relocations_table_offset != -1)
		{
			p += rdh->relocations_table_offset;
			int num_relocs = *((int *)p);
			p += 4;
			ResRelocation *reloc = (ResRelocation *)p;
			for (int j = 0; j < num_relocs; j++)
			{
				int *op = (int *)(obj->body + reloc->offset);
				switch(reloc->type)
				{
				case ResRelocation::STRING_REF:
					{
						char *sp = ((char *)rdh) + rdh->string_table_offset;
						if (*op == -1) *op = 0;
						else *op += (int)sp;
					}
					break;
				case ResRelocation::MESSAGE_REF:
					{
						char *sp = ((char *)rdh) + rdh->messages_table_offset;
						if (*op == -1) *op = 0;
						else *op += (int)sp;
					}
					break;

				case ResRelocation::OBJECT_REF:
					*op += (int)obj->body;
					break;

				case ResRelocation::SPRITE_AREA_REF:
					if (*op == -1) *op = 0;
					else *op = (int)ResObject::client_sprite_pointer();
					break;
				}
				reloc++;
			}

			p +=  num_relocs * 8;
		} else
		{
			p += 12 + obj->total_size;
		}
	}

	return res_file.good();
}


/**
 * Find name in resource
 */
ResFile::const_iterator ResFile::find(std::string name) const
{
	const_iterator i(begin());
	for (; i != end(); ++i)
	{
		if (name == (_res + i._offset + 24)) break;
	}
	return i;
}

/**
 * Returns true if the file contains the named object
 */
bool ResFile::contains(std::string name) const
{
	return (find(name) != end());
}

/**
 * Get resource object with given name.
 *
 * @throws ResObjectNotFound if object does not exist
 */
ResObject ResFile::object(std::string name) const
{
	const_iterator i = find(name);
	if (i == end()) throw ResObjectNotFound(name);
	return *i;
}

/**
 * Return offset to first object in file
 */
int ResFile::first_offset() const
{
	int first = ((ResFileHeader *)_res)->object_offset;
	if (first == -1) first = _length;

	return first;
}

/**
 * Offset of object after last object
 */
int ResFile::end_offset() const
{
	return _length;
}

/**
 * Move to next object
 */
void ResFile::next_object(int &offset) const
{
	char *p = _res + offset;
	ResDataHeader *rdh = (ResDataHeader *)p;

	if (rdh->relocations_table_offset != -1)
	{
		p += rdh->relocations_table_offset;
		int num_relocs = *((int *)p);
		p += 4 + num_relocs * 8;
	} else
	{
		p += 12; // Skip over data header
		ResObjectHeader *obj = (ResObjectHeader *)p;
		p += obj->total_size;
	}

	offset = p - _res;
}
	
/**
 * Return object at given offset
 */
ResObject ResFile::at_offset(int offset) const
{
	return ResObject((ResObjectHeader *)(_res + offset + 12));
}

}
}
