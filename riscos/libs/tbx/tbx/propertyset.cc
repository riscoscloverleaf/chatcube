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

#include "propertyset.h"
#include <sstream>
#include <fstream>
#include <cstdlib>

namespace tbx {

bool PropertySet::exists(std::string name) const
{
	std::map<std::string, std::string>::const_iterator i = _properties.find(name);
	return (i != _properties.end());
}

void PropertySet::set(std::string name, std::string value)
{
	_properties[name] = value;
}

void PropertySet::set(std::string name, const char *value)
{
	_properties[name] = value;
}


std::string PropertySet::get(std::string name, const char *def /*=""*/) const
{
	std::map<std::string, std::string>::const_iterator i = _properties.find(name);
	return (i != _properties.end()) ? (*i).second : def;
}

void PropertySet::set(std::string name, int value)
{
	std::ostringstream os;
	os << value;
	set(name, os.str());
}

int PropertySet::get(std::string name, int def) const
{
	std::map<std::string, std::string>::const_iterator i = _properties.find(name);
	return (i != _properties.end()) ? atoi((*i).second.c_str()) : def;
}

void PropertySet::set(std::string name, bool value)
{
	set(name, (value) ? "true" : "false");
}

bool PropertySet::get(std::string name, bool def) const
{
	std::map<std::string, std::string>::const_iterator i = _properties.find(name);
	bool retBool = def;
	if (i != _properties.end())
	{
		if ((*i).second == "true") retBool = true;
		else if ((*i).second == "false") retBool = false;
	}

	return retBool;
}

void PropertySet::set_indexed(std::string name, int index, std::string value)
{
	std::ostringstream fullname;
	fullname << name << index;
	set(fullname.str(), value);
}

std::string PropertySet::get_indexed(std::string name, int index, const char *def/* = ""*/) const
{
	std::ostringstream fullname;
	fullname << name << index;
	return get(fullname.str(), def);
}

void PropertySet::set_indexed(std::string name, int index, int value)
{
	std::ostringstream fullname;
	fullname << name << index;
	set(fullname.str(), value);
}

int PropertySet::get_indexed(std::string name, int index, int def) const
{
	std::ostringstream fullname;
	fullname << name << index;
	return get(fullname.str(), def);
}

void PropertySet::set_indexed(std::string name, int index, bool value)
{
	std::ostringstream fullname;
	fullname << name << index;
	set(fullname.str(), value);
}

bool PropertySet::get_indexed(std::string name, int index, bool def) const
{
	std::ostringstream fullname;
	fullname << name << index;
	return get(fullname.str(), def);
}

bool PropertySet::erase(std::string name)
{
	std::map<std::string, std::string>::iterator i = _properties.find(name);
	if (i != _properties.end())
	{
	   _properties.erase(i);
	   return true;
	}

	return false;
}

bool PropertySet::exists_indexed(std::string name, int index)
{
	std::ostringstream fullname;
	fullname << name << index;
	return exists(fullname.str());
}

bool PropertySet::erase_indexed(std::string name, int index)
{
	std::ostringstream fullname;
	fullname << name << index;
	return erase(fullname.str());
}


bool PropertySet::write(std::ostream &os) const
{
	std::map<std::string, std::string>::const_iterator i;
	std::string value;

	for (i = _properties.begin(); i != _properties.end(); ++i)
	{
		os << (*i).first << "=";
		value =(*i).second;
		if (!value.empty() && (value[0] == ' ' || value[0] == '"' || *(value.rbegin()) == ' '))
		{
               os << '"' << value << '"';
		} else os << value;
		os << std::endl;
	}

	return os.fail();
}

bool PropertySet::read(std::istream &is)
{
	std::string line, name, value;
	std::string::size_type equalsPos;

	while (!is.eof())
	{
		getline(is, line);
		while (!line.empty() && line[0] <= ' ') line.erase(0);
		if (!line.empty() && line[0] != ';')
		{
			equalsPos = line.find('=');
			if (equalsPos != std::string::npos && equalsPos > 0)
			{
				std::string::size_type idx = equalsPos - 1;
				while(idx > 0 && line[idx] == ' ') idx--;
				name = line.substr(0, idx+1);
				idx = equalsPos+1;
				while (idx < line.size() && line[idx] == ' ') idx++;
				if (idx == line.size()) value.clear();
				else value = line.substr(idx, line.size() - idx);
				if (!value.empty() && value[0] == '"')
				{
					std::string::size_type endQuote = value.rfind('"');
					if (endQuote == std::string::npos || endQuote <= 1)
					{
					   value.clear();
					} else
					   value = value.substr(1, endQuote-1);
				} else
				{
					while (!value.empty() && *(value.rbegin()) <= ' ') value.erase(value.length()-1);
				}
				set(name, value);
			}
		}
	}

	return is.fail();
}

void PropertySet::clear()
{
	_properties.clear();
}

bool PropertySet::empty()
{
	return _properties.empty();
}

bool PropertySet::save(std::string file_name) const
{
	std::ofstream os(file_name.c_str());
	if (!os) return false;
	return write(os);
}

bool PropertySet::load(std::string file_name)
{
	std::ifstream is(file_name.c_str());
	if (!is) return false;
	clear();
	return read(is);
}

}
