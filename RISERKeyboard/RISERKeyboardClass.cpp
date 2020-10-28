#include "RISERKeyboardClass.h"

RISERKeyboardClass::RISERKeyboardClass()
{
	//set all states to false
	for (int i = 0; i < 256; i++)
		this->keystates[i] = false;
}

bool RISERKeyboardClass::KeyIsPressed(const unsigned char keycode) { return this->keystates[keycode]; }
bool RISERKeyboardClass::KeyBufferIsEmpty() { return this->keyBuffer.empty(); }
bool RISERKeyboardClass::CharBufferIsEmpty() { return this->charBuffer.empty(); }
bool RISERKeyboardClass::IsKeysAutoRepeats(){return this->autoRepeatKeys;}
bool RISERKeyboardClass::IsCharsAutoRepeats(){return this->autoRepeatChars;}

RISERKeyboardEvent RISERKeyboardClass::ReadKey()
{
	if (this->keyBuffer.empty())
		return RISERKeyboardEvent();//return empty keyboard event
	else
	{
		//get first keyboard event from queue
		RISERKeyboardEvent ke = this->keyBuffer.front();
		//remove first event form the queue
		this->keyBuffer.pop();
		return ke;
	}
}

unsigned char RISERKeyboardClass::ReadChar()
{
	if (this->charBuffer.empty())
		return 0u;
	else
	{
		//get first char from queue
		unsigned char c = this->charBuffer.front();
		//remove first char form the queue
		this->charBuffer.pop();
		return c;
	}
}

void RISERKeyboardClass::OnKeyPressed(const unsigned char key)
{
	this->keystates[key] = true;
	this->keyBuffer.push(RISERKeyboardEvent(RISERKeyboardEvent::EventType::Press, key));
}

void RISERKeyboardClass::OnKeyReleased(const unsigned char key)
{
	this->keystates[key] = true;
	this->keyBuffer.push(RISERKeyboardEvent(RISERKeyboardEvent::EventType::Release, key));
}

void RISERKeyboardClass::OnChar(const unsigned char key) {this->charBuffer.push(key);}
void RISERKeyboardClass::EnableAutoRepeatKeys() { this->autoRepeatKeys = true; }
void RISERKeyboardClass::DisableAutoRepeatKeys() { this->autoRepeatKeys = false; }
void RISERKeyboardClass::EnableAutoRepeatChars() { this->autoRepeatChars = true; }
void RISERKeyboardClass::DisableAutoRepeatChars() { this->autoRepeatChars = false; }
