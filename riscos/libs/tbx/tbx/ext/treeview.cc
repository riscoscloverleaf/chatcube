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
 * treeview.cc
 *
 *  Created on: 11-Nov-2020
 *      Author: alanb
 */

#include "treeview.h"
#include "treeviewlisteners.h"

#include "../swixcheck.h"

#include <kernel.h>

#include <iostream>

namespace tbx {
namespace ext {

static void node_selected_router(IdBlock &id_block, PollBlock &data, Listener *listener)
{
	TreeViewNodeSelectedEvent event(id_block, data);
	static_cast<TreeViewNodeSelectedListener *>(listener)->treeview_node_selected(event);
}	
static void node_expanded_router(IdBlock &id_block, PollBlock &data, Listener *listener)
{
	TreeViewNodeExpandedEvent event(id_block, data);
	static_cast<TreeViewNodeExpandedListener *>(listener)->treeview_node_expanded(event);
}	
/* Rename currently not supported in Treeview gadget
static void node_renamed_router(IdBlock &id_block, PollBlock &data, Listener *listener)
{
	TreeViewNodeRenamedEvent event(id_block, data);
	static_cast<TreeViewNodeRenamedListener *>(listener)->treeview_node_renamed(event);
}	
*/
static void node_dragged_router(IdBlock &id_block, PollBlock &data, Listener *listener)
{
	TreeViewNodeDraggedEvent event(id_block, data);
	static_cast<TreeViewNodeDraggedListener *>(listener)->treeview_node_dragged(event);
}	

static void scroll_router(IdBlock &id_block, PollBlock &data, Listener *listener)
{
	TreeViewScrollEvent event(id_block, data);
	static_cast<TreeViewScrollListener *>(listener)->treeview_scroll(event);
}	


	
/**
 * Set or clear and state flag
 * @param flag to modify
 * @param set true to set, false to clear
 */ 
void TreeView::state_flag(unsigned int flag, bool set)
{
    _kernel_swi_regs regs;
    regs.r[0] = 0; // Flags are zero
    regs.r[1] = _handle;
    regs.r[2] = 0x4029;
    regs.r[3] = _id;
	regs.r[4] = 0;

    // Run Toolbox_ObjectMiscOp
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));
	
	int current_flags = regs.r[0];
	
	regs.r[0] = 0;
	regs.r[2] = 0x4028;
	if (set) regs.r[4] = current_flags | flag;
	else regs.r[4] = current_flags & ~flag;

	if (regs.r[4] != current_flags)
	{
		// Run Toolbox_ObjectMiscOp
		swix_check(_kernel_swi(0x44ec6, &regs, &regs));
	}
}

/**
 * Get the current tree view node.
 *
 * Use this node to update, add nodes and set values
 * on the nodes of the tree.
 */
TreeViewCurrentNode TreeView::current_node()
{
	return TreeViewCurrentNode(*this);
}

/**
 * Move the current node in the tree to the given node id
 *
 * @param TreeNodeId id to move current node to
 * @return TreeViewCurrentNode of the moved node
 * @throws tbx::OsError if node id is invalid
 */
TreeViewCurrentNode TreeView::current_node(TreeNodeId id)
{
	TreeViewCurrentNode node(*this);
	node.move_to(id);
	
	return node;
}

/**
 * Find the tree node at the given location
 * @param x screen x coordinate of location
 * @param y screen y coordinate of location
 * @returns id of tree node at that location or 0 if none.
 */
TreeNodeId TreeView::find_node(int x, int y)
{
	_kernel_swi_regs regs;
    regs.r[0] = 0; // Flags are zero
    regs.r[1] = _handle;
    regs.r[2] = 0x4040;
    regs.r[3] = _id;
	regs.r[4] = x;
	regs.r[5] = y;

    // Run Toolbox_ObjectMiscOp
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));
	
	return TreeNodeId(regs.r[0]);
}


/**
 * Delete all nodes from the tree
 */
void TreeView::clear()
{
	_kernel_swi_regs regs;
    regs.r[0] = 0; // Flags are zero
    regs.r[1] = _handle;
    regs.r[2] = 0x4038;
    regs.r[3] = _id;

    // Run Toolbox_ObjectMiscOp
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}

