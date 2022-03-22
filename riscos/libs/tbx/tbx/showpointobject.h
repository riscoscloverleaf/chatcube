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

#ifndef TBX_SHOWPOINTOBJECT_H
#define TBX_SHOWPOINTOBJECT_H

#include "object.h"
#include "menu.h"
#include "point.h"

namespace tbx {
namespace res
{
    class ResObject;
}

/**
 * Object that can be shown at a specific position
 * as well as its default place.
 */
class ShowPointObject: public tbx::Object
{
public:
	/**
	 * Constructor with no reference to a Toolbox object
	 *
	 * The object should be assigned to another object that contains
	 * a valid Toolbox object reference before it is used.
	 */
	ShowPointObject()  {}
	/**
	 * Construct from a Toolbox object id
	 *
	 * @param handle Toolbox object id for an object supports the show with full details
	 */
	ShowPointObject(ObjectId handle) : Object(handle) {}
	/**
	 * Construct as reference to another Toolbox object
	 *
	 * @param other object to copy Toolbox object reference from
	 */
	ShowPointObject(const Object &other) : Object(other) {}
	/**
	 * Create a Toolbox object and make this object a reference to it
	 *
	 * @param template_name name of template in the application resources
	 * @throws OsError Unable to create the Toolbox object
	 */
	ShowPointObject(const std::string &template_name) : Object(template_name) {}
	/**
	 * Create Toolbox object from a memory template and make this object
	 * a reference to it.
	 *
	 * @param object_template in memory resource template
	 * @throws OsError Unable to create the Toolbox object
	 */
	ShowPointObject(const res::ResObject &object_template) : Object(object_template) {}

	using Object::show;

	void show(const Object &parent);
	void show(const Component &parent);
	void show(const Point &pos);
	void show(const Point &pos, const Object &parent);
	void show(const Point &pos, const Component &parent);

	void show_as_menu();
	void show_as_menu(const Object &parent);
	void show_as_menu(const Component &parent);
	void show_as_menu(const Point &pos);
	void show_as_menu(const Point &pos, const Object &parent);
	void show_as_menu(const Point &pos, const Component &parent);

	void show_as_submenu(const MenuItem &parent);
	void show_as_submenu(const Point &pos, const MenuItem &parent);
};

}

#endif /* TBX_SHOWPOINTOBJECT_H */
