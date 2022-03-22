/*
 * TbxExWnd.cc
 *
 */

#include "TbxExWnd.h"
#include "tbx/writablefield.h"

// Reposition each new window class
tbx::doc::DocPositionWindow TbxExWnd::_start_pos;

/**
 * Constructor called by document manager after a document
 * has been created or loaded.
 */
TbxExWnd::TbxExWnd(TbxExDoc *doc) : DocWindow(doc, "DocWindow")
{
	// Get the gadget used to edit the text
	tbx::WritableField text_field = _window.gadget(0);
	// Update with contents of document
	text_field.text(doc->text());
	// Listen for changes
	text_field.add_text_changed_listener(this);
	// Show window in a new position
	_start_pos.show(_window);
}

TbxExWnd::~TbxExWnd()
{
}

/**
 * Text changed callback from the writable field
 */
void TbxExWnd::text_changed(tbx::TextChangedEvent &event)
{
	// Update the document text to match.
	document()->text(event.text());
}