/**
 * Update the display.
 * Call this after nodes are added/deleted or modified to reflect
 * the change in the treeview display
 */
void TreeView::update_display()
{
	_kernel_swi_regs regs;
    regs.r[0] = 0; // Flags are zero
    regs.r[1] = _handle;
    regs.r[2] = 0x4039;
    regs.r[3] = _id;

    // Run Toolbox_ObjectMiscOp
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}

/**
 * Get underlying window used for the treeview
 */
Window TreeView::underlying_window() const
{
	_kernel_swi_regs regs;
    regs.r[0] = 0; // Flags are zero
    regs.r[1] = _handle;
    regs.r[2] = 0x4042;
    regs.r[3] = _id;

    // Run Toolbox_ObjectMiscOp
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));
	
	return tbx::Window((ObjectId)regs.r[0]);
}

/**
 * Add a listener for when a node is selected or deselected
 * @param listener listener to add
 */ 
void TreeView::add_node_selected_listener(TreeViewNodeSelectedListener *listener)
{
		add_listener(0x140280, listener, node_selected_router);
}

/**
 * Remove a listener for when a node is selected or deselected
 * @param listener listener to remove
 */ 
void TreeView::remove_node_selected_listener(TreeViewNodeSelectedListener *listener)
{
	remove_listener(0x140280, listener);
}

/**
 * Add a listener for when a node is expanded or contracted
 * @param listener listener to add
 */ 
void TreeView::add_node_expanded_listener(TreeViewNodeExpandedListener *listener)
{
		add_listener(0x140281, listener, node_expanded_router);
}

/**
 * Remove a listener for when a node is expanded or contracted
 * @param listener listener to remove
 */ 
void TreeView::remove_node_expanded_listener(TreeViewNodeExpandedListener *listener)
{
	remove_listener(0x140281, listener);
}

/* Rename not currently supported
 * Add a listener for when a node is renamed
 * @param listener listener to add
void TreeView::add_node_renamed_listener(TreeViewNodeRenamedListener *listener)
{
		add_listener(0x140282, listener, node_renamed_router);
}
 */ 

/* Rename not currently supported
 * Remove a listener for when a node is renamed
 * @param listener listener to remove
 *
void TreeView::remove_node_renamed_listener(TreeViewNodeRenamedListener *listener)
{
	remove_listener(0x140282, listener);
}
*/

/**
 * Add a listener for when a node is dragged
 * @param listener listener to add
 */ 
void TreeView::add_node_dragged_listener(TreeViewNodeDraggedListener *listener)
{
		add_listener(0x140284, listener, node_dragged_router);
}

/**
 * Remove a listener for when a node is dragged
 * @param listener listener to remove
 */ 
void TreeView::remove_node_dragged_listener(TreeViewNodeDraggedListener *listener)
{
	remove_listener(0x140284, listener);
}

/**
 * Add a listener for when the tree view is scrolled
 * @param listener listener to add
 */ 
void TreeView::add_scroll_listener(TreeViewScrollListener *listener)
{
		add_listener(0x140285, listener, scroll_router);
}

/**
 * Remove a listener for when the tree view is scrolled
 * @param listener listener to remove
 */ 
void TreeView::remove_scroll_listener(TreeViewScrollListener *listener)
{
	remove_listener(0x140285, listener);
}

/**
 * Construct the current tree node for the give tree view
 * @param tree_view tree view to reference the current node from
 */
TreeViewCurrentNode::TreeViewCurrentNode(TreeView &tree_view) :
	tbx::Component(tree_view.handle(), tree_view.id())
{
}

/**
 * Set the sprites used for the current tree_view node
 *
 * @param sprite area_id (@see Sprite::area_id() or use 1 for the wimp sprite area)
 * @param sprite_name name of sprite to show (empty string for none)
 * @param expanded_sprite_name name of sprite to show when node is expanded (empty string for none)
 */
