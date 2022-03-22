/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2010-2014 Alan Buckley   All Rights Reserved.
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

#include "path.h"
#include "swis.h"
#include <memory>
#include "stringutils.h"
#include "swixcheck.h"

using namespace tbx;

const char PathDelim = '.';

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**
 * Construct an empty path object
 */
Path::Path()
{

}

/**
 * Construct a path from the given path string
 *
 * @param name string representing path name
 */
Path::Path(const std::string &name)
{
	_name = name;
}

/**
 * Construct a path from the given path string
 *
 * @param name string representing path name
 */
Path::Path(const char *name)
{
	_name = name;
}

/**
 * Copy constructor
 *
 * @param other Path to copy
 */
Path::Path(const Path &other)
{
	_name = other._name;
}

/**
 * Construct a path to a child of another path
 *
 * @param other Path for parent directory
 * @param child name of the child item in the directory
 */
Path::Path(const Path &other, const std::string &child)
{
	_name = other._name;
	_name += PathDelim;
	_name += child;
}

/**
 * Construct a path to a child of another path
 *
 * @param other Path for parent directory
 * @param child name of the child item in the directory
 */
Path::Path(const Path &other, const char *child)
{
	_name = other._name;
	_name += PathDelim;
	_name += child;
}

/**
 * Path destructor
 */
Path::~Path()
{

}

/**
 * Assign to another path
 *
 * @param other Path to copy
 * @returns *this
 */
Path &Path::operator=(const Path &other)
{
	_name = other._name;
	return *this;
}

/**
 * Assign a path to a string
 *
 * @param name path name as a string
 */
Path &Path::operator=(const std::string &name)
{
	_name = name;
	return *this;
}

/**
 * Assign a path to a c string
 *
 * @param name path name as a c string
 */
Path &Path::operator=(const char *name)
{
	_name = name;
	return *this;
}

/**
 * Set path to a child of another path
 *
 * @param other parent directory
 * @param child name of child in the directory
 * @returns *this
 */
Path &Path::set(const Path &other, const std::string &child)
{
	_name = other._name;
	_name += PathDelim;
	_name += child;

	return *this;
}

/**
 * Get path as a string
 *
 * @returns path name as a string
 */
Path::operator const std::string&() const
{
	return _name;
}

/**
 * Get path as a C string
 *
 * @returns path name as a null terminated C string
 */
Path::operator const char *() const
{
	return _name.c_str();
}

/**
 * Get path object for given child name
 *
 * @param child name of child of the current path
 * @returns new Path for the child
 */
Path Path::child(const std::string &child) const
{
	return Path(*this, child);
}

/**
 * Get parent of this path
 *
 * @returns new path for the parent
 */
Path Path::parent() const
{
	Path pp(*this);
	return pp.up();
}

/**
 * Updates path to the given child
 *
 * @param child name of child to update path to
 * @returns *this
 */
Path &Path::down(const std::string &child)
{
	_name += PathDelim;
	_name += child;

	return *this;
}

/**
 * Updates this path to it's parent
 *
 * @returns *this
 */
Path &Path::up()
{
	std::string::size_type idx = _name.rfind(PathDelim);
	if (idx != std::string::npos)
	{
		_name.erase(idx);
	}
	return *this;
}

/**
 * Change the leaf name part of the path.
 *
 * The leaf name part is the last path of the path after the
 * directory delimiter.
 *
 * If a directory delimiter is not found the child is appended
 * to the path.
 *
 * @param child The new leaf name part of the path.
 **/
void tbx::Path::leaf_name(const std::string &child)
{
	std::string::size_type idx = _name.rfind(PathDelim);
	if (idx == std::string::npos)
	{
		_name += PathDelim;
		_name += child;
	} else
		_name.replace(idx+1, _name.length(), child);
}

/**
 * Get the leaf name part of the path.
 *
 * The leaf name part is the last part of the path after
 * the final directory delimiter in the path.
 *
 * @returns leaf name part or "" string if it can't be determined
 */
std::string tbx::Path::leaf_name() const
{
	std::string child;

	std::string::size_type idx = _name.rfind(PathDelim);
	if (idx != std::string::npos)
	{
		child = _name.substr(idx+1);
	}

	return child;
}

/**
 * Check the File system to determine the type of
 * this object this path refers to
 */
PathInfo::ObjectType Path::object_type() const
{
	PathInfo info;
	info.read(_name);
	return info.object_type();
}

/**
 * Read the catalogue information for this path
 *
 * @param info PathInfo updated with the information
 * @returns true if the path is on the file system
 */
bool Path::path_info(PathInfo &info) const
{
	return info.read(_name);
}

/**
 * Read catalogue information for the given path always
 * returning the raw load/exec address.
 *
 * This can be used to get the actual file type for an ImageFS directory.
 *
 * @param path Path referring to a location on a file system
 * @param calc_file_type true to calculate file type from the returned catalogue information,
 * if false file_type -1 will always be returned.
 * @returns true if information read
 */
bool Path::raw_path_info(PathInfo &info, bool calc_file_type) const
{
	return info.read_raw(_name, calc_file_type);
}

/**
 * Check if path exists on the file system
 *
 * @returns true if the path exists.
 */
