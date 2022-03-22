
#include "TbxExDoc.h"

TbxExDoc::TbxExDoc()
{
}

TbxExDoc::~TbxExDoc()
{
}

/**
 * Update the text for the document
 */
void TbxExDoc::text(const std::string &new_text)
{
	_text = new_text;
	modified(true);
}

/**
 * Save the document
 */
bool TbxExDoc::save(std::ostream &os)
{
	os.write(_text.c_str(), _text.length()+1);
	return os.good();
}

/**
 * Load the document
 */
bool TbxExDoc::load(std::istream &is, int estimated_size)
{
	if (estimated_size == -1) estimated_size = 256;
	// Create a buffer too large so the loop only goes round once
	// if the estimated size if correct.
	char *buffer = new char[estimated_size+2];
	while (is)
	{
		is.read(buffer, estimated_size+1);
		buffer[is.gcount()] = 0; // Null terminate
		_text += buffer;
	}
	delete [] buffer;
	return !is.bad();
}
