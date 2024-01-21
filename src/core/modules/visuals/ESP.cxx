import ESP;

import globals;
import playerent;
import utils;
import gl;
import CFont;

import<cstdint>;

#include<stdio.h>
#include<Windows.h>

#include<gl/GL.h>
#pragma comment(lib, "opengl32.lib")

void modules::visuals::ESP::onToggle(
    const playerent& refTarget,
    const gl::CFont& font,
    const CVector2& vec2TargetOriginScreenPosition,
    const GLubyte(&arrColor)[4]
) noexcept
{
    CVector2 vec2TargetEyeScreenPosition = CVector2{ };

    if (!utils::math::worldToScreen(refTarget.vec3EyePosition, vec2TargetEyeScreenPosition)) {
        return;
    }

    float uAdjustedWidth = globals::entity::pLocalPlayer->vec3EyePosition.distance(refTarget.vec3EyePosition);

    if (!uAdjustedWidth) {
        return;
    }

    uAdjustedWidth = 300.f / uAdjustedWidth;

    const float xPosition = vec2TargetEyeScreenPosition.x - uAdjustedWidth;
    const float yPosition = vec2TargetEyeScreenPosition.y + uAdjustedWidth;

    const float xPositionRight = xPosition + static_cast<const float>(uAdjustedWidth * 2);

    const float fPadding = static_cast<const float>(font.getHeight());

    constexpr const float BAR_LINE_WIDTH = 4.f;
    constexpr const float OUT_LINE_WIDTH = 1.f;

    constexpr const GLubyte ARR_WHITE[4] = { 0xFF, 0xFF, 0xFF, 0xFF };
    constexpr const GLubyte ARR_BLUE[4] = { NULL, NULL, 0xFF, 0xFF };
    constexpr const GLubyte OUTLINE_COLOR[3u] = { NULL, NULL, NULL };
    constexpr const GLubyte BACKGROUND_COLOR[3] = { 0x80, 0x80, 0x80 };

    const char* const& cstrRefTargetNickName = refTarget.cstrNickname;

    font.drawCentered(
        CVector2{
            xPosition,
            yPosition + fPadding + 3.f
        },
        xPositionRight,
        ARR_WHITE,
        cstrRefTargetNickName,
        strlen(cstrRefTargetNickName)
    );

    const float fFontHeight = static_cast<const float>(font.getHeight());

    if (refTarget.pCurrentWeapon) {
        CVector2 vec2WeaponInfoPosition = CVector2{
            xPosition,
            vec2TargetOriginScreenPosition.y - fPadding - 3.f
        };

        const CVector2 vec2AmmoBarEndPosition = CVector2{
            vec2WeaponInfoPosition.x +
            (
                (xPositionRight - vec2WeaponInfoPosition.x) *
                (
                    static_cast<const float>(*refTarget.pCurrentWeapon->upAmmo) /
                    static_cast<const float>(refTarget.pCurrentWeapon->pInitialWeaponData->uAmmo)
                )
            ),
            vec2WeaponInfoPosition.y
        };

        gl::drawLineRGBA(
            vec2WeaponInfoPosition,
            vec2AmmoBarEndPosition,
            ARR_WHITE,
            BAR_LINE_WIDTH
        );

        gl::drawLineRGB(
            vec2WeaponInfoPosition,
            CVector2{
                xPositionRight,
                vec2WeaponInfoPosition.y
            },
            BACKGROUND_COLOR,
            BAR_LINE_WIDTH
        );

        gl::drawLineRGB(
            CVector2{
                vec2WeaponInfoPosition.x - OUT_LINE_WIDTH / 2,
                vec2WeaponInfoPosition.y
            },
            CVector2{
                xPositionRight + OUT_LINE_WIDTH / 2,
                vec2WeaponInfoPosition.y
            },
            OUTLINE_COLOR,
            BAR_LINE_WIDTH + OUT_LINE_WIDTH
        );

        const char* const& cstrRefTargetCurrentWeaponName = refTarget.pCurrentWeapon->pInitialWeaponData->cstrName;

        font.drawCentered(
            CVector2{
                vec2WeaponInfoPosition.x,
                vec2WeaponInfoPosition.y - (2.f * fPadding) - 3.f
            },
            xPositionRight,
            ARR_WHITE,
            cstrRefTargetCurrentWeaponName,
            strlen(cstrRefTargetCurrentWeaponName)
        );

        vec2WeaponInfoPosition.y -= (font.getHeight() * 2);

        char cstrAmmoBuffer[10] = { };

        sprintf_s(
            cstrAmmoBuffer,
            "%d",
            *refTarget.pCurrentWeapon->upAmmo
        );

        const size_t ammoStringLength = strlen(cstrAmmoBuffer);

        font.draw(
            CVector2{
                vec2AmmoBarEndPosition.x - ((static_cast<const float>(ammoStringLength) * fFontHeight) / 2.f),
                vec2AmmoBarEndPosition.y
            },
            ARR_WHITE,
            cstrAmmoBuffer,
            ammoStringLength
        );
    }

    glLineWidth(0.1f);
    glColor4ub(
        arrColor[NULL],
        arrColor[1u],
        arrColor[2u],
        arrColor[3u]
    );

    glBegin(GL_LINE_LOOP);

    glVertex2f(
        xPosition,
        yPosition
    );

    glVertex2f(
        xPositionRight,
        yPosition
    );

    glVertex2f(
        xPositionRight,
        vec2TargetOriginScreenPosition.y
    );

    glVertex2f(
        xPosition,
        vec2TargetOriginScreenPosition.y
    );

    glEnd();

    const auto drawInfoBar = [
        &vec2TargetOriginScreenPosition,
        &yPosition,
        &font,
        &ARR_WHITE,
        &ARR_BLUE,
        &fFontHeight,
        &BACKGROUND_COLOR,
        &OUTLINE_COLOR
    ]
    (
        _In_ const float fBarPositionX,
        _In_range_(NULL, 100) const std::int32_t iInfo,
        _In_ bool bInfoIsHealthValue
    ) noexcept -> void
    {
        const float fBarEndPositionY = (
            vec2TargetOriginScreenPosition.y - (
                (vec2TargetOriginScreenPosition.y - yPosition) *
                (static_cast<const float>(iInfo) / 100.f)
            )
        );

        char cstrTextBuffer[10] = { };

        _itoa_s(iInfo, cstrTextBuffer, 10);

        const size_t textStringLength = strlen(cstrTextBuffer);

        font.draw(
            CVector2{
                fBarPositionX - ((static_cast<const float>(textStringLength) * fFontHeight) / 2.f),
                fBarEndPositionY
            },
            ARR_WHITE,
            cstrTextBuffer,
            textStringLength
        );

        if (!bInfoIsHealthValue) {
            gl::drawLineRGBA(
                CVector2{
                    fBarPositionX,
                    vec2TargetOriginScreenPosition.y
                },
                CVector2{
                    fBarPositionX,
                    fBarEndPositionY
                },
                ARR_BLUE,
                BAR_LINE_WIDTH
            );
        }
        else {
            const GLubyte playerHealthColorValue = static_cast<const GLubyte>(static_cast<const float>(iInfo) * 2.25f);

            const GLubyte playerHealthColor[4] = {
                255u - playerHealthColorValue,
                playerHealthColorValue,
                NULL,
                255u
            };

            gl::drawLineRGBA(
                CVector2{
                    fBarPositionX,
                    vec2TargetOriginScreenPosition.y
                },
                CVector2{
                    fBarPositionX,
                    fBarEndPositionY
                },
                playerHealthColor,
                BAR_LINE_WIDTH
            );
        }

        gl::drawLineRGB(
            CVector2{
                fBarPositionX,
                static_cast<const GLfloat>(yPosition),
            },
            CVector2{
                fBarPositionX,
                vec2TargetOriginScreenPosition.y
            },
            BACKGROUND_COLOR,
            BAR_LINE_WIDTH
        );

        gl::drawLineRGB(
            CVector2{
                fBarPositionX,
                yPosition + OUT_LINE_WIDTH / 2
            },
            CVector2{
                fBarPositionX,
                vec2TargetOriginScreenPosition.y - OUT_LINE_WIDTH / 2
            },
            OUTLINE_COLOR,
            BAR_LINE_WIDTH + OUT_LINE_WIDTH
        );
    };

    drawInfoBar(xPosition - fPadding - 3.f, refTarget.iHealth, true);
    drawInfoBar(xPositionRight + fPadding + 3.f, refTarget.iArmor, false);
}