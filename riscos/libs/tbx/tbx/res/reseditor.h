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
 * reseditor.h
 *
 *  Created on: 29 Apr 2010
 *      Author: alanb
 */

#ifndef TBX_RESEDITOR_H_
#define TBX_RESEDITOR_H_

#include "resobject.h"
#include <vector>

namespace tbx {

namespace res {

/**
 * Class to allow creation, loading, editing and saving of a
 * toolbox resource file.
 */
class ResEditor
{
	char *_header;
	std::vector<ResObject> _objects;

public:
	ResEditor();
	virtual ~ResEditor();

	/**
	 * Return header details of the file
	 */
	const ResFileHeader *header() const {return reinterpret_cast<ResFileHeader *>(_header);}

	/**
	 * Constant iterator to iterate through the objects being edited
	 */
	typedef std::vector<ResObject>::const_iterator const_iterator;
	/**
	 * Iterator to iterate through the objects being edited
	 */
	typedef std::vector<ResObject>::iterator iterator;

	/**
	 * Get constant iterator to first object
	 *
	 * @returns constant iterator to first object or end() if no objects
	 */
	const_iterator begin() const {return _objects.begin();}
	/**
	 * Get constant iterator to the end of the objects
	 *
	 * @returns constant iterator referencing the object after the last object
	 */
	const_iterator end()   const {return _objects.end();}
	const_iterator find(std::string name) const;

	/**
	 * Get iterator to first object
	 *
	 * @returns iterator to first object or end() if no objects
	 */
	iterator begin() {return _objects.begin();}
	/**
	 * Get iterator to the end of the objects
	 *
	 * @returns iterator referencing the object after the last object
	 */
	iterator end()   {return _objects.end();}
	iterator find(std::string name);

	/**
	 * Return number of objects
	 */
	unsigned int count() const {return _objects.size();}
	bool contains(std::string name) const;
	const ResObject &object(std::string name) const;

	void clear();
	void add(ResObject obj);
	void replace(ResObject obj);
	void erase(std::string name);

	iterator insert(iterator before, ResObject obj);
	iterator erase(iterator where);
	void replace(iterator where, ResObject obj);

	bool load(std::string file_name);
	bool save(std::string file_name);

private:
	// Only editor can change header
	ResFileHeader *header() {return reinterpret_cast<ResFileHeader *>(_header);}


};

}

}

#endif /* TBX_RESEDITOR_H_ */
