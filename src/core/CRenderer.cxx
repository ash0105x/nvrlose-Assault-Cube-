import CRenderer;

#include"win32api.h"

import CWindow;
import CTrampolineHook32;
import utils;
import globals;
import aimbot;

import CVector2;
import CVector3;

import <array>;

#include<iostream>

#include<tuple>

#define _USE_MATH_DEFINES
#include<math.h>

// std::numeric_limits
#include<limits>

#include<gl/GL.h>
#pragma comment(lib, "opengl32.lib")

#include"../external/ImGui/imgui.h"
#include"../external/ImGui/imgui_impl_win32.h"
#include"../external/ImGui/imgui_impl_opengl2.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

[[nodiscard]] CRenderer::CRenderer(void) noexcept {
    if (
        CRenderer::_AssaultCubeWindow = CWindow{ CWindow::getCurrentWindow() };
        !static_cast<const HWND>(CRenderer::_AssaultCubeWindow)
    )
    {
        return;
    }

    constexpr const std::uint8_t TOTAL_FUNCTIONS_TO_RETRIEVE = 2u;

    std::array<std::tuple<const decltype(__TEXT(' '))* const, const char* const, void** const>, TOTAL_FUNCTIONS_TO_RETRIEVE> arrFunctionsToRetrieve = std::array<std::tuple<const decltype(__TEXT(' '))* const, const char* const, void** const>, TOTAL_FUNCTIONS_TO_RETRIEVE> {
        std::make_tuple<const decltype(__TEXT(' '))* const, const char* const, void** const>(
            __TEXT("SDL.dll"),
            "SDL_WM_GrabInput",
            reinterpret_cast<void** const>(&CRenderer::_p_SDL_WM_GrabInput)
        ),
        std::make_tuple<const decltype(__TEXT(' '))* const, const char* const, void** const>(
            __TEXT("opengl32.dll"),
            "wglSwapBuffers",
            reinterpret_cast<void** const>(&CRenderer::_p_wglSwapBuffers_gateway)
        )
    };

    for (std::tuple<const decltype(__TEXT(' '))* const, const char* const, void** const>& refFunctionToRetrieve : arrFunctionsToRetrieve) {
        typedef enum : std::uint8_t {
            INDEX_MODULE_NAME = NULL,
            INDEX_FUNCTION_NAME,
            INDEX_OUT_FUNCTION_ADDRESS
        }INDEX;

        if (
            const HMODULE hModule = GetModuleHandle(std::get<INDEX::INDEX_MODULE_NAME>(refFunctionToRetrieve));
            hModule &&
            (*std::get<INDEX::INDEX_OUT_FUNCTION_ADDRESS>(refFunctionToRetrieve) = GetProcAddress(hModule, std::get<INDEX::INDEX_FUNCTION_NAME>(refFunctionToRetrieve)))
        )
        {
            continue;
        }

        return;
    }

	constexpr const std::uint8_t WGL_SWAP_BUFFERS_HOOK_LENGTH = 5u;

	this->m_wglSwapBuffersTrampolineHook32 = CTrampolineHook32{ CRenderer::_p_wglSwapBuffers_gateway, WGL_SWAP_BUFFERS_HOOK_LENGTH };

    CRenderer::_p_wglSwapBuffers_gateway = reinterpret_cast<const CRenderer::_wglSwapBuffers_t>(this->m_wglSwapBuffersTrampolineHook32(&CRenderer::hk_wglSwapBuffers));

    this->m_bOk = CRenderer::_p_wglSwapBuffers_gateway;
}

CRenderer::~CRenderer( void ) noexcept {
	this->m_wglSwapBuffersTrampolineHook32.detach();

    if (
        const WNDPROC& refOriginalWndProc = CRenderer::_AssaultCubeWindow.getOriginalWndProc();
        refOriginalWndProc
    )
    {
        CRenderer::_AssaultCubeWindow.changeWndProc(refOriginalWndProc);
        ImGui_ImplOpenGL2_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }
    else if (CRenderer::_bImGuiOpenGLInitialized) {
        ImGui_ImplOpenGL2_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }
    else if (CRenderer::_bImGuiWin32Initialized) {
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }
    else if (ImGui::GetCurrentContext()) {
        ImGui::DestroyContext();
    }
}

