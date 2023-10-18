// dllmain.cpp : Defines the entry point for the DLL application.
#include"win32api.h"

// utility functions
import utils;
// CConsole class
import CConsole;
// CPlayer class
import CPlayer;
// offsets and memory addresses
import offsets;
// CTrampolineHook32 class
import CTrampolineHook32;

import globals;

// std::cout, std::uintptr_t, ...
import <iostream>;

// vec3EyePosition = playerent + 0x0004
// vec3Velocity = playerent + 0x0028
// vec3FeetPosition = playerent + 0x0034
// vec2ViewAngles = playerent + 0x0040
// bOnGround = playerent + 0x69
// fCameraHeight = playerent + 0x005C
// bIsNotShooting = playerent + 0x70
// i32MoveType = playerent + 0x80 -> 1 = forward, 255 = backward, 256 = left, 65280 = right
// i32Health = playerent + 0x00F8
// i32Ammo = playerent + 0x0150
// i32FiringSpeed = playerent + 0x0178
// i32ShotsFired = playerent + 0x01A0
// cstrName = playerent + 0x225
// currentPlayerInGame = "ac_client.exe" + 0x10f500

// entity list -> "ac_client.exe" + 0x0010F4F8
// first entity = ["ac_client.exe" + 0x0010F4F8] + 0x4
// second enttiy = "ac_client.exe" + 0x0010F4F8 + 0x4 + 0x4
// first three entities = ["ac_client.exe" + 0x0010F4F8] + (i * 0x4)
// other three entitites = ["ac_client.exe" + 0x0010F4F8] + 0x190 + (i * 0x4)

//for (std::int32_t i = NULL; i < currentPlayerInGame; ++i) {
//    CPlayer* const pPlayer = *reinterpret_cast<CPlayer* const* const>(pEntityList + (i * 0x04));
//
//    if (!pPlayer) {
//        continue;
//    }
//}

namespace globals {
    namespace thread {
        static DWORD dwId = false;
    }

    static CConsole* pConsole = nullptr;
    static const decltype(__TEXT(' '))* pErrorMessage = nullptr;
}

typedef std::int32_t(__cdecl* const printToConsole_t)(_In_z_ _Printf_format_string_ const char* const, ...) noexcept;

typedef BOOL(__stdcall* wglSwapBuffers_t)(_In_ const HDC) noexcept;

static wglSwapBuffers_t p_wglSwapBuffersGateway = nullptr;

static BOOL __stdcall hk_wglSwapBuffers(
    _In_ const HDC hdc
) noexcept;

static DWORD CALLBACK MainThread(
    _In_ void* const vpInstDLL
) noexcept
{
    std::cout << "\nCurrent thread Id: " << globals::thread::dwId;

    const printToConsole_t printToConsole = reinterpret_cast<const printToConsole_t>(0x4090F0);

    printToConsole("Current thread Id: ", globals::thread::dwId);

    std::cout << "\nRetrieving module \"opengl32.dll\"...\n\t\\\n\t \\___";

    const HMODULE hOpenGL = GetModuleHandle(__TEXT("opengl32.dll"));

    if (!hOpenGL) {
        globals::pErrorMessage = __TEXT("Failed to retrieve module \"opengl32.dll\"");
        globals::pConsole->printColored<COLORED_TEXT::COLORED_TEXT_RED>("Failed!");
        utils::dll::eject(vpInstDLL, ERROR_MOD_NOT_FOUND);
    }

    globals::pConsole->printColored<COLORED_TEXT::COLORED_TEXT_GREEN>("Success! -> 0x%p", hOpenGL);

    std::cout << "\nRetrieving function \"wglSwapBuffers\" of module \"opengl32.dll\"...\n\t\\\n\t \\___";

    if (!(::p_wglSwapBuffersGateway = reinterpret_cast<const wglSwapBuffers_t>(GetProcAddress(hOpenGL, "wglSwapBuffers")))) {
        globals::pErrorMessage = __TEXT("Failed to retrieve function \"wglSwapBuffers\" of module \"opengl32.dll\"");
        globals::pConsole->printColored<COLORED_TEXT::COLORED_TEXT_RED>("Failed!");
        utils::dll::eject(vpInstDLL, ERROR_INVALID_FUNCTION);
    }

    globals::pConsole->printColored<COLORED_TEXT::COLORED_TEXT_GREEN>("Success! -> 0x%p", ::p_wglSwapBuffersGateway);

    std::cout << "\nHooking function \"wglSwapBuffers\" of module \"opengl32.dll\"...\n\t\\\n\t \\___";

    CTrampolineHook32 wglSwapBuffersHook = CTrampolineHook32{ ::p_wglSwapBuffersGateway, 5u };

    if (!(::p_wglSwapBuffersGateway = reinterpret_cast<const wglSwapBuffers_t>(wglSwapBuffersHook(&::hk_wglSwapBuffers)))) {
        globals::pErrorMessage = __TEXT("Failed to hook function \"wglSwapBuffers\" of module \"opengl32.dll\"");
        globals::pConsole->printColored<COLORED_TEXT::COLORED_TEXT_RED>("Failed!");
        utils::dll::eject(vpInstDLL, ERROR_INVALID_FUNCTION);
    }

    globals::pConsole->printColored<COLORED_TEXT::COLORED_TEXT_GREEN>("Success! -> 0x%p", &::hk_wglSwapBuffers);

#ifndef _DEBUG
    delete globals::pConsole;
    globals::pConsole = nullptr;
#endif // !_DEBUG

    globals::entity::pEntityList = *reinterpret_cast<std::array<CPlayer* const, 32u>** const>(globals::modules::ac_client_exe + offsets::ac_client_exe::ENTITY_LIST);

    globals::entity::pLocalPlayer = *reinterpret_cast<CPlayer** const>(globals::modules::ac_client_exe + offsets::ac_client_exe::LOCAL_PLAYER);

    std::cout << "\n&globals::entity::pLocalPlayer->i32MoveType -> 0x" << &globals::entity::pLocalPlayer->iMoveType;

    while (!GetAsyncKeyState(VK_END)) {
        system("cls");
        std::cout << "globals::entity::pLocalPlayer->i32MoveType -> " << globals::entity::pLocalPlayer->iMoveType;

        std::cout << "\nHealth -> " << globals::entity::pLocalPlayer->iHealth << '\n' << globals::entity::pLocalPlayer->vec2ViewAngles.x << " | " << globals::entity::pLocalPlayer->vec2ViewAngles.y;

        Sleep(200ul);
    }

    if (!~wglSwapBuffersHook) {
        globals::pErrorMessage = __TEXT("Failed to unhook function \"wglSwapBuffers\" of module \"opengl32.dll\"");
    }

    utils::dll::eject(vpInstDLL, ERROR_SUCCESS);
    return TRUE;
}