void TreeViewCurrentNode::sprite(int sprite_area_id, const std::string &sprite_name, const std::string &expanded_sprite_name)
{
	_kernel_swi_regs regs;
    regs.r[0] = 0; // Flags are zero
    regs.r[1] = _handle;
    regs.r[2] = 0x402d;
    regs.r[3] = _id;
	regs.r[4] = sprite_area_id;
	regs.r[5] = reinterpret_cast<int>(sprite_name.c_str());
	regs.r[6] = reinterpret_cast<int>(expanded_sprite_name.c_str());
		
    // Run Toolbox_ObjectMiscOp
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));	
}

/**
 * Get the name of the sprite for the current node
 * @returns the name of the sprite
 */
std::string TreeViewCurrentNode::sprite_name() const
{
	_kernel_swi_regs regs;
    regs.r[0] = 0; // 0 - get main sprite
    regs.r[1] = _handle;
    regs.r[2] = 0x4030;
    regs.r[3] = _id;
	
	char name[16]; // 12 is probably enough
	regs.r[4] = reinterpret_cast<int>(name);
	regs.r[5] = sizeof(name);
	
    // Run Toolbox_ObjectMiscOp
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));	
		
	return std::string(name, regs.r[5]);
}

/**
 * Get the name of the expanded sprite for the current node
 * @returns the name of the expanded sprite
 */
std::string TreeViewCurrentNode::expanded_sprite_name() const
{
	_kernel_swi_regs regs;
    regs.r[0] = 1; // 1 - get expanded sprite
    regs.r[1] = _handle;
    regs.r[2] = 0x4030;
    regs.r[3] = _id;
	
	char name[16]; // 12 is probably enough
	regs.r[4] = reinterpret_cast<int>(name);
	regs.r[5] = sizeof(name);
	
    // Run Toolbox_ObjectMiscOp
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));	
	
	return std::string(name, regs.r[5]);	
}


/**
 * Move current node to a give node id
 * @param id node id to move to
 */
void TreeViewCurrentNode::move_to(TreeNodeId id)
{
	_kernel_swi_regs regs;
    regs.r[0] = 0; // Flags are zero
    regs.r[1] = _handle;
    regs.r[2] = 0x4032;
    regs.r[3] = _id;
    regs.r[4] = id;
	
    // Run Toolbox_ObjectMiscOp
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));	
}

/**
 * Move current node to next sibling
 * @param sorted true to traverse nodes in their sorted order
 * @returns true if move is successful
 */ 
bool TreeViewCurrentNode::move_next(bool sorted /*= false*/)
{
	_kernel_swi_regs regs;
    regs.r[0] = (sorted) ? 1 : 0; // Flags are zero
    regs.r[1] = _handle;
    regs.r[2] = 0x4033;
    regs.r[3] = _id;
	
	return (_kernel_swi(0x44ec6, &regs, &regs) == 0);
}

/**
 * Move current node to previous sibling
 * @param sorted true to traverse nodes in their sorted order
 * @returns true if move is successful
 */ 
bool TreeViewCurrentNode::move_prev(bool sorted /*= false*/)
{
	_kernel_swi_regs regs;
    regs.r[0] = (sorted) ? 1 : 0; // Flags are zero
    regs.r[1] = _handle;
    regs.r[2] = 0x4034;
    regs.r[3] = _id;
	
	return (_kernel_swi(0x44ec6, &regs, &regs) == 0);
}

/**
 * Move current node to first child
 * @param sorted true to traverse nodes in their sorted order
 * @returns true if move is successful
 */ 
bool TreeViewCurrentNode::move_child(bool sorted /*= false*/)
{
	_kernel_swi_regs regs;
    regs.r[0] = (sorted) ? 1 : 0; // Flags are zero
    regs.r[1] = _handle;
    regs.r[2] = 0x4035;
    regs.r[3] = _id;
	
	return (_kernel_swi(0x44ec6, &regs, &regs) == 0);
}

/**
 * Move current node to the parent
 * @returns true if move is successful
 */ 	
bool TreeViewCurrentNode::move_parent()
{
	_kernel_swi_regs regs;
    regs.r[0] = 0; // Flags are zero
    regs.r[1] = _handle;
    regs.r[2] = 0x4036;
    regs.r[3] = _id;
	
	return (_kernel_swi(0x44ec6, &regs, &regs) == 0);
}

