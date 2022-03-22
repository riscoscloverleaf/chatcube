// SaveAsSprite.h
// Alan Buckley - Nov 2010
// This is released under the same COPYRIGHT as TBX
//
// Handler for the SaveAsSprite dialogue
//
#ifndef SAVEASSPRITE_H_
#define SAVEASSPRITE_H_

#include "tbx/saveas.h"
#include "tbx/sprite.h"
#include "tbx/abouttobeshownlistener.h"

/**
 * Class to handle messages from the SaveAs dialogue box
 */
class SaveAsSprite :
	tbx::SaveAsSaveToFileHandler,
	tbx::SaveAsFillBufferHandler,
	tbx::SaveAsDialogueCompletedListener,
	tbx::AboutToBeShownListener
{
private:
	tbx::SpriteArea *_sprite_area;
	void create_sprite(tbx::SaveAs saveas);
public:
	SaveAsSprite(tbx::Object obj);
	virtual ~SaveAsSprite();

	virtual void about_to_be_shown(tbx::AboutToBeShownEvent &event);
	virtual void saveas_save_to_file(tbx::SaveAs saveas, bool selection, std::string file_name);
	virtual void saveas_fill_buffer(tbx::SaveAs saveas, bool selection, int size, void *buffer, int already_transmitted);
	virtual void saveas_dialogue_completed(const tbx::SaveAsDialogueCompletedEvent &completed_event);
};

#endif /* SAVEASSPRITE_H_ */
