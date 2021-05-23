#include "VirusTotal.h"

#define CURL_STATICLIB

VirusTotal::VirusTotal()
{
}

VirusTotal::~VirusTotal()
{
}

size_t write_data(void* buffer, size_t size, size_t nmemb, void* userp)
{
    return size * nmemb;
}

size_t writefunc(void* ptr, size_t size, size_t nmemb, std::string* s)
{
    s->append(static_cast<char*>(ptr), size * nmemb);
    return size * nmemb;
}


bool VirusTotal::checkUnknownProcesses(std::string sha1)
{
    /*
    MAKE it more effective, in case a new sha1 check if its already in the watchlist and if no keep the program running.
    */
    CURL* curl;
    CURLcode res;
    std::string js;

    curl = curl_easy_init();

    std::string ls = "https://www.virustotal.com/vtapi/v2/file/report?apikey=<API_KEY>&resource=" + sha1;

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &js);


    if (curl) {
        
        curl_easy_setopt(curl, CURLOPT_URL, ls.c_str());

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        
        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    std::cout << "js be like: " << std::endl;

    if (js != "")
    {
        std::istringstream file(js);
        nlohmann::json j;
        file >> j;

        std::cout << j["positives"] << std::endl;
        std::cout << j["scan_id"] << std::endl;

        if (j["scan_id"] >= 5) { //in case 5 or more AV says its a virus add to watchlist
            return true;
        }
    }
    return false;
    
}


