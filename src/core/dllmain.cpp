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

import CTraceRay;

// std::uintptr_t, std::bad_alloc, ...
#include<iostream>

// std::numeric_limits<const float>::max()
#include<limits>

namespace globals {
    namespace thread {
        static DWORD dwId = NULL;
    }

    namespace modules {
        const std::uintptr_t ac_client_exe = reinterpret_cast<const std::uintptr_t>(GetModuleHandle(__TEXT("ac_client.exe")));
    }

    static CRenderer* pRenderer = nullptr;
}

static constexpr float PI = 3.14159265358979323846f;
static const DWORD* dwpJumpBackAddress = nullptr;

static void __declspec(naked) hkHealthOpcode( void ) noexcept {
    __asm {
        mov eax, dword ptr[globals::modules::ac_client_exe]
        add eax, dword ptr[offsets::ac_client_exe::pointer::LOCAL_PLAYER]
        mov eax, dword ptr[eax]
        add eax, 0xF4u

        cmp ebx, eax

        je onLocalPlayer

        mov eax, dword ptr[ebx + 0x4u]
        sub dword ptr[ebx + 0x4u], eax

        jmp dword ptr[dwpJumpBackAddress]
    onLocalPlayer:
        mov dword ptr[ebx + 0x4u], 9999999u
        xor ecx, ecx

        jmp dword ptr[dwpJumpBackAddress]
    }
}

static DWORD CALLBACK MainThread(
    _In_ void* const vpInstDLL
) noexcept
{
    (*globals::function::pointer::pPopupMessage)("Current thread Id: %d", globals::thread::dwId);

    globals::entity::pEntityList = *reinterpret_cast<const std::array<const CPlayer* const, globals::entity::MAX_ENTITIES>* const* const>(globals::modules::ac_client_exe + offsets::ac_client_exe::pointer::ENTITY_LIST);
    globals::entity::pLocalPlayer = *reinterpret_cast<CPlayer* const* const>(globals::modules::ac_client_exe + offsets::ac_client_exe::pointer::LOCAL_PLAYER);
    globals::match::ipPlayerInGame = reinterpret_cast<const std::uint32_t* const>(globals::modules::ac_client_exe + offsets::ac_client_exe::pointer::I_CURRENT_PLAYER_IN_GAME);

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

    /*try {
        globals::pRenderer = new CRenderer{ };
    }
    catch (const std::bad_alloc&) {
        (*globals::function::pointer::pPopupMessage)("Failed to allocate enough memory to hold the \"CRenderer\"-class!");

        utils::dll::eject(vpInstDLL, ERROR_NOT_ENOUGH_MEMORY);
    }

    if (!globals::pRenderer->ok()) {
        (*globals::function::pointer::pPopupMessage)("Failed to hook function \"wglSwapBuffers\" in module \"OpenGL32.dll\" to make in-game-rendering possible!");
        
        utils::dll::eject(vpInstDLL, ERROR_FUNCTION_FAILED);
    }*/

    const CVector3& vec3RefLocalPlayerEyePosition = globals::entity::pLocalPlayer->vec3EyePosition;

    AllocConsole();
    FILE* pFile = nullptr;
    freopen_s(
        &pFile,
        "CONOUT$",
        "w",
        stdout
    );

    std::cout << std::boolalpha;

    while (!GetAsyncKeyState(VK_END)) {
        if (
            !globals::entity::pLocalPlayer ||
            !globals::entity::pLocalPlayer->uHealth
        )
        {
            continue;
        }

        globals::entity::pLocalPlayer->uAssaultRifleAmmo = 1337u;

        float fPlayerDistanceToLocalPlayer = std::numeric_limits<const float>::max();

        CVector3 vec3Delta = CVector3{ };

        for (std::uint8_t i = 1u; i < *globals::match::ipPlayerInGame; ++i) {
            const CPlayer& refCurrentPlayer = *((*globals::entity::pEntityList)[i]);

            if (
                !refCurrentPlayer.uHealth ||
                &refCurrentPlayer == globals::entity::pLocalPlayer ||
                !refCurrentPlayer.isVisibleTo(vec3RefLocalPlayerEyePosition)
            )
            {
                continue;
            }

            vec3Delta = refCurrentPlayer.vec3EyePosition - vec3RefLocalPlayerEyePosition;

            if (
                const float fCurrentPlayerDistanceToLocalPlayer = vec3Delta.length();
                fCurrentPlayerDistanceToLocalPlayer < fPlayerDistanceToLocalPlayer
            )
            {
                fPlayerDistanceToLocalPlayer = fCurrentPlayerDistanceToLocalPlayer;
            }
        }

        if (std::numeric_limits<float>::max() == fPlayerDistanceToLocalPlayer) {
            continue;
        }

        constexpr const float fRadiansToDegreesValue = 180.f / ::PI;

        // opp. / adj.
        globals::entity::pLocalPlayer->vec2ViewAngles.x = (-std::atan2(vec3Delta.x, vec3Delta.y) * fRadiansToDegreesValue) + 180.f;

        // opp. / hyp.
        globals::entity::pLocalPlayer->vec2ViewAngles.y = std::asin(vec3Delta.z / fPlayerDistanceToLocalPlayer) * fRadiansToDegreesValue;
    }

    fclose(pFile);
    FreeConsole();

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
        if(!globals::modules::ac_client_exe) {
            pErrorMessage = __TEXT("Failed to retrieve module \"ac_client.exe\". This can happen when you inject me into the wrong process!");
            FreeLibrary(hInstDLL);
            return TRUE;
        }

        globals::function::pointer::pPopupMessage = reinterpret_cast<const globals::function::definition::_popupMessage_t>(globals::modules::ac_client_exe + offsets::ac_client_exe::function::POPUP_MESSAGE);

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