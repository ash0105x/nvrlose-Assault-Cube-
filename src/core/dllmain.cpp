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

import aimbot;

import CMidHook32;
import weapon;
import CDetour32;

// std::uintptr_t, std::bad_alloc, ...
#include<iostream>
// assert
#include<assert.h>

namespace globals {
    namespace thread {
        static DWORD dwId = NULL;
    }

    static CRenderer* pRenderer = nullptr;
}

static void hkHealthDecreaseOpcode(void) noexcept {
    CPlayer* pPlayer = nullptr;

    __asm {
        sub ebx, 0xF4u
        mov dword ptr[pPlayer], ebx
    }

    pPlayer->iHealth = pPlayer->uTeamID == globals::entity::pLocalPlayer->uTeamID ? 9999999u : NULL;
}

static void hkAmmoDecreaseOpcode(void) noexcept {
    weapon* pWeapon = nullptr;

    __asm {
        mov dword ptr[pWeapon], esi
    }

    if (pWeapon->pOwner->uTeamID == globals::entity::pLocalPlayer->uTeamID) {
        *pWeapon->upAmmo = 9999999u;
        *pWeapon->upReservedAmmo = 9999999u;
        return;
    }
    *pWeapon->upAmmo = NULL;
    *pWeapon->upReservedAmmo = NULL;
}

//static const void* vpAmmoJumpBackAddress = nullptr;
//
//static void __declspec(naked) hkAmmoDecreaseOpcode(void) noexcept {
//    __asm {
//        // esi holds our weapon object
//        // ebx is free to use
//        // Setting ebx to our localPlayer pointer
//        mov ebx, dword ptr[globals::modules::ac_client_exe]
//        add ebx, dword ptr[offsets::ac_client_exe::pointer::LOCAL_PLAYER]
//        mov ebx, dword ptr[ebx]
//
//        // Reading the localPlayer's teamID and setting it to ebx
//        mov ebx, dword ptr[ebx + 0x032C]
//        push esi
//        mov esi, dword ptr[esi + 0x0008]
//
//        cmp dword ptr[esi + 0x032C], ebx
//        pop esi
//        push esi
//        mov esi, dword ptr[esi + 0x0010]
//        je onTeammate
//
//        mov dword ptr[esi], 0
//        pop esi
//        mov esi, dword ptr[esi + 0x0014]
//        mov dword ptr[esi], 0
//        jmp dword ptr[vpAmmoJumpBackAddress]
//    onTeammate:
//        mov dword ptr[esi], 999999
//        pop esi
//        mov esi, dword ptr[esi + 0x0014]
//        mov dword ptr[esi], 999999
//        jmp dword ptr[vpAmmoJumpBackAddress]
//    }
//}

static DWORD CALLBACK MainThread(
    _In_ void* const vpInstDLL
) noexcept
{
    (*globals::function::pointer::pPopupMessage)("Current thread Id: 0x%p", globals::thread::dwId);

    globals::entity::pLocalPlayer = *reinterpret_cast<CPlayer* const* const>(globals::modules::ac_client_exe + offsets::ac_client_exe::pointer::LOCAL_PLAYER);
    globals::match::ipPlayerInGame = reinterpret_cast<const std::uint32_t* const>(globals::modules::ac_client_exe + offsets::ac_client_exe::pointer::I_CURRENT_PLAYER_IN_GAME);

    const auto exit = [&vpInstDLL](_In_z_ const char* const cstrMessage, _In_ const DWORD dwExitCode) noexcept -> __declspec(noreturn) void {
        assert(cstrMessage);
        
        (*globals::function::pointer::pPopupMessage)(cstrMessage);

        utils::dll::eject(vpInstDLL, dwExitCode);
    };

    CMidHook32<MID_HOOK_ORDER::MID_HOOK_ORDER_STOLEN_BYTES_DISCARD> healthOpcodeHook = CMidHook32<MID_HOOK_ORDER::MID_HOOK_ORDER_STOLEN_BYTES_DISCARD>{
        globals::modules::ac_client_exe + 0x29D1Fu,
        5u
    };

    if (!healthOpcodeHook.attach(&::hkHealthDecreaseOpcode)) {
        exit("Failed to enable godmode cheat", ERROR_FUNCTION_FAILED);
    }

    CMidHook32<MID_HOOK_ORDER::MID_HOOK_ORDER_STOLEN_BYTES_LAST> ammoDecreaseHook = CMidHook32<MID_HOOK_ORDER::MID_HOOK_ORDER_STOLEN_BYTES_LAST>{
        globals::modules::ac_client_exe + 0x637E6u,
        5u
    };

    if (!ammoDecreaseHook.attach(&::hkAmmoDecreaseOpcode)) {
        exit("Failed to enable unlimited ammo cheat", ERROR_FUNCTION_FAILED);
    }

    try {
        globals::pRenderer = new CRenderer{ };
    }
    catch (const std::bad_alloc&) {
        exit("Failed to allocate enough memory to hold the \"CRenderer\"-class!", ERROR_NOT_ENOUGH_MEMORY);
    }

    if (!globals::pRenderer->ok()) {
        exit("Failed to hook function \"wglSwapBuffers\" in module \"OpenGL32.dll\" to make in-game-rendering possible!", ERROR_FUNCTION_FAILED);
    }

    while (!GetAsyncKeyState(VK_END)) {
        Sleep(100ul);
    }

    if (!ammoDecreaseHook.detach()) {
        exit("Failed to disable unlimited ammo cheat", ERROR_FUNCTION_FAILED);
    }

    if (!healthOpcodeHook.detach()) {
        exit("Failed to detach the godmode hook", ERROR_FUNCTION_FAILED);
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
    static bool bInvalidModule = false;

    if (DLL_PROCESS_ATTACH == fdwReason) {
        if(!globals::modules::ac_client_exe) {
            bInvalidModule = true;
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
        if (bInvalidModule) {
            (*utils::messagebox::error)(__TEXT("Failed to retrieve module \"ac_client.exe\". This can happen when you inject me into the wrong process!"));
            return TRUE;
        }

        if (globals::pRenderer) {
            delete globals::pRenderer;
        }
    }

    return TRUE;
}