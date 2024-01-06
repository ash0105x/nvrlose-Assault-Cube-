// dllmain.cpp : Defines the entry point for the DLL application.
#ifndef WIN32
    #error "This project can only be compiled for x86 architecture."
#endif // !WIN32

// Windows header file
#include<Windows.h>

// IHook class forward declaration
class IHook;

// utility functions
import utils;
// playerent class
import playerent;
// globals namespace
import globals;
// patterns to find in AssaultCube's source code
import signatures;
// CRenderer class
import CRenderer;
// CTraceRay function pointer initialisation
import CTraceRay;

// hooking methods and functions
import hooks;
import CMidHook32;
import CDetour32;
import CTrampolineHook32;

// std::array
import<array>;
// TCHAR type and functions
import<tchar.h>;
// assert
import<cassert>;

// globals namespace
namespace globals {
    // pointer to CRenderer class
    static const CRenderer* pRenderer = nullptr;
}

namespace exitFunction {
    // Function pointer type that suits the exit functions.
    typedef void(*type_t)(_In_ void* const, _In_z_ const char* const, _In_ const DWORD) noexcept;

    // Display a message box and uninject the dll.
    static __declspec(noreturn) void messageBox(
        _In_ void* const vpInstDLL,
        _In_z_ const char* const cstrMessage,
        _In_ const DWORD dwExitCode
    ) noexcept;

    // Display a popup message in assault cube and uninject the dll.
    static __declspec(noreturn) void popupMessage(
        _In_ void* const vpInstDLL,
        _In_z_ const char* const cstrMessage,
        _In_ const DWORD dwExitCode
    ) noexcept;
}

