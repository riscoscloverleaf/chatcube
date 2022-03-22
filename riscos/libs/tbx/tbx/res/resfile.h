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

#ifndef TBX_RES_RESFILE_H_
#define TBX_RES_RESFILE_H_

#include "resobject.h"
#include "resiteratorbase.h"

namespace tbx {

namespace res {


/**
 * Load and give read only access to a resource file
 *
 * ResObjects returned from this object are only valid
 * as long as the ResFile object is in memory.
 */
class ResFile
{
	char *_res;
	int _length;

public:
	ResFile(void);
	~ResFile(void);

	bool load(const std::string &fname);

	bool contains(std::string name) const;
	ResObject object(std::string name) const;

	/**
	 * Constant iterator of objects in this resource file
	 */
   class const_iterator : public ResIteratorBase<ResFile>
   {
	   const_iterator(const ResFile *res_file, int offset) : ResIteratorBase<ResFile>(res_file, offset) {}
	   friend class ResFile;
   public:
	   /**
	    * Increment iterator
	    *
	    * @returns incremented iterator
	    */
	   const_iterator &operator++() {_object->next_object(_offset); return *this;}
	   /**
	    * Increment iterator
	    *
	    * @return iterator before increment
	    */
	   const_iterator operator++(int) {const_iterator tmp(*this); _object->next_object(_offset); return tmp;}
	   /**
	    * Get object reference by iterator
	    *
	    * @returns ResObject referenced
	    */
	   ResObject operator*() const {return _object->at_offset(_offset);}
   };
   friend class const_iterator;

   /**
    * Get constant iterator to first object
    *
    * @returns constant iterator to first object
    */
   const_iterator begin() const {return const_iterator(this, first_offset());}
   /**
    * Get constant iterator to end object
    *
    * @returns constant iterator to object after the last object
    */
   const_iterator end() const {return const_iterator(this, end_offset());}
   /**
    * Get constant iterator to first object
    *
    * @returns constant iterator to first object
    */
   const_iterator cbegin() const {return const_iterator(this, first_offset());}
   /**
    * Get constant iterator to end object
    *
    * @returns constant iterator to object after the last object
    */
   const_iterator cend() const {return const_iterator(this, end_offset());}
   const_iterator find(std::string name) const;

protected:
	int first_offset() const;
	int end_offset() const;
	void next_object(int &offset) const;
	ResObject at_offset(int offset) const;
};

}
}

#endif
