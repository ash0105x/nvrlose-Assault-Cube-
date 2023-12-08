// dllmain.cpp : Defines the entry point for the DLL application.
#ifndef WIN32
    #error "This project can only be compiled for x86 architecture."
#endif // !WIN32

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
import signatures;
import CMenu;
import CTraceRay;

import<array>;
import<tchar.h>;

// std::uintptr_t, std::bad_alloc, ...
#include<iostream>
// assert
#include<assert.h>
#include<chrono>
#include<tuple>

namespace globals {
    static CRenderer* pRenderer = nullptr;

    namespace hook {
        static CDetour32* pHealthOpcode = nullptr;
        static CMidHook32<MID_HOOK_ORDER::MID_HOOK_ORDER_STOLEN_BYTES_LAST>* pShoot = nullptr;
    }

    namespace jumpBackAddress {
        static const void* vpHealthOpcode = nullptr;
    }
}

static void __declspec(naked) hkHealthDecreaseOpcode(void) noexcept {
    __asm {
        // ebx is the defender, but 0xF4 bytes ahead from the playerent base

        // Setting eax to the localPlayer pointer
        mov eax, dword ptr[globals::entity::pLocalPlayer]
        // Setting al to the localPlayer's teamID (std::uint8_t)
        mov al, byte ptr[eax + 0x032Cu]
        // Comparing the defender's teamID with our's
        cmp byte ptr[ebx + 0x238u], al
        // Jump if the defender is an enemy
        jne short onEnemy

        // Will be executed if the defender is our teammate
        
        // Setting the damage taken value to zero.
        xor eax, eax
        // Continue original game code execution to prevent crashes
        jmp dword ptr[globals::jumpBackAddress::vpHealthOpcode]
    onEnemy:
        // Will be executed if the defender is an enemy
        // Setting eax to the current defender's health. eax holds the damage taken value
        mov eax, dword ptr[ebx + 0x4u]
        // Setting the defender's health to zero
        mov dword ptr[ebx + 0x4u], NULL
        // Continue original game code execution to prevent crashes
        jmp dword ptr[globals::jumpBackAddress::vpHealthOpcode]
    }
}

static void hkShoot() noexcept {
    weapon* This = nullptr;

    __asm {
        mov dword ptr[This], esi
    }

    if (globals::entity::pLocalPlayer->uTeamID != This->pOwner->uTeamID) {
        *This->upAmmo = NULL;
        *This->upReservedAmmo = NULL;

        return;
    }

    const std::uint32_t& uRefInitialAmmo = This->pInitialWeaponData->uAmmo;

    *This->upAmmo = uRefInitialAmmo + 1;
    *This->upReservedAmmo = uRefInitialAmmo * 2;

    if (globals::entity::pLocalPlayer != This->pOwner) {
        return;
    }

    globals::vecLocalPlayerShotPositions.emplace_back(
        std::tuple<CVector3, CVector3, std::chrono::steady_clock::time_point>{
            globals::entity::pLocalPlayer->vec3EyePosition,
            *globals::screen::pvec3CurrentWeaponEndTrajectory,
            std::chrono::steady_clock::now()
        }
    );
}