bool Path::exists() const
{
	PathInfo info;
	return info.read(_name);
}

/**
 * Check if path is a file on the file system
 *
 * @return true if the path refers to a file
 */
bool Path::file() const
{
	PathInfo info;
	info.read(_name);
	return info.file();
}

/**
 * Check if path is a directory on the file system
 *
 * @returns true if the path refers to a directory
 */
bool Path::directory() const
{
	PathInfo info;
	info.read(_name);
	return info.directory();
}

/**
 * Check if path is a image file system on the file system
 *
 * @return true if the path refers to a image file system
 */
bool Path::image_file() const
{
	PathInfo info;
	info.read(_name);
	return info.image_file();
}

/**
 * Read the file type for this path
 *
 * @returns the file type (or -2 if could not be read)
 */
int Path::file_type() const
{
	PathInfo info;
	info.read(_name);
	return info.file_type();
}

/**
 * Set the file type for this path
 *
 * @param type new file type
 * @returns true if file type set successfully
 */
bool Path::file_type(int type)
{
	_kernel_swi_regs regs;

	regs.r[0] = 18;
	regs.r[1] = reinterpret_cast<int>(_name.c_str());
	regs.r[2] = type;

	return (_kernel_swi(OS_File, &regs, &regs) == 0);
}

/**
 * Get the file type of the named file
 *
 * @param file_name name of file to return type for
 * @returns the file type (or -2 if could not be read)
 */
int Path::file_type(const std::string &file_name)
{
	PathInfo info;
	info.read(file_name);
	return info.file_type();
}

/**
 * Set the file type for the named file
 *
 * @param file_name path to file to set type
 * @param type new file type
 * @returns true if file type set successfully
 */
bool Path::file_type(const std::string &file_name, int type)
{
	_kernel_swi_regs regs;

	regs.r[0] = 18;
	regs.r[1] = reinterpret_cast<int>(file_name.c_str());
	regs.r[2] = type;

	return (_kernel_swi(OS_File, &regs, &regs) == 0);
}

/**
 * Return the file type from the load/exec address.
 *
 * For image FS files it will return the actual file type
 * of the image file whereas file_type returns 0x1000 (directory).
 *
 * The return value is undefined for directories/applications
 * or files that do not have file types.
 *
 * @return raw file type.
 */
int Path::raw_file_type() const
{
	PathInfo info;
	info.read_raw(_name, true);
	return info.raw_file_type();
}


/**
 * Return the modified time from the file in the path.
 *
 * @returns modified time of file or UTCTime(0)
 *          if file doesn't exist or doesn't have a time.
 */
UTCTime Path::modified_time() const
{
	PathInfo info;
	info.read(_name);
	return info.modified_time();
}

/**
 * Return the objects file attributes
 *
 * The lower byte of the attributes are 0 or more of the values in
 * the PathInfo::Attribute enum ored together.
 *
 * @returns file attributes of the object
 */
int Path::attributes() const
{
	PathInfo info;
	info.read(_name);
	return info.attributes();
}

/**
 * Set the attributes of an object
 *
 * @param new_attributes new set of attributes (see PathInfo::Attribute)
 * @returns true if successful
 */
bool Path::attributes(int new_attributes)
{
	_kernel_swi_regs regs;

	regs.r[0] = 4;
	regs.r[1] = reinterpret_cast<int>(_name.c_str());
	regs.r[5] = new_attributes;

	return (_kernel_swi(OS_File, &regs, &regs) == 0);
}


/*TODO: It's unclear in the manual how to set this
bool Path::SetModifiedTime(const UTCTime &utcTime)
{
	_kernel_swi_regs regs;

	regs.r[0] = 9;
	regs.r[1] = reinterpret_cast<int>(_name.c_str());
	regs.r[2] = type;

	return (_kernel_swi(OS_File, &regs, &regs) == 0);
}
*/


/**
 * Create an empty file of the given file type
 *
 * @param type file type for the new file
 * @throws OsError if file failed to created
 */
void Path::create_file(int type) const
{
	_kernel_swi_regs regs;

	regs.r[0] = 11;
	regs.r[1] = reinterpret_cast<int>(_name.c_str());
	regs.r[2] = type;
	regs.r[4] = 0;
	regs.r[5] = 0;

	swix_check(_kernel_swi(OS_File, &regs, &regs));
}

/**
 * Create a directory at the given path.
 *
 * If the directory already exists this does nothing and no error is thrown.
 *
 * @throws OsError if directory cannot be created.
 */
void Path::create_directory() const
{
	_kernel_swi_regs regs;

	regs.r[0] = 8;
	regs.r[1] = reinterpret_cast<int>(_name.c_str());
	regs.r[4] = 0; // Default number of entries

	swix_check(_kernel_swi(OS_File, &regs, &regs));
}

/**
 * Remove this file or directory from the file system
 *
 * Note: it is not an error if the object does not exist
 * @throws OsError if the object is locked against deletion or is a directory
 *                which is not empty or already open.
 */
void Path::remove() const
{
	_kernel_swi_regs regs;

	regs.r[0] = 6;
	regs.r[1] = reinterpret_cast<int>(_name.c_str());

	swix_check(_kernel_swi(OS_File, &regs, &regs));
}

