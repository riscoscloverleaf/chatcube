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
 * drawfile.cpp
 *
 *  Created on: 2 Nov 2010
 *      Author: alanb
 */

#include "drawfile.h"
#include "kernel.h"
#include <fstream>
#include <cstring>

namespace tbx {

/**
 * Construct an unloaded draw file
 */
DrawFile::DrawFile()
{
	_data = 0;
	_size = 0;
}

/**
 * Construct a copy of another drawfile
 */
DrawFile::DrawFile(const DrawFile &other)
{
	delete [] _data;
	if (other._data)
	{
		_data = new char[other._size];
		_size = other._size;
		std::memcpy(_data, other._data, _size);
	} else
	{
		_data = 0;
		_size = 0;
	}
}

/**
 * Destructor deletes loaded drawfile
 */
DrawFile::~DrawFile()
{
	delete [] _data;
}

/**
 * Make into a copy of another draw file
 */
DrawFile &DrawFile::operator=(const DrawFile &other)
{
	delete [] _data;
	if (other._data)
	{
		_data = new char[other._size];
		_size = other._size;
		std::memcpy(_data, other._data, _size);
	} else
	{
		_data = 0;
		_size = 0;
	}

	return *this;
}

/**
 * Load JPEG from file
 *
 * @param file_name name of file to load from
 * @returns true if load succeeded
 */
bool DrawFile::load(const std::string &file_name)
{
	std::ifstream file(file_name.c_str());
	bool loaded = false;

	if (file.is_open())
	{
		file.seekg(0, std::ios_base::end);
		int size = file.tellg();
		if (size > 4)
		{
			if (_data) delete[] _data;
			_data = 0;
			_size = 0;
			file.seekg(0, std::ios_base::beg);
			_data = new char[size];
			_size = size;
			file.read(_data, _size);
			if (*((int *)_data) == 0x77617244) // "Draw"
			{
				loaded = true;
			} else
			{
				delete [] _data;
				_data = 0;
				_size = 0;
			}
		}
	}

	return loaded;
}

/**
 * Plot draw file at given location
 *
 * @param x x coordinate for plot in os units
 * @param y y coordinate for plot in os units
 */
void DrawFile::plot(int x, int y) const
{
	DrawTransform dt;
	dt.translate_os(x,y);
	render(&dt);
}

/**
 * Plot draw file at given location
 *
 * @param pt location for plot in os units
 */
void DrawFile::plot(const tbx::Point &pt) const
{
	DrawTransform dt;
	dt.translate_os(pt.x,pt.y);
	render(&dt);
}

/**
 * Render a draw file to screen
 *
 * @param dt DrawTransform to position, rotate and translate drawing
 *  or 0 for identity transform.
 * @param clip point to box to clip the drawing or 0 for no clip
 * @param flatness for curves or -1 if not specified.
 */
void DrawFile::render(DrawTransform *dt /*= 0*/, BBox *clip /*= 0*/, int flatness /*= -1*/ ) const
{
	if (_data == 0) return;

	_kernel_swi_regs regs;
	regs.r[0] = 0;
	regs.r[1] = (int)_data;
	regs.r[2] = _size;
	regs.r[3] = (int)dt;
	regs.r[4] = (int)clip;
	if (flatness > 0)
	{
		regs.r[0] |= 4;
		regs.r[5] = flatness;
	}
	_kernel_swi(0x45540, &regs, &regs);

}

/**
 * Return the bounding box for a drawfile with the given transform
 *
 * Note: bounds will not be updated if a valid draw file has not been
 * loaded.
 *
 * @param bounds bounds to return - note the coordinates return are in 256ths of an os unit
 * @param dt Transformation applied to the drawing.
 */
void DrawFile::bounds(BBox &bounds, DrawTransform *dt /* = 0*/) const
{
	if (_data == 0) return;

	_kernel_swi_regs regs;
	regs.r[0] = 0;
	regs.r[1] = (int)_data;
	regs.r[2] = _size;
	regs.r[3] = (int)dt;
	regs.r[4] = (int)&bounds.min.x;

	_kernel_swi(0x45541, &regs, &regs);
}

/**
 * Declare all the fonts in the drawfile for printing using
 * PDriver_DeclareFonts
 */
void DrawFile::declare_fonts(bool download_fonts /*= true*/) const
{
	if (_data == 0) return;

	_kernel_swi_regs regs;
	regs.r[0] = (download_fonts) ? 0 : 1;
	regs.r[1] = (int)_data;
	regs.r[2] = _size;

	_kernel_swi(45542, &regs, &regs);
}

}
