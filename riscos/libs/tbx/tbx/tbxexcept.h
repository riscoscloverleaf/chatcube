/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2010-2021 Alan Buckley   All Rights Reserved.
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

#ifndef TBXEXCEPT_H_
#define TBXEXCEPT_H_

#include <stdexcept>

namespace tbx
{
/**
 * Exception thrown when the underlying toolbox object class doesn't
 * match the C++ class used to represent the toolbox object.
 */
class ObjectClassError : public std::invalid_argument
{
public:
	ObjectClassError() : std::invalid_argument("C++ object/toolbox object class mismatch") {}
};

/**
 * Exception thrown when an underlying toolbox gadget id doesn't
 * match the C++ class used to represent the toolbox gadget
 */
class GadgetClassError : public std::invalid_argument
{
public:
	GadgetClassError() : std::invalid_argument("C++ gadget/toolbox gadget class mismatch") {}
};

/**
 * Exception thrown when a C++ object representing a toolbox object
 * uses a method that requires a non-null toolbox handle.
 */
class ObjectNullError : public std::invalid_argument
{
public:
	ObjectNullError() : std::invalid_argument("NULL toolbox object handle") {};
};

/**
 * Exception thrown when a C++ object representing a toolbox gadget
 * uses a method that requires a non-null gadget id.
 */
class ComponentNullError : public std::invalid_argument
{
public:
	ComponentNullError() : std::invalid_argument("NULL gagdet id") {};
};

/**
 * Exception thrown is a component id does not refer to an item in
 * a menu resource.
 */
class ResMenuItemError : public std::invalid_argument
{
public:
	ResMenuItemError() : std::invalid_argument("Component ID isn't in menu resource") {};
};
}

#endif /* TBXEXCEPT_H_ */
