/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2010-2013 Alan Buckley   All Rights Reserved.
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
 * stringutils.h
 *
 *  Created on: 02-Apr-2009
 *      Author: alanb
 */

#ifndef TBX_STRINGUTILS_H_
#define TBX_STRINGUTILS_H_

#include <sstream>
#include <cstring>

namespace tbx
{

/**
 * Convert a value to a string.
 */
template<class T> std::string to_string(const T &value)
{
	std::ostringstream ss;
	ss << value;
	return ss.str();
}

/** Specialisation for std::string */
inline std::string to_string(const std::string &value)
{
	return value;
}

/** Specialisation for constant null terminated string */
inline std::string to_string(const char *value)
{
	return std::string(value);
}

/** Specialisation for null terminated string */
inline std::string to_string(char *value)
{
	return std::string(value);
}

/**
 * Convert a string to another type
 */
template<class T> T from_string(const std::string &str)
{
	T result;
	std::istringstream ss(str);
	ss >> result;
	return result;
}

/**
 * Return string converted to upper case
 */
inline std::string to_upper(const std::string &value)
{
	std::string result;
	for (std::string::const_iterator i = value.begin(); i != value.end(); ++i)
	{
		result += std::toupper(*i);
	}
	return result;
}

/**
 * Return string converted to lower case
 */
inline std::string to_lower(const std::string &value)
{
	std::string result;
	for (std::string::const_iterator i = value.begin(); i != value.end(); ++i)
	{
		result += std::tolower(*i);
	}
	return result;
}

/**
 * Compare two strings ignoring case
 */
inline bool equals_ignore_case(const std::string &s1, const std::string &s2)
{
	if (s1.length() != s2.length()) return false;
	std::string::const_iterator i1, i2 = s2.begin();
	for (i1 = s1.begin(); i1 != s1.end(); ++i1, ++i2)
		if (std::tolower(*i1) != std::tolower(*i2)) return false;
	return true;
}

/**
 * Compares two string ignoring case
 */
inline bool equals_ignore_case(const std::string &s1, const char *cs2)
{
	return (stricmp(s1.c_str(), cs2) == 0);
}

/**
 * Compares two string ignoring case
 */
inline bool equals_ignore_case(const char *cs1, const std::string &s2)
{
	return equals_ignore_case(s2, cs1);
}

/**
 * Compares two string ignoring case
 */
inline bool equals_ignore_case(const char *cs1, const char *cs2)
{
	return (stricmp(cs1, cs2) == 0);
}

/**
 * Compares two strings ignoring case
 *
 * @return < 0 if string 1 is < string 2
 *         0 if equal
 *         > 0 if string 1 is > string 2
 */
inline int compare_ignore_case(const char *cs1, const char *cs2)
{
	return stricmp(cs1,cs2);
}

/**
 * Compares two strings ignoring case
 *
 * @return < 0 if string 1 is < string 2
 *         0 if equal
 *         > 0 if string 1 is > string 2
 */
inline int compare_ignore_case(const std::string &s1, const char *cs2)
{
	return stricmp(s1.c_str(),cs2);
}

/**
 * Compares two strings ignoring case
 *
 * @return < 0 if string 1 is < string 2
 *         0 if equal
 *         > 0 if string 1 is > string 2
 */
inline int compare_ignore_case(const char *cs1, const std::string &s2)
{
	return stricmp(cs1,s2.c_str());
}
/**
 * Compares two strings ignoring case
 *
 * @return < 0 if string 1 is < string 2
 *         0 if equal
 *         > 0 if string 1 is > string 2
 */
inline int compare_ignore_case(const std::string &s1, const std::string &s2)
{
	return stricmp(s1.c_str(),s2.c_str());
}

/**
 * Find a string in another ignoring case
 *
 * @param s string to search
 * @param f string to look for
 * @param start start position (defaults to 0)
 *
 * @returns position found or std::string::npos if not found
 */
inline std::string::size_type find_ignore_case(const std::string &s, std::string f, std::string::size_type start = 0)
{
	if (f.empty()) return std::string::npos;
	if (s.length() < f.length()) return std::string::npos;

	std::string::size_type pos;
	std::string::size_type max_pos = s.length() - f.length();

	char fchar[2];
	fchar[0] = std::tolower(f[0]);
	fchar[1] = std::toupper(f[0]);

	while ((pos = s.find_first_of(fchar, start))!=std::string::npos)
	{
		if (pos > max_pos) return std::string::npos;
		bool found = true;
		for (std::string::size_type i = 1; i < f.length() && found; i++)
			found = (std::tolower(s[pos+i]) == std::tolower(f[i]));
		if (found) return pos;
		start = pos + 1;
	}

	return std::string::npos;
}

}

#endif /* TBX_STRINGUTILS_H_ */
