// dllmain.cpp : Defines the entry point for the DLL application.
#include"win32api.h"

// utility functions
import utils;
// CPlayer class
import CPlayer;
// offsets and memory addresses
import offsets;
// CTrampolineHook32 class
import CTrampolineHook32;
// CRenderer class
import CRenderer;
// globals namespace
import globals;

// std::uintptr_t, std::bad_alloc, ...
#include<iostream>
//import <iostream>;

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

namespace globals {
    namespace thread {
        static DWORD dwId = NULL;
    }

    namespace modules {
        static std::uintptr_t ac_client_exe = NULL;
    }

    static CRenderer* pRenderer = nullptr;
}

static const DWORD* dwpJumpBackAddress = nullptr;

static void __declspec(naked) hkHealthOpcode( void ) noexcept {
    __asm {
        // setting eax to our localPlayer
        // eax is dwpLocalPlayer
        mov eax, dword ptr[globals::modules::ac_client_exe] // dwpLocalPlayer = globals::modules::ac_client_exe
        add eax, dword ptr[offsets::ac_client_exe::LOCAL_PLAYER] // dwpLocalPlayer += offsets::ac_client_exe::LOCAL_PLAYER
        mov eax, dword ptr[eax] // dwpLocalPlayer = *dwpLocalPlayer
        // retrieving the player class of the player calling this function:
        push ebx
        sub ebx, 0xF4u // dwpPlayer -= 0xF4u
        cmp ebx, eax // if (dwpPlayer == dwpLocalPlayer)
        pop ebx
        je onLocalPlayer // jump if dwpPlayer is dwpLocalPlayer
        // edi is iDamage
        // dword ptr[ebx + 0x4u] is iRefHealth
        // else {
        mov edi, dword ptr[ebx + 0x4u] // iDamage = iRefHealth
        jmp stolenBytes
        // }
    onLocalPlayer:
        sub edi, 9999999u // iDamage -= 9999999
        sub edi, dword ptr[ebx + 0x4u] // iDamage -= iRefHealth
    stolenBytes:
        sub dword ptr[ebx + 0x4u], edi // iRefHealth -= iDamage
        mov eax, edi // eax = iDamage
        jmp dword ptr[dwpJumpBackAddress]
    }
}

