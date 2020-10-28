#pragma once

struct RISERMousePoint
{
	int x;
	int y;
};

class RISERMouseEvent
{
public:
	enum EventType
	{
		LPress,
		LRelease,
		RPress,
		RRelease,
		MPress,
		MRelease,
		WheelUp,
		WheelDown,
		Move,
		RAW_MOVE,
		Invalid
	};
private:
	EventType type;
	int x;
	int y;
public:
	RISERMouseEvent();
	RISERMouseEvent(const EventType type, const int x, const int y);
	bool IsValid() const;
	EventType GetType() const;
	RISERMousePoint GetPos() const;
	int GetPosX() const;
	int GetPosY() const;
};
