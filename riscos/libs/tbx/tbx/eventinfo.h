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
 * eventinfo.h
 *
 *  Created on: 5 Jul 2010
 *      Author: alanb
 */

#ifndef TBX_EVENTINFO_H_
#define TBX_EVENTINFO_H_

#include "pollinfo.h"

namespace tbx
{
	/**
	 * Class to provide information on a toolbox event.
	 *
	 * If an event has extra information and class derived from
	 * this event is passed otherwise this event is passed.
	 *
	 * The base class gives access to the toolbox id block
	 *
	 */
	class EventInfo
	{
	private:
		const IdBlock &_id_block;
	protected:
		PollBlock &_data; //!< raw data from the event

	public:
		/**
		 * Contruct the event information from the id_block
		 * and data update by the toolbox
		 */
		EventInfo(const IdBlock &id_block, PollBlock &data) :
				_id_block(id_block), _data(data)
			{
			}

		/**
		 * IdBlock for the current event.
		 *
		 * This returns information on the object/component where event took
		 * place.
		 */
		const IdBlock &id_block() const {return _id_block;}
	};
}


#endif /* TBX_EVENTINFO_H_ */
