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

#ifndef TBX_PROPERTYSET_H_
#define TBX_PROPERTYSET_H_

#include <string>
#include <map>
#include <iostream>

namespace tbx {

/**
* Maintain a simple set of string properties with
* their values.
*
* The property names are case sensitive
*/
class PropertySet {
public:

	/**
	* Check if property is a member of this property set
	*
	* @param name name of property to check
	* @returns true if property exists
	*/
	bool exists(std::string name) const;

	/**
	* Set the value of a string property.
	*
	* @param name property name to set
	* @param value new value
	*/
	void set(std::string name, std::string value);

	/**
	* Set the value of a string property from a C string.
	*
	* @param name property name to set
	* @param value new value
	*/
	void set(std::string name, const char *value);

	/**
	* Get the value of a property as a string
	*
	* @param name property name to get
	* @param def default value if the property is not in the property set
	* @returns value of string property returns the default value if property does not exist
	*/
	std::string get(std::string name, const char *def ="") const;

	/**
	* Set the value of an integer property
	*
	* @param name property name to set
	* @param value new value
	*/
	void set(std::string name, int value);

	/**
	 * Get the value of an integer property
	 *
	 * @param name property name to get
	 * @param def default value if the property is not in the property set
	 * @returns value of property or def if not set
	 */
	int get(std::string name, int def) const;

	/**
	* Set the value of a boolean property
	*
	* @param name property name to set
	* @param value new value
	*/
	void set(std::string name, bool value);
	/**
	 * Get the value of an boolean property
	 *
	 * @param name property name to get
	 * @param def default value if the property is not in the property set
	 * @returns value of property or def if not set
	 */
	bool get(std::string name, bool def) const;

	/**
	* Erases a property from the set.
	*
	*@param name property name to erase
	*@return true if property existed in the set
	*/
	bool erase(std::string name);

	/**
	* Set an indexed string property
	*
	* Short cut that just appends the index to the property name
	* before using it in the above routines.
	*
	* @param name property name to set
	* @param index index to be appended to the property name
	* @param value new value
	*/
	void set_indexed(std::string name, int index, std::string value);
	/**
	 * Get the value of an indexed string property
	 *
 	 * Short cut that just appends the index to the property name
	 * before using it in the above routines.
	 *
	 * @param name property name to get
	 * @param index index to be appended to the property name
	 * @param def default value if the property is not in the property set
	 * @returns value of property or def if not set
	 */
	std::string get_indexed(std::string name, int index, const char *def = "") const;
	/**
	* Set an indexed integer property
	*
	* Short cut that just appends the index to the property name
	* before using it in the above routines.
	*
	* @param name property name to set
	* @param index index to be appended to the property name
	* @param value new value
	*/
	void set_indexed(std::string name, int index, int value);
	/**
	 * Get the value of an indexed integer property
	 *
 	 * Short cut that just appends the index to the property name
	 * before using it in the above routines.
	 *
	 * @param name property name to get
	 * @param index index to be appended to the property name
	 * @param def default value if the property is not in the property set
	 * @returns value of property or def if not set
	 */
	int get_indexed(std::string name, int index, int def) const;
	/**
	* Set an indexed boolean property
	*
	* Short cut that just appends the index to the property name
	* before using it in the above routines.
	*
	* @param name property name to set
	* @param index index to be appended to the property name
	* @param value new value
	*/
	void set_indexed(std::string name, int index, bool value);
	/**
	 * Get the value of an indexed boolean property
	 *
 	 * Short cut that just appends the index to the property name
	 * before using it in the above routines.
	 *
	 * @param name property name to get
	 * @param index index to be appended to the property name
	 * @param def default value if the property is not in the property set
	 * @returns value of property or def if not set
	 */
	bool get_indexed(std::string name, int index, bool def) const;

	/**
	 * Check if the property set contains the indexed property
     *
	 * @param name property name to check
	 * @param index index to be appended to the property name
	 * @returns true if indexed property exists
	 */
	bool exists_indexed(std::string name, int index);
	/**
	 * Erase an indexed property
     *
	 * @param name property name to erased
	 * @param index index to be appended to the property name
	 * @returns true if indexed property was erased
	 */
	bool erase_indexed(std::string name, int index);

	/**
	* Writes the property list to a stream
	*
	* @param os stream to write list to
	* @returns true if successful
	*/
	bool write(std::ostream &os) const;

	/**
	* Reads the properties from a stream
	*
	* Property list is assumed to continue until the end of the stream
	*
	* Note: Any string property values will have leading and
	* trailing spaces removed.
	*
	* @param is stream to read properties from
	* @returns true if successful
	*/
	bool read(std::istream &is);

	/**
	* Remove all properties from the property set
	*/
	void clear();

	/**
	 * Check if property set is empty
	 */
	bool empty();
	/**
	 * Save property set to a file
	 *
	 * @param file_name name of file to save to
	 * @returns true if successful
	 */
	bool save(std::string file_name) const;
	/**
	 * Load property set from a file
	 *
	 * @param file_name name of file to load from
	 * @returns true if successful
	 */
	bool load(std::string file_name);

protected:
	/**
	 * Underlying map to contain the properties
	 */
   std::map<std::string,std::string> _properties;
};

}

#endif /* TBX_PROPERTYSET_H_ */
