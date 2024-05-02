#pragma once

#include <cstdint>
#include <string>

namespace MCSL 
{

    struct PlayerSettings 
    {
        int Gamemode = 0;
        bool AllowFlight = false;
        int OpPermissionLevel = 4;
        bool Hardcore = false;
        bool Pvp = true;
        int MaxPlayers = 20;
        int PlayerIdleTimeout = 0;
        bool ForceGamemode = false;
        bool BroadcastConsoleToOps = true;
        bool BroadcastRconToOps = true;
        bool HideOnlinePlayers = false;
        int FunctionPermissionLevel = 2;
        bool WhiteList = false;
    };

    struct WorldSettings 
    {
        std::string LevelName = "world";
        std::string LevelSeed = "";
        std::string Motd = "";
        int SpawnProtection = 16;
        int MaxWorldSize = 29999984;
        bool AllowNether = true;
        int ViewDistance = 10;
        bool GenerateStructures = true;
        std::string LevelType = "minecraft:normal";
        bool SpawnMonsters = true;
        bool SpawnAnimals = true;
        bool SpawnNpcs = true;
        int Difficulty = 0;
        int SimulationDistance = 10;
        bool EnforceWhitelist = false;
    };

    struct NetworkSettings 
    {
        int ServerPort = 25565;
        std::string ServerIp = "";
        int RconPort = 25575;
        int QueryPort = 25565;
        int NetworkCompressionThreshold = 256;
        bool UseNativeTransport = true;
        bool SyncChunkWrites = true;
        int RateLimit = 0;
        bool LogIps = true;
        bool EnableStatus = true;
        bool EnableQuery = false;
        bool EnableRcon = false;
        bool PreventProxyConnections = false;
    };

    struct MiscSettings 
    {
        bool EnableJmxMonitoring = false;
        bool EnforceSecureProfile = true;
        bool RequireResourcePack = false;
        std::string ResourcePackPrompt = "";
        std::string InitialDisabledPacks = "";
        std::string InitialEnabledPacks = "vanilla";
        std::string ResourcePack = "";
        std::string ResourcePackId = "";
        std::string ResourcePackSha1 = "";
        int MaxTickTime = 60000;
        int MaxChainedNeighborUpdates = 1000000;
        std::string TextFilteringConfig = "";
    };

    struct ServerSettings
    {
        uint32_t MinRAM = 2048;
        uint32_t MaxRAM = 8192;
        PlayerSettings Player;
        WorldSettings World;
        NetworkSettings Network;
        MiscSettings Misc;
    };

    class ServerConfigurator
    {
    public:
        ServerConfigurator();
        ~ServerConfigurator();

    private:
        ServerSettings m_ServerSettings;
    };
}
