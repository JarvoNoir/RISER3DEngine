#pragma once
#include "RISERMouseEvent.h"
#include <queue>

class RISERMouseClass
{
public:
	void OnLeftPressed(int x, int y);
	void OnLeftReleased(int x, int y);
	void OnRightPressed(int x, int y);
	void OnRightReleased(int x, int y);
	void OnMiddlePressed(int x, int y);
	void OnMiddleReleased(int x, int y);
	void OnWheelUp(int x, int y);
	void OnWheelDown(int x, int y);
	void OnMouseMove(int x, int y);
	void OnMouseMoveRaw(int x, int y);
	bool IsLeftDown();
	bool IsRightDown();
	bool IsMiddleDown();
	int GetPosX();
	int GetPosY();
	RISERMousePoint GetPos();
	bool EventBufferIsEmpty();
	RISERMouseEvent ReadEvent();
private:
	std::queue<RISERMouseEvent> eventBuffer;
	bool leftDown = false;
	bool rightDown = false;
	bool middleDown = false;
	int x = 0;
	int y = 0;
};