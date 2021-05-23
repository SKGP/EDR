#include "upTime.h"

void checkUpTime()
{
    std::fstream myfile;
    myfile.open("UpTimeLog.txt", std::fstream::app);
    std::string fileData;
    std::getline(myfile, fileData, (char)myfile.eof());
    while (true)
    {
        myfile.clear();
        myfile << fileData << std::endl;
        int nSysUpTimeMin = GetTickCount64() / 1000 / 60;
        int minutes = nSysUpTimeMin % 60;
        int hours = nSysUpTimeMin / 60;
        std::time_t t = std::time(0);
        std::tm* now = std::localtime(&t);

        std::string computerOn = "The computer was on for: " + std::to_string(hours) + " hours and " + std::to_string(minutes) + " minutes ";
        myfile << computerOn << std::endl;
        std::string timeNow = "Time now: " + std::to_string(now->tm_mday) + "/" + std::to_string(now->tm_mon + 1) + "/" + std::to_string(now->tm_year + 1900) + "       " + std::to_string(now->tm_hour) + ":";
        if (now->tm_min < 10)
        {
            timeNow += '0' + std::to_string(now->tm_min);
        }
        else
        {
            timeNow += std::to_string(now->tm_min);
        }
        myfile << timeNow << std::endl;
        int daysOn = hours / 24;
        int bootDay = now->tm_mday - daysOn;
        int bootMinute = now->tm_min - minutes;
        int bootHour = 0;
        if (bootMinute < 0)
        {
            bootMinute = 60 + bootMinute;
            bootHour--;
        }
        bootHour += now->tm_hour - hours;
        if (bootHour < 0)
        {
            bootHour = 24 + bootHour;
        }
        int bootMonth = now->tm_mon + 1;
        if (bootDay <= 0)
        {
            bootDay = 30 + bootDay;
            bootMonth--;
        }
        int bootYear = now->tm_year + 1900;
        if (bootMonth <= 0)
        {
            bootMonth = 12 + bootMonth;
            bootYear--;
        }
        std::string bootTime = "Boot time: " + std::to_string(bootDay) + "/" + std::to_string(bootMonth) + "/" + std::to_string(bootYear) + "        " + std::to_string(bootHour) + ":";
        if (bootMinute < 10)
        {
            bootTime = bootTime + "0" + std::to_string(bootMinute);
        }
        else
        {
            bootTime = bootTime + std::to_string(bootMinute);
        }
        myfile << bootTime << std::endl;
        myfile << "--------------------------------------" << std::endl;
        std::this_thread::sleep_for(std::chrono::minutes(5));
    }
    myfile.close();
}

int upTimeInMin()
{
    return GetTickCount64() / 1000 / 60;;
}

std::string timeNow()
{
    std::string timeNow = "";
    std::time_t t = std::time(0);
    std::tm* now = std::localtime(&t);
    if (now->tm_hour < 10) //adding zero to create hour in format hh
    {
        timeNow += '0' + std::to_string(now->tm_hour);
    }
    else
    {
        timeNow += std::to_string(now->tm_hour);
    }
    timeNow += ":";
    if (now->tm_min < 10) //adding zero to create min in format mm
    {
        timeNow += '0' + std::to_string(now->tm_min);
    }
    else
    {
        timeNow += std::to_string(now->tm_min);
    }
    timeNow += ":";
    if (now->tm_sec < 10) //adding zero to create sec in format ss
    {
        timeNow += '0' + std::to_string(now->tm_sec);
    }
    else
    {
        timeNow += std::to_string(now->tm_sec);
    }
    return timeNow;
}
