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

#ifndef TBX_SAFELIST_H
#define TBX_SAFELIST_H

namespace tbx
{

/**
 * Simple one way linked list of pointers that provides a single iterator that
 * can be used safely if items are removed or the class containing
 * the list is deleted.
 *
 * To iterate the list do:
 *   SafeList<MyType>::Iterator iter(&my_safe_list)
 *   MyType *ptr;
 *   while ((ptr = iter.next()) != null)
 *   {
 *        ptr->my_method(...);
 *   }
 */
template<class T> class SafeList
{
   private:
      struct Node
      {
          Node(T *ptr) : _ptr(ptr), _next(0) {};
          T *_ptr;
          Node *_next;
      };

   public:
      /**
       * Simple class to iterate through the list.
       *
       * Only one iterator can be used on the list at a time.
       * Items can be safely removed from the list while progressing
       * through the iterator.
       */
      class Iterator
      {
         friend class SafeList;

       private:
          SafeList _list;
          Node *_next;

       public:
          /**
           * Construct the iterator for the list
           */
          Iterator(SafeList &list) : _next(list._head)
          {
             _list._iter = this;
          }

          ~Iterator()
          {
             _list._iter = 0;
          }

          /**
           * Get next pointer from the list
           */
          T *next()
          {
             T *ptr;

             if (_next != 0)
             {
                ptr = _next->_ptr;
                _next = _next->_next;
             } else
             {
                ptr = 0;
             }

             return ptr;
          }
      };

   private:
      Node *_head;
      Iterator *_iter;

      friend class Iterator;

   public:
      /**
       * Construct an empty SafeList
       */
      SafeList() : _head(0), _iter(0) {};

      /**
       * Destructor will clear the list
       */
       ~SafeList() {clear();}

      /**
       * Add pointer to end of list
       *
       *@param ptr Pointer to add
       */
      void push_back(T *ptr)
      {
          if (_head == 0) _head = new Node(ptr);
          else
          {
            Node *last = _head;
            while (last->_next != 0) last = last->_next;
            last->_next = new Node(ptr);
          }
      }

      /**
       * Remove pointer from list
       *
       *@param ptr to remove
       */
      void remove(T *ptr)
      {
          Node *prev = 0;
          Node *check = _head;
          while (check && check->_ptr != ptr)
          {
             prev = check;
             check = check->_next;
          }
          if (check)
          {
             if (prev) prev->_next = check->_next;
             else _head = check->_next;

             if (_iter && _iter->_next == check)
             {
                _iter->_next = check->_next;
             }
             delete check;
          }
      }

      /**
       * Empty list
       */
      void clear()
      {
         if (_iter) _iter->_next = 0;
         Node *check = _head;
         Node *next;
         while (check)
         {
            next = check->_next;
            delete check;
            check = next;
         }
         _head = 0;
      }

      /**
       * Check if list is empty
       */
      bool empty() const
      {
    	  return (_head == 0);
      }
};

}

#endif

