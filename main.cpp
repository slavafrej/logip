#include <iostream>
#include <string>
#include <fstream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;


class IP{
    private:
        const char *getIP = "https://api.ipify.org";
        const char *getInfoByIP = "http://ip-api.com/json/";
        static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
        {
            ((string*)userp)->append((char*)contents, size * nmemb);
            return size * nmemb;
        }
        string sendRequest(string url){
            CURL *curl;
            CURLcode res;
            string readBuffer;

            curl = curl_easy_init();
            if(curl) {
                curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
                curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
                curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
                res = curl_easy_perform(curl);
                curl_easy_cleanup(curl);
            }
            return readBuffer;
        }
    public:
        string ip;
        string rawData;
        string jsonData;
        string status, country, region, city, isp;
        float lat, lon;

        bool getOwnIP(){
            ip = sendRequest(getIP);
            return true;
        }
        string getLocationByIP(){
            rawData = sendRequest(getInfoByIP + ip);
            return rawData;
        }
        bool jsonParser(){
            ofstream jsonFile;
            jsonFile.open(".temp");
            jsonFile << rawData;
            jsonFile.close();
            ifstream f(".temp");
            json data = json::parse(f);
            status = data["status"];
            country = data["country"];
            region = data["regionName"];
            city = data["city"];
            lat = data["lat"];
            lon = data["lon"];
            isp = data["isp"];
            remove(".temp");
            return true;
        }
};


int main(int argc, char *argv[], char *envp[]){
    IP myIP;
    if (argc < 2){
        cerr << "iplog: missing ip address!\n";
        cerr << "warning: your network's ip address will be used\n";
        cerr << "if you want to use ip as an argument: iplog <ip>\n";
        myIP.getOwnIP();
    }else if (argc == 2){
        myIP.ip = argv[1];
    }else{
        cerr << "too many arguments";
        return EXIT_FAILURE;
    }
    myIP.getLocationByIP();
    if (myIP.jsonParser()){
            cout << "[" << myIP.ip << "]" << " -> " << myIP.status << endl;
            cout << "Country: " << myIP.country << endl;
            cout << "City: " << myIP.city << ", " << myIP.region << endl;
            cout << "Coords: " << myIP.lat << ", " << myIP.lon << endl;
            cout << "Provider: " << myIP.isp << endl;
            cout << "================================================" << endl;
    }
    return EXIT_SUCCESS;
}