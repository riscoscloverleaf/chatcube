// GraphicsWnd.h
// Alan Buckley - Nov 2010-2012
// This is released under the same COPYRIGHT as TBX
//
// Handler for the the Window we are showing the graphics in

#ifndef _GRAPHICSWND_H_
#define _GRAPHICSWND_H_

#include "tbx/autocreatelistener.h"
#include "tbx/redrawlistener.h"
#include "tbx/offsetgraphics.h"
#include "tbx/window.h"
#include "tbx/jpeg.h"
#include "tbx/drawfile.h"
#include "tbx/drawpath.h"

/**
 * Class to set up listeners on, and paint graphics to a window.
 */
class GraphicsWnd
  : public tbx::AutoCreateListener,
    public tbx::RedrawListener
{
	  // Example non-sprite images
	  tbx::JPEG _jpeg;
	  tbx::DrawFile _draw;
	  tbx::DrawPath _fish;
public:
    virtual void auto_created(std::string template_name, tbx::Object object);
	virtual void redraw(const tbx::RedrawEvent &e);
	void paint(tbx::OffsetGraphics &g);

	static GraphicsWnd *from_window(tbx::Window window);
};

#endif
