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



#ifndef TBX_DRAGHANDLER_H_
#define TBX_DRAGHANDLER_H_

namespace tbx
{
	/**
	 * Class to report status of a drag started on a window.
	 */
	class DragHandler
	{
	public:
		/**
		 * Destructor
		 */
		virtual ~DragHandler() {}

		/**
		 * Called when drag has finished
		 *
		 * @param final final location of the drag box in screen coordinates.
		 * 		  final.min point is inclusive
		 *        final.max point is exclusive
		 */
		virtual void drag_finished(const BBox &final) = 0;

		/**
		 * Call if drag was cancelled before if finished
		 */
		virtual void drag_cancelled() {};
	};
}

#endif /* TBX_DRAGHANDLER_H_ */
