import CMenu;

import ESP;
import snaplines;
import globals;
import aimbot;
import CWindow;

import<filesystem>;

#include<Windows.h>

#include<gl/GL.h>
#pragma comment(lib, "opengl32.lib")

#include"../external/ImGui/imgui.h"
#include"../external/ImGui/imgui_impl_win32.h"
#include"../external/ImGui/imgui_impl_opengl2.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

[[nodiscard]]
CMenu::CMenu(
    _In_z_ const char* const cstrName
) noexcept
{
    if (
        CMenu::_AssaultCubeWindow = CWindow{ CWindow::getCurrentWindow() };
        !static_cast<HWND>(CMenu::_AssaultCubeWindow)
    )
    {
        return;
    }

    CMenu::_AssaultCubeWindow.setTitleA(cstrName);

    if (
        const HMODULE hSDL = GetModuleHandle(__TEXT("SDL.dll"))
    )
    {
        this->m_bOk = CMenu::_p_SDL_WM_GrabInput = reinterpret_cast<const CMenu::_SDL_WM_GrabInput_t>(GetProcAddress(hSDL, "SDL_WM_GrabInput"));
    }
}

CMenu::~CMenu( void ) noexcept {
    if (
        const WNDPROC& refOriginalWndProc = CMenu::_AssaultCubeWindow.getOriginalWndProc();
        refOriginalWndProc
    )
    {
        CMenu::_AssaultCubeWindow.restoreOriginalWndProc();
        ImGui_ImplOpenGL2_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }
    else if (CMenu::_bImGuiOpenGLInitialized) {
        ImGui_ImplOpenGL2_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }
    else if (CMenu::_bImGuiWin32Initialized) {
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }
    else if (ImGui::GetCurrentContext()) {
        ImGui::DestroyContext();
    }
}

const bool& CMenu::ok( void ) const noexcept {
    return this->m_bOk;
}

LRESULT CALLBACK CMenu::hk_WndProc(
    _In_ const HWND hWnd,
    _In_ const UINT msg,
    _In_ const WPARAM wParam,
    _In_ const LPARAM lParam
) noexcept
{
    if (msg == WM_KEYDOWN) {
        switch (wParam) {
        case VK_INSERT:
            CMenu::_bOpen ^= true;
            break;
        case VK_ESCAPE:
            if (CMenu::_bOpen) {
                CMenu::_bOpen = false;
                return TRUE;
            }
            CMenu::_bOpen = false;
            break;
        case VK_END:
            globals::dll::bUnload = true;
            break;
        default:
            break;
        }
    }

    (*CMenu::_p_SDL_WM_GrabInput)(static_cast<const CMenu::SDL_GrabMode>(static_cast<const std::uint8_t>(CMenu::_bOpen) + 1u));

    if (!CMenu::_bOpen) {
        return(
            CallWindowProc(
                CMenu::_AssaultCubeWindow.getOriginalWndProc(),
                hWnd,
                msg,
                wParam,
                lParam
            )
        );
    }

    ImGui_ImplWin32_WndProcHandler(
        hWnd,
        msg,
        wParam,
        lParam
    );

    return TRUE;
}

void CMenu::drawMain( void ) noexcept {
    constexpr const char* const CLIENT_NAME = (
        "nvrlose client"
#ifdef _DEBUG
        " (debug)"
#endif // _DEBUG
    );

    if (!ImGui::Begin(CLIENT_NAME)) {
        ImGui::End();
        return;
    }

    ImGui::Checkbox("Aimbot", &modules::combat::aimbot::bToggle);
    ImGui::SliderFloat("Aimbot FOV", &modules::combat::aimbot::fFOV, 5.f, 360.f);
    ImGui::Checkbox("Ignore FOV", &modules::combat::aimbot::bIgnoreFOV);
    ImGui::Checkbox("Draw FOV", &modules::combat::aimbot::bDrawFOV);
    ImGui::Checkbox("Snaplines", &modules::visuals::snaplines::bToggle);
    ImGui::SliderFloat("Snaplines distance", &modules::visuals::snaplines::fDistance, modules::visuals::snaplines::MIN_DISTANCE, modules::visuals::snaplines::MAX_DISTANCE);
    ImGui::Checkbox("ESP", &modules::visuals::ESP::bToggle);
    ImGui::SliderFloat("ESP distance", &modules::visuals::ESP::fDistance, modules::visuals::ESP::MIN_DISTANCE, modules::visuals::ESP::MAX_DISTANCE);
    ImGui::SliderFloat("Damage multiplicator", &globals::fDamageMultiplicator, 0.f, 5.f);

    ImGui::End();
}

void CMenu::begin( void ) noexcept {
    ImGui_ImplWin32_NewFrame();
    ImGui_ImplOpenGL2_NewFrame();
    ImGui::NewFrame();
}

void CMenu::end( void ) noexcept {
    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
}

bool CMenu::initialize( void ) noexcept {
    IMGUI_CHECKVERSION();

    if (!ImGui::CreateContext()) {
        return false;
    }

    [[maybe_unused]] const ImGuiIO& refIO = ImGui::GetIO();

    return (
        (CMenu::_bImGuiWin32Initialized = ImGui_ImplWin32_Init(static_cast<const HWND>(CMenu::_AssaultCubeWindow))) &&
        (CMenu::_bImGuiOpenGLInitialized = ImGui_ImplOpenGL2_Init()) &&
        CMenu::_AssaultCubeWindow.setWndProc(&CMenu::hk_WndProc)
    );
}