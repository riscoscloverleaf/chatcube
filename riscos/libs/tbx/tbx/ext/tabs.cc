#include "tabs.h"
#include "tabsselectionlistener.h"

#include "../swixcheck.h"

#include <memory>
#include "kernel.h"

namespace tbx
{
namespace ext
{

/**
 * Get tab index from Window
 *
 * @returns index of supplied window or -1 if window not on tabs
 */
int Tabs::tab_from_window(const tbx::Window &window)
{
   _kernel_swi_regs regs;
   regs.r[0] = 0;
   regs.r[1] = _handle;
   regs.r[2] = 16432;
   regs.r[3] = _id;
   regs.r[4] = window.handle();
   swix_check(_kernel_swi(0x44ec6, &regs, &regs));

   return regs.r[0];
}


/**
 * Get Window from tab index
 *
 * @throws OsException if index invalid
 */
Window Tabs::window_from_tab(int index)
{
   _kernel_swi_regs regs;
   regs.r[0] = 0;
   regs.r[1] = _handle;
   regs.r[2] = 16433;
   regs.r[3] = _id;
   regs.r[4] = index;
   swix_check(_kernel_swi(0x44ec6, &regs, &regs));

   return Window((ObjectId)regs.r[0]);
}


  /**
   * Set the label for the tab.
   * By default the tab has the label of the window title.
   */
void Tabs::label(int index, const std::string &text)
{
   _kernel_swi_regs regs;
   regs.r[0] = 0;
   regs.r[1] = _handle;
   regs.r[2] = 16434;
   regs.r[3] = _id;
   regs.r[4] = index;
   regs.r[5] = reinterpret_cast<int>(text.c_str());
   swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}


  /**
   * Returns the label for a specified index
   */
std::string Tabs::label(int index) const
{
    _kernel_swi_regs regs;
    regs.r[0] = 0; // Flags are zero
    regs.r[1] = _handle;
    regs.r[2] = 16435;
    regs.r[3] = _id;
    regs.r[4] = index;
    regs.r[5] = 0;

    // Run Toolbox_ObjectMiscOp to get the size of the buffer
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));

    std::string value;
    int len = regs.r[6];
    if (len)
    {
       std::auto_ptr<char> m_buffer(new char[len]);
       regs.r[5] = reinterpret_cast<int>(m_buffer.get());
       regs.r[6] = len;

       // Run Toolbox_ObjectMiscOp to get the buffer
       swix_check(_kernel_swi(0x44ec6, &regs, &regs));

       value = m_buffer.get();
    }

    return value;
}

/**
 * Hide or show a tab
 *
 * @param index index of tab to hide/show
 * @param hide true to hide the tab, false to show it
 */
void Tabs::hide_tab(int index, bool hide)
{
    _kernel_swi_regs regs;
    regs.r[0] = 0; // Flags are zero
    regs.r[1] = _handle;
    regs.r[2] = 16436;
    regs.r[3] = _id;
    regs.r[4] = index;
    regs.r[5] = hide ? 1 : 0;

    // Run Toolbox_ObjectMiscOp to get the size of the buffer
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}




/*
 * handle tab selected event
 */
static void tabs_selection_handler(IdBlock &id_block, PollBlock &data, Listener *listener)
{
    TabsSelectionEvent event(id_block, data);
	static_cast<TabsSelectionListener *>(listener)->tabs_selection(event);
}


void Tabs::add_selection_listener(TabsSelectionListener *listener)
{
	add_listener(0x1402c0, listener, tabs_selection_handler);
}

void Tabs::remove_selection_listener(TabsSelectionListener *listener)
{
	remove_listener(0x1402c0, listener);
}

} // end ext namespace
} // end tbx namespace

