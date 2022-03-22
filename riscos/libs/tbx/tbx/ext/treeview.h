/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2020 Alan Buckley   All Rights Reserved.
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
 * treeview.h
 *
 *  Created on: 11-Nov-2020
 *      Author: alanb
 */

#ifndef TBX_TREEVIEW_H_
#define TBX_TREEVIEW_H_

#include "../gadget.h"

namespace tbx {
namespace ext {

class TreeViewNodeSelectedListener;
class TreeViewNodeExpandedListener;
class TreeViewNodeRenamedListener;
class TreeViewNodeDraggedListener;
class TreeViewScrollListener;


typedef int TreeNodeId;
class TreeViewCurrentNode;

/**
 * TreeView wrapper class for an underlying toolbox TreeView gadget.
 *
 * A TreeView is a gadget that shows a hiearchical list of items.
 *
 * The tree view has a concept of the current node and the tree is
 * built up and interrogated by moving the current node and calling
 * the methods on it. @see TreeViewCurrentNode
 *
 * To use a TreeView gadget you will need to install the TreeView module
 * written by Rik Griffin and ensure it is loaded before running your
 * program using RMEnsure.
 */
class TreeView: public tbx::Gadget {
public:
	enum {TOOLBOX_CLASS = 0x4028}; //!< Toolbox class for this gadget.

	TreeView() {} //!< Construct an uninitialised TreeView.

	/**
	 * Destroy a TreeView gadget.
	 *
	 * This does not delete the underlying toolbox gadget.
	 */
	~TreeView() {}

	/**
	 * Construct a TreeView from another TreeView.
	 *
	 * Both TreeViews will refer to the same underlying toolbox gadget.
	 */
	TreeView(const TreeView &other) : Gadget(other) {}

	/**
	 * Construct a TreeView from another gadget.
	 *
	 * The TreeView and the Gadget will refer to the same
	 * underlying toolbox gadget.
	 *
	 * @throws GadgetClassError if the underlying toolbox gadget
	 *         isn't a TreeView
	 */
	TreeView(const Gadget &other) : Gadget(other)	{check_toolbox_class(TreeView::TOOLBOX_CLASS);}

	/**
	 * Construct a TreeView field from a Component.
	 *
	 * @throws ObjectClassError if the component is not in a window.
	 * @throws GadgetClassError if the underlying toolbox gadget
	 *         isn't a TreeView
	 */
	TreeView(const Component &other) : Gadget(other) {}

	/**
	 * Assign a TreeView field to refer to the same underlying toolbox gadget
	 * as another.
	 */
	TreeView &operator=(const TreeView &other) {_handle = other.handle(); _id = other._id; return *this;}

	/**
	 * Assign a writable field to refer to the same underlying toolbox gadget
	 * as an existing Gadget.
	 * @throws GadgetClassError if the underlying toolbox gadget
	 *         isn't a TreeView
	 */
	TreeView &operator=(const Gadget &other) {_handle = other.handle(); _id = other.id(); check_toolbox_class(TOOLBOX_CLASS); return *this;}

	/**
	 * Assign a TreeView field to refer to the same underlying toolbox component
	 * as an existing Gadget.
	 * @throws ObjectClassError if the component is not in a window.
	 * @throws GadgetClassError if the underlying toolbox gadget
	 *         isn't a TreeView
	 */
	TreeView &operator=(const Component &other) {Window check(other.handle()); _handle = other.handle(); _id = other.id(); check_toolbox_class(TOOLBOX_CLASS); return *this;}

	/**
	 * Check if this TreeView refers to the same underlying toolbox gadget
	 * as another gadget.
	 * @returns true if they refer to the same underlying toolbox gadget.
	 */
	bool operator==(const Gadget &other) const {return (_handle == other.handle() && _id == other.id());}

