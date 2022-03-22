// GraphicsWnd.cc
// Alan Buckley - Nov 2010
// This is released under the same COPYRIGHT as TBX
//
// Handler for the the Window we are showing the graphics in

#include "GraphicsWnd.h"
#include "tbx/window.h"
#include "tbx/offsetgraphics.h"
#include "tbx/application.h"
#include "tbx/sprite.h"


using namespace tbx;

/**
 * Set up listeners for the window and loads example images
 */
void GraphicsWnd::auto_created(std::string template_name, Object object)
{
    Window window(object);
    window.client_handle(this);
    window.add_redraw_listener(this);
    _jpeg.load("<Graphics$Dir>.bluebells");
    _draw.load("<Graphics$Dir>.Drawing");

    _fish.move(30,-30);
    _fish.bezier(100, 30, 300,200, 400,0);
    _fish.bezier(300, -200, 100, -30, 30, 30);
    _fish.close_line();
    _fish.circle(360,10, 20);
    _fish.end_path();
}

/**
 * Get GraphicsWnd from window it is attached to.
 */
GraphicsWnd *GraphicsWnd::from_window(tbx::Window window)
{
	return reinterpret_cast<GraphicsWnd *>(window.client_handle());
}


/**
 * Redraw the exposed area of the window
 */
void GraphicsWnd::redraw(const tbx::RedrawEvent &e)
{
	tbx::OffsetGraphics g(e.visible_area());
	paint(g);
}

/**
 * Do the actual painting.
 *
 * This is separated out so it can be used in SaveAsSprite as well.
 */
void GraphicsWnd::paint(tbx::OffsetGraphics &g)
{
	g.text_colours(tbx::Colour::black, tbx::Colour::white);

	g.text(0,-32, "This shows some of the facilities of the graphics object");

	g.text(0,-96, "A point");
	g.foreground(tbx::Colour::black);
	g.point(160,-96);

	g.text(0,-160,"A line");
	g.line(160,-160, 256, -160);

	g.text(0,-224,"Rectangles");
	g.rectangle(160, -232, 200, -202);
	g.foreground(tbx::Colour(255,0,0));
	g.fill_rectangle(232,-232,272,-202);

	g.foreground(tbx::Colour::black);
	g.text(0,-288, "Circles");
	g.circle(160, -280, 32);
	g.foreground(tbx::Colour(0,255, 0));
	g.fill_circle(232, -280, 32);

	g.foreground(tbx::Colour::black);
	g.text(0,-352, "Ellipses");
	g.ellipse(200,-340, 160,-340, 180, -320);
	g.foreground(tbx::Colour(0,0, 255));
	g.fill_ellipse(340,-340, 300,-340, 340, -320);

	g.foreground(tbx::Colour::black);
	g.text(0,-416, "arc, segment and sector");
	g.arc(160, -460, 128, -460, 180, -400);
	g.foreground(tbx::Colour(255,0, 255));
	g.segment(240, -460, 208, -460, 240, -400);
	g.sector(320, -460, 288, -460, 340, -400);

	g.foreground(tbx::Colour::black);
	g.text(0, -524, "A path");
    tbx::Point pts[6];
    pts[0].x = 160; pts[0].y = -524;
    pts[1].x = 180; pts[1].y = -500;
    pts[2].x = 210; pts[2].y = -550;
    pts[3].x = 220; pts[3].y = -512;
    pts[4].x = 300; pts[4].y = -536;
    g.path(pts, 5);

    g.text(0,-588, "Polygons");
    pts[0].x = 0; pts[0].y = 0;
    pts[1].x = 32; pts[1].y = 32;
    pts[2].x = 64; pts[2].y = 32;
    pts[3].x = 96; pts[3].y = 0;
    pts[4].x = 64; pts[4].y = -32;
    pts[5].x = 32; pts[5].y = -32;

    // Demonstrate shifting offset as well
    tbx::Point old_offset = g.offset();
    tbx::Point new_offset(old_offset);
    new_offset.x += 160;
    new_offset.y += -588;
    g.offset(new_offset);
    g.polygon(pts,6);
    new_offset.x += 128;
    g.offset(new_offset);
    g.foreground(tbx::Colour(0,255,255));
    g.fill_polygon(pts, 6);
    g.offset(old_offset);

	g.foreground(tbx::Colour::black);
	g.text(0,-668, "Fonts");
	tbx::Font corpus("Corpus.Medium", tbx::os_to_points_16th(48));
	g.text(160,-668, "This text is in Corpus Medium font", corpus);

	// Show application sprite from WIMP sprite pool
	g.text(0,-732, "Sprites");
	tbx::WimpSprite app_sprite("!graphics");
	g.image(160, -752, app_sprite);

	// Get and show user sprite from sprites loaded with application
	tbx::UserSprite us = app()->sprite_area()->get_sprite("example");
	g.image(320, -752, us);

	g.text(0, -850, "JPEGs");
	if (_jpeg.is_valid()) g.image(160, -1040, _jpeg);
	else g.text(160, -850, "Unable to load 'bluebells' image");

	g.text(0, -1112, "Draw files");
	if (_draw.is_valid()) g.image(160, -1600, _draw);
	else g.text(160, -1112, "Unable to load 'Drawing' image");

	g.text(0, -1328, "Drawing paths");

	g.foreground(tbx::Colour(255,0,0));
	g.fill(160, -1400, _fish);

	// if you want a one pixel line you can just call
	// g.stroke(160, -1400, _fish);

	g.foreground(tbx::Colour::black);
	tbx::DrawCapAndJoin caps;
	g.stroke(160, -1400, _fish, tbx::WINDING_NON_ZERO,1,8,&caps);

}
