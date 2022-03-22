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
 * treeviewlisteners.h
 *
 *  Created on: 13-Nov-2020
 *      Author: alanb
 */

#ifndef TBX_TREEVIEWLISTENERS_H_
#define TBX_TREEVIEWLISTENERS_H_

#include "../eventinfo.h"
#include "../listener.h"
#include "../pointerinfo.h"
#include "../visiblearea.h"

namespace tbx {
namespace ext {

typedef int TreeNodeId;
	
/**
 * Tree node has been selected event
 */
class TreeViewNodeSelectedEvent : public tbx::EventInfo
{
public:
	/**
	 * Construct the event.
	 */
	TreeViewNodeSelectedEvent(IdBlock &id_block, PollBlock &data) :
		EventInfo(id_block, data) {};

	/**
	 * Check if a node was selected.
	 */
	bool selected() const {return (_data.word[3] & 1)!=0;}
	/**
	 * Check if a multiple nodes are selected.
	 */
	bool multiple() const {return (_data.word[3] & 2)!=0;}
	
	/**
	 * Node that was selected/deselected
	 */
	 TreeNodeId node() const {return (TreeNodeId)_data.word[4];}
};

/**
 * Listener for node selected events
 */
class TreeViewNodeSelectedListener: public tbx::Listener {
public:
	/**
	 * Called when a node has been selected or deselected.
	 */
	virtual void treeview_node_selected(const TreeViewNodeSelectedEvent &event) = 0;
};

/**
 * Tree node has been expanded event
 */
class TreeViewNodeExpandedEvent : public tbx::EventInfo
{
public:
	/**
	 * Construct the event.
	 */
	TreeViewNodeExpandedEvent(IdBlock &id_block, PollBlock &data) :
		EventInfo(id_block, data) {};

	/**
	 * Check if a node was expanded.
	 */
	bool expanded() const {return (_data.word[3] & 1)!=0;}
	
	/**
	 * Node that was expanded/contracted
	 */
	 TreeNodeId node() const {return (TreeNodeId)_data.word[4];}
};

/**
 * Listener for node expanded events
 */
class TreeViewNodeExpandedListener: public tbx::Listener {
public:
	/**
	 * Called when a node is expanded or contracted.
	 */
	virtual void treeview_node_expanded(const TreeViewNodeExpandedEvent &event) = 0;
};

/* Renaming is not supported in the current treeview version
 * Tree node has been renamed event
 *
class TreeViewNodeRenamedEvent : public tbx::EventInfo
{
public:
	**
	 * Construct the event.
	 *
	TreeViewNodeRenamedEvent(IdBlock &id_block, PollBlock &data) :
		EventInfo(id_block, data) {};

	**
	 * Node that was renamed
	 *
	 TreeNodeId node() const {return (TreeNodeId)_data.word[4];}
};
*/

/* Renaming is not supported in the current treeview version
 * Listener for node renamed events
 *
class TreeViewNodeRenamedListener: public tbx::Listener {
public:
	// Called when a node is renamed.
	virtual void treeview_node_renamed(const TreeViewNodeRenamedEvent &event) = 0;
};
*/

/**
 * Tree node has been dragged event
 */
class TreeViewNodeDraggedEvent : public tbx::EventInfo
{
public:
	/**
	 * Construct the event.
	 */
	TreeViewNodeDraggedEvent(IdBlock &id_block, PollBlock &data) :
		EventInfo(id_block, data) {};

	/**
	 * Check if a node was dragged outside the treeview.
	 */
	bool outside() const {return (_data.word[3] & 1)!=0;}
	/**
	 * Check if a multiple nodes are being dragged.
	 */
	bool multiple() const {return (_data.word[3] & 2)!=0;}
	
	/**
	 * Node that was dragged
	 */
	 TreeNodeId node() const {return (TreeNodeId)_data.word[4];}
	 
	 /**
	  * Node it was dragged to
	  */
	 TreeNodeId destination_node() const {return (TreeNodeId)_data.word[5];}
	 
	 /**
	  * Buttons pressed
	  */
	  int mouse_buttons() const {return _data.word[6];}
	  
	  /**
	   * Was the select mouse button pressed
	   */
	   bool select() const {return (_data.word[6] == 4);}
	  /**
	   * Was the adjust mouse button pressed
	   */
	   bool adjust() const {return (_data.word[6] == 1);}
	  
	  /**
	   * Destination information
	   */
	  const tbx::PointerInfo &destination() const {return *((const tbx::PointerInfo *)&_data.word[7]);}	 
};

/**
 * Listener for node dragged events
 */
class TreeViewNodeDraggedListener: public tbx::Listener {
public:
	/**
	 * Called when a node is dragged.
	 */
	virtual void treeview_node_dragged(const TreeViewNodeDraggedEvent &event) = 0;
};


/**
 * Tree has been scrolled event
 */
class TreeViewScrollEvent : public EventInfo
{
public:
	/**
	 * Construct the event.
	 */
	TreeViewScrollEvent(IdBlock &id_block, PollBlock &data) :
		EventInfo(id_block, data) {};
	 
	  /**
	   * Scroll information
	   */
	  tbx::VisibleArea info() const {return tbx::VisibleArea(&(_data.word[4]));}
};

/**
 * Listener for tree view scroll events
 */
class TreeViewScrollListener: public tbx::Listener {
public:
	/**
	 * Called when the tree view is scrolled.
	 */
	virtual void treeview_scroll(const TreeViewScrollEvent &event) = 0;
};

	
}
}

#endif


