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
    static const CRenderer* pRenderer = nullptr;

    namespace hook {
        static CDetour32* pHealthOpcode = nullptr;
        static CMidHook32<MID_HOOK_ORDER::MID_HOOK_ORDER_STOLEN_BYTES_LAST>* pShoot = nullptr;
        static CDetour32* pUnknown = nullptr;
    }

    namespace jumpBackAddress {
        static const void* vpHealthOpcode = nullptr;
        static const void* vpUnknownOpcode = nullptr;
    }
}

static void __declspec(naked) hkHealthDecreaseOpcode(void) noexcept {
#ifdef _DEBUG
    __asm {
        mov eax, dword ptr[globals::entity::pLocalPlayer]
        mov al, byte ptr[eax + 0x01E4u]

        cmp byte ptr[ebx + 0x01E4u - 0xF4u], al
        jne short notLocalPlayer
        xor eax, eax
        jmp dword ptr[globals::jumpBackAddress::vpHealthOpcode]
    notLocalPlayer:
        sub dword ptr[ebx + 0x4u], edi
        mov eax, edi
        jmp dword ptr[globals::jumpBackAddress::vpHealthOpcode]
    }
#endif // _DEBUG

    __asm {
        // ebx is the defender, but 0xF4 bytes ahead from the playerent base

        // Setting eax to the localPlayer pointer
        mov eax, dword ptr[globals::entity::pLocalPlayer]
        // Setting al to the localPlayer's teamID (std::uint8_t)
        mov al, byte ptr[eax + 0x032Cu]
        // Comparing the defender's teamID with our's
        cmp byte ptr[ebx + 0x032Cu - 0xF4u], al
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

static void __cdecl setValues(
    const char* const cstrNickname,
    const std::uint32_t uDamageGiven,
    const std::int32_t iHealthRemaining
) noexcept
{
    globals::vecEntitiesHit.emplace_back(
         std::tuple<std::string, std::uint32_t, std::int32_t, std::chrono::steady_clock::time_point> {
            std::string{ cstrNickname },
            uDamageGiven,
            iHealthRemaining,
            std::chrono::steady_clock::now()
         }
    );
}

static void __declspec(naked) hkUnknown(
    //const playerent* const pDefender, // ecx
    //const std::int32_t iDamage, // [ebp + 0x8]
    //const playerent* const pAttacker, // [ebp + 0xC]
    //const std::int32_t a4, // [ebp + 0x10]
    //const bool a5, // [ebp + 0x14]
    //const bool a6 // [ebp + 0x18]
) noexcept
{
    __asm {
        push ebp
        mov ebp, esp

        mov ecx, dword ptr[ebp + 0xC]
        cmp dword ptr[globals::entity::pLocalPlayer], ecx
        je onLocalPlayer
        jmp dword ptr[globals::jumpBackAddress::vpUnknownOpcode]
    onLocalPlayer:
        mov ecx, dword ptr[eax + 0xF8]
        sub ecx, dword ptr[ebp + 0x8]

        push eax

        push ecx
        push dword ptr[ebp + 0x8]
        lea ecx, dword ptr[eax + 0x225]
        push ecx

        call setValues
        add esp, 0xC

        pop eax
        jmp dword ptr[globals::jumpBackAddress::vpUnknownOpcode]
    }
}

static void hkShoot() noexcept;

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
                    signatures::function::popupMessage
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
                    signatures::function::traceLine
                )
            )
        ) ||
        !(
            CTraceRay::_pIsVisibleFn = reinterpret_cast<const CTraceRay::_entityIsVisible_t>(
                utils::memory::findSignature(
                    reinterpret_cast<const HMODULE>(globals::modules::ac_client_exe),
                    signatures::function::isVisible
                )
            )
        ) ||
        !(
            CTraceRay::_pIntersectFn = reinterpret_cast<const CTraceRay::_intersect_t>(
                utils::memory::findSignature(
                    reinterpret_cast<const HMODULE>(globals::modules::ac_client_exe),
                    signatures::function::intersect
                )
            )
        )
    )
    {
        exit("Failed to find functions", ERROR_NOT_FOUND);
    }

    void* const vpUnknownFunction = utils::memory::findSignature(
        reinterpret_cast<const HMODULE>(globals::modules::ac_client_exe),
        signatures::function::unknown
    );

    if (!vpUnknownFunction) {
        exit("Failed to fund unknown function lol", ERROR_NOT_FOUND);
    }

    void* const vpHealthOpCode = utils::memory::findSignature(
        reinterpret_cast<const HMODULE>(globals::modules::ac_client_exe),
        signatures::code::health
    );

    if (!vpHealthOpCode) {
        exit("Failed to find health opcode", ERROR_NOT_FOUND);
    }

    void* const vpShootFunction = utils::memory::findSignature(
        reinterpret_cast<const HMODULE>(globals::modules::ac_client_exe),
        signatures::function::shoot
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

        globals::hook::pUnknown = new std::remove_pointer<decltype(globals::hook::pUnknown)>::type{
            vpUnknownFunction,
            6u
        };
    }
    catch (const std::bad_alloc&) {
        exit("Failed to allocate enough memory to hold the hook classes", ERROR_NOT_ENOUGH_MEMORY);
    }

    if (
        !(globals::jumpBackAddress::vpHealthOpcode = globals::hook::pHealthOpcode->attach(&::hkHealthDecreaseOpcode)) ||
        !globals::hook::pShoot->attach(&::hkShoot) ||
        !(globals::jumpBackAddress::vpUnknownOpcode = globals::hook::pUnknown->attach(&::hkUnknown))
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
    constexpr const std::uint8_t MAX_ERROR_MESSAGE_LENGTH = 200;
    static TCHAR tcstrErrorMessage[MAX_ERROR_MESSAGE_LENGTH] = { __TEXT('\0') };

    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
    {
        typedef std::remove_all_extents<decltype(tcstrErrorMessage)>::type errorType_t;

        const auto exit = [&hInstDLL](const errorType_t* const tcstrError) noexcept -> __declspec(noreturn) void {
            assert(
                tcstrError &&
                tcstrError[NULL] != __TEXT('\0') &&
                _tcslen(tcstrError) < MAX_ERROR_MESSAGE_LENGTH
            );

            const DWORD dwLastError = GetLastError();

            constexpr const errorType_t* const ERROR_MESSAGE_FORMAT = (
                std::_Is_character<errorType_t>::value ?
                __TEXT("%s! Error code: %d (0x%lx)") :
                __TEXT("%ls! Error code: %d (0x%lx)")
            );

            _stprintf_s(
                tcstrErrorMessage,
                MAX_ERROR_MESSAGE_LENGTH,
                ERROR_MESSAGE_FORMAT,
                tcstrError,
                dwLastError,
                dwLastError
            );

            FreeLibrary(hInstDLL);
        };

        if(!globals::modules::ac_client_exe) {
            exit(
                (
                    __TEXT("Failed to retrieve module \"ac_client.exe\" because you injected me into \"") +
                    std::basic_string<TCHAR>{ utils::process::name(GetCurrentProcessId()) } +
                    __TEXT("\" instead of Assault Cube (ac_client.exe)")
                ).c_str()
            );
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
            exit(__TEXT("Failed to create a seperate thread to fully instantiate the cheat"));
        }

        SetThreadDescription(hThread, L"nvrlose client main");

        ResumeThread(hThread);

        CloseHandle(hThread);
    }
        break;
    case DLL_PROCESS_DETACH:
    {
        if (tcstrErrorMessage[NULL] != __TEXT('\0')) {
            MessageBox(
                nullptr,
                tcstrErrorMessage,
                __TEXT("Error!"),
                (MB_OK | MB_ICONERROR)
            );
            return TRUE;
        }

        if (globals::function::pPopupMessage) {
            (*globals::function::pPopupMessage)("Exitting...");
        }

        if (globals::hook::pUnknown) {
            delete globals::hook::pUnknown;
            delete globals::hook::pShoot;
            delete globals::hook::pHealthOpcode;
            delete globals::pRenderer;
        }
        else if (globals::hook::pShoot) {
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
        break;
    default:
        break;
    }

    return TRUE;
}

static void hkShoot() noexcept {
    weapon* This = nullptr;

    __asm {
        mov dword ptr[This], esi
    }

#ifdef _DEBUG
    if (globals::entity::pLocalPlayer != This->pOwner) {
        return;
    }
    {
        const std::uint32_t& uRefInitialAmmo = This->pInitialWeaponData->uAmmo;

        *This->upAmmo = uRefInitialAmmo + 1;
        *This->upReservedAmmo = uRefInitialAmmo * 2;
    }
    return;
#endif // _DEBUG

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