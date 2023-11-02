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
// assert
#include<assert.h>

namespace globals {
    namespace thread {
        static DWORD dwId = NULL;
    }

    namespace modules {
        std::uint8_t* const ac_client_exe = reinterpret_cast<std::uint8_t* const>(GetModuleHandle(__TEXT("ac_client.exe")));
    }

    static CRenderer* pRenderer = nullptr;
}

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
        mov dword ptr[ebx + 0x4u], NULL

        jmp dword ptr[dwpJumpBackAddress]
    onLocalPlayer:
        xor eax, eax
        mov dword ptr[ebx + 0x4u], 9999999u

        jmp dword ptr[dwpJumpBackAddress]
    }
}

static DWORD CALLBACK MainThread(
    _In_ void* const vpInstDLL
) noexcept
{
    (*globals::function::pointer::pPopupMessage)("Current thread Id: 0x%p", globals::thread::dwId);

    globals::entity::pEntityList = *reinterpret_cast<const std::array<const CPlayer* const, globals::entity::MAX_ENTITIES>* const* const>(globals::modules::ac_client_exe + offsets::ac_client_exe::pointer::ENTITY_LIST);
    globals::entity::pLocalPlayer = *reinterpret_cast<CPlayer* const* const>(globals::modules::ac_client_exe + offsets::ac_client_exe::pointer::LOCAL_PLAYER);
    globals::match::ipPlayerInGame = reinterpret_cast<const std::uint32_t* const>(globals::modules::ac_client_exe + offsets::ac_client_exe::pointer::I_CURRENT_PLAYER_IN_GAME);

    const auto exit = [&vpInstDLL](_In_z_ const char* const cstrMessage, _In_ const DWORD dwExitCode) noexcept -> __declspec(noreturn) void {
        assert(cstrMessage);
        
        (*globals::function::pointer::pPopupMessage)(cstrMessage);

        utils::dll::eject(vpInstDLL, dwExitCode);
    };

    BYTE* const bypHookAddress = reinterpret_cast<BYTE* const>(globals::modules::ac_client_exe + 0x29D1Fu);
    constexpr const std::uint8_t HOOK_LENGTH = 5u;

    std::array<BYTE, HOOK_LENGTH> byArrStolenBytes = std::array<BYTE, HOOK_LENGTH>{ };

    memcpy_s(
        byArrStolenBytes.data(),
        byArrStolenBytes.size(),
        bypHookAddress,
        byArrStolenBytes.size()
    );

    ::dwpJumpBackAddress = reinterpret_cast<const DWORD*>(bypHookAddress + HOOK_LENGTH);

    const DWORD dwRelativeOffset = reinterpret_cast<const std::uintptr_t>(&::hkHealthOpcode) - reinterpret_cast<const std::uintptr_t>(bypHookAddress) - 5u;
    
    DWORD dwPreviousProtection = NULL;
    if (
        !VirtualProtect(
            bypHookAddress,
            HOOK_LENGTH,
            PAGE_EXECUTE_READWRITE,
            &dwPreviousProtection
        )
    )
    {
        exit("Failed to get permission to change the ingame code", ERROR_ACCESS_DENIED);
    }

    bypHookAddress[NULL] = 0xE9u;
    *reinterpret_cast<DWORD* const>(bypHookAddress + 1u) = dwRelativeOffset;

    DWORD dwTempProtection = NULL;
    VirtualProtect(
        bypHookAddress,
        HOOK_LENGTH,
        dwPreviousProtection,
        &dwTempProtection
    );

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

    if (
        !VirtualProtect(
            bypHookAddress,
            HOOK_LENGTH,
            PAGE_EXECUTE_READWRITE,
            &dwPreviousProtection
        )
    )
    {
        exit("Failed to get permission to change the ingame code", ERROR_ACCESS_DENIED);
    }
    memcpy_s(
        bypHookAddress,
        byArrStolenBytes.size(),
        byArrStolenBytes.data(),
        byArrStolenBytes.size()
    );
    VirtualProtect(
        bypHookAddress,
        HOOK_LENGTH,
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