const bool& CRenderer::ok( void ) const noexcept {
    return this->m_bOk;
}

void CRenderer::begin( void ) noexcept {
    ImGui_ImplWin32_NewFrame();
    ImGui_ImplOpenGL2_NewFrame();
    ImGui::NewFrame();
}

void CRenderer::end( void ) noexcept {
    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
}

namespace menu {
    static bool bOpen = false;
}

LRESULT CALLBACK CRenderer::hk_WndProc(
    _In_ const HWND hWnd,
    _In_ const UINT msg,
    _In_ const WPARAM wParam,
    _In_ const LPARAM lParam
) noexcept
{

    if (GetAsyncKeyState(VK_INSERT) & 0x01) {
        menu::bOpen ^= true;
    }

    (*CRenderer::_p_SDL_WM_GrabInput)(static_cast<const CRenderer::SDL_GrabMode>(static_cast<const std::uint8_t>(menu::bOpen) + 1u));

    if (!menu::bOpen) {
        return(
            CallWindowProc(
                CRenderer::_AssaultCubeWindow.getOriginalWndProc(),
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

BOOL WINAPI CRenderer::hk_wglSwapBuffers(
	_In_ const HDC hDC
) noexcept
{
    static const bool bInitImGuiOnce = [](void) noexcept -> bool {
        const auto bOnFailure = [](void) noexcept -> bool {
            (*globals::function::pointer::pPopupMessage)("Failed to initialize ImGui");

            return false;
        };

        IMGUI_CHECKVERSION();

        if (!ImGui::CreateContext()) {
            return bOnFailure();
        }

        [[maybe_unused]] const ImGuiIO& refIO = ImGui::GetIO();

        if (!ImGui_ImplWin32_Init(static_cast<const HWND>(CRenderer::_AssaultCubeWindow))) {
            return bOnFailure();
        }

        if (
            CRenderer::_bImGuiWin32Initialized = true;
            !ImGui_ImplOpenGL2_Init()
        )
        {
            return bOnFailure();
        }

        CRenderer::_bImGuiOpenGLInitialized = true;

        return(
            CRenderer::_AssaultCubeWindow.changeWndProc(&CRenderer::hk_WndProc) ?
            true :
            bOnFailure()
        );
    }();

    if (!bInitImGuiOnce) {
        return (*CRenderer::_p_wglSwapBuffers_gateway)(hDC);
    }

    CRenderer::begin();
    {
        if (menu::bOpen) {
            constexpr const char cstrClientName[] = (
                "nvrlose client"
    #ifdef _DEBUG
                " (dev build)"
    #endif // _DEBUG
            );

            if (
                ImGui::Begin(
                    cstrClientName,
                    nullptr,
                    NULL
                )
            )
            {
                ImGui::Checkbox("Aimbot", &modules::aimbot::bToggle);
            }
            ImGui::End();
        }
    }
    CRenderer::end();

    if (
        !globals::entity::pLocalPlayer ||
        !globals::entity::pLocalPlayer->uHealth
    )
    {
        return (*CRenderer::_p_wglSwapBuffers_gateway)(hDC);
    }

    /*globals::entity::pLocalPlayer->bIsShooting = CTraceRay::entityIsVisible(
        globals::entity::pLocalPlayer->vec3EyePosition,
        globals::entity::pLocalPlayer->vec3EyePosition +
        CVector3::sphericalToCartesian(globals::entity::pLocalPlayer->vec2ViewAngles).scale(100.f)
    );*/

    globals::entity::pLocalPlayer->uAssaultRifleAmmo = 1337u;

    if (modules::aimbot::bToggle) {
        modules::aimbot::onToggle();
    }

	return (*CRenderer::_p_wglSwapBuffers_gateway)(hDC);
}