/**
 * Move current node to first selected node
 * @returns true if move is successful
 */ 
bool TreeViewCurrentNode::move_first_selected()
{
	_kernel_swi_regs regs;
    regs.r[0] = 0; // Flags are zero
    regs.r[1] = _handle;
    regs.r[2] = 0x403a;
    regs.r[3] = _id;
	
	return (_kernel_swi(0x44ec6, &regs, &regs) == 0);
}

/**
 * Move current node to the next selected node
 * @returns true if move is successful
 */ 
bool TreeViewCurrentNode::move_next_selected()
{
	_kernel_swi_regs regs;
    regs.r[0] = 0; // Flags are zero
    regs.r[1] = _handle;
    regs.r[2] = 0x403b;
    regs.r[3] = _id;
	
	return (_kernel_swi(0x44ec6, &regs, &regs) == 0);
}

/**
 * Expand or contract the current node
 * @param expand true to expand, false to contract
 * @param recurse recursively expand or contract the descendents (default = false)
 */
void TreeViewCurrentNode::expand(bool expand, bool recurse /*= false*/)
{
	_kernel_swi_regs regs;
    regs.r[0] = (expand ? 1 :0) | (recurse ? 2 : 0);
    regs.r[1] = _handle;
    regs.r[2] = 0x403d;
    regs.r[3] = _id;
	
    // Run Toolbox_ObjectMiscOp
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));	
}

/**
 * Select/deselect a node
 * @param select true to select a node
 * @param add true add to current selection, false replace current selection
 */
void TreeViewCurrentNode::select(bool select, bool add)
{
	_kernel_swi_regs regs;
    regs.r[0] = (select ? 1 : 0) | (add ? 0 : 2);
    regs.r[1] = _handle;
    regs.r[2] = 0x403e;
    regs.r[3] = _id;
	
    // Run Toolbox_ObjectMiscOp
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));	
}

/**
 * Make current node visible in the tree
 */
void TreeViewCurrentNode::make_visible()
{
	_kernel_swi_regs regs;
    regs.r[0] = 0; // Flags are zero
    regs.r[1] = _handle;
    regs.r[2] = 0x403f;
    regs.r[3] = _id;
	
    // Run Toolbox_ObjectMiscOp
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));		
}

unsigned int TreeViewCurrentNode::state() const
{
	_kernel_swi_regs regs;
    regs.r[0] = 0; // Flags are zero
    regs.r[1] = _handle;
    regs.r[2] = 0x4041;
    regs.r[3] = _id;
	
    // Run Toolbox_ObjectMiscOp
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));	
	
	return regs.r[0];	
}

/**
 * Add a new node as the child of the current node
 *
 * @param text Text for the child node
 * @returns node id of new node
 */
TreeNodeId TreeViewCurrentNode::add_child(const std::string &text)
{
	_kernel_swi_regs regs;
    regs.r[0] = 1; // Add as child
    regs.r[1] = _handle;
    regs.r[2] = 0x402a;
    regs.r[3] = _id;
	regs.r[4] = reinterpret_cast<int>(text.c_str());
	
    // Run Toolbox_ObjectMiscOp
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));	
	
	return (TreeNodeId)regs.r[0];
}

/**
 * Add a new node as the child of the current node
 *
 * @param text Text for the child node
 * @returns node id of new node
 */
TreeNodeId TreeViewCurrentNode::add_sibling(const std::string &text)
{	
	_kernel_swi_regs regs;
    regs.r[0] = 0; // Add as sibling
    regs.r[1] = _handle;
    regs.r[2] = 0x402a;
    regs.r[3] = _id;
	regs.r[4] = reinterpret_cast<int>(text.c_str());
	
    // Run Toolbox_ObjectMiscOp
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));	
	
	return (TreeNodeId)regs.r[0];
}

/**
 * Delete the current tree node
 */
void TreeViewCurrentNode::erase()
{
	_kernel_swi_regs regs;
    regs.r[0] = 0; // Add as sibling
    regs.r[1] = _handle;
    regs.r[2] = 0x4037;
    regs.r[3] = _id;
	
    // Run Toolbox_ObjectMiscOp
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));		
}


}
}

