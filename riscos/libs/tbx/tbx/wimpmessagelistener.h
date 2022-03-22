/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2010 Alan Buckley   All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#ifndef TBX_WIMPMESSAGELISTENER_H_
#define TBX_WIMPMESSAGELISTENER_H_

#include "pollinfo.h"

namespace tbx {

/**
 * Wimp message information.
 */
class WimpMessage
{
public:
	/**
	 *  Use to refer to a message in an existing
	 *  wimp poll block.
	 *
	 * The poll block is NOT deleted when this class is
	 * deleted.
	 *
	 *@param poll_block Wimp poll block that contains a message
	 */
	WimpMessage(PollBlock &poll_block);

	/**
	 * Create a new message block for the specified action.
	 *
	 *@param message_id Action the code for the message.
	 *@param size size of the message block in (32 bit) words
	 */
	WimpMessage(int message_id, int size);

	/**
	 * Create a copy of an existing message.
	 *
	 *@param other message to copy
	 *@param size_override - specify the size of the message.
	 *       0 means use the other message size.
	 *       if greater then the other message size, the extra space
	 *       is uninitialised.
	 */
	WimpMessage(const WimpMessage &other, int size_override = 0);

	/**
	 * Create a reference or copy to another message
	 *
	 *@param other message to reference/copy
	 *@param copy true to create a copy
	 */
	WimpMessage(const WimpMessage &other, bool copy);

	/**
	 * Assignment creates a copy of an existing message.
	 *
	 *@param other message to copy.
	 */
	WimpMessage &operator=(const WimpMessage &other);

	/**
	 * Destructor for WimpMessage. Deletes the message block
	 * if it owns it.
	 */
	virtual ~WimpMessage();

	// Common attributes for all messages
	/**
	 * Get length of block, 20 - 256 bytes, a whole number of words
	 */
	int block_size() const	    	{return _message_block[0];}
	/**
	 * Get task handle of sender
	 */
	int sender_task_handle() const 	{return _message_block[1];}
	/**
	 * Get my reference from the message.
	 */
	int my_ref() const				{return _message_block[2];}
	/**
	 * Get your reference from the message
	 */
	int your_ref() const			{return _message_block[3];}
	/**
	 * Get the message ID
	 */
	int message_id() const			{return _message_block[4];}

	/**
	 * Set my reference for the message
	 *
	 * @param ref new reference
	 */
	void my_ref(int ref)			{_message_block[2] = ref;}
	/**
	 * Set your reference for the message
	 *
	 * @param ref new reference
	 */
	void your_ref(int ref)			{_message_block[3] = ref;}
	/**
	 * Set the message id for the message
	 *
	 * @param id new ID for the message
	 */
	void message_id(int id) 		{_message_block[4] = id;}

	/**
	 * Get message word (integer)
	 *
	 * @param index zero based index of the word in the message block.
	 *      i.e. 0 is at byte offset 0, 1 is at byte offset 4 etc.
	 * @returns 32 bit value at the given index
	 */
	int word(int index) const		{return _message_block[index];}

	/**
	 * Return reference to message word so it can be updated
	 *
	 * @param index zero based index of the word in the message block.
	 *      i.e. 0 is at byte offset 0, 1 is at byte offset 4 etc.
	 * @returns reference to the 32 bit value at the given index
	 */
	int &word(int index)			 {return _message_block[index];}

	/**
	 * Get message word (integer)
	 *
	 * @param index zero based index of the word in the message block.
	 *      i.e. 0 is at byte offset 0, 1 is at byte offset 4 etc.
	 * @returns 32 bit value at the given index
	 */
	int operator[] (int index) const {return _message_block[index];}

	/**
	 * Return reference to message word so it can be updated
	 *
	 * @param index zero based index of the word in the message block.
	 *      i.e. 0 is at byte offset 0, 1 is at byte offset 4 etc.
	 * @returns reference to the 32 bit value at the given index
	 */
	int &operator[] (int index) {return _message_block[index];}

	/**
	 * Return char * for part of the message
	 *
	 * @param index zero based index of the word in the message block
	 *   where the string starts.
	 *      i.e. 0 is at byte offset 0, 1 is at byte offset 4 etc.
	 * @returns char * pointing to the start of the string
	 */
	const char *str(int index) const {return reinterpret_cast<char *>(_message_block+index);}

	/**
	 * Return char * for part of the message
	 *
	 * @param index zero based index of the word in the message block
	 *   where the string starts.
	 *      i.e. 0 is at byte offset 0, 1 is at byte offset 4 etc.
	 * @returns char * pointing to the start of the string
	 */
	char *str(int index) {return reinterpret_cast<char *>(_message_block+index);}

	// Operations
	/**
	 * Enumeration specifying the type of message to send
	 */
	enum SendType {User = 17, Recorded = 18, Acknowledge = 19};
	/**
	 * Enumeration with special targets for sending messages to.
	 */
	enum SpecialDestination {Broadcast = 0, Iconbar = -2};
	int send(SendType type, int destination, int icon_handle = 0);

protected:
	int *_message_block; //!< pointer to the message data
	bool _owns_block;    //!< True if the message data will be deleted when this object is deleted.
};


/**
 * Wimp message received event.
 *
 * Call claim to stop further listeners processing the event.
 */
class WimpMessageEvent
{
private:
	WimpMessage _message;
	bool _claimed;

public:
	/**
	 * Constructor
	 */
	WimpMessageEvent(PollBlock &poll_block)  : _message(poll_block), _claimed(false) {}

	/**
	 * WimpMessage that was received.
	 */
	const WimpMessage &message() const  {return _message;}

	/**
	 * Claim the event so no other listeners process it
	 */
	void claim() {_claimed = true;}

	/**
	 * Has the event been claimed.
	 */
	bool claimed() const {return _claimed;}
};

/**
 * Base for listener for wimp messages
 */
class WimpMessageListener
{
public:
	/**
	 * Destructor
	 */
	virtual ~WimpMessageListener() {};
};

/**
 * Listener to handle WimpUserMessages (event code 17)
 */
class WimpUserMessageListener : public WimpMessageListener
{
public:
	/**
	 * Destructor does nothing
	 */
	virtual ~WimpUserMessageListener() {}

	/**
	 * Override this to process user messages
	 */
	virtual void user_message(WimpMessageEvent &event) {}
};

/**
 * Listener to handle WimpRecordedMessages (event code 18)
 */
class WimpRecordedMessageListener : public WimpMessageListener
{
public:
	/**
	 * Destructor does nothing
	 */
	virtual ~WimpRecordedMessageListener() {}


	/**
	 * Override this to process recorded user messages.
	 *
	 * These are message that require an acknowledgement.
	 */
	virtual void recorded_message(WimpMessageEvent &event, int reply_to) {}
};

/**
 * Listener to handle WimpAcnowledgeMessages (event code 19)
 */
class WimpAcknowledgeMessageListener : public WimpMessageListener
{
public:
	/**
	 * Destructor does nothing
	 */
	virtual ~WimpAcknowledgeMessageListener() {}
	/**
	 * Override this to process an acknowledgement.
	 */
	virtual void acknowledge_message(WimpMessageEvent &event) {}
};

}

#endif /* TBX_WIMPMESSAGELISTENER_H_ */
