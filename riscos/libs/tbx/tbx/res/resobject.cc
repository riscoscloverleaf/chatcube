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
#include "resexcept.h"
#include "../application.h"
#include "../sprite.h"

namespace tbx {
namespace res {

OsSpriteAreaPtr ResObject::_sprite_area_ptr = 0;

/**
* Construct from raw object header
*
* The object owns the passed pointers and will delete them
* when it is deleted.
*
* @param header - pointer to object header
* @param data - pointer to resource data (or 0 if no data)
*/
ResObject::ResObject(ResObjectHeader *object_header, ResData *data)
 : ResBase(new ResImpl(object_header,
		              object_header->body - (char *)object_header,
					  (object_header->body - (char *)object_header) + object_header->body_size,
					  data))
{
}

/**
 * Construct from pointer to an object header.
 *
 * In this case the object header is not owned by the ResObject so it
 * should be ensured it is not deleted before the ResObject is.
 *
 * This constructor is normally only called from tbx::Application
 */
ResObject::ResObject(ResObjectHeader *object_header)
  : ResBase(new ResImpl(object_header,
          object_header->body - (char *)object_header,
		  (object_header->body - (char *)object_header) + object_header->body_size))
{
}


/**
 * Protected constructor for creating a subclass
 */
ResObject::ResObject(std::string name, int class_id, int version, int object_size)
: ResBase(new ResImpl(new char[object_size],
		              sizeof(ResObjectHeader),
					  object_size, 0))
{
	ResObjectHeader *hdr = reinterpret_cast<ResObjectHeader *>(_impl->header());	
	memset(hdr, 0, object_size);
	int body_offset = sizeof(ResObjectHeader);
	hdr->body = _impl->body();
	hdr->body_size = object_size - body_offset;
	hdr->total_size = object_size;
	hdr->class_id = class_id;
	hdr->version = version;
	name.copy(hdr->name, 12);
}

/**
 * Copy constructor
 */
ResObject::ResObject(const ResObject &other) : ResBase(other)
{
   if (other._impl->read_only())
   {
		object_header()->body = _impl->body();
   }
}

/**
 * Destructor
 */
ResObject::~ResObject()
{
}

/**
 * Assignment
 */
ResObject &ResObject::operator=(const ResObject &other)
{
	ResBase::operator=(other);
	if (other._impl->read_only())
	{
		object_header()->body = _impl->body();
	}

	return *this;
}

/**
 * Get pointer to client sprite area.
 *
 * This is used for new and loaded objects
 */
OsSpriteAreaPtr ResObject::client_sprite_pointer()
{
	if (_sprite_area_ptr) return _sprite_area_ptr;
	if (tbx::app() && tbx::app()->sprite_area()) return tbx::app()->sprite_area()->pointer();
	else return (OsSpriteAreaPtr)1;
}

/**
 * Set Sprite area to use for new and loaded objects
 */
void ResObject::client_sprite_pointer(OsSpriteAreaPtr ptr)
{
	_sprite_area_ptr = ptr;
}

/**
 * Load a resource object from a stream
 *
 * @param file binary stream to read the object from
 * @returns pointer to loaded object or 0 if load failed
 */
ResObject *ResObject::load(std::istream &file)
{
	ResDataHeader data_header;
	ResObjectHeader object_header;

	file.read((char *)&data_header, sizeof(ResDataHeader));
	if (file.fail()) return 0;
	file.read((char *)&object_header, sizeof(ResObjectHeader));
	if (file.fail()) return 0;

	// Read object body
	char *data = new char[(int)object_header.body + object_header.body_size];
	std::memcpy(data, &object_header, sizeof(ResObjectHeader));
	file.read(data + sizeof(ResObjectHeader), (int)object_header.body + object_header.body_size - sizeof(ResObjectHeader));

	// Make body offset to real pointer
	reinterpret_cast<ResObjectHeader *>(data)->body += (int)data;

	ResData *res_data = 0;

	if (data_header.relocations_table_offset != -1)
	{
		int strings_size = object_header.total_size - (int)object_header.body - object_header.body_size;

		// Read in strings
		char *strings = 0;
		if (strings_size)
		{
			strings = new char[strings_size];
			file.read(strings, strings_size);
		}

		int numRelocs = 0;
		ResRelocation *relocs = 0;
		file.read((char *)&numRelocs,4);

		if (file)
		{
			relocs = new ResRelocation[numRelocs];
			file.read((char *)relocs, numRelocs * sizeof(ResRelocation));
		}
		if (file)
		{
			// ResData owns strings and relocs after the following line
			res_data = new ResData(data_header, strings, numRelocs, relocs);
		} else
		{
			delete [] data;
			delete [] strings;
			delete [] relocs;
			return 0;
		}
	}

	ResObject *obj = new ResObject(reinterpret_cast<ResObjectHeader *>(data), res_data);
	if (res_data)
	{
		res_data->offsets_to_pointers(obj->object_header()->body);
		res_data->calculate_string_sizes(obj->object_header()->body);
	}

	return obj;
}

/**
 * Save a resource object to a stream
 *
 * @param file binary stream to save object to
 * @returns true if save successful
 */
bool ResObject::save(std::ostream &file)
{
	ResDataHeader data_header;
	const ResData *data = _impl->data();
	int body_offset = object_header()->body - _impl->header();
	data_header.messages_table_offset = -1;
	data_header.string_table_offset = -1;
	if (data == 0 || data->reloc_size() == 0)
	{
		data_header.relocations_table_offset = -1;
	} else
	{
		int table_pos = body_offset + object_header()->body_size + 12;
		if (data->strings_size())
		{
			data_header.string_table_offset = table_pos;
			table_pos += (data->strings_size() + 3) & ~3;
		}
		if (data->messages_size())
		{
			data_header.messages_table_offset = table_pos;
			table_pos += (data->messages_size() + 3) & ~3;
		}

		data_header.relocations_table_offset = table_pos;
		object_header()->total_size = table_pos - 12;

		// Change to offsets for save
		data->pointers_to_offsets(object_header()->body);
	}
	// Resource header
	file.write((char *)&data_header, sizeof(ResDataHeader));

	// Object header
	object_header()->body = (char *)body_offset; // Make offset for save
	file.write(_impl->header(), body_offset + object_header()->body_size);
	object_header()->body = _impl->header() + body_offset; // Restore body pointer

	// Object data tables
	if (data)
	{
		data->write(file);
		if (data->reloc_size())
		{
			// Put offsets back to pointers
			data->offsets_to_pointers(object_header()->body);
		}
	}

	return file.good();
}

/**
 * Make this ResObject writeable.
 *
 * @throws std::invalid_argument if object is read only without a type relocation table.
 */
void ResObject::make_writeable()
{
	if (_impl->unique()) return; // Nothing to do

	ResImpl *impl = _impl;
	_impl = new ResImpl(*impl, true); // Make a copy
	object_header()->body = _impl->body();
	impl->release();
}

/**
 * Set the name of the object
 */
void ResObject::name(std::string name)
{
	make_writeable();
	name.copy(object_header()->name, 12);
	int size = name.size();
	if (size > 11) size = 11;
	object_header()->name[size] = 0;
}

/**
 * Set version for object
 */
void ResObject::object_version(int new_version)
{
	make_writeable();
	object_header()->version = new_version;
}

/**
 * Set the object flags for this object
 *
 * You can use the create_on_load etc methods to change/interrogate
 * these flags in a clearer way.
 *
 * @param flags new value for the object flags
 */
void ResObject::object_flags(unsigned int flags)
{
	make_writeable();
	object_header()->flags = flags;
}

/**
 * Set if the object should be automatically created when the Resource file is loaded
 *
 * @param value set to true if the object should be created when loaded
 */
void ResObject::create_on_load(bool value)
{
	make_writeable();
	if (value) object_header()->flags |= 1;
	else object_header()->flags &= ~1;
}

/**
 * Set if the object to be automatically shown when it is created.
 *
 * @param value true to set object to be shown when created
 */
void ResObject::show_on_create(bool value)
{
	make_writeable();
	if (value) object_header()->flags |= 2;
	else  object_header()->flags &= ~2;
}

/**
 * Set if the object is to be a shared object
 *
 * An object that is shared will only create one Toolbox object that
 * will be reference counted whenever it is created rather than a
 * new object every time it is created.
 *
 * The Toolbox object will not be deleted until every call to create it
 * has been match by a call to destroy it.
 *
 * @param value true to make this a shared object
 */
void ResObject::shared(bool value)
{
	make_writeable();
	if (value) object_header()->flags |= 4;
	else  object_header()->flags &= ~4;
}

/**
 * Set if the object should be an ancestor object
 *
 * An ancestor object will have its object/component placed in
 * the event ID block of any event raised on any of its
 * descendents
 *
 * @param value true to make this an ancestor object
 */
void ResObject::ancestor(bool value)
{
	make_writeable();
	if (value) object_header()->flags |= 8;
	else  object_header()->flags &= ~8;
}

/**
 * Check if this objects class id is as specified.
 *
 * @param class_id the toolbox class id to check against
 * @throws ResObjectClassMismatch if the class id does not match this objects class id
 */
void ResObject::check_class_id(int class_id) const
{
	if (object_header()->class_id != class_id)
	{
		throw ResObjectClassMismatch(name(), object_header()->class_id, class_id);
	}
}

/**
 * Insert a component in an object
 *
 * @param offset offset to insert the component
 * @param comp_impl private component implementation handler
 */
void ResObject::insert_component(int offset, const ResImpl *comp_impl)
{
	_impl->insert(offset, comp_impl);
	object_header()->body = _impl->body();
	object_header()->body_size += comp_impl->size();
}

/**
 * Replace a component
 *
 * @param offset location to replace
 * @param comp_impl replacement component
 * @param old_size size of component replace
 */
void ResObject::replace_component(int offset, int old_size, const ResImpl *comp_impl)
{
	_impl->replace(offset, old_size, comp_impl);
	object_header()->body = _impl->body();
	object_header()->body_size += comp_impl->size() - old_size;
}

/**
 * Erase a component of the given size
 *
 * @param offset location of component
 * @param size size of component
 */
void ResObject::erase_component(int offset, int size)
{
	_impl->erase(offset, size);
	object_header()->body = _impl->body();
	object_header()->body_size -= size;	
}



}
}
