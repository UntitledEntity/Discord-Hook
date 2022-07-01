#include "includes.hpp"

Vector GetBonePos(DWORD Player, int Bone)
{
    Vector EnemyBones;
    DWORD Bonebase = Memory::Get().Read<DWORD>(Player + hazedumper::netvars::m_dwBoneMatrix);
    EnemyBones.x = Memory::Get().Read<float>(Bonebase + 0x30 * Bone + 0x0C);
    EnemyBones.y = Memory::Get().Read<float>(Bonebase + 0x30 * Bone + 0x1C);
    EnemyBones.z = Memory::Get().Read<float>(Bonebase + 0x30 * Bone + 0x2C);
    return EnemyBones;
}

int WorldToScreenDist(DWORD Player, RECT game_window, int radius)
{
    int x = (int)GetBonePos(Player, 8).x - (game_window.right - game_window.left) / 2;
    int y = (int)GetBonePos(Player, 8).y - (game_window.bottom - game_window.top) / 2;
    return radius * radius - (x * x + y * y);
}

bool WorldToScreen(Vector pos, Vector& screen, view q, int windowWidth, int windowHeight) // converts 3D coords to 2D coords
{
    //Matrix-vector Product, multiplying world(eye) coordinates by projection matrix = clipCoords

    VectorAligned clipCoords;
    clipCoords.x = pos.x * q.v[0] + pos.y * q.v[1] + pos.z * q.v[2] + q.v[3];
    clipCoords.y = pos.x * q.v[4] + pos.y * q.v[5] + pos.z * q.v[6] + q.v[7];
    clipCoords.z = pos.x * q.v[8] + pos.y * q.v[9] + pos.z * q.v[10] + q.v[11];
    clipCoords.w = pos.x * q.v[12] + pos.y * q.v[13] + pos.z * q.v[14] + q.v[15];

    if (clipCoords.w < 0.1f) // if the camera is behind our player don't draw, i think?
        return false;


    Vector NDC;
    // Normalize our coords
    NDC.x = clipCoords.x / clipCoords.w;
    NDC.y = clipCoords.y / clipCoords.w;
    NDC.z = clipCoords.z / clipCoords.w;

    // Convert to window coords (x,y)
    screen.x = (windowWidth / 2 * NDC.x) + (NDC.x + windowWidth / 2);
    screen.y = -(windowHeight / 2 * NDC.y) + (NDC.y + windowHeight / 2);
    return true;
}

#define AIM_FOV 30
#define AIM_SMOOTH 10

DWORD WINAPI OnDllAttach(LPVOID base)
{
    Utils::OpenConsole();

    if (!Utils::GetPid("csgo.exe"))
        Utils::Print("Waiting for CS:GO. ");
    
    if (!Utils::GetPid("DiscordCanary.exe") && !Utils::GetPid("Discord.exe")) {
        Utils::Print("This will only work if injected into discord. ");
        FreeLibraryAndExitThread(static_cast<HMODULE>(base), 0);
        return 0;
    }

    while (!Utils::GetPid("csgo.exe")) Sleep(100);

    Memory::Get().Init();

    Utils::Print("Initiated. ");
    Utils::Print("F10 to unload, Mouse5 for triggerbot. ");

    while (!Memory::Get().Client || !Memory::Get().Engine) {
        Utils::Print("Unable to find client/engine module. Retrying in 1 second.");
        Sleep(1000);
        Memory::Get().Init();
    }

    Utils::Print("Found client and engine modules");

#if _DEBUG
    std::string proc_print = "CS:GO Process Id: " + std::to_string(Memory::Get().ProcId);
    Utils::Print(proc_print.c_str(), true);

    std::string client_print = "Client: " + std::to_string(Memory::Get().Client);
    Utils::Print(client_print.c_str(), true);

    std::string engine_print = "Engine: " + std::to_string(Memory::Get().Engine);
    Utils::Print(engine_print.c_str(), true);
 
    printf("[DEBUG] Handle: %p", Memory::Get().Handle);
#endif

    RECT GameWindow, ModifiedGameWindow;
    GetWindowRect(FindWindow(NULL, TEXT("Counter-Strike: Global Offensive - Direct3D 9")), &GameWindow);
    GetClientRect(FindWindow(NULL, TEXT("Counter-Strike: Global Offensive - Direct3D 9")), &ModifiedGameWindow);


    while (!(GetAsyncKeyState(VK_F10) & 1))
    {
        Memory::Get().Local = Memory::Get().Read<DWORD>(Memory::Get().Client + hazedumper::signatures::dwLocalPlayer);

        if (Entity::Get().IsAlive(Memory::Get().Local))
        {
            // trigger
            if (GetAsyncKeyState(VK_XBUTTON2))
                AimBot::Get().Trigger();

            // bhop
            Misc::Get().Bhop();

            // loop players
            for (int i = 1; i < 32; i++)
            {
                DWORD Player = Memory::Get().Read<DWORD>(Memory::Get().Client + hazedumper::signatures::dwEntityList + (i * 0x10));
                view Matrix = Memory::Get().Read<view>(Memory::Get().Client + hazedumper::signatures::dwViewMatrix);

                Vector Origin, HeadPos;
                Vector PlayerOrigin = Memory::Get().Read<Vector>(Player + hazedumper::netvars::m_vecOrigin);

                //AimBot::Get().Init(Player);

                // force radar
                if (Entity::Get().IsEnemy(Player, Memory::Get().Local) && Entity::Get().IsAlive(Player))
                    Memory::Get().Write<bool>(Player + hazedumper::netvars::m_bSpotted, true);

                // "chams" XDD
                Visuals::Get().Chams(Player, Entity::Get().IsEnemy(Player, Memory::Get().Local));

                // glow
                Visuals::Get().Glow(Player, Entity::Get().IsEnemy(Player, Memory::Get().Local));
            }

            /*if (GetAsyncKeyState(VK_LBUTTON))
                AimBot::Get().Aim(AIM_SMOOTH);*/
        }
    }

    FreeLibraryAndExitThread(static_cast<HMODULE>(base), 1);
    return 1;
}

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        DisableThreadLibraryCalls(hModule);

        auto Handle = CreateThread(nullptr, 0, OnDllAttach, hModule, 0, nullptr);
        if (Handle)
            CloseHandle(Handle);

        break;
    }
    case DLL_PROCESS_DETACH:
        Utils::CloseConsole();
        break;
    }
    return TRUE;
}

