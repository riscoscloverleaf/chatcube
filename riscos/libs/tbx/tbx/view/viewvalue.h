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

#ifndef TBX_VIEWVALUE_H
#define TBX_VIEWVALUE_H

namespace tbx
{
namespace view
{

/**
 * Base class to provide a value of a given type
 * for an item view
 *
 * T is the type the for the view
 */
template<class T> class ItemViewValue
{
   public:
      virtual ~ItemViewValue() {};

      /**
       * Provide the value for the view
       */
      virtual T value(unsigned int index) const = 0;

};

/**
 * Convenience template to return an item of a collection as
 * the value for a view.
 *
 * T is the type to return
 * C is the collection to return it from
 */
template<class T, class C> class IndexItemViewValue : public ItemViewValue<T>
{
   private:
      const C &_collection;

   public:
      /**
       * Construct with collection to index
       */
      IndexItemViewValue(const C &collection) : _collection(collection) {}

      /**
       * Return value for given index
       */
      virtual T value(unsigned int index) const {return _collection[index];}
};

/**
 * Convenience template to return a member of the class in a collection
 * as the value for a view.
 *
 * T is the type returned
 * C is the collection to return from
 * I is the type of the items in the collection
 */
template<class T, class C, class I> class MethodItemViewValue : public ItemViewValue<T>
{
   private:
      C *_collection;
      T (I::*_method)() const;

    public:
       /**
        * Construct for the given collection with the given data
        * retrieval method
        */
       MethodItemViewValue(C *collection, T (I::*method)() const) :
    	   _collection(collection), _method(method) {}

       /**
        * Get the value for the index by calling the method
        * from the constructor on the object
        */
       virtual T value(unsigned int index) const
       {
    	   return (((*_collection)[index]).*_method)();
       }
};

/**
 * Convenience template to return a member of the class in a collection
 * of pointers as the value for a view.
 *
 * T is the type returned
 * C is the collection to return from
 * I is the type of the items in the collection
 */
template<class T, class C, class I> class MethodItemPtrViewValue : public ItemViewValue<T>
{
   private:
      C *_collection;
      T (I::*_method)() const;

    public:
       /**
        * Construct for the given collection with the given data
        * retrieval method
        */
       MethodItemPtrViewValue(C *collection, T (I::*method)() const) :
    	   _collection(collection), _method(method) {}

       /**
        * Get the value for the index by calling the method
        * from the constructor on the object
        */
       virtual T value(unsigned int index) const
       {
    	   return (((*_collection)[index])->*_method)();
       }
};


// end of namespaces
}
}


#endif
