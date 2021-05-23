#pragma once

#include <exception>
#include <iostream>
#include <thread>
#include <WinSock2.h>
#include <Windows.h>
#include <iostream>
#include <exception>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <curl/curl.h>
#include <numeric>
#include <cmath>
//mysql
#include <mysql.h>
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <mysql_driver.h>
//warnings
#pragma comment(lib, "Ws2_32.lib")

#include "handlerHelp.h"
#include "msgCodes.h"
#include "DataAccess.h"
#include "Agent.h"
#include "WSAInitializer.h"
#include "json.hpp"
#include "VirusTotal.h"
#include "Score.h"
#include "Analyzer.h"
