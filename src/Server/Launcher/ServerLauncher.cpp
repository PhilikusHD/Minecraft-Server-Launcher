#include "Server/Launcher/ServerLauncher.h"
#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QMessageBox>

#include "Server/Downloader/ServerDownloader.h"
#include "Server/Config/ServerConfigurator.h"

#include <fstream>
#include <iostream>

namespace MCSL
{
    namespace Utils 
    {
        void GenerateLaunchScript(uint32_t minRAM, uint32_t maxRAM, const std::string& jarName)
        {
            std::ofstream launchScript;
            std::string scriptName;
            bool isWindows = false;

            // Windows, why you gotta be 'special'
            #ifdef _WIN32
                std::cout << "Detected Platform: 'Windows'" << std::endl;
                scriptName = "launch.bat";
                isWindows = true;
            #else
                std::cout << "Detected Platform: 'Unix'" << std::endl;
                scriptName = "launch.sh";
            #endif

            launchScript.open(scriptName);
            if(launchScript.is_open())
            {
                if(isWindows)
                {
                    launchScript << "@echo off\n";
                }
                else 
                {
                    launchScript << "#!/bin/bash\n";
                }
                
                launchScript << "java -Xms" << minRAM << "M -Xmx" << maxRAM << "M -jar " << jarName << " nogui";

                launchScript.close();
                std::cout << "Launch script generated successfully: " << scriptName << std::endl;
            }
            else 
            {
                std::cerr << "Error: Unable to create launch script: " << scriptName << std::endl;
            }
            
        }

        bool GenerateAndAcceptEula()
        {
            std::ofstream eula; 
            eula.open("eula.txt");
            std::string userChoice;
            std::cout << "Do you agree to the eula at: https://aka.ms/MinecraftEULA? [y/N] ";
            std::cin >> userChoice;

            if(userChoice == "Y" || userChoice == "y")
            {
                std::cout << "EULA agreement accepted. You can now proceed." << std::endl;
                eula << "eula=true\n";
                eula.close();
                return true;
            }
            else 
            {
                std::cerr << "EULA agreement not accepted. The server cannot be started without agreeing to the EULA." << std::endl;
                eula << "eula=false\n";
                eula.close();
                return false;
            }
        }
    }


    ServerLauncher::ServerLauncher(QWidget *parent) 
        : QWidget(parent)
    {
        QVBoxLayout *layout = new QVBoxLayout(this);

        QLabel *versionLabel = new QLabel("Version:", this);
        layout->addWidget(versionLabel);

        m_VersionLineEdit = new QLineEdit(this);
        layout->addWidget(m_VersionLineEdit);

        QLabel *nameLabel = new QLabel("Server Name:", this);
        layout->addWidget(nameLabel);

        m_NameLineEdit = new QLineEdit(this);
        layout->addWidget(m_NameLineEdit);

        m_LaunchButton = new QPushButton("Launch Server", this);
        layout->addWidget(m_LaunchButton);
        connect(m_LaunchButton, &QPushButton::clicked, this, &ServerLauncher::LaunchServer);

        setLayout(layout);
        setWindowTitle("Minecraft Server Launcher");
    }

    void ServerLauncher::LaunchServer()
    {
        QString version = m_VersionLineEdit->text();
        QString name = m_NameLineEdit->text();

        if (version.isEmpty() || name.isEmpty())
        {
            QMessageBox::critical(this, "Error", "Version and Server Name cannot be empty.");
            return;
        }

        ServerDownloader downloader;
        downloader.DownloadServer(version.toStdString(), name.toStdString());

        ServerSettings settings;
        settings.MinRAM = 2048; // Default minimum RAM
        settings.MaxRAM = 4096; // Default maximum RAM

        Utils::GenerateLaunchScript(settings.MinRAM, settings.MaxRAM, downloader.GetJarName());

        if (!Utils::GenerateAndAcceptEula())
        {
            QMessageBox::critical(this, "Error", "You must agree to the EULA to proceed with launching the server.");
            return;
        }

        QMessageBox::information(this, "Success", "Everything is done, ready to launch your server!");
        system("sh launch.sh");
    }
}
