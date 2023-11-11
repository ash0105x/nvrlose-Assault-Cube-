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
    namespace thread {
        static DWORD dwId = NULL;
    }

    static CRenderer* pRenderer = nullptr;
}

static const void* vpHealthJumpBackAddress = nullptr;

static void __declspec(naked) hkHealthDecreaseOpcode(void) noexcept {
    __asm {
        mov eax, dword ptr[globals::modules::ac_client_exe]
        add eax, dword ptr[offsets::ac_client_exe::pointer::LOCAL_PLAYER]
        mov eax, dword ptr[eax]
        mov al, byte ptr[eax + 0x032Cu] // 0x032C is the offset for uTeamID in the playerent class

        cmp byte ptr[ebx + 0x238u], al
        je short onTeammate

        mov eax, dword ptr[ebx + 0x4u]
        mov dword ptr[ebx + 0x4u], NULL

        jmp dword ptr[vpHealthJumpBackAddress]
    onTeammate:
        xor eax, eax
        mov dword ptr[ebx + 0x4u], 100u

        jmp dword ptr[vpHealthJumpBackAddress]
    }
}

static const void* vpAmmoJumpBackAddress = nullptr;

static void __declspec(naked) hkAmmoDecreaseOpcode(void) noexcept {
    __asm {
        // esi holds our weapon object
        // Setting ebx to our localPlayer pointer
        mov ebx, dword ptr[globals::modules::ac_client_exe]
        add ebx, dword ptr[offsets::ac_client_exe::pointer::LOCAL_PLAYER]
        mov ebx, dword ptr[ebx]

        mov eax, dword ptr[esi + 0x0008u] // 0x0008 is the offset for pOwner in the weapon class

        // Reading the localPlayer's teamID and setting it to bl
        mov bl, byte ptr[ebx + 0x032Cu] // 0x032C is the offset for uTeamID in the playerent class

        cmp byte ptr[eax + 0x032Cu], bl // 0x032C is the offset for uTeamID in the playerent class
        mov ebx, dword ptr[esi + 0x0010u] // 0x0010u is the offset for upReservedAmmo in the weapon class
        je short onTeammate

        xor eax, eax
        jmp short applyNewHealth
    onTeammate:
        mov eax, 999999999u
    applyNewHealth:
        mov dword ptr[ebx], eax
        mov esi, dword ptr[esi + 0x0014u] // 0x0014u is the offset for upAmmo in the weapon class
        mov dword ptr[esi], eax
        jmp dword ptr[vpAmmoJumpBackAddress]
    }
}

static DWORD CALLBACK MainThread(
    _In_ void* const vpInstDLL
) noexcept
{
    (*globals::function::pointer::pPopupMessage)("Current thread Id: 0x%p", globals::thread::dwId);

    globals::entity::pLocalPlayer = *reinterpret_cast<playerent* const* const>(globals::modules::ac_client_exe + offsets::ac_client_exe::pointer::LOCAL_PLAYER);
    globals::match::bypPlayerInGame = reinterpret_cast<const std::uint8_t* const>(globals::modules::ac_client_exe + offsets::ac_client_exe::pointer::I_CURRENT_PLAYER_IN_GAME);
    globals::screen::pModelViewProjectionMatrix = reinterpret_cast<float* const>(globals::modules::ac_client_exe + offsets::ac_client_exe::pointer::MODEL_VIEW_PROJECTION_MATRIX);

    const auto exit = [&vpInstDLL](_In_z_ const char* const cstrMessage, _In_ const DWORD dwExitCode) noexcept -> __declspec(noreturn) void {
        assert(cstrMessage);
        
        (*globals::function::pointer::pPopupMessage)(cstrMessage);

        utils::dll::eject(vpInstDLL, dwExitCode);
    };

    CDetour32 healthOpcodeHook = CDetour32{
        globals::modules::ac_client_exe + 0x29D1Fu,
        5u
    };

    if (!(::vpHealthJumpBackAddress = healthOpcodeHook.attach(&::hkHealthDecreaseOpcode))) {
        exit("Failed to enable godmode cheat", ERROR_FUNCTION_FAILED);
    }

    CDetour32 ammoDecreaseHook = CDetour32{
        globals::modules::ac_client_exe + 0x637E6u,
        5u
    };

    if (!(::vpAmmoJumpBackAddress = ammoDecreaseHook.attach(&::hkAmmoDecreaseOpcode))) {
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