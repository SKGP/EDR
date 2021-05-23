#include "tcpSegments.h"

/*
The function create tcpstat and allocate memory for it
then GetTcpStatistics initialize pTcpStats with all the stats
then we call dwInSegs to get all the in segements since the pc turned on and dwoutsegs to get all the out segs
*/
std::vector<int> getTcpStatsA()
{
    DWORD dwRetVal = 0;
    PMIB_TCPSTATS pTcpStats;

    pTcpStats = (MIB_TCPSTATS*)malloc(sizeof(MIB_TCPSTATS));
    if (pTcpStats == NULL) //make sure allocation was succesful
    {
        printf("failed allocation memory\n");
    }
    if ((dwRetVal = GetTcpStatistics(pTcpStats)) != NO_ERROR)
    {
        printf("error bitch\n");
    }
    printf("seg in: %u\n", pTcpStats->dwInSegs);
    printf("seg out: %u\n", pTcpStats->dwOutSegs);
    if (pTcpStats)
        free(pTcpStats);
    std::vector<int> values;
    values.push_back(pTcpStats->dwInSegs);
    values.push_back(pTcpStats->dwOutSegs);
    return values;
}