// Entrypoint
BOOL APIENTRY DllMain(
    _In_				  const HINSTANCE   hInstDLL,
    _In_				  const DWORD       fdwReason,
    _In_ [[maybe_unused]] const void* const lpvReserved
) noexcept
{
    if (DLL_PROCESS_ATTACH == fdwReason) {
        try {
            globals::pConsole = new CConsole{ __TEXT("nvrlose") };
        }
        catch (const std::bad_alloc&) {
            globals::pErrorMessage = __TEXT("Failed to allocate enough memory to create a console");
            FreeLibrary(hInstDLL);
            return TRUE;
        }

        std::cout << "Disabling thread library calls...\n\t\\\n\t \\___";

        if (DisableThreadLibraryCalls(hInstDLL)) {
            globals::pConsole->printColored<COLORED_TEXT::COLORED_TEXT_GREEN>("Success!");
        }
        else {
            globals::pConsole->printColored<COLORED_TEXT::COLORED_TEXT_RED>("Failed!");
        }

        if (!globals::modules::ac_client_exe) {
            globals::pErrorMessage = __TEXT("Failed to retrieve module \"ac_client.exe\". This can happen when you inject me into a wrong process!");
            globals::pConsole->printColored<COLORED_TEXT::COLORED_TEXT_RED>("\nFailed to retrieve module \"ac_client.exe\"!");
            FreeLibrary(hInstDLL);
            return TRUE;
        }

        std::cout << "\nCreating a thread to fully instantiate the cheat...\n\t\\\n\t \\___";

        const HANDLE hThread = CreateThread(
            nullptr,
            NULL,
            &::MainThread,
            hInstDLL,
            CREATE_SUSPENDED,
            &globals::thread::dwId
        );

        if (!hThread) {
            globals::pConsole->printColored<COLORED_TEXT::COLORED_TEXT_RED>("Failed!");
            globals::pErrorMessage = __TEXT("Failed to create a seperate thread!");
            FreeLibrary(hInstDLL);
            return TRUE;
        }

        globals::pConsole->printColored<COLORED_TEXT::COLORED_TEXT_GREEN>("Success!");

        SetThreadDescription(hThread, L"nvrlose client main");

        ResumeThread(hThread);

        CloseHandle(hThread);
    }
    else if (DLL_PROCESS_DETACH == fdwReason) {
        if (globals::pConsole) {
            delete globals::pConsole;
        }

        if (globals::pErrorMessage) {
            (*utils::messagebox::error)(globals::pErrorMessage);
        }
    }

    return TRUE;
}

static BOOL __stdcall hk_wglSwapBuffers(
    _In_ const HDC hdc
) noexcept
{
    static decltype(auto) printONCE = [](void) noexcept -> bool {
        std::cout << "\nSuccessfully hooked!";
        return true;
    }();

    static std::uint64_t counter = NULL;

    if (counter > 5u) {
        return (*::p_wglSwapBuffersGateway)(hdc);
    }

    std::cout << '\n' << ++counter;

    return (*::p_wglSwapBuffersGateway)(hdc);
}