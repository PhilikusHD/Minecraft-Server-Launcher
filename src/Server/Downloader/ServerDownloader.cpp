#include "Server/Downloader/ServerDownloader.h"
#include <cstddef>
#include <cstdio>
#include <curl/curl.h>
#include <curl/easy.h>
#include <filesystem>
#include <fstream>
#include <ostream>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <string>
#include "json/json.hpp"

#define OPENSSL_NO_DEPRECATED_3_0
#include <openssl/sha.h>

using json = nlohmann::json;

namespace MCSL 
{
    ServerDownloader::ServerDownloader()
    {
        curl_global_init(CURL_GLOBAL_ALL);
    }

    ServerDownloader::~ServerDownloader()
    {
        curl_global_cleanup();
    }

    void ServerDownloader::DownloadServer(const std::string& version)
    {
        ManifestBlob manifest = FetchManifest();

        std::string targetVersion;
        std::string targetUrl;
        for(const auto& entry : manifest.Versions)
        {
            if(entry.Id == version)
            {
                targetVersion = version;
                targetUrl = entry.Url;
                break;
            }
        }

        if(targetVersion.empty())
        {
            std::cerr << "Error: Unable to find manifest entry with version '" << version << "'" << std::endl;
            return;
        }

        VersionBlob versionBlob = FetchVersion(targetVersion, targetUrl);

        const DownloadEntry& download = versionBlob.Downloads;
        std::cout << "Found server jar at: " << download.Url << "\n";
        std::cout << "Size: " << static_cast<double>(download.Size) / (1024 * 1024) << "MB" << std::endl;

        // // Download file
        CURL* curl = curl_easy_init();
        FILE* fp;
        if(curl)
        {
            std::string dirPath = "test/" + versionBlob.Id + "/";
            if(!std::filesystem::exists(dirPath))
            {
                std::filesystem::create_directories(dirPath);
            }
            std::string filePath = dirPath + "server-" + targetVersion + ".jar";
            fp = fopen(filePath.c_str(), "wb");
            if(!fp)
            {
                std::cerr << "Error: Could not create file " << filePath << std::endl;
                return;
            }

            curl_easy_setopt(curl, CURLOPT_URL, download.Url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
            curl_easy_setopt(curl, CURLOPT_USERAGENT, m_UserAgent.c_str());

            std::cout << "Downloading file, please stand by..." << std::endl;
            CURLcode res = curl_easy_perform(curl);
            curl_easy_strerror(res);
            if(res != CURLE_OK)
            {
                std::cerr << "Error: failed to download file (" << curl_easy_strerror(res) << ")" << std::endl;
                RemoveFile(filePath);
                return;            
            }
            fclose(fp);               

            std::cout << "Successfully downloaded to: '" << filePath << "'" << std::endl;
        }
        else 
        {
             std::cerr << "Error: Could not initialize curl" << std::endl;
        }
    }

    ManifestBlob ServerDownloader::FetchManifest()
    {
        ManifestBlob manifest;
        CURL* curl = curl_easy_init();
        if(curl)
        {
            std::string responseBuffer;

            curl_easy_setopt(curl, CURLOPT_URL, m_ManifestUrl.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBuffer);
            curl_easy_setopt(curl, CURLOPT_USERAGENT, m_UserAgent.c_str());

            CURLcode res = curl_easy_perform(curl);
            if (res == CURLE_OK) 
            {
                try 
                { 
                    json jsonData = json::parse(responseBuffer);

                    manifest.Latest.Release = jsonData["latest"]["release"].get<std::string>();
                    manifest.Latest.Snapshot = jsonData["latest"]["snapshot"].get<std::string>();

                    for(const auto& version : jsonData["versions"])
                    {
                        Version ver;
                        ver.Id = version["id"].get<std::string>();
                        ver.Url = version["url"].get<std::string>();
                        manifest.Versions.push_back(ver);
                    }
                } 
                catch (const std::exception& e) 
                {
                    std::cerr << "Error: " << e.what()<< std::endl;
                }
            } 
            else 
            {
                std::cerr << "Error: failed to fetch manifest (" << curl_easy_strerror(res) << ")" << std::endl;
            }

            curl_easy_cleanup(curl);
        } 
        else 
        {
            std::cerr << "Error: could not initialize curl" << std::endl;
        }
        
        return manifest;
    }

    VersionBlob ServerDownloader::FetchVersion(const std::string& versionId, const std::string& versionUrl) 
    {
        VersionBlob versionBlob;
        CURL* curl = curl_easy_init();
        if(curl)
        {
            std::string responseBuffer;

            curl_easy_setopt(curl, CURLOPT_URL, versionUrl.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBuffer);
            curl_easy_setopt(curl, CURLOPT_USERAGENT, m_UserAgent.c_str());

            CURLcode res = curl_easy_perform(curl);
            if (res == CURLE_OK) 
            {
                try 
                { 
                    json jsonData = json::parse(responseBuffer);
                    versionBlob.Id = jsonData["id"].get<std::string>();

                    versionBlob.Downloads.Url = jsonData["downloads"]["server"]["url"].get<std::string>();
                    versionBlob.Downloads.Sha1 = jsonData["downloads"]["server"]["sha1"].get<std::string>();
                    versionBlob.Downloads.Size = jsonData["downloads"]["server"]["size"].get<size_t>();
                } 
                catch (const std::exception& e) 
                {
                    std::cerr << "Error: " << e.what()<< std::endl;
                }
            } 
            else 
            {
                std::cerr << "Error: failed to fetch manifest (" << curl_easy_strerror(res) << ")" << std::endl;
            }

            curl_easy_cleanup(curl);
        } 
        else 
        {
            std::cerr << "Error: could not initialize curl" << std::endl;
        }
        return versionBlob;
    }

    void ServerDownloader::SaveToFile(const std::string& filePath, const std::string& content) 
    {
        std::ofstream file(filePath);
        if (file.is_open()) 
        {
            file << content;
            file.close();
        }
        else 
        {
            std::cerr << "Error: could not create file " << filePath << std::endl;
        }
    }

    void ServerDownloader::RemoveFile(const std::string& filePath) 
    {
        if (std::remove(filePath.c_str()) != 0) 
        {
            std::cerr << "Error: could not remove file " << filePath << std::endl;
        }
    }

    size_t ServerDownloader::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* buffer) 
    {
        size_t totalSize = size * nmemb;
        buffer->append((char*)contents, totalSize);
        return totalSize;
    }
}