	/**
	 * Check if this TreeView refers to the same underlying toolbox gadget
	 * as another gadget.
	 * @returns true if they do not refer to the same underlying toolbox gadget.
	 */
	bool operator!=(const Gadget &other) const {return (_handle != other.handle() || _id != other.id());}

	unsigned int state() const {return int_property(0x4029);}
	void state(unsigned int new_state) {int_property(0x4028, new_state);}
	bool state_flag(unsigned int flag) const {return (state() & flag) != 0;}
	void state_flag(unsigned int flag, bool set);

	bool allow_vscroll() const {return state_flag(1);}
	void allow_vscroll(bool value) {state_flag(1, value);}
	bool allow_hscroll() const {return state_flag(2);}
	void allow_hscroll(bool value) {state_flag(2, value);}
	bool auto_update() const {return state_flag(4);}
	void auto_update(bool value) {state_flag(4, value);}
	bool show_current_node() const {return state_flag(8);}
	void show_current_node(bool value) {state_flag(8, value);}
	bool plot_lines() const {return state_flag(16);}
	void plot_lines(bool value) {state_flag(16, value);}
	bool allow_selection() const {return state_flag(32);}
	void allow_selection(bool value) {state_flag(32,value);}
	bool notify_selection() const {return state_flag(64);}
	void notify_selection(bool value) {state_flag(64, value);}
	bool select_children() const {return state_flag(1u<<7);}
	void select_children(bool value) {state_flag(1u<<7, value);}
	bool allow_expand() const {return state_flag(1u<<8);}
	void allow_expand(bool value) {state_flag(1u<<8, value);}
	bool notify_expansion() const {return state_flag(1u<<9);}
	void notify_expansion(bool value) {state_flag(1u<<9, value);}
	bool expand_by_default() const {return state_flag(1u<<10);}
	void expand_by_default(bool value) {state_flag(1u<<10, value);}
	bool allow_rename() const {return state_flag(1u<<11);}
	void allow_rename(bool value) {state_flag(1u<<11, value);}
	bool notify_rename() const {return state_flag(1u<<12);}
	void notify_rename(bool value) {state_flag(1u<<12, value);}
	bool allow_drags() const {return state_flag(1u<<13);}
	void allow_drags(bool value) {state_flag(1u<<13, value);}
	bool notify_drag_end() const {return state_flag(1u<<14);}
	void notify_drag_end(bool value) {state_flag(1u<<14, value);}
	bool all_events() const {return state_flag(1u<<15);}
	void all_events(bool value) {state_flag(1u<<15,value);}
	bool double_click_expands() const {return state_flag(1u<<16);}
	void double_click_expands(bool value) {state_flag(1u<<16,value);}
	bool flat_mode() const {return state_flag(1u<<17);}
	void flat_mode(bool value) {state_flag(1u<<17,value);}
	bool text_below_sprite() const {return state_flag(1u<<18);}
	void text_below_sprite(bool value) {state_flag(1u<<18,value);}
	bool sort_display() const {return state_flag(1u<<19);}
	void sort_display(bool value) {state_flag(1u<<19,value);}
	bool sort_reversed() const {return state_flag(1u<<20);}
	void sort_reversed(bool value) {state_flag(1u<<20,value);}
	bool sort_by_sprite() const {return state_flag(1u<<21);}
	void sort_by_sprite(bool value) {state_flag(1u<<21,value);}

	/**
	 * Set menu for treeview
	 */
	void menu(Menu menu) {int_property(0x403c, (int)menu.handle());}

	TreeViewCurrentNode current_node();
	TreeViewCurrentNode current_node(TreeNodeId id);
	TreeNodeId find_node(int x, int y);

	void clear();

	void update_display();

	Window underlying_window() const;

