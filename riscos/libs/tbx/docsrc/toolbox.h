/**
 * @page toolbox tbx and toolbox objects/components
 *
 * The tbx library provides a light wrapper around the toolbox objects,
 * gadgets and menu items.
 *
 * The classes to manipulate the toolbox objects can be treated as
 * references to the underlying toolbox object.
 *
 * Copying the C++ classes does not make a copy of the object, rather it
 * copies the reference so both classes refer to the same object.
 *
 * To associate a toolbox class with a C++ class you can either create
 * the toolbox class from the C++ class by using the constructor that
 * takes a template name or use the set_autocreate_listener on the
 * Application class which will be triggered when an object is auto-created
 * by the toolbox.
 *
 * You can then add a listener for the has been deleted event if the toolbox
 * object is deleted automatically or you can delete the toolbox object
 * from the C++ class by calling the delete_object method.
 *
 * A MatchLifetime templated class provides a convenient way to match
 * the life time of a C++ object with a toolbox object. If its constructor
 * is provided with a template name it will automatically add the above
 * listeners for you.
 *
 * MenuItems are retrieved with the Menu classes item method passing the
 * menu item component id.
 *
 * Gadgets are retrieved using the Window classes gadget method.
 *
 * The resources used for toolbox applications can be created with !ResEd
 * and tested using !ResTest. Both the programs are freely available
 * from the RISC OS Open website. www.riscosopen.org.
 */

