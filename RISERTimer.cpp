#include "RISERTimer.h"

RISERTimer::RISERTimer()
{
	start = std::chrono::high_resolution_clock::now();
	stop = std::chrono::high_resolution_clock::now();
}

double RISERTimer::GetMillisecondsElapsed()
{
	if (isRunning)
	{
		auto elapsed = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - start);
		return elapsed.count();
	}
	else
	{
		auto elapsed = std::chrono::duration<double, std::milli>(stop - start);
		return elapsed.count();
	}
}

void RISERTimer::Restart()
{
	isRunning = true;
	start = std::chrono::high_resolution_clock::now();
}

bool RISERTimer::Stop()
{
	if (!isRunning)
		return false;
	else
	{
		stop = std::chrono::high_resolution_clock::now();
		isRunning = false;
		return true;
	}
}

bool RISERTimer::Start()
{
	if (isRunning)
	{
		return false;
	}
	else
	{
		start = std::chrono::high_resolution_clock::now();
		isRunning = true;
		return true;
	}
}
