/*
 * TbxDocExWnd.h
 *
 */

#ifndef TBXEXWND_H_
#define TBXEXWND_H_

#include "tbx/doc/docwindow.h"
#include "tbx/doc/docpositionwindow.h"
#include "tbxexdoc.h"
#include "tbx/textchangedlistener.h"


/**
 * Class that shows the document in a window and
 * interacts with it.
 */
class TbxExWnd :
	public tbx::doc::DocWindow,
    public tbx::TextChangedListener
{
private:
	// This class is used to show each window at a
	// different position
	static tbx::doc::DocPositionWindow _start_pos;

public:
	TbxExWnd(TbxExDoc *doc);
	virtual ~TbxExWnd();

	// Text changed listener
	virtual void text_changed(tbx::TextChangedEvent &event);

	// Convenience functions to get a typed version of the document
	TbxExDoc *document() {return static_cast<TbxExDoc *>(_document);}
	TbxExDoc *document() const {return static_cast<TbxExDoc *>(_document);}
};

#endif /* TBXEXWND_H_ */
