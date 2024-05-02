#pragma once

#include <string>
#include <vector>
#include <curl/curl.h>


namespace MCSL
{
    // The following is a C++ rewrite of Meshiest's mcdl, go star the repo: 
    // https://github.com/Meshiest/mcdl
    struct Version 
    {
        std::string Id;
        std::string Url;
    };

    struct LatestVersions 
    {
        std::string Release;
        std::string Snapshot;
    };

    struct ManifestBlob 
    {
        LatestVersions Latest;
        std::vector<Version> Versions;
    };

    struct DownloadEntry 
    {
        std::string Sha1;
        size_t Size;
        std::string Url;
    };

    struct VersionBlob 
    {
        std::string Id;
        DownloadEntry Downloads;
    };

    class ServerDownloader 
    {
    public:
        ServerDownloader();
        ~ServerDownloader();

        void DownloadServer(const std::string& version, const std::string& serverName);
        std::string GetJarName() const { return m_JarName; }
    private:
        ManifestBlob FetchManifest();
        VersionBlob FetchVersion(const std::string& versionId, const std::string& versionUrl);
        void SaveToFile(const std::string& filePath, const std::string& content);
        std::string CalculateSha1(const std::string& filePath);
        void RemoveFile(const std::string& filePath);
        bool ValidateSha1(const std::string& filePath, const std::string& expectedHash);

        static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* buffer);

        const std::string m_ManifestUrl = "https://launchermeta.mojang.com/mc/game/version_manifest.json";
        const std::string m_UserAgent = "Mozilla/5.0";
        std::string m_JarName;
    };
    
}
