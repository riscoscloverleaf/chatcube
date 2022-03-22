/*
MIT License

Copyright (c) 2017-2020 Matthias C. M. Troffaes

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include <cstring>
#include <string>
#include <iostream>
#include <list>
#include <map>
#include <algorithm>
#include <functional>
#include <cctype>
#include <sstream>

namespace inipp {

namespace detail {

// trim functions based on http://stackoverflow.com/a/217605

template <class CharT>
inline void ltrim(std::basic_string<CharT> & s) {
	s.erase(s.begin(),
                std::find_if(s.begin(), s.end(),
                             [](int ch) { return !std::isspace(ch); }));
}

template <class CharT>
inline void rtrim(std::basic_string<CharT> & s) {
	s.erase(std::find_if(s.rbegin(), s.rend(),
                             [](int ch) { return !std::isspace(ch); }).base(),
                s.end());
}
} // namespace detail

template <typename CharT, typename T>
inline bool extract(const std::basic_string<CharT> & value, T & dst) {
	CharT c;
	std::basic_istringstream<CharT> is{ value };
	T result;
	if ((is >> std::boolalpha >> result) && !(is >> c)) {
		dst = result;
		return true;
	}
	else {
		return false;
	}
}

template <typename CharT>
inline bool extract(const std::basic_string<CharT> & value, std::basic_string<CharT> & dst) {
	dst = value;
	return true;
}

template<class CharT>
class Ini
{
public:
	typedef std::basic_string<CharT> String;
	typedef std::map<String, String> Section;
	typedef std::map<String, Section> Sections;

	Sections sections;
	std::list<String> errors;

	static const CharT char_section_start  = (CharT)'[';
	static const CharT char_section_end    = (CharT)']';
	static const CharT char_assign         = (CharT)'=';
	static const CharT char_comment        = (CharT)';';

	void generate(std::basic_ostream<CharT> & os) const {
		for (auto const & sec : sections) {
			os << char_section_start << sec.first << char_section_end << std::endl;
			for (auto const & val : sec.second) {
				os << val.first << char_assign << val.second << std::endl;
			}
			os << std::endl;
		}
	}

	void parse(std::basic_istream<CharT> & is) {
		String line;
		String section;
		while (std::getline(is, line)) {
			detail::ltrim(line);
			detail::rtrim(line);
			const auto length = line.length();
			if (length > 0) {
				const auto pos = line.find_first_of(char_assign);
				const auto & front = line.front();
				if (front == char_comment) {
					continue;
				}
				else if (front == char_section_start) {
					if (line.back() == char_section_end)
						section = line.substr(1, length - 2);
					else
						errors.push_back(line);
				}
				else if (pos != 0 && pos != String::npos) {
					String variable(line.substr(0, pos));
					String value(line.substr(pos + 1, length));
					detail::rtrim(variable);
					detail::ltrim(value);
					auto & sec = sections[section];
					if (sec.find(variable) == sec.end())
						sec.insert(std::make_pair(variable, value));
					else
						errors.push_back(line);
				}
				else {
					errors.push_back(line);
				}
			}
		}
	}

	void clear() {
		sections.clear();
		errors.clear();
	}
};

} // namespace inipp
