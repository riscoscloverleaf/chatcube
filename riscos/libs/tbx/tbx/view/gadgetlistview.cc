/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2015 Alan Buckley   All Rights Reserved.
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
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2015 Alan Buckley   All Rights Reserved.
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

#include "gadgetlistview.h"
#include "../gadgetcopyvalue.h"
#include "../displayfield.h"
#include "../writablefield.h"
#include <stdexcept>

namespace tbx
{
namespace view
{

GadgetListCopyRow GadgetListView::_default_copy;

GadgetListView::GadgetListView() :
    _res_gadgets(0),
    _first_gadget_id(tbx::NULL_ComponentId),
    _last_gadget_id(tbx::NULL_ComponentId),
    _num_gadgets(0),
    _width(0),
    _row_height(32),
    _spacing(8),
    _count(0),
    _copy_interface(&_default_copy),
    _updates_enabled(false)
   {
   }

GadgetListView::~GadgetListView()
{
   for (unsigned int j = 0; j < _num_gadgets; j++)
   {
      delete _res_gadgets[j];
   }
   delete [] _res_gadgets;
}

/**
 * Setup the gadget list view
 *
 * The margin is calculated from the bounding box of the gadget compared to the window extent.
 * first_gadget_id to last_gadget_id must be a continuos range
 * new rows are created with ids from last_gadget_id + 1
 *
 * @param window - the window gadgets will be created on
 * @param res_window - The resource to get the gadget templates from and calculate the margin
 * @param first_gadget_id - id of first gadget for list in resource
 * @param last_gadget_id - id of last gadget for list in resource
 * @param spacing vertical spacing between rows
 * @param first_row_created true if the first row of the list has already been created
 */
void GadgetListView::setup(tbx::Window window, tbx::res::ResWindow &res_window, tbx::ComponentId first_gadget_id, tbx::ComponentId last_gadget_id, unsigned int spacing, bool first_row_created)
{
    _window = window;
    _updates_enabled = true;
    _first_gadget_id = first_gadget_id;
    _last_gadget_id = last_gadget_id;
    _num_gadgets = last_gadget_id - first_gadget_id + 1;
    _spacing = spacing;
    if (first_row_created) _count = 1;
    _res_gadgets = new tbx::res::ResGadget*[_num_gadgets];
    tbx::BBox gadget_bounds;
    if (_num_gadgets == 0) gadget_bounds = tbx::BBox(0,0,0,0);
    // Make copy of resources to use
    for (unsigned int c = 0; c < _num_gadgets; c++)
    {
       tbx::ComponentId comp_id = first_gadget_id + c;
       _res_gadgets[c] = new tbx::res::ResGadget(res_window.gadget(comp_id));
       tbx::BBox bounds(_res_gadgets[c]->xmin(), _res_gadgets[c]->ymin(),
           _res_gadgets[c]->xmax(), _res_gadgets[c]->ymax());
       if (c == 0) gadget_bounds = bounds;
       else gadget_bounds.cover(bounds);
    }

    _width = gadget_bounds.width();
    _row_height = gadget_bounds.height();
    _margin.left = gadget_bounds.min.x - res_window.work_xmin();
    _margin.right = res_window.work_xmax() - gadget_bounds.max.x;
    _margin.top = res_window.work_ymax() - gadget_bounds.max.y;
    _margin.bottom = gadget_bounds.min.y - res_window.work_ymin();

    // Set position for next add
    if (first_row_created)
    {
       for (unsigned int c = 0; c < _num_gadgets; c++)
       {
          _res_gadgets[c]->ymin(_res_gadgets[c]->ymin() - _row_height - _spacing);
          _res_gadgets[c]->ymax(_res_gadgets[c]->ymax() - _row_height - _spacing);
       }
    }
}

/**
 * Set the margin around the view to the edge of the extent
 *
 * Changing the top or left will move any gadgets already added
 * to the list
 *
 * @param margin the new margin
 */
void GadgetListView::margin(const tbx::Margin &margin)
{
    int x_offset = margin.left - _margin.left;
    int y_offset = margin.top - _margin.top;
    _margin = margin;

    if (x_offset)
    {
       // Reset new row gadgets x co-ordinate, y is relative so
       // does not need changing
       for (unsigned int c = 0; c < _num_gadgets; c++)
       {
          _res_gadgets[c]->xmin(_res_gadgets[c]->xmin() + x_offset);
          _res_gadgets[c]->xmax(_res_gadgets[c]->xmax() + x_offset);
       }
    }

    if (y_offset)
    {
       // Reset new row gadgets x co-ordinate, y is relative so
       // does not need changing
       for (unsigned int c = 0; c < _num_gadgets; c++)
       {
          _res_gadgets[c]->ymin(_res_gadgets[c]->ymin() + y_offset);
          _res_gadgets[c]->ymax(_res_gadgets[c]->ymax() + y_offset);
       }
    }

    if (_count > 0 && (x_offset || y_offset))
    {
       tbx::ComponentId id = _first_gadget_id;
       int num_to_move = _count * _num_gadgets;
       while (num_to_move--)
       {
          tbx::Gadget g = _window.gadget(id++);
          g.move_by(x_offset, y_offset);
       }
    }

    update_window_extent();
}

/**
 * Update the windows extents to fit the current size of the list
 * and its margin
 */
void GadgetListView::update_window_extent()
{
   if (!updates_enabled()) return;
   int width = _width + _margin.left + _margin.right;
   int height = _count * (_row_height + _spacing) + _margin.top + _margin.bottom;
   if (_count > 0) height -= _spacing;
   tbx::WindowState state;
   _window.get_state(state);
   if (width < state.visible_area().bounds().width())
      width = state.visible_area().bounds().width();
   if (height < state.visible_area().bounds().height())
      height = state.visible_area().bounds().height();

   tbx::BBox extent(0, -height, width, 0);
   _window.extent(extent);
}

/**
 * Set object that implement the copy row interface to copy data from one
 * row to another.
 *
 * If not set the default will copy the "value/text" property for some of
 * the standard gadgets.
 * @param copy object implementing the GadgetListCopyRow interface or 0 to
 * restore it to the default.
 */
void GadgetListView::copy_interface(GadgetListCopyRow *copy)
{
  if (copy == 0) copy = &_default_copy;
  _copy_interface = copy;
}

/**
 * Add row(s) to the end of the list
 *
 * @param count number of rows to add (default 1)
 * @returns id of the first component in the new rows
 * @throws std::invalid_argument if count is 0
 */
tbx::ComponentId GadgetListView::add(unsigned int count /* = 1*/)
{
   if (count == 0) throw std::invalid_argument("count must be > 0");
   tbx::ComponentId first_id = _count * _num_gadgets + _first_gadget_id;
   tbx::ComponentId add_id = first_id;
   int y = _row_height + _spacing;

   while (count--)
   {
      for (unsigned int c = 0; c < _num_gadgets; c++)
      {
         tbx::res::ResGadget *res_gadget = _res_gadgets[c];
         res_gadget->component_id(add_id++);
         _window.add_gadget(*res_gadget);
         // Move to position for next add
         res_gadget->ymin(res_gadget->ymin() - y);
         res_gadget->ymax(res_gadget->ymax() - y);
      }
      _count++;
   }
   update_window_extent();
   return first_id;
}

/**
 * Insert row(s) before the given row number
 *
 * The new rows are not cleared or initialised, so must be
 * initialised after calling this function.
 *
 * @param row the number of the row to insert before. Can be the value of
 *            count() to insert at the end.
 * @param count the number of rows to insert (default 1)
 * @returns id of the first component in the first new row
 * @throws std::out_of_range if row is greater than count()
 * @throws std::invalid_argument if count is 0
 */
tbx::ComponentId GadgetListView::insert(unsigned int row, unsigned int count /*= 1*/)
{
    if (row > _count) throw std::out_of_range("row out of range");
    bool appending = (row >= _count);
    tbx::ComponentId insert_row_id = add(count);
    if (!appending)
    {
        insert_row_id = row * _num_gadgets + _first_gadget_id;
        for (unsigned int copy = _count-1; copy >= row + count; copy--)
        {
           _copy_interface->copy_row(*this, copy-count, copy);
        }
    }
    return insert_row_id;
}

/**
 * Remove row(s) from the list
 *
 * @param row first row to remove
 * @param count number of rows to remove (default 1)
 * @throws std::out_of_range if row >= count() or row + count > count()
 * @throws std::invalid_argument if count == 0
 */
void GadgetListView::remove(unsigned int row, unsigned int count /*= 1*/)
{
    if (row >= _count) throw std::out_of_range("row outside list");
    if (count == 0) throw std::invalid_argument("count is 0");
    if (row + count > _count) throw std::out_of_range("row + count outside list");
    if (row < _count - count)
    {
       for (unsigned int copy = row ; copy < _count - count ; copy++)
       {
           _copy_interface->copy_row(*this, copy+count, copy);
       }
    }

   // Reset position for next add
    int y = (_row_height + _spacing) * count;
    for (unsigned int c = 0; c < _num_gadgets; c++)
    {
       tbx::res::ResGadget *res_gadget = _res_gadgets[c];
       res_gadget->ymin(res_gadget->ymin() + y);
       res_gadget->ymax(res_gadget->ymax() + y);
    }

    tbx::ComponentId remove_id = (_count-count) * _num_gadgets + _first_gadget_id;
    while(count--)
    {
       for (unsigned int c = 0; c < _num_gadgets; c++)
       {
           _window.remove_gadget(remove_id++);
       }
       _count--;
    }

    update_window_extent();
 }

/**
 * Clear all gadgets from the list
 */
void GadgetListView::clear()
{
   if (_count == 0) return; // Nothing to do

   // Reset position for next add
    int y = (_row_height + _spacing) * _count;
    for (unsigned int c = 0; c < _num_gadgets; c++)
    {
       tbx::res::ResGadget *res_gadget = _res_gadgets[c];
       res_gadget->ymin(res_gadget->ymin() + y);
       res_gadget->ymax(res_gadget->ymax() + y);
    }

    // Now delete all the gadgets
    tbx::ComponentId remove_id = _first_gadget_id;
    int rows_to_remove = _count;
    while(rows_to_remove--)
    {
       for (unsigned int c = 0; c < _num_gadgets; c++)
       {
           _window.remove_gadget(remove_id++);
       }
    }
    _count = 0;

    update_window_extent();
}


/**
 * Use currently assigned copy function to copy from one row to another
 *
 * @param from_row row to copy from
 * @param to_row row to copy to
 */
void GadgetListView::copy(unsigned int from_row, unsigned int to_row)
{
   _copy_interface->copy_row(*this, from_row, to_row);
}

/**
 * Default function to copy data from one row to another
 *
 * It will just copy the value/text property
 *
 * @param glv list to copy row from
 * @param from_row row to copy from
 * @param to_row row to copy to
 */
void GadgetListCopyRow::copy_row(GadgetListView &glv, unsigned int from_row, unsigned int to_row)
{
    for (unsigned int c = 0; c < glv.num_gadgets(); c++)
    {
        tbx::Gadget from_gadget = glv.gadget(from_row, c);
        tbx::Gadget to_gadget = glv.gadget(to_row, c);
        gadget_copy_value(from_gadget, to_gadget);
    }
}

} /* end of view namespace */
} /* end of tbx namespace */

