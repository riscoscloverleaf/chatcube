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
 * reseditor.cc
 *
 *  Created on: 29 Apr 2010
 *      Author: alanb
 */

#include "reseditor.h"
#include "resexcept.h"
#include "../path.h"
#include <fstream>
#include <memory>

namespace tbx {

namespace res {

/**
 * Construct an empty resource file
 */
ResEditor::ResEditor()
{
	_header = new char[sizeof(ResFileHeader)];
	header()->file_id = RESF_MARKER;
	header()->version = 101; // 1.01
	header()->object_offset = -1; // No objects
}

ResEditor::~ResEditor()
{
	delete [] _header;
}

/**
 * Remove all objects from the editor
 */
void ResEditor::clear()
{
	_objects.clear();
}

/**
 * Load resources from a file
 *
 * @param file_name - name of file for load
 * @returns true if load succeeded
 */
bool ResEditor::load(std::string file_name)
{
	_objects.clear();

	std::ifstream file(file_name.c_str(), std::ios_base::in | std::ios_base::binary);
	if (!file) return false;

	ResFileHeader h;
	file.read((char *)&h, sizeof(ResFileHeader));
	if (!file) return false;

	if (h.file_id != RESF_MARKER) return false;

	 char *new_header = new char[h.object_offset == -1 ? 12 : h.object_offset];
	std::memcpy(new_header, &h, sizeof(ResFileHeader));

	if (h.object_offset > (int)sizeof(ResFileHeader))
	{
		file.read(new_header+12, h.object_offset - sizeof(ResFileHeader));
	}
	delete [] _header;
	_header = new_header;

	while (file)
	{
		ResObject *obj = ResObject::load(file);
		if (obj != 0)
		{
			_objects.push_back(*obj);
			delete obj;
		}
	}

	return true;
}

/**
 * Save resources to the name file
 *
 * @param file_name  name of file to save to
 * @returns true if save was successful
 */
bool ResEditor::save(std::string file_name)
{
	std::ofstream file(file_name.c_str(), std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);

	int obj_offset = header()->object_offset;
	if (_objects.empty()) obj_offset = -1;
	else if (obj_offset == -1) obj_offset = sizeof(ResFileHeader);
	header()->object_offset = obj_offset;

	file.write(_header, obj_offset);
	for (std::vector<ResObject>::iterator i = _objects.begin(); i != _objects.end(); ++i)
	{
		ResObject &obj = (*i);
		if (!obj.save(file)) return false;
	}

    bool saved = file.good();

    if (saved)
    {
       tbx::Path::file_type(file_name, 0xfae); // Resource file
    }

	return saved;
}


/**
 * Check if editor contains the named object
 *
 * @param name to check for
 * @returns true if object is in this editor
 */
bool ResEditor::contains(std::string name) const
{
	return (find(name) != end());
}
/**
 * Get object
 *
 * @param name name of the object
 * @returns ResObject with the given name
 * @throws ResObjectNotFound if object with name doesn't exists
 */
const ResObject &ResEditor::object(std::string name) const
{
	const_iterator i = find(name);
	if (i != end()) return (*i);
	throw ResObjectNotFound(name);
}

/**
 * Add a new object
 *
 * @param obj object to add
 * @throws ResObjectExists if name is already used for an object
 */
void ResEditor::add(ResObject obj)
{
	if (find(obj.name()) != end()) throw ResObjectExists(obj.name());
	_objects.push_back(obj);
}

/**
 * Replace object with same name as object given
 *
 * @param obj to replace.
 * @throws ResObjectNotFound if object with name doesn't exists
 */
void ResEditor::replace(ResObject obj)
{
	std::vector<ResObject>::iterator i = find(obj.name());
	if (i == _objects.end()) throw ResObjectNotFound(obj.name());
	*i = obj;
}

/**
 * Erase object with given name
 *
 * @throws ResObjectNotFound if object with name doesn't exists
 */
void ResEditor::erase(std::string name)
{
	std::vector<ResObject>::iterator i = find(name);
	if (i == _objects.end()) throw ResObjectNotFound(name);
	_objects.erase(i);
}

/**
 * Find object with given name
 *
 * @param name to find
 * @returns const_iterator for found object or end() if not found
 */
ResEditor::const_iterator ResEditor::find(std::string name) const
{
	const_iterator i;
	for (i = _objects.begin(); i != _objects.end(); ++i)
	{
		if (name == (*i).name()) break;
	}

	return i;
}

/**
 * Find object with given name
 *
 * @param name to find
 * @returns iterator for found object or end() if not found
 */
ResEditor::iterator ResEditor::find(std::string name)
{
	ResEditor::iterator i;
	for (i = _objects.begin(); i != _objects.end(); ++i)
	{
		if (name == (*i).name()) break;
	}

	return i;
}

/**
 * Insert object before given object
 * @throws ResObjectExists if name is already used for an object
 */
ResEditor::iterator ResEditor::insert(iterator before, ResObject obj)
{
	if (find(obj.name()) != end())  throw ResObjectExists(obj.name());
	return _objects.insert(before, obj);
}

/**
 * Erase object at iterator
 */
ResEditor::iterator ResEditor::erase(iterator where)
{
	return _objects.erase(where);
}

/**
 * Replace object at location
 *
 * It is recommend this is used instead of just *where = obj as this
 * routine checks that the name is unique in the editor
 *
 * @throws ResObjectExists if name is already used for an object
 * other than the one that is being replaced
 */
void ResEditor::replace(iterator where, ResObject obj)
{
	if (std::strcmp((*where).name(), obj.name()) != 0)
	{
		if (find(obj.name()) != end())  throw ResObjectExists(obj.name());
	}
	*where = obj;
}

}
}

