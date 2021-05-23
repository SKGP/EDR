#pragma once

#include <exception>
#include <iostream>
#include <thread>
#include <WinSock2.h>
#include <Windows.h>
#include <iostream>
#include <exception>
#include <iomanip>
#include <vector>
#include <ctime>
#include <string>
#include <fstream>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>

//mysql
#include <mysql.h>
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <mysql_driver.h>

#pragma comment(lib, "Ws2_32.lib")


#include "Server.h"
#include "WSAInitializer.h"
#include "Communicator.h"
#include "Sniffer.h"
#include "helpCPU.h"
#include "upTime.h"
#include "tcpSegments.h"
#include "DataAccess.h"