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

#ifndef TBX_RES_RESSTRUCT_H_
#define TBX_RES_RESSTRUCT_H_

#include "resobject.h"

namespace tbx
{

namespace res
{

// Low level structure for dealing with resources files
const int RESF_MARKER = 0x46534552;// 'RESF'

/**
 * Structure representing the header of a resource file
 */
struct ResFileHeader
{
	int file_id; //!< Magic number should be RESF_MARKER
	unsigned int version; //!< Version number * 100
	int object_offset;    //!< Offset to first object in the file or -1 for no objects
};

/**
 * This structure proceeds a resource object in a file to give its data
 */
struct ResDataHeader
{
	int string_table_offset;      //!< Offset to string table
	int messages_table_offset;    //!< Offset to messages table
	int relocations_table_offset; //!< Offset to relocations table
};


/**
 * Structure for one relocation record
 */
struct ResRelocation
{
	/**
	 * Relocation type enumeration
	 */
	enum Type {STRING_REF = 1, MESSAGE_REF, SPRITE_AREA_REF, OBJECT_REF};
	int offset; //!< Offset in object to relocation
	Type type;  //!< Type of relocation
};

/**
 * Common header for all resource objects
 */
struct ResObjectHeader
{
    int class_id;		//!< Object class
    int flags;			//!< flags
    int version;		//!< version * 100
	char name[12];		//!< Null terminated name
	int total_size;     //!< Total size of object including tables
	char *body;			//!< Pointer to body
	int body_size;	    //!< Size of header and body only
};


}
}

#endif
