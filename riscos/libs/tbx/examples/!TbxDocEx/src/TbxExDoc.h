/*
 * TbxExDoc.h
 */

#ifndef TBXEXDOC_H_
#define TBXEXDOC_H_

#include "tbx/doc/document.h"
#include <string>

/**
 * Simple document that just looks after a line of text
 */
class TbxExDoc: public tbx::doc::Document
{
	std::string _text;

public:
	TbxExDoc();
	virtual ~TbxExDoc();

	enum {FILE_TYPE = 0x010};
	
	void text(const std::string &new_text);

	/**
	 * Get the text from the document
	 */
	const std::string &text() const {return _text;}

	/**
	 * Return the file type for this document
	 */
	virtual int file_type() const {return FILE_TYPE;}

	/**
	 * Return the size of the document.
	 *
	 * We always know the size so return that
	 */
	virtual int document_size() const {return _text.length()+1;}

	virtual bool save(std::ostream &os);
	virtual bool load(std::istream &is, int estimated_size);
};

#endif /* TBXEXDOC_H_ */