// Main hack thread
static DWORD CALLBACK MainThread(
    _In_ void* const vpInstDLL
) noexcept
{
    // Function pointer to the currently most suitable exit function.
    exitFunction::type_t pExit = &exitFunction::messageBox;

    // try-catch block to ensure proper heap allocation
    // and error handling when failed.
    try {
        // Try to allocate memory for the CRenderer class and instanciate it.
        // The CRenderer class hooks the "wglSwapBuffers" function in OpenGL32.dll
        // to render graphics in assault cube.
        globals::pRenderer = new CRenderer{ "nvrlose client (Assault Cube)" };
    }
    // Catch block that is executed when the heap allocation failed
    catch (const std::bad_alloc&) {
        // Uninject the dll, exit out of the function and
        // give the user the information that the heap
        // allocation failed.
        (*pExit)(vpInstDLL, "Failed to allocate enough memory to hold the \"CRenderer\"-class!", ERROR_NOT_ENOUGH_MEMORY);
    }

    // if-statement that checks whether the CRenderer instance
    // successfully instanciated or not.
    if (!globals::pRenderer->ok()) {
        // Uninject the dll, exit out of the function and
        // give the user the information that we weren't able
        // to render graphics in assault cube.
        (*pExit)(vpInstDLL, "Failed to make in-game-rendering possible!", ERROR_FUNCTION_FAILED);
    }

    // Try to find the the signature that can be found in
    // the popupMessage function in assault cube, assign its
    // value to globals::function::pPopupMessage and check
    // whether the function could be found or not.
    if (
        !(
            globals::function::pPopupMessage = reinterpret_cast<const globals::function::definition::_popupMessage_t>(
                utils::memory::findSignature(
                    globals::modules::ac_client_exe.asHandle,
                    signatures::function::POPUP_MESSAGE
                )
            )
        )
    )
    {
        // If the findSignature function failed to find the popupMessage function
        // then uninject the dll, exit out of the function and give the user the
        // information that we failed to find the popupMessage function.
        (*pExit)(vpInstDLL, "Failed to find function popupMessage", ERROR_NOT_FOUND);
    }

    // Let the pExit function pointer point to the exit::popupMessage
    // function as the globals::function::pPopupMessage variable
    // is now at a valid state and can be safely called.
    pExit = &exitFunction::popupMessage;
    
    // Pointers that point to functions and other pieces
    // of code that we need to hook.
    void* vpUnknownFunction = nullptr;
    void* vpHealthOpCode = nullptr;
    void* vpShootFunction = nullptr;

    // Compile time varibale that holds the number of
    // patterns that we will try to find in the assault
    // cube game code
    constexpr const std::uint8_t PATTERNS_TO_FIND = 13;

    // Array that holds PATTERNS_TO_FIND amount of std::pair. The pairs store
    // the SignatureData (cstrPattern, cstrMask) and a pointer
    // to a void pointer.
    const std::array<const std::pair<const SignatureData_t&, void** const>, PATTERNS_TO_FIND> arrPatterns = std::array<const std::pair<const SignatureData_t&, void** const>, PATTERNS_TO_FIND>{
        std::make_pair<const SignatureData_t&, void** const>(
            signatures::pointer::LOCAL_PLAYER,
            &reinterpret_cast<void*&>(globals::entity::pLocalPlayer)
        ),
        std::make_pair<const SignatureData_t&, void** const>(
            signatures::pointer::PLAYER_IN_GAME,
            &reinterpret_cast<void*&>(globals::match::bypPlayerInGame)
        ),
        std::make_pair<const SignatureData_t&, void** const>(
            signatures::pointer::ENTITY_LIST,
            &reinterpret_cast<void*&>(globals::entity::ppEntityList)
        ),
        std::make_pair<const SignatureData_t&, void** const>(
            signatures::pointer::MODEL_VIEW_PROJECTION_MATRIX,
            &reinterpret_cast<void*&>(globals::screen::pfArrModelViewProjectionMatrix)
        ),
        std::make_pair<const SignatureData_t&, void** const>(
            signatures::pointer::VEC3_CURRENT_WEAPON_END_TRAJECTORY,
            &reinterpret_cast<void*&>(globals::screen::pVec3CurrentWeaponEndTrajectory)
        ),
        std::make_pair<const SignatureData_t&, void** const>(
            signatures::pointer::FPS,
            &reinterpret_cast<void*&>(globals::screen::fpFPS)
        ),
        std::make_pair<const SignatureData_t&, void** const>(
            signatures::pointer::CURRENT_PLAYING_MAP,
            &reinterpret_cast<void*&>(globals::match::cstrPlayingMap)
        ),
        std::make_pair<const SignatureData_t&, void** const>(
            signatures::function::TRACE_LINE,
            &reinterpret_cast<void*&>(CTraceRay::_pTraceLineFn)
        ),
        std::make_pair<const SignatureData_t&, void** const>(
            signatures::function::IS_VISIBLE,
            &reinterpret_cast<void*&>(CTraceRay::_pIsVisibleFn)
        ),
        std::make_pair<const SignatureData_t&, void** const>(
            signatures::function::INTERSECT,
            &reinterpret_cast<void*&>(CTraceRay::_pIntersectFn)
        ),
        std::make_pair<const SignatureData_t&, void** const>(
            signatures::function::UNKNOWN,
            &vpUnknownFunction
        ),
        std::make_pair<const SignatureData_t&, void** const>(
            signatures::code::HEALTH,
            &vpHealthOpCode
        ),
        std::make_pair<const SignatureData_t&, void** const>(
            signatures::function::SHOOT,
            &vpShootFunction
        )
    };

    // Iterate through every element of the array that we have
    // previously initialised, try to find the signature in the
    // game source code and assign its position to the void pointer
    // that we have passed in.
    for (const std::pair<const SignatureData_t&, void** const>& refPattern : arrPatterns) {
        // Definition of the std::pair indexes.
        typedef enum : std::uint8_t {
            SIGNATURE_INDEX_DATA = NULL,
            SIGNATURE_INDEX_POINTER
        }SIGNATURE_INDEX;

        // Ensure that the pointer is valid.
        assert(std::get<SIGNATURE_INDEX::SIGNATURE_INDEX_POINTER>(refPattern) != nullptr);

        // Store the dereferenced pointer to a void pointer reference named
        // vpRefNewValue to keep simplicity and code readability.
        void*& vpRefNewValue = *std::get<SIGNATURE_INDEX::SIGNATURE_INDEX_POINTER>(refPattern);

        // if-statement that checks whether the signature
        // could be found in the game or not.
        if (
            // Assign the signature data to a reference variable named refSignatureData .
            const SignatureData_t& refSignatureData = std::get<SIGNATURE_INDEX::SIGNATURE_INDEX_DATA>(refPattern);
            // Try to find the signature and check wheter it
            // could be found or not.
            !(
                vpRefNewValue = utils::memory::findSignature(
                    globals::modules::ac_client_exe.asHandle,
                    refSignatureData,
                    std::strlen(std::get<SIGNATURE_DATA_INDEX::SIGNATURE_DATA_INDEX_MASK>(refSignatureData))
                )
            )
        )
        {
            // Uninject the dll, exit out of the function and
            // give the user the information that we weren't able
            // to find all signatures in the game's source code
            (*pExit)(vpInstDLL, "Failed to analyse Assault Cube", ERROR_NOT_FOUND);
        }
    }

    // Extracting pointers from the game source code of Assault Cube.
    //
    // For instance:
    // mov eax, g_pLocalPlayer
    //
    // Our globals::entity::pLocalPlayer variable references an address like 0x450000.
    // This is where the mov instruction points. However, we require the g_pLocalPlayer address,
    // situated at 0x410000. To achieve this, we increment globals::entity::pLocalPlayer by ONE byte.
    // Consequently, it now directs to 0x450001. Subsequently, we perform a single dereference
    // to obtain the pointer value that directs to this address. Following this procedure,
    // the globals::entity::pLocalPlayer variable should correctly point to g_pLocalPlayer (0x410000).
    // If not, it indicates an issue with our signature.
    globals::match::cstrPlayingMap = *reinterpret_cast < char* const* const > ( globals::match::cstrPlayingMap + 1 );
    globals::match::bypPlayerInGame = *reinterpret_cast < std::uint8_t* const* const > ( globals::match::bypPlayerInGame + 2 );
    globals::screen::fpFPS = *reinterpret_cast < float* const* const > ( reinterpret_cast < const std::uint8_t* const > ( globals::screen::fpFPS ) + 2 );
    globals::entity::pLocalPlayer = **reinterpret_cast < playerent* const* const* const > ( reinterpret_cast < const std::uint8_t* const > ( globals::entity::pLocalPlayer ) + 2 );
    globals::screen::pVec3CurrentWeaponEndTrajectory = *reinterpret_cast < CVector3* const* const > ( reinterpret_cast < const std::uint8_t* const > ( globals::screen::pVec3CurrentWeaponEndTrajectory ) + 1 );
    globals::screen::pfArrModelViewProjectionMatrix = *reinterpret_cast < std::array < const float, 16 /*4x4 matrix*/ > * const* const > ( reinterpret_cast < const std::uint8_t* const > ( globals::screen::pfArrModelViewProjectionMatrix ) + 1 );
    globals::entity::ppEntityList = *reinterpret_cast < std::array < const playerent* const, globals::entity::MAX_ENTITIES > ** const* const > ( reinterpret_cast < const std::uint8_t* const > ( globals::entity::ppEntityList ) + 1 );

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

    while (!GetAsyncKeyState(VK_END) && !globals::dll::bUnload) {
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
            if ((dwSuspendCount = ResumeThread(hThread)) != (static_cast<const DWORD>(-1))) {
                continue;
            }

            CloseHandle(hThread);
            SetLastError(ERROR_CONTINUE);
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

        {
            constexpr const std::uint8_t HOOK_COUNT = 3;

            const std::array<const IHook* const* const, HOOK_COUNT> arrSuccessed = std::array<const IHook* const* const, HOOK_COUNT>{
                &std::get<HOOK_INDEX::HOOK_INDEX_METHOD>(hooks::healthDecreaseOpcode),
                &std::get<HOOK_INDEX::HOOK_INDEX_METHOD>(hooks::shootFunction),
                &std::get<HOOK_INDEX::HOOK_INDEX_METHOD>(hooks::unknownOpcode)
            };

            for (const IHook* const* const& ppRefHook : arrSuccessed) {
                const IHook* const& pRefHook = *ppRefHook;

                if (pRefHook) {
                    delete pRefHook;
                }
            }
        }

        if (globals::pRenderer) {
            delete globals::pRenderer;
        }
    
        break;
    default:
        break;
    }

    return TRUE;
}

