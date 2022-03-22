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
/*
 * resobject.h
 *
 *  Created on: 29 Apr 2010
 *      Author: alanb
 */

#ifndef RESOBJECT_H_
#define RESOBJECT_H_

#include "resbase.h"
#include <string>
#include <iostream>

#include "resstruct.h"

namespace tbx {

class Application;
typedef int *OsSpriteAreaPtr;

namespace res {

class ResFile;

/**
 * Base class for a resource object that can be edited
 */
class ResObject : public ResBase
{
	ResObject(ResObjectHeader *object_header, ResData *data);
	ResObject(ResObjectHeader *object_header);
	friend class tbx::Application;
	friend class ResFile;
	static OsSpriteAreaPtr _sprite_area_ptr;
public:
	ResObject(const ResObject &other);
	virtual ~ResObject();

	ResObject &operator=(const ResObject &other);

	static ResObject *load(std::istream &file);
	bool save(std::ostream &file);

	static OsSpriteAreaPtr client_sprite_pointer();
	static void client_sprite_pointer(OsSpriteAreaPtr ptr);

	/**
	 * Return a pointer to the header information for this object
	 */
	const ResObjectHeader *object_header() const {return reinterpret_cast<const ResObjectHeader *>(_impl->header());}

	/**
	 * Return pointer to name of object
	 */
	const char *name() const {return object_header()->name;}

	/**
	 * Set the name of the object
	 */
	void name(std::string name);

	/**
	 * Return class id of object
	 */
    int class_id() const {return object_header()->class_id;}
    /**
     * Return version number * 100
     */
    int object_version() const {return object_header()->version;}
	void object_version(int new_version);

	/**
	 * object flags - use methods below for named access to the
	 * bits of the flags
	 */
	unsigned int object_flags() const {return object_header()->flags;}
	void object_flags(unsigned int flags);

	/**
	 * Get create on load flag
	 *
	 * @returns true if this object will be created when it is loaded
	 */
	bool create_on_load() const {return (object_header()->flags & 1) != 0;}
	void create_on_load(bool value);
	/**
	 * Get show on create flags
	 *
	 * @returns true if this object will be shown when it is created
	 */
	bool show_on_create() const {return (object_header()->flags & 2) != 0;}
	void show_on_create(bool value);
	/**
	 * Get shared flag
	 *
	 * @returns true if this creates shared objects
	 */
	bool shared() const {return (object_header()->flags & 4) != 0;}
	void shared(bool value);
	/**
	 * Get ancestor flags
	 *
	 * @returns true if this object will be created as an ancestor
	 */
	bool ancestor() const {return (object_header()->flags & 8) != 0;}
	void ancestor(bool value);


	void check_class_id(int class_id) const;


protected:
	ResObject(std::string name, int class_id, int version, int object_size);

	/**
	 * Return a pointer to the header information for this object
	 */
	ResObjectHeader *object_header() {return reinterpret_cast<ResObjectHeader *>(_impl->header());}

	/**
	 * Make object writeable, call before any change.
	 */
	void make_writeable();

	/**
	 * Extract a copy of a components data from this object
	 *
	 * @param new_body - new component body to copy relocations to
	 * @param offset - offset to start of components in body
	 * @param size - size of component
	 */
	ResData *component_data(char *new_body, int offset, int size) const {return _impl->component_data(new_body, offset, size);}
	void insert_component(int offset, const ResImpl *comp_impl);
	void replace_component(int offset, int old_size, const ResImpl *comp_impl);
	void erase_component(int offset, int size);
};


}

}

#endif /* RESOBJECT_H_ */
