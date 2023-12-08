import CRenderer;
import CWindow;
import CTrampolineHook32;
import utils;
import globals;
import snaplines;
import ESP;
import gl;
import CMenu;
import aimbot;
import CFont;

import CTraceRay;

import CVector2;
import CVector3;

import offsets;

import<array>;

#define NOMINMAX
#include<Windows.h>

#include<gl/GL.h>
#pragma comment(lib, "opengl32.lib")

#include"../external/ImGui/imgui_impl_win32.h"
#include"../external/ImGui/imgui_impl_opengl2.h"

#include<iostream>
#include<string>
#include<tuple>
#include<chrono>

#define _USE_MATH_DEFINES
#include<math.h>

// std::numeric_limits
#include<limits>

[[nodiscard]] CRenderer::CRenderer(
    _In_z_ const char* const cstrName
) noexcept
    : m_menu(CMenu{ cstrName })
{
    if (!this->m_menu.ok()) {
        return;
    }
    
    if (
        const HMODULE hOpenGL32 = GetModuleHandle(__TEXT("opengl32.dll"));
        !hOpenGL32 ||
        !(CRenderer::_p_wglSwapBuffers_gateway = reinterpret_cast<const CRenderer::_wglSwapBuffers_t>(GetProcAddress(hOpenGL32, "wglSwapBuffers")))
    )
    {
        return;
    }

	constexpr std::uint8_t WGL_SWAP_BUFFERS_HOOK_LENGTH = 5u;

	this->m_wglSwapBuffersTrampolineHook32 = CTrampolineHook32{ CRenderer::_p_wglSwapBuffers_gateway, WGL_SWAP_BUFFERS_HOOK_LENGTH };

    CRenderer::_p_wglSwapBuffers_gateway = reinterpret_cast<const CRenderer::_wglSwapBuffers_t>(this->m_wglSwapBuffersTrampolineHook32.attach(&CRenderer::hk_wglSwapBuffers));

    this->m_bOk = CRenderer::_p_wglSwapBuffers_gateway;
}

CRenderer::~CRenderer( void ) noexcept {
	this->m_wglSwapBuffersTrampolineHook32.detach();
}

const bool& CRenderer::ok( void ) const noexcept {
    return this->m_bOk;
}

