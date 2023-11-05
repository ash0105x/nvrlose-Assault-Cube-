import CRenderer;

#include"win32api.h"

import CWindow;
import CTrampolineHook32;
import utils;
import globals;
import snaplines;
import ESP;
import gl;

import CTraceRay;

import CVector2;
import CVector3;

import offsets;

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

namespace modules {
    namespace combat {
        namespace aimbot {
            bool bToggle = false;
        }
    }
}

namespace globals {
    namespace menu {
        static bool bOpen = false;
    }
}

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

    const std::array<const std::tuple<const decltype(__TEXT('\0'))* const, const char* const, void** const>, TOTAL_FUNCTIONS_TO_RETRIEVE> arrFunctionsToRetrieve = std::array<const std::tuple<const decltype(__TEXT('\0'))* const, const char* const, void** const>, TOTAL_FUNCTIONS_TO_RETRIEVE> {
        std::make_tuple<const decltype(__TEXT('\0'))* const, const char* const, void** const>(
            __TEXT("SDL.dll"),
            "SDL_WM_GrabInput",
            reinterpret_cast<void** const>(&CRenderer::_p_SDL_WM_GrabInput)
        ),
        std::make_tuple<const decltype(__TEXT('\0'))* const, const char* const, void** const>(
            __TEXT("opengl32.dll"),
            "wglSwapBuffers",
            reinterpret_cast<void** const>(&CRenderer::_p_wglSwapBuffers_gateway)
        )
    };

    for (const std::tuple<const decltype(__TEXT('\0'))* const, const char* const, void** const>& refFunctionToRetrieve : arrFunctionsToRetrieve) {
        typedef enum : std::uint8_t {
            INDEX_MODULE_NAME = NULL,
            INDEX_FUNCTION_NAME,
            INDEX_OUT_FUNCTION_ADDRESS
        }INDEX;

        const decltype(__TEXT('\0'))* const& pRefModuleName = std::get<INDEX::INDEX_MODULE_NAME>(refFunctionToRetrieve);

        const HMODULE hModule = GetModuleHandle(pRefModuleName);

        constexpr const bool bModuleNameIsCharString = std::_Is_character<std::remove_const<std::remove_pointer<std::remove_reference<decltype(pRefModuleName)>::type>::type>::type>::value;

        constexpr const char* const cstrModuleNameFormat = bModuleNameIsCharString ? "%s -> 0x%p" : "%ls -> 0x%p";

        (*globals::function::pointer::pPopupMessage)(
            cstrModuleNameFormat, pRefModuleName, hModule
        );

        if (!hModule) {
            return;
        }

        const char* const& cstrRefFunctionName = std::get<INDEX::INDEX_FUNCTION_NAME>(refFunctionToRetrieve);

        void*& vpRefFunctionAddress = *std::get<INDEX::INDEX_OUT_FUNCTION_ADDRESS>(refFunctionToRetrieve);

        vpRefFunctionAddress = GetProcAddress(hModule, cstrRefFunctionName);

        constexpr const char* const cstrFunctionNameFormat = bModuleNameIsCharString ? "%s - %s -> 0x%p" : "%ls - %s -> 0x%p";

        (*globals::function::pointer::pPopupMessage)(
            cstrFunctionNameFormat, pRefModuleName, cstrRefFunctionName, vpRefFunctionAddress
        );

        if (!vpRefFunctionAddress) {
            return;
        }
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
    gl::setupOrtho();

    ImGui_ImplWin32_NewFrame();
    ImGui_ImplOpenGL2_NewFrame();
    ImGui::NewFrame();
}

void CRenderer::end( void ) noexcept {
    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
}

