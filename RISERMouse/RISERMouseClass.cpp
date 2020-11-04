#include "RISERMouseClass.h"

void RISERMouseClass::OnLeftPressed(int x, int y)
{
	this->leftDown = true;
	RISERMouseEvent m(RISERMouseEvent::EventType::LPress, x, y);
	this->eventBuffer.push(m);
}

void RISERMouseClass::OnLeftReleased(int x, int y)
{
	this->leftDown = false;
	this->eventBuffer.push(RISERMouseEvent(RISERMouseEvent::EventType::LRelease, x, y));
}

void RISERMouseClass::OnRightPressed(int x, int y)
{
	this->rightDown = true;
	this->eventBuffer.push(RISERMouseEvent(RISERMouseEvent::EventType::RPress, x, y));
}

void RISERMouseClass::OnRightReleased(int x, int y)
{
	this->rightDown = false;
	this->eventBuffer.push(RISERMouseEvent(RISERMouseEvent::EventType::RRelease, x, y));
}

void RISERMouseClass::OnMiddlePressed(int x, int y)
{
	this->middleDown = true;
	this->eventBuffer.push(RISERMouseEvent(RISERMouseEvent::EventType::MPress, x, y));
}

void RISERMouseClass::OnMiddleReleased(int x, int y)
{
	this->middleDown = false;
	this->eventBuffer.push(RISERMouseEvent(RISERMouseEvent::EventType::MRelease, x, y));
}

void RISERMouseClass::OnWheelUp(int x, int y)
{
	this->eventBuffer.push(RISERMouseEvent(RISERMouseEvent::EventType::WheelUp, x, y));
}

void RISERMouseClass::OnWheelDown(int x, int y)
{
	this->eventBuffer.push(RISERMouseEvent(RISERMouseEvent::EventType::WheelDown, x, y));
}

void RISERMouseClass::OnMouseMove(int x, int y)
{
	this->x = x;
	this->y = y;
	this->eventBuffer.push(RISERMouseEvent(RISERMouseEvent::EventType::Move, x, y));
}

void RISERMouseClass::OnMouseMoveRaw(int x, int y)
{
	this->eventBuffer.push(RISERMouseEvent(RISERMouseEvent::EventType::RAW_MOVE, x, y));
}

bool RISERMouseClass::IsLeftDown() { return this->leftDown; }
bool RISERMouseClass::IsRightDown() { return this->rightDown; }
bool RISERMouseClass::IsMiddleDown() { return this->middleDown; }
int RISERMouseClass::GetPosX() { return this->x; }
int RISERMouseClass::GetPosY() { return this->y; }
RISERMousePoint RISERMouseClass::GetPos() { return { this->x, this->y }; }
bool RISERMouseClass::EventBufferIsEmpty() { return this->eventBuffer.empty(); }

RISERMouseEvent RISERMouseClass::ReadEvent()
{
	if (this->eventBuffer.empty())
		return RISERMouseEvent();
	else
	{
		//get first event from eventBuffer
		RISERMouseEvent m = this->eventBuffer.front();
		//remove it from event buffer
		this->eventBuffer.pop();
		return m;
	}
}