/**
 * Rename file.
 *
 * This is a simple rename that will only work on single objects
 * on the same file system.
 *
 * @throws OsError if it fails
 */
void Path::rename(const std::string &new_name)
{
	_kernel_swi_regs regs;

	regs.r[0] = 25;
	regs.r[1] = reinterpret_cast<int>(_name.c_str());
	regs.r[2] = reinterpret_cast<int>(new_name.c_str());

	swix_check(_kernel_swi(OS_FSControl, &regs, &regs));
}

/**
 * Copy the object.
 *
 * @param copyto location to copy to
 * @param options bitwise or of flags from CopyOption enum.
 * @throws OsError if the copy fails
 */
void Path::copy(const std::string &copyto, unsigned int options)
{
	_kernel_swi_regs regs;

	regs.r[0] = 26;
	regs.r[1] = reinterpret_cast<int>(_name.c_str());
	regs.r[2] = reinterpret_cast<int>(copyto.c_str());
	regs.r[3] = options;
	regs.r[4] = 0;
	regs.r[5] = 0;
	regs.r[6] = 0;
	regs.r[7] = 0;
	regs.r[8] = 0;

	swix_check(_kernel_swi(OS_FSControl, &regs, &regs));
}
/**
 * Copy the object.
 *
 * This version uses the given buffer for workspace.
 *
 * @param copyto location to copy to
 * @param options bitwise or of flags from CopyOption enum.
 * @param buffer  buffer to use
 * @param size size of buffer to use
 * @throws OsError if the copy fails
 */
void Path::copy(const std::string &copyto, unsigned int options, void *buffer, unsigned int size)
{
	unsigned int descriptor[2];
	descriptor[0] = reinterpret_cast<unsigned int>(buffer);
	descriptor[1] = size;

	_kernel_swi_regs regs;

	regs.r[0] = 26;
	regs.r[1] = reinterpret_cast<int>(_name.c_str());
	regs.r[2] = reinterpret_cast<int>(copyto.c_str());
	regs.r[3] = options | 0x2000; /* 0x2000 = USE DESCRIPTOR */
	regs.r[4] = 0;
	regs.r[5] = 0;
	regs.r[6] = 0;
	regs.r[7] = 0;
	regs.r[8] = reinterpret_cast<int>(descriptor);

	swix_check(_kernel_swi(OS_FSControl, &regs, &regs));
}

/**
 * Move a file to a new location.
 *
 * This moves the file by copying it then deleting the
 * source.
 *
 * @param copyto location to move to
 * @param options bitwise or of flags from CopyOption enum.
 * @throws OsError if the move fails
 */
void Path::move(const std::string &copyto, unsigned int options)
{
	_kernel_swi_regs regs;

	regs.r[0] = 26;
	regs.r[1] = reinterpret_cast<int>(_name.c_str());
	regs.r[2] = reinterpret_cast<int>(copyto.c_str());
	regs.r[3] = options | 0x80; /* 0x80 = DELETE after copy */
	regs.r[4] = 0;
	regs.r[5] = 0;
	regs.r[6] = 0;
	regs.r[7] = 0;
	regs.r[8] = 0;

	swix_check(_kernel_swi(OS_FSControl, &regs, &regs));
}

/**
 * Move a file to a new location.
 *
 * This moves the file by copying it then deleting the
 * source.
 *
 * This version uses the given buffer for workspace.
 *
 * @param copyto location to move to
 * @param options bitwise or of flags from CopyOption enum.
 * @param buffer  buffer to use
 * @param size size of buffer to use
 * @throws OsError if the move fails
 */
void Path::move(const std::string &copyto, unsigned int options, void *buffer, unsigned int size)
{
	unsigned int descriptor[2];
	descriptor[0] = reinterpret_cast<unsigned int>(buffer);
	descriptor[1] = size;

	_kernel_swi_regs regs;

	regs.r[0] = 26;
	regs.r[1] = reinterpret_cast<int>(_name.c_str());
	regs.r[2] = reinterpret_cast<int>(copyto.c_str());
	regs.r[3] = options | 0x80 | 0x2000; /* 0x80 = DELETE, 0x2000 = USE DESCRIPTOR */
	regs.r[4] = 0;
	regs.r[5] = 0;
	regs.r[6] = 0;
	regs.r[7] = 0;
	regs.r[8] = reinterpret_cast<int>(descriptor);

	swix_check(_kernel_swi(OS_FSControl, &regs, &regs));
}

/**
 * Load this file into a character array
 *
 * @param length updated to length of file if specified
 * @returns new char[] with contents of file
 * @throws OsError if load failed
 */
char *Path::load_file(int *length /*= 0*/) const
{
	char *data = 0;
	PathInfo info;

	if (length) *length = 0;

	if (info.read(_name) && !info.directory())
	{
		data = new char[info.length()];
		_kernel_swi_regs regs;
		regs.r[0] = 16;
		regs.r[1] = reinterpret_cast<int>(_name.c_str());
		regs.r[2] = reinterpret_cast<int>(data);
		regs.r[3] = 0;

        try
        {
           swix_check(_kernel_swi(OS_File, &regs, &regs));
		   if (length) *length = regs.r[4];
		} catch(...)
		{
			delete [] data;
			data = 0;
			throw;
		}
	}

	return data;
}

