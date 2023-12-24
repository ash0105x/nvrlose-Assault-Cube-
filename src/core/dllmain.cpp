// dllmain.cpp : Defines the entry point for the DLL application.
#ifndef WIN32
    #error "This project can only be compiled for x86 architecture."
#endif // !WIN32

#include<Windows.h>
#include<iostream>
#include<tuple>

// utility functions
import utils;
// playerent class
import playerent;
// offsets and memory addresses
import offsets;
// CRenderer class
import CRenderer;
// globals namespace
import globals;
// CTraceRay function pointer initialisation
import CTraceRay;
// patterns to find in AssaultCube's source code
import signatures;

// hooking
import hooks;
import CMidHook32;
import CDetour32;
import CTrampolineHook32;

import<array>;
import<tchar.h>;
import<cassert>;

namespace globals {
    static const CRenderer* pRenderer = nullptr;
}

static DWORD CALLBACK MainThread(
    _In_ void* const vpInstDLL
) noexcept
{
    globals::entity::pLocalPlayer = *reinterpret_cast<playerent* const* const>(globals::modules::ac_client_exe + offsets::ac_client_exe::pointer::LOCAL_PLAYER);

    const auto exitSecure = [&vpInstDLL](_In_z_ const TCHAR* const tcstrMessage, _In_ const DWORD dwExitCode) noexcept -> __declspec(noreturn) void {
        assert(tcstrMessage);

        MessageBox(
            nullptr,
            tcstrMessage,
            __TEXT("Error"),
            (MB_ICONERROR | MB_OK)
        );

        FreeLibraryAndExitThread(static_cast<const HMODULE>(vpInstDLL), dwExitCode);
    };

    try {
        globals::pRenderer = new CRenderer{ "nvrlose client (Assault Cube)" };
    }
    catch (const std::bad_alloc&) {
        exitSecure(__TEXT("Failed to allocate enough memory to hold the \"CRenderer\"-class!"), ERROR_NOT_ENOUGH_MEMORY);
    }

    if (!globals::pRenderer->ok()) {
        exitSecure(__TEXT("Failed to make in-game-rendering possible!"), ERROR_FUNCTION_FAILED);
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
        exitSecure(__TEXT("Failed to find function popupMessage"), ERROR_NOT_FOUND);
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
        std::get<HOOK_INDEX::HOOK_INDEX_METHOD>(hooks::healthDecreaseOpcode) = new CDetour32{
            vpHealthOpCode,
            5u
        };

        std::get<HOOK_INDEX::HOOK_INDEX_METHOD>(hooks::shootFunction) = new CMidHook32<MID_HOOK_ORDER::MID_HOOK_ORDER_STOLEN_BYTES_LAST>{
            vpShootFunction,
            8u
        };

        std::get<HOOK_INDEX::HOOK_INDEX_METHOD>(hooks::unknownOpcode) = new CDetour32{
            vpUnknownFunction,
            6u
        };
    }
    catch (const std::bad_alloc&) {
        exit("Failed to allocate enough memory to hold the hook classes", ERROR_NOT_ENOUGH_MEMORY);
    }

    if (
        !(std::get<HOOK_INDEX::HOOK_INDEX_JUMP_BACK_ADDRESS>(hooks::healthDecreaseOpcode) = std::get<HOOK_INDEX::HOOK_INDEX_METHOD>(hooks::healthDecreaseOpcode)->attach(std::get<HOOK_INDEX::HOOK_INDEX_FUNCTION>(hooks::healthDecreaseOpcode))) ||
        !std::get<HOOK_INDEX::HOOK_INDEX_METHOD>(hooks::shootFunction)->attach(std::get<HOOK_INDEX::HOOK_INDEX_FUNCTION>(hooks::shootFunction)) ||
        !(std::get<HOOK_INDEX::HOOK_INDEX_JUMP_BACK_ADDRESS>(hooks::unknownOpcode) = std::get<HOOK_INDEX::HOOK_INDEX_METHOD>(hooks::unknownOpcode)->attach(std::get<HOOK_INDEX::HOOK_INDEX_FUNCTION>(hooks::unknownOpcode)))
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

            return FALSE;
        }

        if (globals::function::pPopupMessage) {
            (*globals::function::pPopupMessage)("Exitting...");
        }

        if (std::get<HOOK_INDEX::HOOK_INDEX_METHOD>(hooks::unknownOpcode)) {
            delete std::get<HOOK_INDEX::HOOK_INDEX_METHOD>(hooks::unknownOpcode);
            delete std::get<HOOK_INDEX::HOOK_INDEX_METHOD>(hooks::shootFunction);
            delete std::get<HOOK_INDEX::HOOK_INDEX_METHOD>(hooks::healthDecreaseOpcode);
            delete globals::pRenderer;
        }
        else if (std::get<HOOK_INDEX::HOOK_INDEX_METHOD>(hooks::shootFunction)) {
            delete std::get<HOOK_INDEX::HOOK_INDEX_METHOD>(hooks::shootFunction);
            delete std::get<HOOK_INDEX::HOOK_INDEX_METHOD>(hooks::healthDecreaseOpcode);
            delete globals::pRenderer;
        }
        else if (std::get<HOOK_INDEX::HOOK_INDEX_METHOD>(hooks::healthDecreaseOpcode)) {
            delete std::get<HOOK_INDEX::HOOK_INDEX_METHOD>(hooks::healthDecreaseOpcode);
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