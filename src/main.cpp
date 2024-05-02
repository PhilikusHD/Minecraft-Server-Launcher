#include "Server/Downloader/ServerDownloader.h"
#include <iostream>



int main(int argc, char** argv)
{
    std::cout << "Welcome to the Minecraft Server Launcher! Let's begin by creating your server!\n";
    std::cout << "Which version do you want to play on?\n";
    std::cout << "Version: ";
    std::string version;
    std::cin >> version;

    std::cout << "\nAlright! Attempting to download the server jar now!" << std::endl;

    MCSL::ServerDownloader downloader;
    downloader.DownloadServer(version);

    return 0;
}