/**
 * Save an array of characters to a file
 *
 * @param data the array of characters to save
 * @param length the number of characters to save from the array
 * @param file_type file type to save data as
 * @throws OsError if save fails
 */
void Path::save_file(const char *data, int length, int file_type) const
{
	_kernel_swi_regs regs;

	regs.r[0] = 10;
	regs.r[1] = reinterpret_cast<int>(_name.c_str());
	regs.r[2] = file_type;
	regs.r[4] = reinterpret_cast<int>(data);
	regs.r[5] = reinterpret_cast<int>(data+length);

	swix_check(_kernel_swi(OS_File, &regs, &regs));
}


/**
 * Canonicalise the path name
 */
void Path::canonicalise()
{
  _name = canonicalise(_name);
}

/**
 * Static function to provide the canonicalised version of a
 * path name.
 *
 * @param path path to canonicalise
 * @returns new path name
 */
std::string Path::canonicalise(const std::string &path)
{
	_kernel_swi_regs regs;

	regs.r[0] = 37;
	regs.r[1] = reinterpret_cast<int>(path.c_str());
	regs.r[2] = 0; // Path buffer 0 to read size required
	regs.r[3] = 0; // Don't use path variable
	regs.r[4] = 0; // Don't use path string
	regs.r[5] = 0; // Buffer size 0 to read size required
	swix_check(_kernel_swi(OS_FSControl, &regs, &regs));

	regs.r[5] = 1 - regs.r[5]; // Size of buffer required
	char buffer[regs.r[5]];
	regs.r[1] = reinterpret_cast<int>(path.c_str());
	regs.r[2] = reinterpret_cast<int>(buffer);
	swix_check(_kernel_swi(OS_FSControl, &regs, &regs));

	std::string result(buffer);
	return result;
}

/**
 * Compare this path with another to see if their canonical names
 * are the same using a case insensitive compare.
 *
 * @param compare_to Path to compare to
 * @return true if they are the same
 */
bool Path::canonical_equals(const tbx::Path &compare_to) const
{
	return tbx::equals_ignore_case(canonicalise(_name), canonicalise(compare_to._name));
}
/**
 * Compare this paths with a string see if their canonical names
 * are the same using a case insensitive compare.
 *
 * @param compare_to Path to compare to
 * @return true if they are the same
 */
bool Path::canonical_equals(const std::string &compare_to) const
{
	return tbx::equals_ignore_case(canonicalise(_name), canonicalise(compare_to));
}

/**
 * Set the OS current directory to this path
 *
 * @returns true if successful
 */
bool Path::set_current_directory() const
{
	_kernel_swi_regs regs;
	regs.r[0] = 0;
	regs.r[1] = reinterpret_cast<int>(_name.c_str());

	return (_kernel_swi(OS_FSControl, &regs, &regs) == 0);
}

/**
 * Return a Path for a temporary file
 *
 * The file will be inside the scrap directory and the
 * leaf name will start with the prefix given (or tbx
 * if there is no prefix).
 *
 * The file name is checked against other
 *
 * @param prefix for leaf name of the path
 * @returns Path with temporary file name
 */
Path Path::temporary(const char *prefix /* = 0 */)
{
	if (prefix == 0) prefix="tbx";
	unsigned int num = UTCTime::now().low_word();
	std::string file_name;
	tbx::Path temp;
	do
	{
		std::ostringstream ss;
		ss << "<Wimp$ScrapDir>." << prefix << std::hex << num;
		temp = ss.str();
		num++;
	} while (temp.exists());

	return temp;
}


/**
 * Create an iterator to search the current directory.
 *
 * @param wild_card Wildcard to use for search.
 * @returns iterator for the directory
 */
Path::Iterator Path::begin(const std::string &wild_card)
{
	return Iterator(_name, wild_card.c_str());
}

/**
 * Create an iterator to scan the whole directory.
 *
 * @returns iterator for first item in the directory
 */
Path::Iterator Path::begin()
{
	return Iterator(_name, 0);
}

/**
 * End iterator.
 *
 * @returns end iterator for the directory in this path
 */
Path::Iterator Path::end()
{
	return Iterator();
}

/**
 * Construct a path iterator from a directory name and wild card
 *
 * @param dirName name of directory to iterate
 * @param wildCard wild card to match against file names to return
 */
Path::Iterator::Iterator(const std::string &dirName, const char *wildCard)
{
	_iterBlock = new IterBlock(dirName, wildCard);
	if (_iterBlock->next_name() == 0)
	{
		_iterBlock->release();
		_iterBlock = 0;
	} else
	{
		_name = _iterBlock->next_name();
		_iterBlock->next();
	}
}

// No arguments just creates the end iterator
Path::Iterator::Iterator()
{
	_name = "";
	_iterBlock = 0;
}

/**
 * Copy constructor
 *
 * @param other Iterator to copy
 */
Path::Iterator::Iterator(const Iterator &other)
{
	_name = other._name;
	_iterBlock = other._iterBlock;
	if (_iterBlock) _iterBlock->add_ref();
}

