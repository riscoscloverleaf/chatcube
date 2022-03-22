/**
 * @page using Using the library
 *
 * If installed with a package manager the TBX path will already be set
 * up, otherwise double click on the !TBX application to set the path.
 *
 * The library is in ELF format so you will need to compile your
 * application using GCC4 or later.
 *
 * To include the library for your source use:
 *
 * @code
 * -ITBX: on the compile line
 *@endcode
 *
 *  and include the classes using
 *
 *@code
 *  #include "tbx/tbxclass.h"
 *@endcode
 *
 *  All classes are in the tbx namespace so either put using namespace tbx
 *  at the beginning of your source file or prefix the class with tbx::
 *
 * To link the library add the following to the linker line
 *
 *@code
 * -LTBX: -ltbx
 * @endcode
 *
 */
