/**
 * @page history History/Changelog
 *
 * <B>0.7.5 Alpha July 2019</B>
 *
 *  - Added URI class to launch URIs using the Acorn URI module.
 *
 * <B>0.7.4 Alpha March 2016</B>
 *
 *  - Added Font::handle() to return RISC OS font handle
 *  - Added fpcs namespace with function to create font paint control sequences
 *  - Added FontItemRenderer to render items with a given font
 *  - Added common and wimp colours to Colour class enum
 *  - Added Menu/MenuItem submenu and selection listeners
 *  - Added DeleteClassAndObjectOnHidden listener
 *  - Fixed hang when saving a TagDoc where a node had attributes
 *  - Added GadgetListView class to handle a list made up of one or more gadgets on a line
 *  - Added copy_gadget_value function to copy the main value between gadgets of the same type
 *  - Added Gadget toolbox_class and toolbox_class_and_size methods
 *  - Added new class LockExtentPosition to keep gadget edges locked to the side of the windows extent
 *  - Fixed memory leak in show_message/show_message_as_menu and class MessageWindow
 *  - Added close_command method to show_message/show_message_as_menu and class MessageWindow to set a command to be run when the message window is closed.
 *  - Added TaskWindow class to run a child task using the RISC OS TaskWindow command
 *
 * <B>0.7.3 Alpha December 2014</B>
 *
 * - Added Path::temporary() method to return a temporary path in the Wimp scrap directory.
 * - Added ext namespace for classes that access third-party applications/modules.
 * - Add StrongHelp class to ext namespace to help using the StrongHelp help system.
 * - Added OleClient class to the ext namespace to help using OLE editing.
 * - Added PThreadGuard class to disable threading around wimp poll and event processing when using pthreads to prevent crashes caused by interrupting event processing.
 * - Added prepoll and post event listener to application (used by PThreadGuard)
 * - Fixed plotting of 16M colour sprites to any other mode caused a crash due to a bug in the TranslationTable class.
 * - Added SpriteArea::set and get_sprite(OsSpritePtr) to allow easier passing of a sprite area/user sprite to the TBX classes.
 * - Fixed bug where PropertySet set bool method went into an infinite loop
 * - Fixed display of all buttons with default styling after the first one was set in the message window.
 *
 * <B>0.7.2 Alpha March 2014 </B>
 * - uncaught_exception method added to application to allow user processing of this error condition
 * - Added Font::read_details to get the details of a font
 * - Added Application::yield to allow multi-tasking in the middle of a long operation
 * - Fixed possible memory corruption when removing idle commands from within a running idle command
 * - Added TaskManager class to interrogate running tasks
 *
 * <B>0.7.1 Alpha December 2013</B>
 * - Window::remove_gadget now removes all listeners for the gadget
 * - Fixed Window::title() always threw an exception
 * - Fixed crash in Font copy constructor
 * - Added string bounds and size methods to Font and made some of the existing methods const
 * - Added stringviewvalue.h header with template classes to always return ItemViewValues as string as
 * required by the WimpFontItemRenderer. It uses tbx::to_string to convert the item type to a string.
 * - stringutils.h added overrides to to_string for simple conversion of char * and const std::string & types.
 * - added UserSprite iterator to SpriteArea class
 * - Replaced ModeInfo set/get mode methods with mode() and mode(int) to be consistant with other tbx setters and getters
 * - Added static method ModeInfo::screen_mode to return the mode specifier for the current mode.
 * - Window::update background redrawer made optional
 * - VisibleArea::screen(Point) fixed bug where it converted the point given to work area co-ordinates instead of screen coordinates.
 * - UserSprite::area_id did not return the correct value
 * - Window::DragSpriteFlags added DSFLAG_SOLID for solid drags
 * - Path::raw_path_info and PathInfo::read_raw added to get raw load/executable address for a file
 * - Fixed UserSprite::create_mask to stop it corrupting the internal sprite pointers. The sprite was unusable
 * unless it was reobtained using SpriteArea::get_sprite
 * - Fixed conversion operators and other minor bugs in tbx::Fixed16
 * - added showing() method to Object
 * - ItemView updates_enabled() added to pause window updates.
 *
 * <B>0.7 Alpha May 2013</B>
 * - show_as_subwindow added to Window class to show a child window attached to a parent
 * - Add attributes enum and setting for Path class
 * - Modifed Application::directory method to return directory as a path
 *   (i.e. with a dot added if it was specified as just a directory initially)
 *   so it returns consistent results on RISC OS 4 and 5.
 * - Added Window::outline to return the area covered by the window on the desktop
 * - Added Saver::safe method so it is possible to check if a save location is safe before write a file.
 * - Fixed bug in VisibleArea::screen methods for a BBox. It was returning
 *   the original rectangle not the screen rectangle.
 * - Minor documentation fixes
 *
 * <B>0.6 Alpha September 2012</B>
 * - Fixed incorrect return value from Font class string_width methods
 * - Add a few more methods to the WindowInfo class
 * - Added canonicalise and canonical_equals methods to Path class
 * - deleteonhidden.h was missing object.h include
 * - Added show_message function and the MessageWindow class (in
 *   messagewindow.h) to make it easy to show a message in a window.
 * - Added FilerAction class to make it easy to run multi-tasking file
 *   operation via the desktop filer
 * - Added COPY_FORCE option to force overwrite of the target in Path::copy
 *   and Path::move methods. The documentation for previous releases said they were
 *   overwriting always when they were incorrectly adding the recurse flag.
 * - Added application::set_post_poll_listener so an application can look
 *   at the raw messages it recieves.
 * - Added Gadget::move_by to move a gadget relative to current position
 * - Add TextDisplayWindow class to display a window resized to a text message
 * - Added show_question function and QuestionWindow class to make it easy to
 * display a question in a window (in questionwindow.h)
 * - Remove show_as_menu and show_as_submenu with ShowFullSpec parameter from
 * ShowFullObject class as the full spec does not affect the show.
 * - Added methods for window flags to ResWindow class
 * - Added has_been_hidden_listener to SaveAs dialogue
 * - Fixed slider bounds getting/setting which was switching the lower and upper bounds
 * - Included TbxRes resource file with pre-built template as a basis for an alternative
 * save as window.
 * - DrawPath class to create RISC OS Draw paths and paint them on the screen
 * - Graphics classes modified to allow filling and stroking of draw paths, with the
 * user units in the draw path mapped to OS units.
 * - DrawTransform class modified to use new Fixed16 class to represent the 16.16 bit fixed point values in it.
 * - Fixed removing individual events or handlers
 * - add start_wimp_task method to the Application class
 * - add os_cli method to the Application class
 * - Fixed incorrect case and missing c includes that stopped cross compiling with latest GCC4
 * - Removed ScrollListSelectionEvent selected() method as it always returned 0
 * - Added ScrollListSelectionEvent adjust() method which returns true if the selection was done with adjust
 * - create directory/copy/move and similar functions now throw exceptions rather then returning bool so the failure OsError can be used.
 *
 * <B>0.5 November 2011</B>
 * - const char * constructor for Path class
 * - pointer operator for Path and PathInfo iterators
 * - Added foreground and background methods for setting the font colours in the TextView
 * - Removed space at beginning of a wrapped line in the TextView
 * - Added Point/Size constructor the BBox
 * - Missing ActionButton, Button, NumberRange, Slider and StringSet methods added
 * - JPEG added draw transform plot and file checking methods
 * - Fixed error when the last idle command was removed during an idle command
 *
 * <B>0.4 Alpha September 2011</B>
 * - New userguide
 * - Added missing reference documentation on classes/methods etc.
 * - Minor class tidying
 *
 * <B>0.3 Alpha February 2011</B>
 * - Release so version used with PackIt 0.2 is available
 * - Added res namespace containing Toolbox Resource file/object/gadget handling
 * - WriteableField renamed to WritableField.
 * - Method/parameter names for file names modified to have consistent naming throughout the library
 * - Added functions to support message files
 * - Added doc namespace containing classes to help in writing document based applications
 * - Added static methods for accessing/setting a files file type.
 * - Added raw_file_type Path/PathInfo method to get file type for ImageFS files
 * - Added Opener handlers to application to be used for opening a file double clicked in the filer.
 * - Added Window scroll request listener
 * - Split out Wimp Message Listener to use listeners explicit for the three message types.
 * - Added drag methods to Window
 * - Updated ItemView to improve paging.
 * - Added mode changed and palette changed listeners to application.
 * - Various minor API and documentation fixes/improvements.
 * - fixed equals_ignore_case function as it failed when both parameters were strings.
 * - Added defer_delete template function to delay deletion of an object until the next null poll.
 * - Added UserSprite methods to set and get pixels.
 *
 * <B>0.2 Alpha July 2010</B>
 * - Release so version used with PackIt is available.
 * - Removed uncaught exception handling so program stops with an error.
 * - Automatically remove file loaders from an object when it is destroyed.
 * - ReportView didn't refresh enough lines when items were inserted.
 * - All toolbox and Window WIMP events information now derives from EventInfo class
 * - Added missing toolbox objects ColourMenu, FileInfo, FontDbox, FontMenu,
 * PrintDbox, ProgInfo and Scale.
 * - Partial implementation of ColourDbox
 * - Added missing gadgets Adjuster, PopUp, RadioButton, Button, Draggable and ToolAction.
 * - WimpFont::paint - changed to paint font along base line provided
 * - Desktop font item rendering and textview rendering modifed to use 40 os units for the height.
 * - Added key pressed event processing to Window object.
 * - Fixed bug where command added to application could be executed multiple times
 *
 * <B>0.1 Alpha June 2010</B>
 * - Alpha release so library used with !PackMan is available
 */