/**
 * Assign this iterator as the value of another
 *
 * @param other iterator to copy
 * @returns *this
 */
Path::Iterator &Path::Iterator::operator=(const Iterator &other)
{
	_name = other._name;
	if (_iterBlock) _iterBlock->release();
	_iterBlock = other._iterBlock;
	if (_iterBlock) _iterBlock->add_ref();
	return *this;
}

/**
 * Check if this iterator is the same as another
 *
 * @param other Iterator to compare to
 * @returns true if iterator are the same
 */
bool Path::Iterator::operator==(const Iterator &other)
{
	return (_iterBlock == other._iterBlock && _name == other._name);
}

/**
 * Check if this iterator is different from another
 *
 * @param other Iterator to compare to
 * @returns true if iterator are different
 */
bool Path::Iterator::operator!=(const Iterator &other)
{
	return (_iterBlock != other._iterBlock || _name != other._name);
}

/**
 * Move iterator to next item
 *
 * @returns *this
 */
Path::Iterator &Path::Iterator::operator++()
{
	next();
	return *this;
}

/**
 * Move iterator to next item
 *
 * @returns copy of iterator before the move
 */
Path::Iterator Path::Iterator::operator++(int)
{
	Iterator retCopy(*this);
	next();
	return retCopy;
}

/**
 * Moves iterator on to the next item
 */
void Path::Iterator::next()
{
	_name = "";

	if (_iterBlock == 0)
	{
		return;
	} else if (_iterBlock->next_name() == 0)
	{
		_iterBlock->release();
		_iterBlock = 0;
		return;
	}

	_name = _iterBlock->next_name();
	if (!_iterBlock->next())
	{
		_iterBlock->release();
		_iterBlock = 0;
	}
}

/**
 * Construct internal iterator block
 *
 * @param dirName name of directory
 * @param wildCard wild card string
 */
Path::Iterator::IterBlock::IterBlock(const std::string &dirName, const char *wildCard)
{
	_ref = 1;
	_nextName = 0;
	_toRead = 0;

	_dirName = new char[dirName.length()+1];
	strcpy(_dirName, dirName.c_str());
	if (wildCard == 0) _wildCard = 0;
	else
	{
		_wildCard = new char[strlen(wildCard)+1];
		strcpy(_wildCard, wildCard);
	}

	_regs.r[0] = 9; // Read file names only
	_regs.r[1] = reinterpret_cast<int>(_dirName);
	_regs.r[2] = reinterpret_cast<int>(_readData);
	_regs.r[3] = 32; // Number of objects to read
	_regs.r[4] = 0;  // First item to read, 0 to start
	_regs.r[5] = _readSize; // Buffer length
	_regs.r[6] = reinterpret_cast<int>(_wildCard);

	next();
}

/**
 * Move to next record in iterator block
 *
 * @returns true if there are more items
 */
bool Path::Iterator::IterBlock::next()
{
	if (_toRead == 0)
	{
		_nextName = 0;

		_regs.r[3] = 32; // Number of objects to read

		if (_regs.r[4] == -1
		    || (_kernel_swi(0x0c, &_regs, &_regs) != 0)
		    || (_regs.r[4] == -1 && _regs.r[3] == 0)
		   )
		{
			return false; // No more data to read
		} else
		{
			_toRead = _regs.r[3];
			_nextName = _readData;
		}
	} else
	{
		_nextName = _nextName + strlen(_nextName) + 1;
	}

	_toRead--;

	return true;
}


/**
 * Construct UTC time for Midnight, 1st Jan 1990
 */
UTCTime::UTCTime()
{
	_centiseconds = 0;
}

/**
 * Construct a UTC time for the given number of centiseconds
 *
 * @param csecs number of centiseconds since Midnight, 1st Jan 1990
 */
UTCTime::UTCTime(long long csecs) : _centiseconds(csecs)
{

}

/**
 * Construct a UTC time give file system catalog information
 *
 * @param load_address load address of the file
 * @param exec_address execute address of the file
 */
UTCTime::UTCTime(unsigned int load_address, unsigned int exec_address)
{
	_centiseconds = exec_address
			| ((long long)(load_address & 0xFF) << 32);
}

/**
 * Copy constructor
 *
 * @param other UTCTime to copy
 */
UTCTime::UTCTime(const UTCTime &other)
{
	_centiseconds = other._centiseconds;
}

/**
 * Assign to value fo another UTCTime
 *
 * @param other UTCTime to copy
 * @returns *this
 */
UTCTime &UTCTime::operator=(const UTCTime &other)
{
	_centiseconds = other._centiseconds;

	return *this;
}

/**
 * Get a UTCTime representing the current time
 *
 * @returns a new UTCTime for now
 */
UTCTime UTCTime::now()
{
   UTCTime now;

   _kernel_swi_regs in, out;

   in.r[0] = 14;
   in.r[1] = (int)&(now._centiseconds);
   now._centiseconds = 3; // Reason code for os call
   _kernel_swi(OS_Word, &in, &out);

   return now;
}

