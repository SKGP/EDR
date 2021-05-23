#pragma once

#include "stdafx.h"

static float CalculateCPULoad(unsigned long long idleTicks, unsigned long long totalTicks);
static unsigned long long FileTimeToInt64(const FILETIME& ft);
float GetCPULoad();
// the foo to use to check suspicious jumps in CPU usage
int checkCPU();
void checkjumps();