static DWORD CALLBACK MainThread(
    _In_ void* const vpInstDLL
) noexcept
{
    (*globals::function::pointer::pPopupMessage)("Current thread Id: %d", globals::thread::dwId);

    globals::entity::pEntityList = *reinterpret_cast<std::array<CPlayer* const, 32u>** const>(globals::modules::ac_client_exe + offsets::ac_client_exe::ENTITY_LIST);
    globals::entity::pLocalPlayer = *reinterpret_cast<CPlayer** const>(globals::modules::ac_client_exe + offsets::ac_client_exe::LOCAL_PLAYER);
    globals::match::ipPlayerInGame = reinterpret_cast<const std::uint32_t*>(globals::modules::ac_client_exe + offsets::ac_client_exe::CURRENT_PLAYER_IN_GAME);

    BYTE* const bypHookAddress = reinterpret_cast<BYTE* const>(globals::modules::ac_client_exe + 0x29D1Fu);
    constexpr const std::uint8_t u8HookLength = 5u;

    std::array<BYTE, u8HookLength> byArrStolenBytes = std::array<BYTE, u8HookLength>{ };

    memcpy_s(
        byArrStolenBytes.data(),
        byArrStolenBytes.size(),
        bypHookAddress,
        byArrStolenBytes.size()
    );

    ::dwpJumpBackAddress = reinterpret_cast<const DWORD*>(bypHookAddress + u8HookLength);

    const DWORD dwRelativeOffset = reinterpret_cast<const std::uintptr_t>(&::hkHealthOpcode) - reinterpret_cast<const std::uintptr_t>(bypHookAddress) - 5u;
    
    DWORD dwPreviousProtection = NULL;
    if (
        !VirtualProtect(
            bypHookAddress,
            u8HookLength,
            PAGE_EXECUTE_READWRITE,
            &dwPreviousProtection
        )
    )
    {
        (*globals::function::pointer::pPopupMessage)("Failed to get permission to change the ingame code");
        utils::dll::eject(vpInstDLL, ERROR_ACCESS_DENIED);
    }

    bypHookAddress[NULL] = 0xE9u;
    *reinterpret_cast<DWORD* const>(bypHookAddress + 1u) = dwRelativeOffset;

    DWORD dwTempProtection = NULL;
    VirtualProtect(
        bypHookAddress,
        u8HookLength,
        dwPreviousProtection,
        &dwTempProtection
    );

    (*globals::function::pointer::pPopupMessage)("Exploiting vulnerabilities to make in-game-rendering possible");

    try {
        globals::pRenderer = new CRenderer{ };
    }
    catch (const std::bad_alloc&) {
        (*globals::function::pointer::pPopupMessage)("Failed to allocate enough memory to hold the \"CRenderer\"-class!");

        utils::dll::eject(vpInstDLL, ERROR_NOT_ENOUGH_MEMORY);
    }

    if (!globals::pRenderer->ok()) {
        (*globals::function::pointer::pPopupMessage)("Failed to hook function \"wglSwapBuffers\" in module \"OpenGL32.dll\" to make in-game-rendering possible!");
        
        utils::dll::eject(vpInstDLL, ERROR_FUNCTION_FAILED);
    }

    (*globals::function::pointer::pPopupMessage)("Success :D");

    while (!GetAsyncKeyState(VK_END)) {
        /*system("cls");

        for (std::uint32_t i = 1u; i < *globals::match::ipPlayerInGame; ++i) {
            std::cout << "Entity #" << i << " @ 0x";

            const CPlayer& refEntity = (*(*globals::entity::pEntityList)[i]);

            std::cout << &refEntity << " health -> " << refEntity.iHealth << '\n';
        }*/

        Sleep(200ul);
    }

    if (
        !VirtualProtect(
            bypHookAddress,
            u8HookLength,
            PAGE_EXECUTE_READWRITE,
            &dwPreviousProtection
        )
    )
    {
        (*globals::function::pointer::pPopupMessage)("Failed to get permission to change the ingame code");
        utils::dll::eject(vpInstDLL, ERROR_ACCESS_DENIED);
    }
    memcpy_s(
        bypHookAddress,
        byArrStolenBytes.size(),
        byArrStolenBytes.data(),
        byArrStolenBytes.size()
    );
    VirtualProtect(
        bypHookAddress,
        u8HookLength,
        dwPreviousProtection,
        &dwTempProtection
    );

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
    static const decltype(__TEXT(' '))* pErrorMessage = nullptr;

    if (DLL_PROCESS_ATTACH == fdwReason) {
        if(!(globals::modules::ac_client_exe = reinterpret_cast<const std::uintptr_t>(GetModuleHandle(__TEXT("ac_client.exe"))))) {
            pErrorMessage = __TEXT("Failed to retrieve module \"ac_client.exe\". This can happen when you inject me into the wrong process!");
            FreeLibrary(hInstDLL);
            return TRUE;
        }

        globals::function::pointer::pPopupMessage = reinterpret_cast<const globals::function::definition::_popupMessage_t>(offsets::function::POPUP_MESSAGE);

        if (!DisableThreadLibraryCalls(hInstDLL)) {
            (*globals::function::pointer::pPopupMessage)("Failed to disable thread library calls!");
        }

        const HANDLE hThread = CreateThread(
            nullptr,
            NULL,
            &::MainThread,
            hInstDLL,
            CREATE_SUSPENDED,
            &globals::thread::dwId
        );

        if (!hThread) {
            (*globals::function::pointer::pPopupMessage)("Failed to create a seperate thread to fully instantiate the cheat!");
            FreeLibrary(hInstDLL);
            return TRUE;
        }

        SetThreadDescription(hThread, L"nvrlose client main");

        ResumeThread(hThread);

        CloseHandle(hThread);
    }
    else if (DLL_PROCESS_DETACH == fdwReason) {
        if (pErrorMessage) {
            (*utils::messagebox::error)(pErrorMessage);
            return TRUE;
        }

        if (globals::pRenderer) {
            delete globals::pRenderer;
        }
    }

    return TRUE;
}