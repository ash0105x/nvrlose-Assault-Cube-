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
// globals namespace
import globals;
// patterns to find in AssaultCube's source code
import signatures;
// CRenderer class
import CRenderer;
// CTraceRay function pointer initialisation
import CTraceRay;

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

static __declspec(noreturn) void exitMessageBox(
    _In_ void* const vpInstDLL,
    _In_z_ const char* const cstrMessage,
    _In_ const DWORD dwExitCode
) noexcept
{
    assert(cstrMessage);

    MessageBoxA(
        nullptr,
        cstrMessage,
        "Error",
        (MB_ICONERROR | MB_OK)
    );

    FreeLibraryAndExitThread(static_cast<const HMODULE>(vpInstDLL), dwExitCode);
}

static __declspec(noreturn) void exitPopupMessage(
    _In_ void* const vpInstDLL,
    _In_z_ const char* const cstrMessage,
    _In_ const DWORD dwExitCode
) noexcept
{
    assert(cstrMessage);

    (*globals::function::pPopupMessage)(cstrMessage);

    FreeLibraryAndExitThread(static_cast<const HMODULE>(vpInstDLL), dwExitCode);
}

static DWORD CALLBACK MainThread(
    _In_ void* const vpInstDLL
) noexcept
{
    globals::entity::pLocalPlayer = *reinterpret_cast<playerent* const* const>(globals::modules::ac_client_exe.asBytePtr + offsets::ac_client_exe::pointer::LOCAL_PLAYER);

    typedef void(*exit_t)(_In_ void* const, _In_z_ const char* const, _In_ const DWORD) noexcept;

    exit_t pExit = &exitMessageBox;

    try {
        globals::pRenderer = new CRenderer{ "nvrlose client (Assault Cube)" };
    }
    catch (const std::bad_alloc&) {
        (*pExit)(vpInstDLL, "Failed to allocate enough memory to hold the \"CRenderer\"-class!", ERROR_NOT_ENOUGH_MEMORY);
    }

    if (!globals::pRenderer->ok()) {
        (*pExit)(vpInstDLL, "Failed to make in-game-rendering possible!", ERROR_FUNCTION_FAILED);
    }

    if (
        !(
            globals::function::pPopupMessage = reinterpret_cast<const globals::function::definition::_popupMessage_t>(
                utils::memory::findSignature(
                    globals::modules::ac_client_exe.asHandle,
                    signatures::function::popupMessage
                )
            )
        )
    )
    {
        (*pExit)(vpInstDLL, "Failed to find function popupMessage", ERROR_NOT_FOUND);
    }

    pExit = &exitPopupMessage;
    
    void* vpUnknownFunction = nullptr;
    void* vpHealthOpCode = nullptr;
    void* vpShootFunction = nullptr;

    constexpr const std::uint8_t PATTERNS_TO_FIND = 6;

    const std::array<const std::pair<const SignatureData_t&, void** const>, PATTERNS_TO_FIND> patterns = std::array<const std::pair<const SignatureData_t&, void** const>, PATTERNS_TO_FIND>{
        std::make_pair<const SignatureData_t&, void** const>(
            signatures::function::traceLine,
            reinterpret_cast<void** const>(&CTraceRay::_pTraceLineFn)
        ),
        std::make_pair<const SignatureData_t&, void** const>(
            signatures::function::isVisible,
            reinterpret_cast<void** const>(&CTraceRay::_pIsVisibleFn)
        ),
        std::make_pair<const SignatureData_t&, void** const>(
            signatures::function::intersect,
            reinterpret_cast<void** const>(&CTraceRay::_pIntersectFn)
        ),
        std::make_pair<const SignatureData_t&, void** const>(
            signatures::function::unknown,
            &vpUnknownFunction
        ),
        std::make_pair<const SignatureData_t&, void** const>(
            signatures::code::health,
            &vpHealthOpCode
        ),
        std::make_pair<const SignatureData_t&, void** const>(
            signatures::function::shoot,
            &vpShootFunction
        )
    };

    for (const std::pair<const SignatureData_t&, void** const>& refPattern : patterns) {
        typedef enum : std::uint8_t {
            SIGNATURE_INDEX_DATA = NULL,
            SIGNATURE_INDEX_POINTER
        }SIGNATURE_INDEX;

        assert(std::get<SIGNATURE_INDEX::SIGNATURE_INDEX_POINTER>(refPattern));

        void*& vpRefNewValue = *std::get<SIGNATURE_INDEX::SIGNATURE_INDEX_POINTER>(refPattern);

        if (
            const SignatureData_t& refSignatureData = std::get<SIGNATURE_INDEX::SIGNATURE_INDEX_DATA>(refPattern);
            !(
                vpRefNewValue = utils::memory::findSignature(
                    globals::modules::ac_client_exe.asHandle,
                    refSignatureData,
                    std::strlen(std::get<SIGNATURE_DATA_INDEX::SIGNATURE_DATA_INDEX_MASK>(refSignatureData))
                )
            )
        )
        {
            (*pExit)(vpInstDLL, "Failed to analyse Assault Cube", ERROR_NOT_FOUND);
        }
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
        (*pExit)(vpInstDLL, "Failed to allocate enough memory to hold the hook classes", ERROR_NOT_ENOUGH_MEMORY);
    }

    if (
        !(std::get<HOOK_INDEX::HOOK_INDEX_JUMP_BACK_ADDRESS>(hooks::healthDecreaseOpcode) = std::get<HOOK_INDEX::HOOK_INDEX_METHOD>(hooks::healthDecreaseOpcode)->attach(std::get<HOOK_INDEX::HOOK_INDEX_FUNCTION>(hooks::healthDecreaseOpcode))) ||
        !std::get<HOOK_INDEX::HOOK_INDEX_METHOD>(hooks::shootFunction)->attach(std::get<HOOK_INDEX::HOOK_INDEX_FUNCTION>(hooks::shootFunction)) ||
        !(std::get<HOOK_INDEX::HOOK_INDEX_JUMP_BACK_ADDRESS>(hooks::unknownOpcode) = std::get<HOOK_INDEX::HOOK_INDEX_METHOD>(hooks::unknownOpcode)->attach(std::get<HOOK_INDEX::HOOK_INDEX_FUNCTION>(hooks::unknownOpcode)))
    )
    {
        (*pExit)(vpInstDLL, "Failed to instantiate hooks", ERROR_FUNCTION_FAILED);
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

        const auto exit = [&hInstDLL](_In_z_ const errorType_t* const tcstrError) noexcept -> __declspec(noreturn) void{
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

        DisableThreadLibraryCalls(hInstDLL);

        if (!globals::modules::ac_client_exe.asBytePtr) {
            SetLastError(ERROR_MOD_NOT_FOUND);

            exit(
                (
                    __TEXT("Failed to retrieve module \"ac_client.exe\" because you injected me into \"") +
                    std::basic_string<TCHAR>{ utils::process::name(GetCurrentProcessId()) } +
                    __TEXT("\" instead of Assault Cube (ac_client.exe)")
                ).c_str()
            );
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
            exit(__TEXT("Failed to create a seperate thread to fully instantiate the cheat"));
        }

        SetThreadDescription(hThread, L"nvrlose client main");

        DWORD dwSuspendCount = NULL;

        do {
            if ((dwSuspendCount = ResumeThread(hThread)) != ((DWORD)(-1))) {
                continue;
            }

            CloseHandle(hThread);
            exit(__TEXT("Failed to resume the main thread"));
        } while (dwSuspendCount > 1);

        CloseHandle(hThread);
    }
        break;
    case DLL_PROCESS_DETACH:
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
    
        break;
    default:
        break;
    }

    return TRUE;
}