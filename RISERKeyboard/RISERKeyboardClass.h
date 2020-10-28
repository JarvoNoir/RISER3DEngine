#pragma once
#include "RISERKeyboardEvent.h"
#include <queue>

class RISERKeyboardClass
{
public:
	RISERKeyboardClass();
	bool KeyIsPressed(const unsigned char keycode);
	bool KeyBufferIsEmpty();
	bool CharBufferIsEmpty();
	bool IsKeysAutoRepeats();
	bool IsCharsAutoRepeats();
	RISERKeyboardEvent ReadKey();
	unsigned char ReadChar();
	void OnKeyPressed(const unsigned char key);
	void OnKeyReleased(const unsigned char key);
	void OnChar(const unsigned char key);
	void EnableAutoRepeatKeys();
	void DisableAutoRepeatKeys();
	void EnableAutoRepeatChars();
	void DisableAutoRepeatChars();
private:
	bool autoRepeatKeys = false;
	bool autoRepeatChars = false;
	bool keystates[256];
	std::queue<RISERKeyboardEvent> keyBuffer;
	std::queue<unsigned char> charBuffer;
};