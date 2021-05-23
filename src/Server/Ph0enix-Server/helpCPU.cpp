#pragma once

#include "helpCPU.h"

static float CalculateCPULoad(unsigned long long idleTicks, unsigned long long totalTicks)
{
	static unsigned long long _previousTotalTicks = 0;
	static unsigned long long _previousIdleTicks = 0;

	unsigned long long totalTicksSinceLastTime = totalTicks - _previousTotalTicks;
	unsigned long long idleTicksSinceLastTime = idleTicks - _previousIdleTicks;

	float ret = 1.0f - ((totalTicksSinceLastTime > 0) ? ((float)idleTicksSinceLastTime) / totalTicksSinceLastTime : 0);

	_previousTotalTicks = totalTicks;
	_previousIdleTicks = idleTicks;
	return ret;
}

static unsigned long long FileTimeToInt64(const FILETIME& ft) { return (((unsigned long long)(ft.dwHighDateTime)) << 32) | ((unsigned long long)ft.dwLowDateTime); }

// Returns 1.0f for "CPU fully pinned", 0.0f for "CPU idle", or somewhere in between
// You'll need to call this at regular intervals, since it measures the load between
// the previous call and the current one.  Returns -1.0 on error.
float GetCPULoad()
{
	FILETIME idleTime, kernelTime, userTime;
	return GetSystemTimes(&idleTime, &kernelTime, &userTime) ? CalculateCPULoad(FileTimeToInt64(idleTime), FileTimeToInt64(kernelTime) + FileTimeToInt64(userTime)) : -1.0f;
}

int checkCPU()
{
	std::vector<float> CPUvalus;
	float avg = 0;
	float cpuPersent = 0;
	for (int i = 0; i < 500; i++)
	{
		cpuPersent = GetCPULoad() * 100;
		CPUvalus.push_back(cpuPersent);
		avg += cpuPersent;
		std::this_thread::sleep_for(std::chrono::microseconds(100));
	}
	avg /= 500;

	
	return avg;
}

void checkjumps()
{
	int avg = checkCPU();
	float cpuPersent = 0;
	std::fstream myfile;
	myfile.open("CpuUsageLog.txt", std::fstream::app);
	myfile << "Average CPU usage: " << std::setprecision(2) << std::fixed << avg << std::endl;
	while (true)
	{
		cpuPersent = GetCPULoad() * 100;
		if (cpuPersent > (avg + 20))
		{
			myfile << "CPU usage jumped 20 percent up from avg: " << std::endl;
		}
		std::this_thread::sleep_for(std::chrono::microseconds(500));
	}
	myfile.close();
}

