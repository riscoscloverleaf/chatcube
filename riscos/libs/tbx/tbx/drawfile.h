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
 * drawfile.h
 *
 *  Created on: 2 Nov 2010
 *      Author: alanb
 */

#ifndef TBX_DRAWFILE_H_
#define TBX_DRAWFILE_H_

#include "image.h"
#include "bbox.h"
#include "drawtransform.h"
#include <string>

namespace tbx {

/**
 * Class to render a draw file to the screen.
 *
 * This class uses the DrawFile module to render
 * the drawfile.
 */
class DrawFile : public Image
{
private:
	char *_data;
	int _size;
public:
	DrawFile();
	DrawFile(const DrawFile &other);
	virtual ~DrawFile();

	DrawFile &operator=(const DrawFile &other);

	bool load(const std::string &file_name);

	/**
	 * Check if a draw file has been loaded.
	 */
	bool is_valid() const {return _data != 0;}

	// Image overrides
	virtual void plot(int x, int y) const;
	virtual void plot(const tbx::Point &pt) const;

	void render(DrawTransform *dt = 0, BBox *clip = 0, int flatness = -1 ) const;
	void bounds(BBox &bounds, DrawTransform *dt = 0) const;
	void declare_fonts(bool download_fonts = true) const;

};

}

#endif /* DRAWFILE_H_ */
