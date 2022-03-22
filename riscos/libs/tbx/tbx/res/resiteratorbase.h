
#ifndef TBX_RES_RESITERATORBASE_H
#define TBX_RES_RESITERATORBASE_H

namespace tbx
{
namespace res
{

/**
* Base class for Resource component iterators
*/
template <class T> class ResIteratorBase
{
protected:
   const T *_object; //!< Reference to object being iterated
   int _offset;      //!< Offset of current item being iterated
   
   /**
    * Construct from an object and initial offset
    *
    * @param object object being iterated
    * @param offset offset within object
    */
   ResIteratorBase(const T *object, int offset) : _object(object), _offset(offset) {};

public:
   /**
    * Check if this ResIteratorBase is the same as another
    *
    * @param other ResIteratorBase to check
    * @returns true if they are the same
    */
   bool operator==(const ResIteratorBase &other) const {return _object == other._object && _offset == other._offset;}
   /**
    * Check if this ResIteratorBase is not the same as another
    *
    * @param other ResIteratorBase to check
    * @returns true if they are not the same
    */
   bool operator!=(const ResIteratorBase &other) const {return _object != other._object || _offset != other._offset;}
};

}
}


#endif
