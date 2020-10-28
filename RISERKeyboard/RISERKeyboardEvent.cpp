#include "RISERKeyboardEvent.h"

RISERKeyboardEvent::RISERKeyboardEvent()
	:
	type(EventType::Invalid),
	key(0u)
{
}

RISERKeyboardEvent::RISERKeyboardEvent(const EventType type, const unsigned char key)
	:
	type(type),
	key(key)
{
}

bool RISERKeyboardEvent::isPress() const { return this->type == EventType::Press; }
bool RISERKeyboardEvent::isRelease() const { return this->type == EventType::Release; }
bool RISERKeyboardEvent::isValid() const { return this->type == EventType::Invalid; }

unsigned char RISERKeyboardEvent::GetKeyCode() const { return this->key; }
