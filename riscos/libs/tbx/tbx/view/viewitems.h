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
 * viewitems.h
 *
 *  Created on: 27 Apr 2010
 *      Author: alanb
 */

#ifndef TBX_VIEWITEMS_H_
#define TBX_VIEWITEMS_H_

#include <vector>
#include "itemview.h"

namespace tbx
{
namespace view
{

/**
 * Template class to store a list of items for an item view
 * and to call the item view update routines automatically
 * where possible.
 */
template<class T> class ViewItems
{
	std::vector<T> _items;
	ItemView *_view;

public:
	/**
	 * Construct the view items
	 *
	 * @param v view for items (default to 0 so view will be set later)
	 */
	ViewItems(ItemView *v = 0) : _view(v) {}

	/**
	 * Set view for the items
	 */
	void view(ItemView *v)
	{
		if (_view) _view->cleared();
		_view = v;
		if (v)
		{
			v->cleared();
			if (_items.size()) v->inserted(0, _items.size());
		}
	}

	/**
	 * Return size of items
	 */
	unsigned int size() const {return _items.size();}

	/**
	 * Set the item at a specified index
	 */
	void item(unsigned int index, const T&item)
	{
		if (_view) _view->changing(index,1);
		_items[index] = item;
		if (_view) _view->changed(index,1);
	}

	/**
	 * Get the item at the specified index
	 */
	const T &item(unsigned int index) const
	{
		return _items[index];
	}

	/**
	 * Get item at specified index
	 */
	const T &operator[](unsigned int index) const
	{
		return _items[index];
	}

	/**
	 * If the items contain method and fields that can be
	 * changed call this function prior to a change to
	 * inform the view that a change is about to occur.
	 *
	 * @param index first index of item changed
	 * @param how_many number of items changed
	 */
	void changing(unsigned int index, unsigned int how_many = 1)
	{
		if (_view) _view->changing(index, how_many);
	}

	/**
	 * If the items contain method and fields that can be
	 * changed call this function after a change to
	 * inform the view that a change has occurred.
	 *
	 * @param index first index of item changed
	 * @param how_many number of items changed
	 */
	void changed(unsigned int index, unsigned int how_many = 1)
	{
		if (_view) _view->changed(index, how_many);
	}

	/**
	 * Add an item to the end of the list
	 *
	 * @param item the item to add
	 */
	void add(const T& item)
	{
		_items.push_back(item);
		if (_view) _view->inserted(_items.size()-1, 1);
	}

	/**
	 * Insert an item in the list.
	 *
	 * @param index for insert (if the same as size() will add to the
	 *  end of the list)
	 * @param item item to insert
	 */
	void insert(unsigned int index, const T&item)
	{
		if (index == _items.size()) add(item);
		else
		{
			_items.insert(_items.begin() + index, item);
			if (_view) _view->inserted(index, 1);
		}
	}

	/**
	 * Delete an item from a list
	 */
	void erase(unsigned int index)
	{
		if (_view) _view->removing(index, 1);
		_items.erase(_items.begin() + index);
		if (_view) _view->removed(index, 1);
	}

	/**
	 * Clear the whole list
	 */
	void clear()
	{
		_items.clear();
		if (_view) _view->cleared();
	}
};


}
}

#endif /* TBX_VIEWITEMS_H_ */
