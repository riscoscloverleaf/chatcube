// SaveAsSprite.cc
// Alan Buckley - Nov 2010
// This is released under the same COPYRIGHT as TBX
//
// Handler for the SaveAsSprite dialogue

#include "SaveAsSprite.h"
#include "GraphicsWnd.h"
#include "tbx/modeinfo.h"

/**
 * Constructor adds listeners to the SaveAs object to this
 * object to allow it to process the saveas messages.
 */
SaveAsSprite::SaveAsSprite(tbx::Object obj)
{
	tbx::SaveAs save_as(obj);
	save_as.add_about_to_be_shown_listener(this);
	save_as.set_save_to_file_handler(this);
	save_as.set_fill_buffer_handler(this);
	save_as.add_dialogue_completed_listener(this);
	_sprite_area = 0;
}

/**
 * Destructor deletes the sprite area if it is not already deleted.
 */
SaveAsSprite::~SaveAsSprite()
{
	delete _sprite_area;
}

/**
 * This event is generated whenever the SaveAs dialogue is about
 * to be shown and is used here to give an estimated size of
 * the file we would be saving.
 */
void SaveAsSprite::about_to_be_shown(tbx::AboutToBeShownEvent &event)
{
	tbx::SaveAs save_as = event.id_block().self_object();
	save_as.file_size(449000); // Estimated size of sprite
}


/**
 * Save directly to file
 */
void SaveAsSprite::saveas_save_to_file(tbx::SaveAs saveas, bool selection, std::string file_name)
{
	create_sprite(saveas);
	if (_sprite_area != 0)
	{
		_sprite_area->save(file_name);
	}
	saveas.file_save_completed((_sprite_area != 0), file_name);
}

/**
 * Save using ram transfer
 */
void SaveAsSprite::saveas_fill_buffer(tbx::SaveAs saveas, bool selection, int size, void *buffer, int already_transmitted)
{
	if (buffer == 0)
	{
		// First call, so create the sprite
		create_sprite(saveas);
		if (_sprite_area == 0)
		{
			saveas.buffer_filled(0, 0);
			return;
		}
	}
	// +/-4 is because saved sprite does not save the first work of the sprite area block.
	buffer = ((char *)_sprite_area->pointer()) + already_transmitted + 4;
	int left = _sprite_area->size() - already_transmitted - 4;
	if (left > size) left = size;
	saveas.buffer_filled(buffer, left);
}

/**
 * Clean up after save has finished
 */
void SaveAsSprite::saveas_dialogue_completed(const tbx::SaveAsDialogueCompletedEvent &completed_event)
{
	delete _sprite_area;
	_sprite_area = 0;
}

/**
 * Create a sprite of the graphics window content
 */
void SaveAsSprite::create_sprite(tbx::SaveAs saveas)
{
	tbx::Window graphics_window = saveas.ancestor_object();
	GraphicsWnd *graphics_wnd = GraphicsWnd::from_window(graphics_window);

	tbx::BBox extent = graphics_window.extent();
	int sprite_width = extent.width();
	int sprite_height = extent.height();

	_sprite_area = new tbx::SpriteArea();

	// Use 256 colour sprite
	tbx::ModeInfo mi;
	int sprite_mode;
	switch(mi.colours())
	{
	case 2: sprite_mode = tbx::SF_Colour2dpi90; break;
	case 4: sprite_mode = tbx::SF_Colour4dpi90; break;
	case 16: sprite_mode = tbx::SF_Colour16dpi90; break;
	case 256: sprite_mode = tbx::SF_Colour256dpi90; break;
	default:
		// On a modern machine with high colour modes
		sprite_mode = tbx::sprite_mode(tbx::SC_Colour16M);
		break;
	}

	tbx::UserSprite sprite = _sprite_area->create_sprite("graphics", sprite_width, sprite_height, sprite_mode);
	if (sprite.is_valid())
	{
		// Use SpriteCapture to grab what would be sent to the screen
		tbx::SpriteCapture cap(&sprite, true);
		tbx::OffsetGraphics g(0, sprite_height);
		g.background(tbx::Colour::white);
		g.clear();
		graphics_wnd->paint(g);
	} else
	{
		delete _sprite_area;
		_sprite_area = 0;
	}
}
