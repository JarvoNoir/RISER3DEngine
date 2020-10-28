#pragma once

class RISERKeyboardEvent
{
public:
	enum EventType
	{
		Press,
		Release,
		Invalid
	};

	RISERKeyboardEvent();
	RISERKeyboardEvent(const EventType type, const unsigned char key);
	bool isPress() const;
	bool isRelease() const;
	bool isValid() const;
	unsigned char GetKeyCode() const;
private:
	EventType type;
	unsigned char key;
};