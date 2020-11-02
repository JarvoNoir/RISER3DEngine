#pragma once
#include <chrono>

class RISERTimer
{
public:
	RISERTimer();
	double GetMillisecondsElapsed();
	void Restart();
	bool Start();
	bool Stop();
private:
	bool isRunning = false;

#ifdef _WIN32
	std::chrono::time_point<std::chrono::steady_clock> start;
	std::chrono::time_point<std::chrono::steady_clock> stop;
#else
	std::chrono::time_point < std::chrono::steady_clock> start;
	std::chrono::time_point < std::chrono::steady_clock> stop;
#endif
};