BOOL WINAPI CRenderer::hk_wglSwapBuffers(
	_In_ const HDC hDC
) noexcept
{
    static const bool bInitImGuiOnce = [](void) noexcept -> bool {
        return CMenu::initialize();
    }();

    if (!bInitImGuiOnce) {
        return (*CRenderer::_p_wglSwapBuffers_gateway)(hDC);
    }

    class AutoOrtho final {
    public:
        [[nodiscard]] explicit AutoOrtho(void) noexcept {
            gl::setupOrtho();
        }
        ~AutoOrtho(void) noexcept {
            gl::restoreOrtho();
        }
    }autoOrtho;

    constexpr std::uint8_t ARIAL_FONT_HEIGHT = 12;

    static gl::CFont arial = gl::CFont{ __TEXT("arial"), ARIAL_FONT_HEIGHT, hDC };

    constexpr GLubyte white[4u] = { 255, 255, 255, 255 };

    if (!CRenderer::_bCanSafelyRender) {
        constexpr const char cstrInitializingText[] = "Initializing...";

        arial.draw(
            CVector2{
                0.f,
                static_cast<const float>(globals::screen::viewPort[VIEW_PORT_ELEMENT::VIEW_PORT_ELEMENT_HEIGHT] - (ARIAL_FONT_HEIGHT * 2) - 3)
            },
            white,
            cstrInitializingText,
            sizeof(cstrInitializingText)
        );

        return (*CRenderer::_p_wglSwapBuffers_gateway)(hDC);
    }

    globals::entity::pEntityList = *reinterpret_cast<const std::array<const playerent* const, globals::entity::MAX_ENTITIES>* const* const>(globals::modules::ac_client_exe + offsets::ac_client_exe::pointer::ENTITY_LIST);

    arial.drawf(
        CVector2{
            0.f,
            static_cast<const float>(globals::screen::viewPort[VIEW_PORT_ELEMENT::VIEW_PORT_ELEMENT_HEIGHT] - ARIAL_FONT_HEIGHT)
        },
        white,
        "Thread Id: %d (0x%xl)",
        globals::thread::dwId,
        globals::thread::dwId
    );

    const auto renderMenu = [](void) noexcept -> void {
        CMenu::begin();
        if (CMenu::_bOpen) {
            CMenu::drawMain();
        }
        CMenu::end();
    };

    if (!globals::entity::pLocalPlayer) {
        renderMenu();
        return (*CRenderer::_p_wglSwapBuffers_gateway)(hDC);
    }

    const std::chrono::steady_clock::time_point currentTimePoint = std::chrono::steady_clock::now();

    for (std::size_t i = NULL; i < globals::vecLocalPlayerShotPositions.size(); ++i) {
        typedef enum _SHOT_POSITION_INDEX : std::uint8_t {
            SHOT_POSITION_INDEX_VEC3_START,
            SHOT_POSITION_INDEX_VEC3_END,
            SHOT_POSITION_INDEX_TIME_STAMP
        }SHOT_POSITION_INDEX;

        const std::tuple<CVector3, CVector3, std::chrono::steady_clock::time_point>& localPlayerShotPosition = globals::vecLocalPlayerShotPositions[i];

        if (std::chrono::duration_cast<std::chrono::seconds>(currentTimePoint - std::get<SHOT_POSITION_INDEX::SHOT_POSITION_INDEX_TIME_STAMP>(localPlayerShotPosition)).count() >= 5) {
            globals::vecLocalPlayerShotPositions.erase(globals::vecLocalPlayerShotPositions.begin() + i);
            continue;
        }

        CVector2 vec2ShotStartScreenPosition = CVector2{ };
        utils::math::worldToScreen(std::get<SHOT_POSITION_INDEX::SHOT_POSITION_INDEX_VEC3_START>(localPlayerShotPosition), vec2ShotStartScreenPosition);

        CVector2 vec2ShotEndScreenPosition = CVector2{ };
        utils::math::worldToScreen(std::get<SHOT_POSITION_INDEX::SHOT_POSITION_INDEX_VEC3_END>(localPlayerShotPosition), vec2ShotEndScreenPosition);

        gl::drawLineRGBA(
            vec2ShotStartScreenPosition,
            vec2ShotEndScreenPosition,
            white,
            0.1f
        );
    }
    
    float fPlayerDistanceToLocalPlayer = std::numeric_limits<const float>::max();

    CVector3 vec3Delta = CVector3{ };

    for (std::uint8_t i = globals::entity::FIRST_ENTITY_INDEX; i < *globals::match::bypPlayerInGame; ++i) {
        const playerent& refCurrentPlayer = *((*globals::entity::pEntityList)[i]);

        if (refCurrentPlayer.iHealth <= NULL) {
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

        const CVector3 vec3CurrentPlayerDelta = refCurrentPlayer.vec3EyePosition - globals::entity::pLocalPlayer->vec3EyePosition;

        const float fCurrentPlayerDistanceToLocalPlayer = vec3CurrentPlayerDelta.length();

        if (modules::visuals::ESP::bToggle || modules::visuals::snaplines::bToggle) {
            CVector2 vec2TargetOriginScreenPosition = CVector2{ };
            if (utils::math::worldToScreen(refCurrentPlayer.vec3Origin, vec2TargetOriginScreenPosition)) {
                constexpr GLubyte arrTeamHiddenColor[4u] = { NULL, NULL, 255u, 255u };
                constexpr GLubyte arrTeamVisibleColor[4u] = { NULL, 255u, NULL, 255u };

                constexpr GLubyte arrEnemyHiddenColor[4u] = { 255, 255, 255, 255 };
                constexpr GLubyte arrEnemyVisibleColor[4u] = { 255, NULL, NULL, 255 };

                const GLubyte(&arrColor)[4u] = (
                    refCurrentPlayer.uTeamID == globals::entity::pLocalPlayer->uTeamID ?
                    traceResult.bCollided ?
                    arrTeamHiddenColor :
                    arrTeamVisibleColor :
                    traceResult.bCollided ?
                    arrEnemyHiddenColor :
                    arrEnemyVisibleColor
                );

                if (modules::visuals::snaplines::bToggle && fCurrentPlayerDistanceToLocalPlayer <= modules::visuals::snaplines::fDistance) {
                    gl::drawLineRGBA(
                        CVector2{
                            static_cast<const float>(globals::screen::viewPort[VIEW_PORT_ELEMENT::VIEW_PORT_ELEMENT_WIDTH]) / 2.f,
                            -40.f
                        },
                        vec2TargetOriginScreenPosition,
                        arrColor,
                        0.1f
                    );
                }

                if (modules::visuals::ESP::bToggle && fCurrentPlayerDistanceToLocalPlayer <= modules::visuals::ESP::fDistance) {
                    modules::visuals::ESP::onToggle(
                        refCurrentPlayer,
                        arial,
                        vec2TargetOriginScreenPosition,
                        arrColor
                    );
                }
            }
        }

        if (
            !modules::combat::aimbot::bToggle ||
            globals::entity::pLocalPlayer->iHealth <= NULL
        )
        {
            if (modules::visuals::ESP::bToggle || modules::visuals::snaplines::bToggle) {
                continue;
            }
            break;
        }

        if (
            refCurrentPlayer.uTeamID == globals::entity::pLocalPlayer->uTeamID ||
            traceResult.bCollided ||
            fCurrentPlayerDistanceToLocalPlayer >= fPlayerDistanceToLocalPlayer
        )
        {
            continue;
        }

        fPlayerDistanceToLocalPlayer = fCurrentPlayerDistanceToLocalPlayer;
        vec3Delta = vec3CurrentPlayerDelta;
    }

    if (std::numeric_limits<float>::max() != fPlayerDistanceToLocalPlayer) {
        globals::entity::pLocalPlayer->vec2ViewAngles = CVector2{
            // opp. / adj.
            atan2f(vec3Delta.y, vec3Delta.x) * CVector3::fDegreesRadiansConversionValue + 90.f,
            // opp. / hyp.
            (vec3Delta.z / fPlayerDistanceToLocalPlayer) * CVector3::fDegreesRadiansConversionValue
        };
    }

    renderMenu();

	return (*CRenderer::_p_wglSwapBuffers_gateway)(hDC);
}