LRESULT CALLBACK CRenderer::hk_WndProc(
    _In_ const HWND hWnd,
    _In_ const UINT msg,
    _In_ const WPARAM wParam,
    _In_ const LPARAM lParam
) noexcept
{
    if (msg == WM_KEYDOWN) {
        if (wParam == VK_INSERT) {
            globals::menu::bOpen ^= true;
        }
        else if (wParam == VK_ESCAPE) {
            globals::menu::bOpen = false;
        }
    }

    (*CRenderer::_p_SDL_WM_GrabInput)(static_cast<const CRenderer::SDL_GrabMode>(static_cast<const std::uint8_t>(globals::menu::bOpen) + 1u));

    if (!globals::menu::bOpen) {
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

        return (
            (CRenderer::_bImGuiWin32Initialized = ImGui_ImplWin32_Init(static_cast<const HWND>(CRenderer::_AssaultCubeWindow))) &&
            (CRenderer::_bImGuiOpenGLInitialized = ImGui_ImplOpenGL2_Init()) &&
            CRenderer::_AssaultCubeWindow.changeWndProc(&CRenderer::hk_WndProc) ?
            true :
            bOnFailure()
        );
    }();

    if (!bInitImGuiOnce) {
        return (*CRenderer::_p_wglSwapBuffers_gateway)(hDC);
    }

    globals::entity::pEntityList = *reinterpret_cast<const std::array<const CPlayer* const, globals::entity::MAX_ENTITIES>* const* const>(globals::modules::ac_client_exe + offsets::ac_client_exe::pointer::ENTITY_LIST);

    CRenderer::begin();

    if (globals::menu::bOpen) {
        static const bool bCenterWindowOnce = [](void) noexcept -> bool {
            const ImVec2 vec2WindowSize = ImGui::GetWindowSize();

            ImGui::SetNextWindowPos(
                ImVec2{
                    static_cast<const float>((globals::screen::viewPort[VIEW_PORT_ELEMENT::VIEW_PORT_ELEMENT_WIDTH] - static_cast<const std::uint32_t>(vec2WindowSize.x)) / 2u),
                    static_cast<const float>((globals::screen::viewPort[VIEW_PORT_ELEMENT::VIEW_PORT_ELEMENT_HEIGHT] - static_cast<const std::uint32_t>(vec2WindowSize.y)) / 2u)
                }
            );

            return false;
        }();

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
            ImGui::Checkbox("Aimbot", &modules::combat::aimbot::bToggle);
            ImGui::Checkbox("Snaplines", &modules::visuals::snaplines::bToggle);
            ImGui::SliderFloat("Width", &modules::visuals::snaplines::fWidth, 0.1f, 10.f);
            ImGui::Checkbox("ESP", &modules::visuals::ESP::bToggle);
        }
        ImGui::End();
    }
    
    CRenderer::end();

    if (!globals::entity::pLocalPlayer) {
        return (*CRenderer::_p_wglSwapBuffers_gateway)(hDC);
    }

    float fPlayerDistanceToLocalPlayer = std::numeric_limits<const float>::max();

    CVector3 vec3Delta = CVector3{ };

    const std::uint32_t uHalfScreenWidth = globals::screen::viewPort[VIEW_PORT_ELEMENT::VIEW_PORT_ELEMENT_WIDTH] / 2u;

    for (std::uint8_t i = globals::entity::FIRST_ENTITY_INDEX; i < *globals::match::bypPlayerInGame; ++i) {
        const CPlayer& refCurrentPlayer = *((*globals::entity::pEntityList)[i]);

        if (!refCurrentPlayer.iHealth) {
            continue;
        }

        if (modules::visuals::ESP::bToggle || modules::visuals::snaplines::bToggle) {
            CVector2 vec2TargetOriginScreenPosition = CVector2{ };
            if (utils::math::worldToScreen(refCurrentPlayer.vec3Origin, vec2TargetOriginScreenPosition)) {

                constexpr const GLubyte arrTeamColor[4u] = { 127u, 255u, 212u, 255u };
                constexpr const GLubyte arrEnemyColor[4u] = { 123u, 104u, 238u, 255u };

                const GLubyte (&arrColor)[4u] = (
                    refCurrentPlayer.uTeamID == globals::entity::pLocalPlayer->uTeamID ?
                    arrTeamColor :
                    arrEnemyColor
                );
            
                if (modules::visuals::snaplines::bToggle) {
                    gl::drawLineRGBA(
                        CVector2{
                            static_cast<const float>(uHalfScreenWidth),
                            -20.f
                        },
                        vec2TargetOriginScreenPosition,
                        arrColor,
                        0.1f
                    );
                }
                if (modules::visuals::ESP::bToggle) {
                    CVector2 vec2TargetEyeScreenPosition = CVector2{ };

                    if (utils::math::worldToScreen(refCurrentPlayer.vec3EyePosition, vec2TargetEyeScreenPosition)) {
                        const std::uint32_t uAdjustedWidth = 300u / static_cast<const std::uint32_t>(globals::entity::pLocalPlayer->vec3EyePosition.distance(refCurrentPlayer.vec3EyePosition));

                        const std::uint32_t xPosition = static_cast<const std::uint32_t>(vec2TargetEyeScreenPosition.x) - uAdjustedWidth;
                        const std::uint32_t yPosition = static_cast<const std::uint32_t>(vec2TargetEyeScreenPosition.y) + uAdjustedWidth;

                        glLineWidth(0.1f);
                        glColor4ub(
                            arrColor[NULL],
                            arrColor[1u],
                            arrColor[2u],
                            arrColor[3u]
                        );

                        glBegin(GL_LINE_LOOP);

                        glVertex2i(
                            xPosition,
                            yPosition
                        );

                        glVertex2i(
                            xPosition + (uAdjustedWidth * 2u),
                            yPosition
                        );

                        glVertex2i(
                            xPosition + (uAdjustedWidth * 2u),
                            static_cast<const std::uint32_t>(vec2TargetOriginScreenPosition.y)
                        );

                        glVertex2i(
                            xPosition,
                            static_cast<const std::uint32_t>(vec2TargetOriginScreenPosition.y)
                        );

                        glEnd();
                    }
                }
            }
        }

        if (
            !modules::combat::aimbot::bToggle ||
            !globals::entity::pLocalPlayer->iHealth ||
            refCurrentPlayer.uTeamID == globals::entity::pLocalPlayer->uTeamID
        )
        {
            continue;
        }

        CTraceRay traceResult = CTraceRay{ };

        traceResult.traceLine(
            globals::entity::pLocalPlayer->vec3EyePosition,
            refCurrentPlayer.vec3EyePosition,
            globals::entity::pLocalPlayer,
            false,
            false
        );

        if (traceResult.bCollided) {
            continue;
        }

        vec3Delta = refCurrentPlayer.vec3EyePosition - globals::entity::pLocalPlayer->vec3EyePosition;

        if (
            const float fCurrentPlayerDistanceToLocalPlayer = vec3Delta.length();
            fCurrentPlayerDistanceToLocalPlayer < fPlayerDistanceToLocalPlayer
        )
        {
            fPlayerDistanceToLocalPlayer = fCurrentPlayerDistanceToLocalPlayer;
        }
    }

    if (std::numeric_limits<float>::max() != fPlayerDistanceToLocalPlayer) {
        globals::entity::pLocalPlayer->vec2ViewAngles = CVector2{
            // opp. / adj.
            -atan2f(vec3Delta.x, vec3Delta.y) * CVector3::fDegreesRadiansConversionValue + 180.f,
            // opp. / hyp.
            (vec3Delta.z / fPlayerDistanceToLocalPlayer) * CVector3::fDegreesRadiansConversionValue
        };
    }

    /*CTraceRay traceResult = CTraceRay{ };

    traceResult.traceLine(
        globals::entity::pLocalPlayer->vec3EyePosition,
        globals::entity::pLocalPlayer->vec3EyePosition +
        CVector3::sphericalToCartesian(globals::entity::pLocalPlayer->vec2ViewAngles).scale(100.f),
        globals::entity::pLocalPlayer,
        false,
        false
    );

    globals::entity::pLocalPlayer->bIsShooting = !traceResult.bCollided;*/

	return (*CRenderer::_p_wglSwapBuffers_gateway)(hDC);
}