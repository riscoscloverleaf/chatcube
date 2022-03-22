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
 * Selection.cc
 *
 *  Created on: 12 Mar 2010
 *      Author: alanb
 */

#include "selection.h"
#include <algorithm>
#include <stdexcept>

namespace tbx
{
namespace view
{

/**
 * Helper for subclasses to fire Selection Changed Events
 */
void Selection::fire_event(const SelectionChangedEvent &event)
{
	std::vector<SelectionListener *>::iterator i;
	for (i = _listeners.begin(); i != _listeners.end(); ++i)
	{
		(*i)->selection_changed(event);
	}
}
/**
 * Helper for subclasses to fire Selection Changed Events
 */
void Selection::fire_event(unsigned int index, bool selected, bool final)
{
	SelectionChangedEvent e(index, index, selected, final);
	fire_event(e);
}
/**
 * Helper for subclasses to fire Selection Changed Events
 */
void Selection::fire_event(unsigned int from, unsigned int to, bool selected, bool final)
{
	SelectionChangedEvent e(from, to, selected, final);
	fire_event(e);
}

/**
 * Add listener for selection changes
 */
void Selection::add_listener(SelectionListener *listener)
{
	_listeners.push_back(listener);
}

/**
 * Remove listener for selection changes
 */
void Selection::remove_listener(SelectionListener *listener)
{
	std::vector<SelectionListener *>::iterator found =
			std::find(_listeners.begin(), _listeners.end(), listener);
	if (found != _listeners.end())
	{
		_listeners.erase(found);
	}
}

/**
 * Get iterator to first selected item
 *
 * @returns iterator to first selected item or end() if none.
 */
Selection::Iterator Selection::begin() const
{
	return Iterator(get_iterator_impl());
}

/**
 * Get iterator to end of selection
 *
 * @returns iterator to item after last item in the selection.
 */
Selection::Iterator Selection::end() const
{
	return Iterator(0);
}

/**
 * Copy constructor
 *
 * @param other selection iterator to copy
 */
Selection::Iterator::Iterator(const Selection::Iterator &other)
{
	_impl = other._impl;
	if (_impl) _impl->add_ref();
}

// Destructor
Selection::Iterator::~Iterator()
{
	if (_impl) _impl->release();
}

/**
 * Assignment
 */
Selection::Iterator &Selection::Iterator::operator=(const Selection::Iterator &other)
{
	if (_impl != other._impl)
	{
		_impl->release();
		_impl=other._impl;
		if (_impl) _impl->add_ref();
	}
	return *this;
}

/**
 * Compare two iterators
 */
bool Selection::Iterator::operator==(const Selection::Iterator &other) const
{
	unsigned int index = (_impl == 0) ? NO_SELECTION : _impl->index();
	unsigned int check = (other._impl == 0) ? NO_SELECTION : other._impl->index();
	return (index == check);
}

/**
 * Compare two iterators
 */
bool Selection::Iterator::operator!=(const Selection::Iterator &other) const
{
	unsigned int index = (_impl == 0) ? NO_SELECTION : _impl->index();
	unsigned int check = (other._impl == 0) ? NO_SELECTION : other._impl->index();
	return (index != check);
}


/**
 * Get index for iterator
 */
unsigned int Selection::Iterator::operator*() const
{
	return (_impl) ? _impl->index() : NO_SELECTION;
}

/**
 * Prefix operator
 */
Selection::Iterator &Selection::Iterator::operator++()
{
	if (_impl)
	{
		if (_impl->shared())
		{
			_impl->release();
			_impl = _impl->clone();
		}
		_impl->next();
	}
	return *this;
}

/**
 * Postfix operator
 */
Selection::Iterator Selection::Iterator::operator++(int)
{
	Iterator tmp(_impl);
	if (_impl)
	{
		_impl = _impl->clone();
		_impl->next();
	}
	return *this;
}


/**
 * Called by the object selection is on when new items have
 * been inserted and selected item need to be moved
 *
 * @param index of the insertion
 * @param count of items inserted
 */
 void SingleSelection::inserted(unsigned int index, unsigned int count)
 {
	 if (_selected != NO_SELECTION && _selected >= index)
	 {
		 _selected += count;
	 }
 }

/**
 * Called by the object selection is on when items have
 * been removed and selected item to be deleted or moved
 *
 * @param index of the insertion
 * @param count of items inserted
 */
 void SingleSelection::removed(unsigned int index, unsigned int count)
 {
     if (_selected != NO_SELECTION && _selected >= index)
     {
    	 if (_selected < index + count)
    	 {
    		 _selected = NO_SELECTION;
    	 } else
    	 {
    		 _selected -= count;
    	 }
     }
 }

/**
 * Clear the current selection.
 */
void SingleSelection::clear()
{
	if (_selected != NO_SELECTION)
	{
		unsigned int old = _selected;
		_selected = NO_SELECTION;
		fire_event(old, false, true);
	}
}

/**
 * Select an item.
 *
 * Causes current item to be deselected so in this
 * case is identical to set(index)
 *
 * @param index to select
 */
void SingleSelection::select(unsigned int index)
{
	set(index);
}

/**
 * Select an item.
 *
 * Caused current selected item to be deselected.
 *
 * @param index to select
 */
void SingleSelection::set(unsigned int index)
{
   if (index != _selected)
   {
		if (_selected != NO_SELECTION)
		{
			unsigned int old = _selected;
			_selected = NO_SELECTION;
			fire_event(old, false, false);
		}

		_selected = index;

		fire_event(_selected, true, true);
   }
}

/**
 * Clear the selection for an item
 *
 * @param index to deselect
 */
void SingleSelection::deselect(unsigned int index)
{
	if (_selected == index)
	{
		_selected = NO_SELECTION;
		fire_event(index, false, true);
	}
}

/**
 * Toggle the selection for an item
 *
 * @param index to toggle
 */
void SingleSelection::toggle(unsigned int index)
{
	if (index == _selected) deselect(index);
	else select(index);
}

/**
 * Selection a range of items.
 *
 * For a single selection from and to must be identical and this
 * is equivalent to set(from).
 *
 * @param from first index to select
 * @param to last index to select
 * @throws std::runtime_error if from != to
 */
void SingleSelection::select(unsigned int from, unsigned int to)
{
	if (from != to) throw std::runtime_error("from != to in SingleSelection::select");
	set(from);
}

/**
 * Set the selection a range of items.
 *
 * For a single selection from and to must be identical
 *
 * @param from first index to select
 * @param to last index to select
 * @throws std::runtime_error if from != to
 */
void SingleSelection::set(unsigned int from, unsigned int to)
{
	if (from != to) throw std::runtime_error("from != to in SingleSelection::set");
	set(from);
}

/**
 * Deselect a range of items
 *
 * For a single selection from and to must be identical
 *
 * @param from first index to deselect
 * @param to last index to deselect
 * @throws std::runtime_error if from != to
 */
void SingleSelection::deselect(unsigned int from, unsigned int to)
{
	if (from != to) throw std::runtime_error("from != to in SingleSelection::deselect");
	deselect(from);
}

/**
 * Toggle the selection of a range of items
 *
 * For a single selection from and to must be identical
 *
 * @param from first index to toggle
 * @param to last index to toggle
 * @throws std::runtime_error if from != to
 */
void SingleSelection::toggle(unsigned int from, unsigned int to)
{
	if (from != to) throw std::runtime_error("from != to in SingleSelection::toggle");
	toggle(from);
}


/**
 * Find the first range where the last value is ge to the given index
 */
MultiSelection::RangeIterator MultiSelection::find_last_ge(unsigned int index)
{
	if (_first == NO_SELECTION || index > _last) return _selected.end();
	if (index <= _first) return _selected.begin();
	RangeIterator i = _selected.begin();
	while (index > i->second) i++;
	return i;
}

/**
 * Fire events from an arrays of changes to the selection
 *
 * Also ensures last change is marked as final
 */
void MultiSelection::fire_changes(std::vector<SelectionChangedEvent> &changes)
{
	if (!changes.empty())
	{
		changes.back().final(true);
		std::vector<SelectionChangedEvent>::iterator c;
		for (c = changes.begin(); c != changes.end(); ++c)
		{
			fire_event(*c);
		}
	}
}

/**
 * Check if the given index is selected
 *
 * @param index to check
 * @return true if index is selected
 */
bool MultiSelection::selected(unsigned int index) const
{
	if (_first == NO_SELECTION || index < _first || index > _last) return false;

	std::vector<Range>::const_iterator i;
	for (i = _selected.begin(); i != _selected.end(); ++i)
	{
		if (index < (*i).first) return false;
		if (index <= (*i).second) return true;
	}

	return false;
}

/**
 * Count number of selected items
 */
unsigned int MultiSelection::count() const
{
	std::vector<Range>::const_iterator i;
	unsigned int c = 0;
	for (i = _selected.begin(); i != _selected.end(); ++i)
	{
		c += (*i).second - (*i).first + 1;
	}

	return c;
}

/**
 * Data has been inserted so adjust selection
 */
void MultiSelection::inserted(unsigned int index, unsigned int count)
{
	if (_first == NO_SELECTION || index > _last) return;

	RangeIterator i = find_last_ge(index);
	if (i != _selected.end())
	{
		if (i->first < index)
		{
			Range extra(i->first, index-1);
			i->first = index;
			i = _selected.insert(i, extra);
			i++;
		}
	}
	while (i != _selected.end())
	{
		i->first += count;
		i->second += count;
	}

	if (index <= _first) _first += count;
	_last += count;
}

/**
 * Data has been removed so adjust selection
 */
void MultiSelection::removed(unsigned int index, unsigned int count)
{
	if (_first == NO_SELECTION || index > _last) return;

	RangeIterator i = find_last_ge(index);
	if (i != _selected.end())
	{
		if (i->first < index)
		{
			if (i->second < i->first + count) i->second = index-1;
			else i->second -= count;
			++i;
		}
	}
	while (i != _selected.end())
	{
		if (i->second < index + count) i = _selected.erase(i);
		else
		{
			i->first -= count;
			i->second -= count;
			++i;
		}
	}

	if (_selected.empty())
	{
		_first = _last = NO_SELECTION;
	} else
	{
		_first = _selected.front().first;
		_last = _selected.back().second;
	}
}

/**
 * Clear the selection
 */
void MultiSelection::clear()
{
	if (_first == NO_SELECTION) return;

	std::vector<Range> old(_selected.begin(), _selected.end());
	_selected.clear();
	_first = _last = NO_SELECTION;
	std::vector<Range>::iterator i = old.begin();
	SelectionChangedEvent e;
	e.selected(false);
	e.final(false);
	while (i != old.end())
	{
		e.first(i->first);
		e.last(i->second);
		++i;
		if (i == old.end()) e.final(true);
		fire_event(e);
	}
}

/**
 * Modifiy the selection so it only consist of the given index
 *
 * @param index new selection
 */
void MultiSelection::set(unsigned int index)
{
	if (_first == NO_SELECTION)
	{
		_first = index;
		_last = index;
		_selected.push_back(Range(index, index));
		fire_event(index, true, true);
	} else
	{
		RangeIterator i = _selected.begin();
		std::vector<SelectionChangedEvent> changes;

		while (i != _selected.end())
		{
			if (i->first > index || i->second < index)
			{
				changes.push_back(SelectionChangedEvent(i->first, i->second, false, false));
				i = _selected.erase(i);
			} else
			{
				if (index > i->first)
				{
					changes.push_back(SelectionChangedEvent(i->first, index-1, false, false));
					i->first = index;
				}
				if (index < i->second)
				{
					changes.push_back(SelectionChangedEvent(index+1, i->second, false, false));
					i->second = index;
				}
				++i;
			}
		}
		if (_selected.empty())
		{
			_selected.push_back(Range(index, index));
			changes.push_back(SelectionChangedEvent(index, index, true, true));
		}
		_first = index;
		_last = index;

		fire_changes(changes);
	}
}

/**
 * Add the index to the selection
 */
void MultiSelection::select(unsigned int index)
{
	if (_first == NO_SELECTION)
	{
		_first = index;
		_last = index;
		_selected.push_back(Range(index, index));
		fire_event(index, true, true);
	} else if (index > _last)
	{
		if (index == _last + 1)
		{
			_selected.back().second = index;
		} else
		{
			_selected.push_back(Range(index, index));
		}
		_last = index;
		fire_event(index, true, true);
	} else if (index < _first)
	{
		if (index + 1 == _first)
		{
			_selected.front().first = index;
		} else
		{
			_selected.insert(_selected.begin(), Range(index, index));
		}
		_first = index;
		fire_event(index, true, true);
	} else
	{
		RangeIterator i;
		if (index == 0) i = _selected.begin();
		else i = find_last_ge(index - 1);
		// will never return _selected.end() as we've eliminated this above
		if (index < i->first)
		{
			if (index + 1 == i->first)
			{
				i->first = index;
			} else
			{
				_selected.insert(i, Range(index, index));
			}
			fire_event(index, true, true);
		} else if (index == i->second + 1)
		{
			RangeIterator check(i);
			++check;
			if (index + 1 == check->first)
			{
				i->second = check->second;
				_selected.erase(check);
			} else
			{
				i->second = index;
			}
			fire_event(index, true, true);
		}
	}
}

/**
 * Remove selection from given index
 */
void MultiSelection::deselect(unsigned int index)
{
	RangeIterator i = find_last_ge(index);
	if (i != _selected.end() && index >= i->first)
	{
		if (i->first == index)
		{
			if (i->second == index)
			{
				_selected.erase(i);
				if (_selected.empty())
				{
					_first = _last = NO_SELECTION;
				} else
				{
					_first = _selected.front().first;
					_last = _selected.back().second;
				}
			} else
			{
				i->first++;
				if (_first == index) _first++;
			}
		} else if (i->second == index)
		{
			if (i->second == _last) _last--;
			i->second--;
		} else
		{
			Range front(i->first, index - 1);
			i->first = index + 1;
			_selected.insert(i, front);
		}
		fire_event(index, false, true);
	}
}

/**
 * Toggle the selected index
 */
void MultiSelection::toggle(unsigned int index)
{
	if (_first == NO_SELECTION || index < _first || index > _last) select(index);
	else if (index == _first)
	{
		if (index == _last)
		{
			_selected.clear();
			_first = _last = NO_SELECTION;
		} else
		{
			RangeIterator i = _selected.begin();
			i->first++;
			if (i->first > i->second) _selected.erase(i);
			_first = _selected.front().first;
		}
		fire_event(index, false, true);
	} else if (index == _last)
	{
		if (_selected.back().second == _selected.back().first) _selected.pop_back();
		else _selected.back().second--;
		_last = _selected.back().second;
		fire_event(index, false, true);
	} else
	{
		RangeIterator i;
		if (index == 0) i = _selected.begin();
		else i = find_last_ge(index - 1);

		if (index < i->first)
		{
			if (index + 1 == i->first)
			{
				i->first = index;
			} else
			{
				Range r(index, index);
				_selected.insert(i, r);
			}
			fire_event(index, true, true);
		} else if (index == i->second + 1)
		{
			RangeIterator check(i);
			++check;
			if (index + 1 == check->first)
			{
				i->second = check->second;
				_selected.erase(check);
			} else
			{
				i->second = index;
			}
			fire_event(index, true, true);
		} else if (index == i->first)
		{
			i->first++;
			if (i->first > i->second) _selected.erase(i);
			fire_event(index, false, true);
		} else if (index == i->second)
		{
			i->second--;
			fire_event(index, false, true);
		} else
		{
			Range r(i->first, index - 1);
			i->first = index + 1;
			_selected.insert(i, r);
			fire_event(index, false, true);
		}
	}
}

/**
 * Change the selection to be the specified range
 */
void MultiSelection::set(unsigned int from, unsigned int to)
{
	if (_first == NO_SELECTION)
	{
		_first = from;
		_last = to;
		_selected.push_back(Range(from, to));
		fire_event(from, to, true, true);
	} else
	{
		RangeIterator i = _selected.begin();
		std::vector<SelectionChangedEvent> changes;

		while (i != _selected.end())
		{
			if (i->first > to || i->second < from)
			{
				changes.push_back(SelectionChangedEvent(i->first, i->second, false, false));
				i = _selected.erase(i);
			} else
			{
				if (from > i->first)
				{
					changes.push_back(SelectionChangedEvent(i->first, from-1, false, false));
					i->first = from;
				}
				if (to < i->second)
				{
					changes.push_back(SelectionChangedEvent(to+1, i->second, false, false));
					i->second = to;
				}
				++i;
			}
		}
		if (_selected.empty())
		{
			_selected.push_back(Range(from, to));
			changes.push_back(SelectionChangedEvent(from, to, true, true));
		}
		_first = from;
		_last = to;

		fire_changes(changes);
	}
}

/**
 * Select a range of values
 */
void MultiSelection::select(unsigned int from, unsigned int to)
{
	if (_first == NO_SELECTION)
	{
		_first = from;
		_last = to;
		Range r(from,to);
		_selected.push_back(r);
		fire_event(from, to, true, true);
	} else if (to < _first)
	{
		if (to + 1 == _first)
		{
			_selected.front().first = from;
		} else
		{
			Range r(from,to);
			_selected.insert(_selected.begin(), r);
		}
		_first = from;
		fire_event(from, to, true, true);
	} else if (from > _last)
	{
		if (from == _last + 1)
		{
			_selected.back().second = to;
		} else
		{
			Range r(from, to);
			_selected.push_back(r);
		}
		_last = to;
		fire_event(from, to, true, true);
	} else
	{
		RangeIterator i;
		std::vector<SelectionChangedEvent> changes;

		if (from == 0) i = _selected.begin();
		else i= find_last_ge(from - 1);

		if (from < i->first )
		{
			changes.push_back(SelectionChangedEvent(from, i->first-1, true, false));
			i->first = from;
			if (from < _first) _first = from;
		}
		if (to > i->second)
		{
			if (to > _last) _last = to;
			unsigned int oldlast = i->second;
			i->second = to;
			RangeIterator check = i;
			check++;
			while (check != _selected.end() && check->first <= to)
			{
				changes.push_back(SelectionChangedEvent(oldlast+1, check->first-1, true, false));
				oldlast= check->second;
			}

			if (oldlast > i->second) i->second = oldlast;
			if (oldlast < to)
			{
				changes.push_back(SelectionChangedEvent(oldlast+1, to, true, false));
			}
			++i;
			while (i != _selected.end() && i->second <= to)
			{
				i = _selected.erase(i);
			}
		}

		fire_changes(changes);
	}
}

/**
 * Deselect a range of values
 */
void MultiSelection::deselect(unsigned int from, unsigned int to)
{
	if (_first == NO_SELECTION || from > _last || to < _first) return;

	if (from <= _first && to >= _last)
	{
		std::vector<SelectionChangedEvent> changes;
		for (RangeIterator i = _selected.begin(); i != _selected.end(); ++i)
		{
			changes.push_back(SelectionChangedEvent(i->first, i->second, false, false));
		}
		_selected.clear();
		_first = _last = NO_SELECTION;
		fire_changes(changes);
	} else
	{
		if (from <= _first) _first = to + 1;
		if (to >= _last) _last = from - 1;

		RangeIterator i = find_last_ge(from);

		if (to >= i->first)
		{
			std::vector<SelectionChangedEvent> changes;
			if (from > i->first)
			{
				if (to >= i->second)
				{
					changes.push_back(SelectionChangedEvent(from, i->second, false, false));
					i->second = from - 1;
					i++;
				} else
				{
					changes.push_back(SelectionChangedEvent(to, from, false, false));
					i = _selected.insert(i, Range(i->first, to-1));
					i++;
					i->first = from + 1;
				}
			}

			while (i != _selected.end() && i->second <= to)
			{
				changes.push_back(SelectionChangedEvent(i->first, i->second, false, false));
				i = _selected.erase(i);
			}
			if (i != _selected.end())
			{
				if (i->second > to)
				{
					changes.push_back(SelectionChangedEvent(to, i->second, false, false));
					i->second = to - 1;
				}
			}

			fire_changes(changes);
		}
	}
}

/**
 * Toggle selected items in the range
 */
void MultiSelection::toggle(unsigned int from, unsigned int to)
{
	if (_first == NO_SELECTION || to < _first || from > _last) select(from, to);
	else
	{
		RangeIterator i = find_last_ge(from);
		std::vector<SelectionChangedEvent> changes;

		if (to < i->first)
		{
			changes.push_back(SelectionChangedEvent(from, to, true, false));
			i = _selected.insert(i, Range(from, to));
			++i;
		} else if (from < i->first)
		{
			changes.push_back(SelectionChangedEvent(from, i->first-1, true, false));
			i = _selected.insert(i, Range(from, i->first-1));
			++i;
		}
		unsigned int last_second = i->second;

		while (i != _selected.end() && i->first <= to)
		{
			if (i->first > last_second+1)
			{
				changes.push_back(SelectionChangedEvent(last_second+1, i->first-1, true, false));
				i = _selected.insert(i, Range(last_second+1, i->first - 1));
				++i;
			}
			last_second = i->second;
			if (i->first < from)
			{
				if (i->second <= to)
				{
					changes.push_back(SelectionChangedEvent(from, i->second, false, false));
					i->second = from-1;
				} else
				{
					changes.push_back(SelectionChangedEvent(from, to, false, false));
					i = _selected.insert(i, Range(i->first, from - 1));
					i++;
					i->first = to + 1;
				}
			} else if (i->second <= to)
			{
				changes.push_back(SelectionChangedEvent(i->first, i->second, false, false));
				i = _selected.erase(i);
			} else
			{
				changes.push_back(SelectionChangedEvent(i->first, to, false, false));
				i->first = to + 1;
				++i;
			}
		}

		if (to > last_second)
		{
			changes.push_back(SelectionChangedEvent(last_second+1, to, true, false));
			i = _selected.insert(i, Range(last_second+1, to));
		}

		if (_selected.empty())
		{
			_first = _last = NO_SELECTION;
		} else
		{
			_first = _selected.front().first;
			_last = _selected.back().second;
		}

		fire_changes(changes);
	}
}

/**
 * Construct iterator implementation from iterators
 */
MultiSelection::MultiIteratorImpl::MultiIteratorImpl(ConstRangeIterator start, ConstRangeIterator end)
{
	_current = start;
	_end = end;
	if (start == end) _index = NO_SELECTION;
	else _index = (*start).first;
}

/**
 * Make a copy of the current iterator
 */
Selection::IteratorImpl *MultiSelection::MultiIteratorImpl::clone()
{
	MultiIteratorImpl *copy = new MultiIteratorImpl(_current, _end);
	copy->_index = _index;
	return copy;
}

/**
 * Advance iterator
 */
void MultiSelection::MultiIteratorImpl::next()
{
	if (_index != NO_SELECTION)
	{
		if (_index == _current->second)
		{
			if (++_current == _end) _index = NO_SELECTION;
			else _index = _current->first;
		} else
		{
			_index++;
		}
	}
}

/**
 * Get initial iterator implementation
 */
Selection::IteratorImpl *MultiSelection::get_iterator_impl() const
{
	return new MultiIteratorImpl(_selected.begin(), _selected.end());
}


}
}
