import CRenderer;

import CTrampolineHook32;
import utils;
import globals;
import snaplines;
import ESP;
import gl;
import CMenu;
import aimbot;
import CFont;
import playerent;
import CTraceRay;
import CVector2;
import CVector3;

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

	constexpr const std::uint8_t WGL_SWAP_BUFFERS_HOOK_LENGTH = 5u;

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
    static const bool bInitOnce = []( void ) noexcept -> bool {
        const bool bReturnValue = CMenu::initialize();

        if (!bReturnValue) {
            (*globals::function::pPopupMessage)("Failed to initialize renderings");
        }

        return bReturnValue;
    }();

    if (!bInitOnce) {
        globals::dll::bUnload = true;
        return (*CRenderer::_p_wglSwapBuffers_gateway)(hDC);
    }

    gl::setupOrtho();

    constexpr const std::uint8_t ARIAL_FONT_HEIGHT = 12;

    static gl::CFont arial = gl::CFont{ __TEXT("arial"), ARIAL_FONT_HEIGHT, hDC };

    constexpr const GLubyte ARR_WHITE[4u] = { 255, 255, 255, 255 };

    CVector2 vec2TopLeftCorner = CVector2{
         0.f,
         static_cast<const float>(globals::screen::viewPort[VIEW_PORT_ELEMENT::VIEW_PORT_ELEMENT_HEIGHT] - ARIAL_FONT_HEIGHT)
    };

    arial.drawf(
        vec2TopLeftCorner,
        ARR_WHITE,
        "Thread Id: %d (0x%xl)",
        globals::thread::dwId, globals::thread::dwId
    );

    /*vec2TopLeftCorner.y -= ARIAL_FONT_HEIGHT;

    arial.drawf(
        vec2TopLeftCorner,
        ARR_WHITE,
        "FPS: %f",
        *globals::screen::fpFPS
    );*/

    vec2TopLeftCorner.y -= ARIAL_FONT_HEIGHT;

    arial.drawf(
        vec2TopLeftCorner,
        ARR_WHITE,
        "Map: %s",
        reinterpret_cast<char*>(&globals::match::cstrPlayingMap)
    );

    if (!CRenderer::_bCanSafelyRender) {
        constexpr const char cstrInitializingText[] = "Initializing...";

        arial.draw(
            CVector2{
                0.f,
                static_cast<const float>(globals::screen::viewPort[VIEW_PORT_ELEMENT::VIEW_PORT_ELEMENT_HEIGHT] - (ARIAL_FONT_HEIGHT * 2) - 3)
            },
            ARR_WHITE,
            cstrInitializingText,
            sizeof(cstrInitializingText)
        );

        return (*CRenderer::_p_wglSwapBuffers_gateway)(hDC);
    }

    const auto renderMenu = []( void ) noexcept -> void {
        gl::restoreOrtho();

        CMenu::begin();
        if (CMenu::_bOpen) {
            CMenu::drawMain();
        }
        CMenu::end();
    };

    if (!globals::entity::pLocalPlayer || globals::entity::pLocalPlayer->iHealth <= NULL) {
        renderMenu();
        return (*CRenderer::_p_wglSwapBuffers_gateway)(hDC);
    }

    const std::chrono::steady_clock::time_point currentTimePoint = std::chrono::steady_clock::now();

    const CVector2 vec2MiddleScreen = CVector2{
        static_cast<const float>(globals::screen::viewPort[VIEW_PORT_ELEMENT::VIEW_PORT_ELEMENT_WIDTH]) / 2.f,
        static_cast<const float>(globals::screen::viewPort[VIEW_PORT_ELEMENT::VIEW_PORT_ELEMENT_HEIGHT]) / 2.f
    };

    if (std::chrono::duration_cast<std::chrono::milliseconds>(currentTimePoint - globals::indicator::lastHitTimer).count() <= 250) {
        constexpr const float LINE_LENGTH = 7.f;
        constexpr const float MIDDLE_PADDING = 5.f;
        constexpr const float HITMARKER_LINE_WIDTH = 0.5f;

        for (std::uint32_t i = 1; i < 3; ++i) {
            const float fAdjustedAngle = static_cast<const float>(i * 45);

            const float fCosAngle = cosf(fAdjustedAngle);
            const float fSinAngle = sinf(fAdjustedAngle);

            CVector2 vec2HitmarkerStartPosition = CVector2{
                vec2MiddleScreen.x - fCosAngle * MIDDLE_PADDING,
                vec2MiddleScreen.y + fSinAngle * MIDDLE_PADDING
            };

            CVector2 vec2HitmarkerEndPosition = CVector2{
                vec2HitmarkerStartPosition.x - fCosAngle * LINE_LENGTH,
                vec2HitmarkerStartPosition.y + fSinAngle * LINE_LENGTH
            };

            gl::drawLineRGBA(
                vec2HitmarkerStartPosition,
                vec2HitmarkerEndPosition,
                ARR_WHITE,
                HITMARKER_LINE_WIDTH
            );

            vec2HitmarkerStartPosition.y = vec2MiddleScreen.y - fSinAngle * MIDDLE_PADDING;
            vec2HitmarkerEndPosition.y = vec2HitmarkerStartPosition.y - fSinAngle * LINE_LENGTH;

            gl::drawLineRGBA(
                vec2HitmarkerStartPosition,
                vec2HitmarkerEndPosition,
                ARR_WHITE,
                HITMARKER_LINE_WIDTH
            );
        }
    }

    for (std::size_t i = NULL; i < globals::indicator::vecEntitiesHit.size(); ++i) {
        typedef enum : std::uint8_t {
            ENTITY_HIT_INDEX_NICK_NAME = NULL,
            ENTITY_HIT_INDEX_DAMAGE_GIVEN,
            ENTITY_HIT_INDEX_HEALTH_REMAINING,
            ENTITY_HIT_INDEX_TIME_STAMP
        }ENTITY_HIT_INDEX;
        
        const std::tuple<std::string, std::uint32_t, std::int32_t, std::chrono::steady_clock::time_point>& refEntityHit = globals::indicator::vecEntitiesHit[i];

        if (std::chrono::duration_cast<std::chrono::seconds>(currentTimePoint - std::get<ENTITY_HIT_INDEX::ENTITY_HIT_INDEX_TIME_STAMP>(refEntityHit)).count() >= 5) {
            globals::indicator::vecEntitiesHit.erase(globals::indicator::vecEntitiesHit.begin() + i);
            continue;
        }

        constexpr const std::uint8_t MAX_VISIBLE_HIT_INDICATOR_TEXT = 10;

        arial.drawCenteredf(
            CVector2{
                0.f,
                static_cast<const float>(MAX_VISIBLE_HIT_INDICATOR_TEXT * ARIAL_FONT_HEIGHT - (i * ARIAL_FONT_HEIGHT))
            },
            static_cast<const float>(globals::screen::viewPort[VIEW_PORT_ELEMENT::VIEW_PORT_ELEMENT_WIDTH]),
            ARR_WHITE,
            "Hit %s for %d damage (%d health remaining)",
            std::get<ENTITY_HIT_INDEX::ENTITY_HIT_INDEX_NICK_NAME>(refEntityHit).c_str(),
            std::get<ENTITY_HIT_INDEX::ENTITY_HIT_INDEX_DAMAGE_GIVEN>(refEntityHit),
            std::get<ENTITY_HIT_INDEX::ENTITY_HIT_INDEX_HEALTH_REMAINING>(refEntityHit)
        );
    }

    constexpr const float FLOAT_MAX_VALUE = std::numeric_limits<const float>::max();

    float fPlayerDistanceToLocalPlayer = FLOAT_MAX_VALUE;
    float fAimbotFOV = FLOAT_MAX_VALUE;
    CVector3 vec3Delta = CVector3{ };

    for (std::uint8_t i = globals::entity::FIRST_ENTITY_INDEX; i < *globals::match::bypPlayerInGame; ++i) {
        const playerent& refCurrentPlayer = *((**globals::entity::ppEntityList)[i]);

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
                constexpr const GLubyte arrTeamHiddenColor[4u] = { 0xF3, 0x9F, 0x5A, 0xFF };
                constexpr const GLubyte arrTeamVisibleColor[4u] = { 0xE8, 0xBC, 0xB9, 0xFF };

                constexpr const GLubyte arrEnemyHiddenColor[4u] = { 0x45, 0x19, 0x52, 0xFF };
                constexpr const GLubyte arrEnemyVisibleColor[4u] = { 0xAE, 0x44, 0x5A, 0xFF };

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
            globals::entity::pLocalPlayer->iHealth <= NULL ||
            globals::entity::pLocalPlayer->uPlayerState == PLAYER_STATE::PLAYER_STATE_GHOST
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

        if (modules::combat::aimbot::bIgnoreFOV) {
            if (fCurrentPlayerDistanceToLocalPlayer >= fPlayerDistanceToLocalPlayer) {
                continue;
            }

            fPlayerDistanceToLocalPlayer = fCurrentPlayerDistanceToLocalPlayer;
            vec3Delta = vec3CurrentPlayerDelta;
            fAimbotFOV = 0.f;

            continue;
        }

        CVector2 vec2EnemyWorldPosition = CVector2{ };
        if (!utils::math::worldToScreen(refCurrentPlayer.vec3EyePosition, vec2EnemyWorldPosition)) {
            continue;
        }

        const float fCurrentPlayerAimbotFOV = vec2MiddleScreen.distance(vec2EnemyWorldPosition);

        if (fCurrentPlayerAimbotFOV > modules::combat::aimbot::fFOV || fCurrentPlayerAimbotFOV > fAimbotFOV) {
            continue;
        }

        fAimbotFOV = fCurrentPlayerAimbotFOV;
        vec3Delta = vec3CurrentPlayerDelta;
    }

    if (fAimbotFOV != FLOAT_MAX_VALUE) {
        globals::entity::pLocalPlayer->vec2ViewAngles = (
            CVector2{
                // opp. / adj.
                atan2f(vec3Delta.y, vec3Delta.x) * CVector3::fDegreesRadiansConversionValue + 90.f,
                // opp. / hyp.
                asinf(vec3Delta.z / (FLOAT_MAX_VALUE == fPlayerDistanceToLocalPlayer ? vec3Delta.length() : fPlayerDistanceToLocalPlayer)) * CVector3::fDegreesRadiansConversionValue
            }
        );
    }

    if (modules::combat::aimbot::bToggle && modules::combat::aimbot::bDrawFOV && !modules::combat::aimbot::bIgnoreFOV) {
        constexpr const GLubyte ARR_RED[4u] = { 255, NULL, NULL, 255 };

        gl::drawCircle(
            vec2MiddleScreen,
            static_cast<const std::uint32_t>(modules::combat::aimbot::fFOV),
            40,
            ARR_RED,
            0.1f
        );
    }

    renderMenu();

	return (*CRenderer::_p_wglSwapBuffers_gateway)(hDC);
}