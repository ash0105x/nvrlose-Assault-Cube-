import CRenderer;

import CWindow;
import CTrampolineHook32;
import utils;
import globals;

#include"win32api.h"

#include<string>

#include"../external/ImGui/imgui.h"
#include"../external/ImGui/imgui_impl_opengl3_loader.h"
#include"../external/ImGui/imgui_impl_opengl3.h"
#include"../external/ImGui/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

[[nodiscard]] CRenderer::CRenderer( void ) noexcept {
	CRenderer::_AssaultCubeWindow = CWindow{ CWindow::getCurrentWindow() };

	if (!static_cast<const HWND>(CRenderer::_AssaultCubeWindow)) {
		return;
	}

	const HMODULE hOpenGL = GetModuleHandle(__TEXT("opengl32.dll"));

    (*globals::function::pointer::pPopupMessage)("OpenGL32.dll -> 0x%p", hOpenGL);

	if (!hOpenGL) {
		return;
	}

    CRenderer::_p_wglSwapBuffers_gateway = reinterpret_cast<CRenderer::_wglSwapBuffers_t>(GetProcAddress(hOpenGL, "wglSwapBuffers"));

    (*globals::function::pointer::pPopupMessage)("OpenGL32.dll - wglSwapBuffers -> 0x%p", CRenderer::_p_wglSwapBuffers_gateway);

	if (!CRenderer::_p_wglSwapBuffers_gateway) {
		return;
	}

	constexpr const std::uint8_t u8_wglSwapBuffersHookLength = 5u;

	this->m_wglSwapBuffersTrampolineHook32 = CTrampolineHook32{ CRenderer::_p_wglSwapBuffers_gateway, u8_wglSwapBuffersHookLength };

	this->m_bOk = this->m_wglSwapBuffersTrampolineHook32(&CRenderer::hk_wglSwapBuffers);
}

CRenderer::~CRenderer( void ) noexcept {
	this->m_wglSwapBuffersTrampolineHook32.detach();

    if (
        const WNDPROC& refOriginalWndProc = CRenderer::_AssaultCubeWindow.getOriginalWndProc();
        refOriginalWndProc
    )
    {
        CRenderer::_AssaultCubeWindow.changeWndProc(refOriginalWndProc);
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }
    else if (CRenderer::_bImGuiOpenGLInitialized) {
        ImGui_ImplOpenGL3_Shutdown();
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
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
}

void CRenderer::end( void ) noexcept {
    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

LRESULT CALLBACK CRenderer::hk_WndProc(
    _In_ const HWND hWnd,
    _In_ const UINT msg,
    _In_ const WPARAM wParam,
    _In_ const LPARAM lParam
) noexcept
{
    return (
        ImGui_ImplWin32_WndProcHandler(
            hWnd,
            msg,
            wParam,
            lParam
        ) ?
        TRUE :
        CallWindowProc(
            CRenderer::_AssaultCubeWindow.getOriginalWndProc(),
            hWnd,
            msg,
            wParam,
            lParam
        )
    );
}

namespace globals {
    namespace ImGui {
        bool bInitialized = false;
        bool bTriedToInitializeAtleastOnce = false;
    }
}

BOOL __stdcall CRenderer::hk_wglSwapBuffers(
	_In_ const HDC hDC
) noexcept
{
    if (!globals::ImGui::bInitialized) {
        if (globals::ImGui::bTriedToInitializeAtleastOnce) {
            return (*CRenderer::_p_wglSwapBuffers_gateway)(hDC);
        }

        ImGui::CreateContext();

        if (!ImGui_ImplWin32_Init(static_cast<const HWND>(CRenderer::_AssaultCubeWindow))) {
            globals::ImGui::bTriedToInitializeAtleastOnce = true;

            (*globals::function::pointer::pPopupMessage)("Failed to initialize ImGui");

            return (*CRenderer::_p_wglSwapBuffers_gateway)(hDC);
        }

        CRenderer::_bImGuiWin32Initialized = true;

        if (!ImGui_ImplOpenGL3_Init(nullptr)) {
            globals::ImGui::bTriedToInitializeAtleastOnce = true;

            (*globals::function::pointer::pPopupMessage)("Failed to initialize ImGui");

            return (*CRenderer::_p_wglSwapBuffers_gateway)(hDC);
        }

        CRenderer::_bImGuiOpenGLInitialized = true;

        CRenderer::_AssaultCubeWindow.changeWndProc(&CRenderer::hk_WndProc);

        globals::ImGui::bInitialized = true;

        /*if (!(globals::ImGui::bInitialized = CRenderer::_AssaultCubeWindow.changeWndProc(&CRenderer::hk_WndProc))) {
            return (*CRenderer::_p_wglSwapBuffers_gateway)(hDC);
        }*/
    }

    if (!globals::ImGui::bInitialized) {
        return (*CRenderer::_p_wglSwapBuffers_gateway)(hDC);
    }

    CRenderer::begin();

    constexpr const char cstrClientName[] = (
        "nvrlose client "
#ifdef _DEBUG
        "(dev build) "
#endif // _DEBUG
    );

    if (
        ImGui::Begin(
            cstrClientName
        )
    )
    {
        ImGui::Text("Hello World!");
        
        static bool bIsChecked = false;

        ImGui::Checkbox("Am i checked?", &bIsChecked);

    }
    ImGui::End();

    CRenderer::end();

	return (*CRenderer::_p_wglSwapBuffers_gateway)(hDC);
}