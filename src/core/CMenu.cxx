import CMenu;

import ESP;
import snaplines;
import globals;
import aimbot;
import CWindow;
import utils;

import<filesystem>;

#include<Windows.h>

#include<memory>

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
        !static_cast<const HWND>(CMenu::_AssaultCubeWindow)
    )
    {
        (*globals::function::pointer::pPopupMessage)("Failed to retrieve the window by Assault Cube");
        return;
    }

    if (!(CMenu::_p_SDL_WM_GrabInput = static_cast<const CMenu::_SDL_WM_GrabInput_t>(utils::module::retrieveFunction(__TEXT("SDL.dll"), "SDL_WM_GrabInput")))) {
        (*globals::function::pointer::pPopupMessage)("Failed to retrieve function SDL.dll - SDL_WM_GrabInput");
        return;
    }

    char* cstrAppdataPath = nullptr;

    if (
        // Duplicate the AppData environment variable securely
        std::size_t bufferCount = NULL;
        _dupenv_s(
            &cstrAppdataPath,
            &bufferCount,
            "APPDATA"
        ) ||
        !cstrAppdataPath
    )
    {
        return;
    }

    this->m_filePath = std::filesystem::path{ cstrAppdataPath };
    this->m_filePath.append(cstrName);

    const auto tryCreatingDirectory = [this](const std::filesystem::path& path) noexcept -> bool {
        return(
            (
                std::filesystem::exists(path) &&
                std::filesystem::is_directory(path)
            ) ||
            std::filesystem::create_directory(this->m_filePath)
        );
    };

    if (tryCreatingDirectory(this->m_filePath)) {
        this->m_bOk = true;
        free(cstrAppdataPath);
        return;
    }

    for (std::uint8_t i = NULL; i < UCHAR_MAX; ++i) {
        const std::filesystem::path newPathAttempt = this->m_filePath / (" (" + std::to_string(i) + ')');

        if (!tryCreatingDirectory(newPathAttempt)) {
            continue;
        }

        this->m_filePath = newPathAttempt;
        this->m_bOk = true;
        break;
    }

    free(cstrAppdataPath);
}

CMenu::~CMenu(void) noexcept {
    if (
        const WNDPROC& refOriginalWndProc = CMenu::_AssaultCubeWindow.getOriginalWndProc();
        refOriginalWndProc
    )
    {
        CMenu::_AssaultCubeWindow.changeWndProc(refOriginalWndProc);
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

const bool& CMenu::ok(void) const noexcept {
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
        if (wParam == VK_INSERT) {
            CMenu::_bOpen ^= true;
        }
        else if (wParam == VK_ESCAPE) {
            if (CMenu::_bOpen) {
                CMenu::_bOpen = false;
                return TRUE;
            }
            CMenu::_bOpen = false;
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

void CMenu::drawMain(void) noexcept {
    constexpr char cstrClientName[] = (
        "nvrlose client"
#ifdef _DEBUG
        " (dev build)"
#endif // _DEBUG
    );

    if (!ImGui::Begin(cstrClientName, nullptr, NULL)) {
        ImGui::End();
        return;
    }

    ImGui::Checkbox("Aimbot", &modules::combat::aimbot::bToggle);
    ImGui::Checkbox("Snaplines", &modules::visuals::snaplines::bToggle);
    ImGui::SliderFloat("Snaplines distance", &modules::visuals::snaplines::fDistance, 1.f, 300.f);
    ImGui::SliderFloat("Width", &modules::visuals::snaplines::fWidth, 0.1f, 10.f);
    ImGui::Checkbox("ESP", &modules::visuals::ESP::bToggle);
    ImGui::SliderFloat("ESP distance", &modules::visuals::ESP::fDistance, 1.f, 300.f);

    ImGui::End();
}

void CMenu::begin(void) noexcept {
    ImGui_ImplWin32_NewFrame();
    ImGui_ImplOpenGL2_NewFrame();
    ImGui::NewFrame();
}

void CMenu::end(void) noexcept {
    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
}

bool CMenu::initialize(void) noexcept {
    const auto bOnFailure = [](void) noexcept -> bool {
        (*globals::function::pointer::pPopupMessage)("Failed to initialize ImGui");

        return false;
    };

    IMGUI_CHECKVERSION();

    if (!ImGui::CreateContext()) {
        return bOnFailure();
    }

    [[maybe_unused]] const ImGuiIO& refIO = ImGui::GetIO();

    return (
        (
            (CMenu::_bImGuiWin32Initialized = ImGui_ImplWin32_Init(static_cast<const HWND>(CMenu::_AssaultCubeWindow))) &&
            (CMenu::_bImGuiOpenGLInitialized = ImGui_ImplOpenGL2_Init()) &&
            CMenu::_AssaultCubeWindow.changeWndProc(&CMenu::hk_WndProc)
        ) ||
        bOnFailure()
    );
}