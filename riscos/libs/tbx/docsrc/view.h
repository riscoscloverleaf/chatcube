/**
 * @namespace view Helper classes to display and edit data
 *
 * The view namespace provides helper classes to make it easier to
 * display and interact with formatted data.
 *
 * There are two types of view provided, a text viewer and various item
 * viewers.
 *
 * The text viewer is a provided by the TextView class and is concerned
 * with displaying multiple lines of text in a single font.
 *
 * The item viewers display multiple items in a window.
 *
 * The item viewers are derived from the ItemView class. There are
 * classes to display the data in a list, a multiple column list
 * and as tiled items (this is like the WIMP filer display).
 *
 * A common selection model for all item viewers is provided
 * by classes derived from the Selection class.
 * TBX includes classed for single selection and the standard RISC OS type
 * multiple selection.
 *
 * The item viewers also use a concept of an item renderer
 * to draw the items in the view. You can create your own
 * or use the WimpFontItemRender for text, the SpriteItemRenderer
 * for sprite or the IconItemRenderer to display text and
 * sprites together.
 *
 * Each of these item renderer classes uses a class derived
 * from the templated ItemViewValue class to provide the
 * data to render for a specific item.
 */

