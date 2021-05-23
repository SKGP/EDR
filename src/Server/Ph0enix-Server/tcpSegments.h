#pragma once

#ifndef  WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // ! WIN32_LEAN_AND_MEAN

#include <Windows.h>

#include <WinSock2.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Iphlpapi.lib")

std::vector<int> getTcpStatsA();