	// Listeners
	void add_node_selected_listener(TreeViewNodeSelectedListener *listener);
	void remove_node_selected_listener(TreeViewNodeSelectedListener *listener);
	void add_node_expanded_listener(TreeViewNodeExpandedListener *listener);
	void remove_node_expanded_listener(TreeViewNodeExpandedListener *listener);
	/* Renaming is not currently supported
	 *void add_node_renamed_listener(TreeViewNodeRenamedListener *listener);
	 *void remove_node_renamed_listener(TreeViewNodeRenamedListener *listener);
	 */
	void add_node_dragged_listener(TreeViewNodeDraggedListener *listener);
	void remove_node_dragged_listener(TreeViewNodeDraggedListener *listener);
	void add_scroll_listener(TreeViewScrollListener *listener);
	void remove_scroll_listener(TreeViewScrollListener *listener);
};

/**
 * Class representing the current node in a tree view
 *
 * The nodes in a tree view are created, updated and properties checked
 * by setting the current node to the node in question and calling the
 * appropriate methods on it.
 */
class TreeViewCurrentNode : public tbx::Component
{
public:
	TreeViewCurrentNode() {} //!< Construct an uninitialised TreeViewCurrentNode.
	/**
	 * Construct the current node for the given tree view
	 * @param tree_view tree view watch the current node on
	 */
	TreeViewCurrentNode(TreeView &tree_view);
	/**
	 * Construct from an existing tree view current node.
	 *
	 * Both nodes refer to the same item.
	 */
	TreeViewCurrentNode(const TreeViewCurrentNode &other) {_handle = other._handle; _id = other._id;}
	/**
	 * Assign from an existing tree view current node.
	 *
	 * Both nodes refer to the same item.
	 */
	TreeViewCurrentNode&operator=(const TreeViewCurrentNode &other) {_handle = other._handle; _id = other._id; return *this;}
	/**
	 * Check if this current tree node item refers to the current node of the same tree view
	 *
	 * @return true if they are the same
	 */
	bool operator==(const TreeViewCurrentNode &other) const  {return (_handle == other._handle && _id == other._id);}
	/**
	 * Check if this current tree node item refers to the current node of the same tree view
	 *
	 * @return true if they are not the same
	 */
	bool operator!=(const TreeViewCurrentNode &other) const  {return (_handle != other._handle && _id != other._id);}

	/**
	 * Get the ID of the current node
	 */
	TreeNodeId node_id() const {return (TreeNodeId)int_property(0x402b);}

	void text(const std::string &text) {string_property(0x402c, text);}
	std::string text() const {return string_property(0x402f);}
	void sprite(int sprite_area_id, const std::string &sprite_name, const std::string &expanded_sprite_name);
	std::string sprite_name() const;
	std::string expanded_sprite_name() const;

	void private_word(void *word) {int_property(0x402e, (int)word);}
	void *private_word() const {return (void *)int_property(0x4031);}

    void move_to(TreeNodeId id);
	bool move_next(bool sorted = false);
	bool move_prev(bool sorted = false);
	bool move_child(bool sorted = false);
	bool move_parent();
	bool move_first_selected();
	bool move_next_selected();

	void expand(bool expand, bool recurse = false);
	void select(bool select, bool add);
	void make_visible();

	unsigned int state() const;
	bool has_next_sibling() const {return (state() & (1u<<0)) != 0;}
	bool has_prev_sibling() const {return (state() & (1u<<1)) != 0;}
	bool has_child() const {return (state() & (1u<<2)) != 0;}
	bool parent_is_root() const {return (state() & (1u<<3)) != 0;}
	bool has_sprite() const {return (state() & (1u<<4)) != 0;}
	bool has_expanded_sprite() const {return (state() & (1u<<5)) != 0;}
	bool has_text() const {return (state() & (1u<<6)) != 0;}
	bool expanded() const {return (state() & (1u<<7)) != 0;}
	bool selected() const {return (state() & (1u<<8)) != 0;}

	TreeNodeId add_child(const std::string &text);
	TreeNodeId add_sibling(const std::string &text);
	void erase();

};

}
}

#endif /* TBX_TREEVIEW_H_ */
