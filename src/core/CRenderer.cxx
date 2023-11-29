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

    constexpr std::uint8_t TOTAL_FUNCTIONS_TO_RETRIEVE = 2u;

    if (!(CRenderer::_p_wglSwapBuffers_gateway = static_cast<CRenderer::_wglSwapBuffers_t>(utils::module::retrieveFunction(__TEXT("opengl32.dll"), "wglSwapBuffers")))) {
        (*globals::function::pointer::pPopupMessage)("Failed to retrieve function opengl32.dll - wglSwapBuffers");
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

    globals::entity::pEntityList = *reinterpret_cast<const std::array<const playerent* const, globals::entity::MAX_ENTITIES>* const* const>(globals::modules::ac_client_exe + offsets::ac_client_exe::pointer::ENTITY_LIST);

    gl::setupOrtho();

    constexpr std::uint8_t ARIAL_FONT_HEIGHT = 12u;

    static gl::CFont arial = gl::CFont{ "arial", ARIAL_FONT_HEIGHT,  hDC };

    constexpr GLubyte white[4u] = { 255, 255, 255, 255 };

    arial.draw(
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

        gl::restoreOrtho();
    };

    if (!globals::entity::pLocalPlayer) {
        renderMenu();
        return (*CRenderer::_p_wglSwapBuffers_gateway)(hDC);
    }
    
    float fPlayerDistanceToLocalPlayer = std::numeric_limits<const float>::max();

    CVector3 vec3Delta = CVector3{ };

    const std::uint32_t uHalfScreenWidth = globals::screen::viewPort[VIEW_PORT_ELEMENT::VIEW_PORT_ELEMENT_WIDTH] / 2u;

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
                            static_cast<const float>(uHalfScreenWidth),
                            -40.f
                        },
                        vec2TargetOriginScreenPosition,
                        arrColor,
                        0.00000000001f
                    );
                }

                if (modules::visuals::ESP::bToggle && fCurrentPlayerDistanceToLocalPlayer <= modules::visuals::ESP::fDistance) {
                    CVector2 vec2TargetEyeScreenPosition = CVector2{ };

                    if (
                        std::int32_t uAdjustedWidth = NULL;
                        utils::math::worldToScreen(refCurrentPlayer.vec3EyePosition, vec2TargetEyeScreenPosition) &&
                        (uAdjustedWidth = static_cast<const std::int32_t>(globals::entity::pLocalPlayer->vec3EyePosition.distance(refCurrentPlayer.vec3EyePosition)))
                    )
                    {
                        uAdjustedWidth = 300 / uAdjustedWidth;

                        const std::int32_t xPosition = static_cast<const std::int32_t>(vec2TargetEyeScreenPosition.x) - uAdjustedWidth;
                        const std::int32_t xPositionRight = xPosition + (uAdjustedWidth * 2);

                        const std::int32_t yPosition = static_cast<const std::int32_t>(vec2TargetEyeScreenPosition.y) + uAdjustedWidth;

                        CVector2 vec2TextInformationPosition = CVector2{
                            static_cast<const float>(xPosition),
                            static_cast<const float>(yPosition)
                        };
                            
                        arial.drawCentered(
                            vec2TextInformationPosition,
                            static_cast<const float>(xPositionRight),
                            9.f,
                            white,
                            refCurrentPlayer.cstrNickname
                        );

                        if (refCurrentPlayer.pCurrentWeapon) {
                            vec2TextInformationPosition.y = vec2TargetOriginScreenPosition.y;

                            arial.drawCentered(
                                vec2TextInformationPosition,
                                static_cast<const float>(xPositionRight),
                                9.f,
                                white,
                                refCurrentPlayer.pCurrentWeapon->cstrName
                            );

                            vec2TextInformationPosition.y = static_cast<const float>(vec2TargetOriginScreenPosition.y) - (static_cast<const float>(uAdjustedWidth) / 1.5f);

                            char cstrAmmo[10] = {  };
                            _itoa_s(*refCurrentPlayer.pCurrentWeapon->upAmmo, cstrAmmo, 10);

                            arial.drawCentered(
                                vec2TextInformationPosition,
                                static_cast<const float>(xPositionRight),
                                9.f,
                                white,
                                cstrAmmo
                            );
                        }

                        vec2TextInformationPosition = CVector2{
                            static_cast<const float>(xPosition) - (static_cast<const float>(uAdjustedWidth) / 1.5f),
                            (vec2TextInformationPosition.y + vec2TargetEyeScreenPosition.y) / 2.f
                        };

                        arial.draw(
                            vec2TextInformationPosition,
                            white,
                            std::to_string(refCurrentPlayer.iHealth).c_str()
                        );

                        // Colored esp box
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
                            xPositionRight,
                            yPosition
                        );

                        glVertex2i(
                            xPositionRight,
                            static_cast<const GLint>(vec2TargetOriginScreenPosition.y)
                        );

                        glVertex2i(
                            xPosition,
                            static_cast<const GLint>(vec2TargetOriginScreenPosition.y)
                        );

                        glEnd();

                        // Black outlined esp box
                        glLineWidth(4.f);
                        glColor4ub(
                            NULL,
                            NULL,
                            NULL,
                            NULL
                        );

                        glBegin(GL_LINE_LOOP);

                        glVertex2i(
                            xPosition,
                            yPosition
                        );

                        glVertex2i(
                            xPositionRight,
                            yPosition
                        );

                        glVertex2i(
                            xPositionRight,
                            static_cast<const GLint>(vec2TargetOriginScreenPosition.y)
                        );

                        glVertex2i(
                            xPosition,
                            static_cast<const GLint>(vec2TargetOriginScreenPosition.y)
                        );

                        glEnd();

                        // Colored esp health bar
                        glLineWidth(0.1f);
                        glBegin(GL_LINES);

                        const GLubyte playerHealthColorValue = static_cast<const GLubyte>(refCurrentPlayer.iHealth * 2.25f);

                        glColor4ub(
                            255u - playerHealthColorValue,
                            playerHealthColorValue,
                            NULL,
                            255u
                        );

                        const float fHealthBarPositionX = static_cast<const float>(xPosition) - (static_cast<const float>(uAdjustedWidth) / 1.f);

                        glVertex2f(
                            fHealthBarPositionX,
                            vec2TargetOriginScreenPosition.y
                        );

                        glVertex2f(
                            fHealthBarPositionX,
                            vec2TargetOriginScreenPosition.y - ((vec2TargetOriginScreenPosition.y - static_cast<const float>(yPosition)) * (static_cast<const float>(refCurrentPlayer.iHealth) / 100.f))
                        );

                        // grey esp health bar
                        glColor4ub(
                            128u,
                            128u,
                            128u,
                            255u
                        );

                        glVertex2f(
                            fHealthBarPositionX,
                            static_cast<const GLfloat>(yPosition)
                        );

                        glVertex2f(
                            fHealthBarPositionX,
                            vec2TargetOriginScreenPosition.y
                        );

                        glEnd();

                        constexpr GLubyte arrBlack[3u] = { NULL, NULL, NULL };

                        gl::drawLineRGB(
                            CVector2{
                                fHealthBarPositionX,
                                static_cast<const float>(yPosition) + 1.f
                            },
                            CVector2{
                                fHealthBarPositionX,
                                vec2TargetOriginScreenPosition.y - 1.f
                            },
                            arrBlack,
                            4.f
                        );
                    }
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

        if (refCurrentPlayer.uTeamID == globals::entity::pLocalPlayer->uTeamID || traceResult.bCollided) {
            continue;
        }

        if (fCurrentPlayerDistanceToLocalPlayer < fPlayerDistanceToLocalPlayer) {
            fPlayerDistanceToLocalPlayer = fCurrentPlayerDistanceToLocalPlayer;
            vec3Delta = vec3CurrentPlayerDelta;
        }
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