/**
 *  Get time/date as text in standard format.
 *
 * The standard format is read from the system variable
 * Sys$DateFormat
 *
 * @returns string with formatted date
*/
std::string UTCTime::text() const
{
	char dateText[32];

	_kernel_swi_regs in, out;

   in.r[0] = (int)&_centiseconds;
   in.r[1] = (int)dateText;
   in.r[2] = 32;

   if (_kernel_swi(OS_ConvertStandardDateAndTime, &in, &out) != 0) dateText[0] = 0;

   return std::string(dateText);
}

/**
 * Get time/date as text in specified format.
 *
 * Text from the format is copied directly into the result
 * unless it starts with a "%" in which case the following
 * is substituted.
 *   CS Centiseconds
 *   SE Seconds
 *   MI Minutes
 *   12 Hours in 12 hour format
 *   24 Hours in 24 hour format
 *   AM AM or PM indicator (in local language)
 *   PM AM or PM indicator (in local language)
 *   WE Weekday - full (in local language)
 *   W3 Weekday - short (in local language)
 *   WN Weekday - number
 *   DY Day of the month (in local language)
 *   ST Ordinal pre/suffix (in local language) (e.g st nd rd th)
 *   MO Month name - full (in local language)
 *   M3 Month name - short (in local language)
 *   MN Month - number
 *   CE Century
 *   YR Year within century
 *   WK Week of year (using local start of week)
 *   DN Day of the year
 *   TZ Timezone
 *   0  Insert an ASCII 0 zero byte
 *   %  Insert a '%'
 *
 *@returns string with formatted date
 */
std::string UTCTime::text(const std::string &format) const
{
   _kernel_swi_regs in,out;
   int bufLen = format.length() + 64;
   std::auto_ptr<char> buffer(new char[bufLen]);

   in.r[0] = (int)&_centiseconds;
   in.r[1] = reinterpret_cast<int>(buffer.get());
   in.r[2] = bufLen;
   in.r[3] = reinterpret_cast<int>(format.c_str());

   if (_kernel_swi(OS_ConvertDateAndTime, &in, &out) != 0)
	   *(buffer.get()) = 0;

   return std::string(buffer.get());
}

/**
 * Construct and empty PathInfo not referring to any file
 */
PathInfo::PathInfo()
{
	_object_type = NOT_FOUND;
	_file_type = -2;

	_load_address	= 0;
	_exec_address	= 0;
	_length		= 0;
	_attributes	= 0;
}

/**
 * Copy constructor
 *
 * @param other PathInfo to copy
 */
PathInfo::PathInfo(const PathInfo &other)
{
	_name			= other._name;
	_object_type	= other._object_type;
	_load_address	= other._load_address;
	_exec_address	= other._exec_address;
	_length		= other._length;
	_attributes	= other._attributes;
	_file_type		= other._file_type;
}

/**
 * Assign to the value of another PathInfo
 *
 * @param other PathInfo to copy
 * @returns *this
 */
PathInfo &PathInfo::operator=(const PathInfo &other)
{
	_name			= other._name;
	_object_type	= other._object_type;
	_load_address	= other._load_address;
	_exec_address	= other._exec_address;
	_length		= other._length;
	_attributes	= other._attributes;
	_file_type		= other._file_type;

	return *this;
}

/**
 * Checks if this path info is identical to another
 *
 * Note: name check is case-sensitive
 *
 * @param other PathInfo to check
 * @returns true if PathInfos are the same
 */
bool PathInfo::operator==(const PathInfo &other)
{
	return (_name == other._name)
		&& (_object_type == other._object_type)
		&& (_load_address == other._load_address)
		&& (_exec_address == other._exec_address)
		&& (_length	== other._length)
		&& (_attributes == other._attributes)
		&& (_file_type	== other._file_type);
}

/**
 * Checks if this path info is different to another
 *
 * Note: name check is case-sensitive
 *
 * @param other PathInfo to check
 * @returns true if PathInfos are different
 */
bool PathInfo::operator!=(const PathInfo &other)
{
	return (_object_type != other._object_type)
		|| (_load_address != other._load_address)
		|| (_exec_address != other._exec_address)
		|| (_length	!= other._length)
		|| (_attributes != other._attributes)
		|| (_file_type	!= other._file_type)
		|| (_name != other._name);
}

/**
 * Read catalogue information for the given path
 *
 * @param path Path referring to a location on a file system
 * @returns true if information read
 */
bool PathInfo::read(const Path &path)
{
	_kernel_swi_regs regs;

	_name = path.leaf_name();

	regs.r[0] = 23;
	regs.r[1] = reinterpret_cast<int>(path.name().c_str());

	// Call OSFile
	if (_kernel_swi(0x08, &regs, &regs) == 0)
	{
		_object_type = ObjectType(regs.r[0]);
		_load_address = (unsigned int)regs.r[2];
		_exec_address = (unsigned int)regs.r[3];
		_length = regs.r[4];
		_attributes = regs.r[5];
		_file_type = regs.r[6];
	} else
	{
		_object_type = NOT_FOUND;
		_file_type = -2;
	}

	return (_object_type != NOT_FOUND);
}

/**
 * Read catalogue information for the given path always
 * returning the raw load/exec address.
 *
 * This can be used to get the actual file type for an ImageFS directory.
 *
 * @param path Path referring to a location on a file system
 * @param calc_file_type true to calculate file type from the returned catalogue information,
 * if false file_type -1 will always be returned.
 * @returns true if information read
 */
