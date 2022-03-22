#ifndef TAB_SELECTED_LISTENER_H
#define TAB_SELECTED_LISTENER_H


#include "../gadget.h"
#include "../listener.h"
#include "../eventinfo.h"

namespace tbx
{
namespace ext
{

/**
 * A tab has been selected event information
 */
class TabsSelectionEvent : public EventInfo
{
public:
	/**
	 * Construct the event.
	 */
	TabsSelectionEvent(IdBlock &id_block, PollBlock &data) :
		EventInfo(id_block, data) {};
	
	 /**
	  * The new index of the tab
	  */
	 int selected() const {return _data.word[5];}
	 
     /**
      * The index of the tab prior to the selection
      */	  
	 int previous() const {return _data.word[6];}
};

/**
 * Listener for change of tab
 */
class TabsSelectionListener : public tbx::Listener
{
public:
	TabsSelectionListener() {};
	virtual ~TabsSelectionListener() {};

	virtual void tabs_selection(const TabsSelectionEvent &event) = 0;
};

}
}

#endif
