// dllmain.cpp : Defines the entry point for the DLL application.
#include<Windows.h>

// utility functions
import utils;
// playerent class
import playerent;
// offsets and memory addresses
import offsets;
// CTrampolineHook32 class
import CTrampolineHook32;
// CRenderer class
import CRenderer;
// globals namespace
import globals;

import CMidHook32;
import weapon;
import CDetour32;

// std::uintptr_t, std::bad_alloc, ...
#include<iostream>
// assert
#include<assert.h>

namespace globals {
    static CRenderer* pRenderer = nullptr;
}

static const void* vpHealthJumpBackAddress = nullptr;

static void __declspec(naked) hkHealthDecreaseOpcode(void) noexcept {
    __asm {
        // ebx is the defender, but 0xF4 bytes ahead from the playerent base

        // Setting eax to the localPlayer pointer
        mov eax, dword ptr[globals::modules::ac_client_exe]
        add eax, dword ptr[offsets::ac_client_exe::pointer::LOCAL_PLAYER]
        mov eax, dword ptr[eax]

        // Setting al to the localPlayer's teamID (std::uint8_t)
        mov al, byte ptr[eax + 0x032Cu]

        // Comparing the defender's teamID with our's
        cmp byte ptr[ebx + 0x238u], al

        // Jump if the defender is our teammate
        je short onTeammate

        // Will be executed if the defender is NOT our teammate
        // Setting eax to the current defender's health. eax holds the damage taken value
        mov eax, dword ptr[ebx + 0x4u]
        // Setting the defender's health to zero
        mov dword ptr[ebx + 0x4u], NULL

        // Continue original game code execution to prevent crashes
        jmp dword ptr[vpHealthJumpBackAddress]
    onTeammate:
        // Setting the damage taken value to zero.
        xor eax, eax
        // Setting the defender's health to 100
        mov dword ptr[ebx + 0x4u], 100u

        // Continue original game code execution to prevent crashes
        jmp dword ptr[vpHealthJumpBackAddress]
    }
}

static void hkShoot() noexcept {
    weapon* This = nullptr;

    __asm {
        mov dword ptr[This], esi
    }

    if (This->pOwner->uTeamID == globals::entity::pLocalPlayer->uTeamID) {
        *This->upAmmo = 999999u;
        *This->upReservedAmmo = 999999u;
    }
    else {
        *This->upAmmo = NULL;
        *This->upReservedAmmo = NULL;
    }
}

static DWORD CALLBACK MainThread(
    _In_ void* const vpInstDLL
) noexcept
{
    (*globals::function::pointer::pPopupMessage)("Thread Id: %d (0x%xl)", globals::thread::dwId);

    globals::entity::pLocalPlayer = *reinterpret_cast<playerent* const* const>(globals::modules::ac_client_exe + offsets::ac_client_exe::pointer::LOCAL_PLAYER);
    globals::match::bypPlayerInGame = reinterpret_cast<const std::uint8_t* const>(globals::modules::ac_client_exe + offsets::ac_client_exe::pointer::I_CURRENT_PLAYER_IN_GAME);
    globals::screen::pModelViewProjectionMatrix = reinterpret_cast<const float* const>(globals::modules::ac_client_exe + offsets::ac_client_exe::pointer::MODEL_VIEW_PROJECTION_MATRIX);
    globals::screen::vec3CurrentWeaponEndTrajectory = reinterpret_cast<const CVector3* const>(globals::modules::ac_client_exe + offsets::ac_client_exe::pointer::VEC3_CURRENT_WEAPON_END_TRAJECTORY);

    const auto exit = [&vpInstDLL](_In_z_ const char* const cstrMessage, _In_ const DWORD dwExitCode) noexcept -> __declspec(noreturn) void {
        assert(cstrMessage);
        
        (*globals::function::pointer::pPopupMessage)(cstrMessage);

        FreeLibraryAndExitThread(static_cast<const HMODULE>(vpInstDLL), dwExitCode);
    };

    try {
        globals::pRenderer = new CRenderer{ "nvrlose client (Assault Cube)" };
    }
    catch (const std::bad_alloc&) {
        exit("Failed to allocate enough memory to hold the \"CRenderer\"-class!", ERROR_NOT_ENOUGH_MEMORY);
    }

    if (!globals::pRenderer->ok()) {
        exit("Failed to make in-game-rendering possible!", ERROR_FUNCTION_FAILED);
    }

    CDetour32 healthOpcodeHook = CDetour32{
        globals::modules::ac_client_exe + 0x29D1Fu,
        5u
    };

    if (!(::vpHealthJumpBackAddress = healthOpcodeHook.attach(&::hkHealthDecreaseOpcode))) {
        exit("Failed to enable godmode cheat", ERROR_FUNCTION_FAILED);
    }

    CMidHook32<MID_HOOK_ORDER::MID_HOOK_ORDER_STOLEN_BYTES_LAST> ammoDecreaseHook = CMidHook32<MID_HOOK_ORDER::MID_HOOK_ORDER_STOLEN_BYTES_LAST>{
        globals::modules::ac_client_exe + 0x6366Cu,
        8u
    };

    if (!ammoDecreaseHook.attach(&::hkShoot)) {
        if (!healthOpcodeHook.detach()) {
            (*globals::function::pointer::pPopupMessage)("Failed to detach the godmode hook", ERROR_FUNCTION_FAILED);
        }

        exit("Failed to enable unlimited ammo cheat", ERROR_FUNCTION_FAILED);
    }

    while (!GetAsyncKeyState(VK_END)) {
        Sleep(100ul);
    }

    if (!ammoDecreaseHook.detach()) {
        (*globals::function::pointer::pPopupMessage)("Failed to disable unlimited ammo cheat", ERROR_FUNCTION_FAILED);
    }

    if (!healthOpcodeHook.detach()) {
        (*globals::function::pointer::pPopupMessage)("Failed to detach the godmode hook", ERROR_FUNCTION_FAILED);
    }

    FreeLibraryAndExitThread(static_cast<const HMODULE>(vpInstDLL), ERROR_SUCCESS);
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
            utils::messagebox::error(__TEXT("Failed to retrieve module \"ac_client.exe\". This can happen when you inject me into the wrong process!"));
            return TRUE;
        }

        (*globals::function::pointer::pPopupMessage)("Exitting...");

        if (globals::pRenderer) {
            delete globals::pRenderer;
        }
    }

    return TRUE;
}