//
//  FUNCTION: exitFunction::messageBox(_In_ void* const, _In_z_ const char* const, _In_ const DWORD)
//
//  PURPOSE:  Display a message box and uninject the dll.
//
//  PARAMETERS:
//    vpInstDLL   - instance to the current DLL
//    cstrMessage - error message to be displayed
//    dwExitCode  - exit code (e.g. ERROR_SUCCESS, ERROR_NOT_ENOUGH_MEMORY, ERROR_NOT_FOUND, ...)
//
//  EXTRA:
//      This function WON'T return back
//
static __declspec(noreturn) void exitFunction::messageBox(
    _In_ void* const vpInstDLL,
    _In_z_ const char* const cstrMessage,
    _In_ const DWORD dwExitCode
) noexcept
{
    // Ensure that the error message is a valid string to prevent
    // crashes
    assert(cstrMessage);

    // Display a messagebox with the error message
    MessageBoxA(
        nullptr,
        cstrMessage,
        "Error",
        (MB_ICONERROR | MB_OK)
    );

    // Pass the exit code and uninject the dll
    FreeLibraryAndExitThread(static_cast<const HMODULE>(vpInstDLL), dwExitCode);
}

//
//  FUNCTION: exitFunction::popupMessage(_In_ void* const, _In_z_ const char* const, _In_ const DWORD)
//
//  PURPOSE: Display a popup message in assault cube and uninject the dll.
//
//  PARAMETERS:
//    vpInstDLL   - instance to the current DLL
//    cstrMessage - error message to be displayed
//    dwExitCode  - exit code (e.g. ERROR_SUCCESS, ERROR_NOT_ENOUGH_MEMORY, ERROR_NOT_FOUND, ...)
//
//  EXTRA:
//      This function WON'T return back
//
static __declspec(noreturn) void exitFunction::popupMessage(
    _In_ void* const vpInstDLL,
    _In_z_ const char* const cstrMessage,
    _In_ const DWORD dwExitCode
) noexcept
{
    // Ensure that the error message is a valid string to prevent
    // crashes
    assert(cstrMessage && cstrMessage[NULL] != '\0');

    // Call assault cube's popup message function to display the
    // error message
    (*globals::function::pPopupMessage)(cstrMessage);

    // Pass the exit code and uninject the dll
    FreeLibraryAndExitThread(static_cast<const HMODULE>(vpInstDLL), dwExitCode);
}