bool PathInfo::read_raw(const Path &path, bool calc_file_type)
{
	_kernel_swi_regs regs;

	_name = path.leaf_name();

	regs.r[0] = 17;
	regs.r[1] = reinterpret_cast<int>(path.name().c_str());

	// Call OSFile
	if (_kernel_swi(0x08, &regs, &regs) == 0)
	{
		_object_type = ObjectType(regs.r[0]);
		_load_address = (unsigned int)regs.r[2];
		_exec_address = (unsigned int)regs.r[3];
		_length = regs.r[4];
		_attributes = regs.r[5];
		_file_type = -1;
		if (calc_file_type)
		{
			regs.r[0] = 38; // Convert file type
			regs.r[6] = _object_type;
			if (_kernel_swi(OS_FSControl, &regs, &regs) == 0)
			{
				_file_type = regs.r[2];
			}
		}
	} else
	{
		_object_type = NOT_FOUND;
		_file_type = -2;
	}

	return (_object_type != NOT_FOUND);
}


/**
 * Return the object type read for this path
 *
 * @returns object type
 */
PathInfo::ObjectType PathInfo::object_type() const
{
	return _object_type;
}

/**
 * Check if this path info has file type information
 *
 * @returns true if the object has a file type
 */
bool PathInfo::has_file_type() const
{
	return (_file_type >= 0);
}

/**
 * Get the file type of the object.
 *
 * @returns The file type of the object or
 *  -2 if invalid object
 *  -1 if the object doesn't have a file type
 *  0x1000 if the object is a directory
 *  0x2000 if the object is an application directory
 */
int PathInfo::file_type() const
{
	return _file_type;
}

/**
 * Return the file type from the load/exec address.
 *
 * For image FS files it will return the actual file type
 * of the image file whereas file_type returns 0x1000 (directory).
 *
 * The return value is undefined for directories/applications
 * or files that do not have file types.
 *
 * The file_type() must be -1 or the PathInfo read with the
 * read_raw functions for this to give the correct results.
 *
 * @return raw file type.
 */
int PathInfo::raw_file_type() const
{
	return (_load_address & 0xFFF00) >> 8;
}

/**
 * Return the modification time of the file if it has it
 *
 * @returns modified time or UTCTime()
 */
UTCTime PathInfo::modified_time() const
{
	if (has_file_type()) return UTCTime(_load_address, _exec_address);
	else return UTCTime();
}


/**
 * Check if path info has a load address and executable address
 *
 * @returns true if this object has a load address
 */
bool PathInfo::has_load_address() const
{
	return (_file_type == -1);
}

/**
 * Get the load address for this PathInfo
 *
 * This value only refers to a real load address if has_load_address is true
 *
 * @returns load address of this object
 */
unsigned int PathInfo::load_address() const
{
	return _load_address;
}

/**
 * Get the executable address for this PathInfo
 *
 * This value only refers to a real executable address if has_load_address is true
 *
 * @returns executable address for this object
 */
unsigned int PathInfo::exec_address() const
{
	return _exec_address;
}

/**
 * Get the length of this object on the file system
 *
 * @returns the object length in number of bytes
 */
int PathInfo::length() const
{
	return _length;
}

/**
 * Return the objects file attributes
 *
 * The lower byte of the attributes are 0 or more of the values in
 * the PathInfo::Attribute enum ored together.
 *
 * @returns file attributes of the object
 */
int PathInfo::attributes() const
{
	return _attributes;
}

/**
 * Returns an iterator for the given path that will return the PathInfo
 * information for each object matched.
 *
 * @param path directory to iterate
 * @param wildCard wild carded string for iteration
 *
 * @returns iterator
 */
PathInfo::Iterator PathInfo::begin(const Path &path, const std::string &wildCard)
{
	return Iterator(path.name(), wildCard.c_str());
}

/**
 * Returns an iterator for the given path that will return the PathInfo
 * information for all objects in the directory
 *
 * @param path directory to iterate
 *
 * @returns iterator
 */
PathInfo::Iterator PathInfo::begin(const Path &path)
{
	return Iterator(path.name(), 0);
}

/**
 * Get iterator for checking end of iteration of a path
 *
 * @returns end of directory iterator
 */
PathInfo::Iterator PathInfo::end()
{
	return Iterator();
}

/**
 * Construct new iterator to iterate through a directory
 *
 * Call the PathInfo::begin method to get this
 *
 * @param dirName directory name to iterate
 * @param wildCard wild card to select certain paths
 */
PathInfo::Iterator::Iterator(const std::string &dirName, const char *wildCard)
{
	_info = new PathInfo;
	_iterBlock = new IterBlock(dirName, wildCard);
	if (_iterBlock->next_record() == 0)
	{
		_iterBlock->release();
		_iterBlock = 0;
	} else
	{
		_iterBlock->info(*_info);
		_iterBlock->next();
	}
}

// No arguments just creates the end iterator
PathInfo::Iterator::Iterator()
{
	_iterBlock = 0;
	_info = new PathInfo;
}

/**
 * Construct an the iterator as a copy of another
 *
 * @param other Iterator to copy
 */
