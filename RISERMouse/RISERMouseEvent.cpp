#include "RISERMouseEvent.h"

RISERMouseEvent::RISERMouseEvent()
	:
	type(EventType::Invalid),
	x(0),
	y(0)
{}

RISERMouseEvent::RISERMouseEvent(EventType type, int x, int y)
	:
	type(type),
	x(x),
	y(y)
{}

bool RISERMouseEvent::IsValid() const { return this->type != EventType::Invalid; }
RISERMouseEvent::EventType RISERMouseEvent::GetType() const { return this->type; }
RISERMousePoint RISERMouseEvent::GetPos() const { return { this->x, this->y }; }
int RISERMouseEvent::GetPosX() const { return this->x; }
int RISERMouseEvent::GetPosY() const { return this->y; }