static DWORD CALLBACK MainThread(
    _In_ void* const vpInstDLL
) noexcept
{
    globals::entity::pLocalPlayer = *reinterpret_cast<playerent* const* const>(globals::modules::ac_client_exe + offsets::ac_client_exe::pointer::LOCAL_PLAYER);
    
    const auto exitSecure = [&vpInstDLL](_In_z_ const char* const cstrMessage, _In_ const DWORD dwExitCode) noexcept -> __declspec(noreturn) void{
        assert(cstrMessage);

        MessageBoxA(
            nullptr,
            cstrMessage,
            "Error",
            (MB_ICONERROR | MB_OK)
        );

        FreeLibraryAndExitThread(static_cast<const HMODULE>(vpInstDLL), dwExitCode);
    };

    try {
        globals::pRenderer = new CRenderer{ "nvrlose client (Assault Cube)" };
    }
    catch (const std::bad_alloc&) {
        exitSecure("Failed to allocate enough memory to hold the \"CRenderer\"-class!", ERROR_NOT_ENOUGH_MEMORY);
    }

    if (!globals::pRenderer->ok()) {
        exitSecure("Failed to make in-game-rendering possible!", ERROR_FUNCTION_FAILED);
    }

    if (
        !(
            globals::function::pPopupMessage = reinterpret_cast<const globals::function::definition::_popupMessage_t>(
                utils::memory::findSignature(
                    reinterpret_cast<const HMODULE>(globals::modules::ac_client_exe),
                    signatures::function::popupMessage.pattern,
                    signatures::function::popupMessage.cstrMask
                )
            )
        )
    )
    {
        exitSecure("Failed to find function popupMessage", ERROR_NOT_FOUND);
    }

    const auto exit = [&vpInstDLL](_In_z_ const char* const cstrMessage, _In_ const DWORD dwExitCode) noexcept -> __declspec(noreturn) void{
        assert(cstrMessage);

        (*globals::function::pPopupMessage)(cstrMessage);

        FreeLibraryAndExitThread(static_cast<const HMODULE>(vpInstDLL), dwExitCode);
    };

    if (
        !(
            CTraceRay::_pTraceLineFn = reinterpret_cast<const CTraceRay::_traceLine_t>(
                utils::memory::findSignature(
                    reinterpret_cast<const HMODULE>(globals::modules::ac_client_exe),
                    signatures::function::traceLine.pattern,
                    signatures::function::traceLine.cstrMask
                )
            )
        ) ||
        !(
            CTraceRay::_pIsVisibleFn = reinterpret_cast<const CTraceRay::_entityIsVisible_t>(
                utils::memory::findSignature(
                    reinterpret_cast<const HMODULE>(globals::modules::ac_client_exe),
                    signatures::function::isVisible.pattern,
                    signatures::function::isVisible.cstrMask
                )
            )
        ) ||
        !(
            CTraceRay::_pIntersectFn = reinterpret_cast<const CTraceRay::_intersect_t>(
                utils::memory::findSignature(
                    reinterpret_cast<const HMODULE>(globals::modules::ac_client_exe),
                    signatures::function::intersect.pattern,
                    signatures::function::intersect.cstrMask
                )
            )
        )
    )
    {
        exit("Failed to find functions", ERROR_NOT_FOUND);
    }

    void* const vpHealthOpCode = utils::memory::findSignature(
        reinterpret_cast<const HMODULE>(globals::modules::ac_client_exe),
        signatures::code::health.pattern,
        signatures::code::health.cstrMask
    );

    if (!vpHealthOpCode) {
        exit("Failed to find health opcode", ERROR_NOT_FOUND);
    }

    void* const vpShootFunction = utils::memory::findSignature(
        reinterpret_cast<const HMODULE>(globals::modules::ac_client_exe),
        signatures::function::shoot.pattern,
        signatures::function::shoot.cstrMask
    );

    if (!vpShootFunction) {
        exit("Failed to find function shoot", ERROR_NOT_FOUND);
    }

    try {
        globals::hook::pHealthOpcode = new std::remove_pointer<decltype(globals::hook::pHealthOpcode)>::type{
            vpHealthOpCode,
            5u
        };

        globals::hook::pShoot = new std::remove_pointer<decltype(globals::hook::pShoot)>::type{
            vpShootFunction,
            8u
        };
    }
    catch (const std::bad_alloc&) {
        exit("Failed to allocate enough memory to hold the hook classes", ERROR_NOT_ENOUGH_MEMORY);
    }

    if (
        !(globals::jumpBackAddress::vpHealthOpcode = globals::hook::pHealthOpcode->attach(&::hkHealthDecreaseOpcode)) ||
        !globals::hook::pShoot->attach(&::hkShoot)
    )
    {
        exit("Failed to instantiate hooks", ERROR_FUNCTION_FAILED);
    }

    CRenderer::_bCanSafelyRender = true;

    while (!GetAsyncKeyState(VK_END)) {
        Sleep(100ul);
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
    static TCHAR tcstrErrorMessage[125] = { __TEXT('\0') };

    if (DLL_PROCESS_ATTACH == fdwReason) {
        const auto setError = [](const TCHAR* const tcstrError) noexcept -> void {
            _stprintf_s(
                tcstrErrorMessage,
                tcstrError,
                GetLastError()
            );
        };

        if(!globals::modules::ac_client_exe) {
            setError(__TEXT("Failed to retrieve module \"ac_client.exe\". This can happen when you inject me into the wrong process! Error code: %d"));

            FreeLibrary(hInstDLL);
            return TRUE;
        }

        DisableThreadLibraryCalls(hInstDLL);
        
        const HANDLE hThread = CreateThread(
            nullptr,
            NULL,
            &::MainThread,
            hInstDLL,
            CREATE_SUSPENDED,
            &globals::thread::dwId
        );

        if (!hThread) {
            setError(__TEXT("Failed to create a seperate thread to fully instantiate the cheat! Error code: %d"));
            
            FreeLibrary(hInstDLL);
            return TRUE;
        }

        SetThreadDescription(hThread, L"nvrlose client main");

        ResumeThread(hThread);

        CloseHandle(hThread);
    }
    else if (DLL_PROCESS_DETACH == fdwReason) {
        if (tcstrErrorMessage[NULL] != __TEXT('\0')) {
            MessageBox(
                nullptr,
                __TEXT("Error!"),
                tcstrErrorMessage,
                (MB_OK | MB_ICONERROR)
            );
            return TRUE;
        }

        if (globals::function::pPopupMessage) {
            (*globals::function::pPopupMessage)("Exitting...");
        }

        if (globals::hook::pShoot) {
            delete globals::hook::pShoot;
            delete globals::hook::pHealthOpcode;
            delete globals::pRenderer;
        }
        else if (globals::hook::pHealthOpcode) {
            delete globals::hook::pHealthOpcode;
            delete globals::pRenderer;
        }
        else if (globals::pRenderer) {
            delete globals::pRenderer;
        }
    }

    return TRUE;
}