PathInfo::Iterator::Iterator(const Iterator &other)
{
	*_info = *(other._info);
	_iterBlock = other._iterBlock;
	if (_iterBlock) _iterBlock->add_ref();
}

PathInfo::Iterator::~Iterator()
{
	if (_iterBlock) _iterBlock->release();
	delete _info;
}

/**
 * Assign this iterator to another
 *
 * @param other Iterator to copy
 * @returns *this
 */
PathInfo::Iterator &PathInfo::Iterator::operator=(const Iterator &other)
{
	*_info = *(other._info);
	if (_iterBlock) _iterBlock->release();
	_iterBlock = other._iterBlock;
	if (_iterBlock) _iterBlock->add_ref();
	return *this;
}

/**
 * Check if this iterator is the same as another
 *
 * @param other iterator to compare to
 * @returns true if iterators are the same
 */
bool PathInfo::Iterator::operator==(const Iterator &other)
{
	return (_iterBlock == other._iterBlock && *_info == *(other._info));
}

/**
 * Check if this iterator and another are different
 *
 * @param other iterator to compare to
 * @returns true if iterators are not the same
 */
bool PathInfo::Iterator::operator!=(const Iterator &other)
{
	return (_iterBlock != other._iterBlock || *_info != *(other._info));
}

/**
 * Move iterator to next item
 *
 * @returns *this
 */
PathInfo::Iterator &PathInfo::Iterator::operator++()
{
	next();
	return *this;
}

/**
 * Move iterator to next item
 *
 * @returns copy of iterator before it was moved
 */
PathInfo::Iterator PathInfo::Iterator::operator++(int)
{
	Iterator retCopy(*this);
	next();
	return retCopy;
}

/**
 * Move iterator to next item
 */
void PathInfo::Iterator::next()
{
	if (_iterBlock == 0)
	{
		*_info = PathInfo(); // Reset to invalid
		return;
	} else if (_iterBlock->next_record() == 0)
	{
		*_info = PathInfo(); // Reset to invalid
		_iterBlock->release();
		_iterBlock = 0;
		return;
	}

	_iterBlock->info(*_info);
	if (!_iterBlock->next())
	{
		_iterBlock->release();
		_iterBlock = 0;
	}
}

/**
 * Get the PathInfo for the iterator
 *
 * @returns PathInfo for the iterators current position
 */
PathInfo &PathInfo::Iterator::operator*()
{
	return *_info;
}

/**
 * Get the PathInfo for the iterator
 *
 * @returns PathInfo for the iterators current position
 */
PathInfo *PathInfo::Iterator::operator->()
{
	return _info;
}


/**
 * Constructe Iterator block for given directory and file name
 *
 * @param dirName directory name
 * @param wildCard Wild card for search
 */
PathInfo::Iterator::IterBlock::IterBlock(const std::string &dirName, const char *wildCard)
{
	_ref = 1;
	_nextRecord = 0;
	_toRead = 0;

	_dirName = new char[dirName.length()+1];
	strcpy(_dirName, dirName.c_str());
	if (wildCard == 0) _wildCard = 0;
	else
	{
		_wildCard = new char[strlen(wildCard)+1];
		strcpy(_wildCard, wildCard);
	}

	_regs.r[0] = 12; // Read file names only
	_regs.r[1] = reinterpret_cast<int>(_dirName);
	_regs.r[2] = reinterpret_cast<int>(_readData);
	_regs.r[3] = 32; // Number of objects to read
	_regs.r[4] = 0;  // First item to read, 0 to start
	_regs.r[5] = _readSize; // Buffer length
	_regs.r[6] = reinterpret_cast<int>(_wildCard);

	next();
}

/**
 * Move to next record returned from OS
 *
 * @returns true if more data to read
 */
bool PathInfo::Iterator::IterBlock::next()
{
	if (_toRead == 0)
	{
		_nextRecord = 0;

		_regs.r[3] = 32; // Number of objects to read

		if (_regs.r[4] == -1
		    || (_kernel_swi(0x0c, &_regs, &_regs) != 0)
		    || (_regs.r[4] == -1 && _regs.r[3] == 0)
		   )
		{
			return false; // No more data to read
		} else
		{
			_toRead = _regs.r[3];
			_nextRecord = _readData;
		}
	} else
	{
		_nextRecord += 24 + strlen(_nextRecord + 24) + 1;
		if ((int)_nextRecord & 3) _nextRecord += 4 - ((int)_nextRecord & 3);
	}

	_toRead--;

	return true;
}

/**
 * Get path information for iterator block
 *
 * @param info updated with path information if any
 * @returns true if there is more path information
 */
bool PathInfo::Iterator::IterBlock::info(PathInfo &info)
{
	if (_nextRecord == 0) return false;

	info._name = _nextRecord+24;
	info._load_address = *(int *)&_nextRecord[0];
	info._exec_address = *(int *)&_nextRecord[4];
	info._length	   = *(int *)&_nextRecord[8];
	info._attributes  = *(int *)&_nextRecord[12];
	info._object_type  = PathInfo::ObjectType(*(int *)&_nextRecord[16]);
	info._file_type    = *(int *)&_nextRecord[20];

	return true;
}
