import ESP;

import globals;
import playerent;
import utils;
import gl;
import CFont;

import <cstdint>;

#include<stdio.h>
#include<Windows.h>

#include<gl/GL.h>
#pragma comment(lib, "opengl32.lib")

void modules::visuals::ESP::onToggle(
    const playerent& refTarget,
    const gl::CFont& font,
    const CVector2& vec2TargetOriginScreenPosition,
    const GLubyte (&arrColor)[4]
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

    constexpr const GLubyte white[4] = { 255, 255, 255, 255 };

    const char* const& cstrRefTargetNickName = refTarget.cstrNickname;

    font.drawCentered(
        CVector2{
            xPosition,
            yPosition + fPadding + 3.f
        },
        xPositionRight,
        white,
        cstrRefTargetNickName,
        strlen(cstrRefTargetNickName)
    );

    const float fFontHeight = static_cast<const float>(font.getHeight());

    constexpr const float BAR_LINE_WIDTH = 4.f;
    constexpr const float OUT_LINE_WIDTH = 1.f;

    constexpr const GLubyte ARR_BLACK [3u] = { NULL, NULL, NULL };
    constexpr const GLubyte ARR_GREY[3] = { 128u, 128u, 128u };

    if (refTarget.pCurrentWeapon) {
        CVector2 vec2WeaponInfoPosition = CVector2{
            xPosition,
            vec2TargetOriginScreenPosition.y - fPadding - 3.f
        };

        constexpr const GLubyte ARR_WHITE[3] = { 255, 255, 255 };

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

        gl::drawLineRGB(
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
            ARR_GREY,
            BAR_LINE_WIDTH
        );

        gl::drawLineRGB(
            CVector2{
                vec2WeaponInfoPosition.x - (BAR_LINE_WIDTH / 2),
                vec2WeaponInfoPosition.y
            },
            CVector2{
                xPositionRight + (BAR_LINE_WIDTH / 2),
                vec2WeaponInfoPosition.y
            },
            ARR_BLACK,
            OUT_LINE_WIDTH
        );

        const char* const& cstrRefTargetCurrentWeaponName = refTarget.pCurrentWeapon->pInitialWeaponData->cstrName;

        font.drawCentered(
            CVector2{
                vec2WeaponInfoPosition.x,
                vec2WeaponInfoPosition.y - (2.f * fPadding) - 3.f
            },
            xPositionRight,
            white,
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
            white,
            cstrAmmoBuffer,
            ammoStringLength
        );
    }

    const float fHealthBarPositionX = xPosition - fPadding - 3.f;
    const float fHealthBarEndPositionY = vec2TargetOriginScreenPosition.y - ((vec2TargetOriginScreenPosition.y - yPosition) * (static_cast<const float>(refTarget.iHealth) / 100.f));

    char cstrHealthBuffer[10] = { };

    sprintf_s(
        cstrHealthBuffer,
        "%d",
        refTarget.iHealth
    );

    const size_t healthStringLength = strlen(cstrHealthBuffer);

    font.draw(
        CVector2{
            fHealthBarPositionX - ((static_cast<const float>(healthStringLength) * fFontHeight) / 2.f),
            fHealthBarEndPositionY
        },
        white,
        cstrHealthBuffer,
        healthStringLength
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

    // Colored esp health bar
    
    const GLubyte playerHealthColorValue = static_cast<const GLubyte>(static_cast<const float>(refTarget.iHealth) * 2.25f);

    const GLubyte playerHealthColor[4] = {
        255u - playerHealthColorValue,
        playerHealthColorValue,
        NULL,
        255u
    };
    
    gl::drawLineRGBA(
        CVector2{
            fHealthBarPositionX,
            vec2TargetOriginScreenPosition.y
        },
        CVector2{
            fHealthBarPositionX,
            fHealthBarEndPositionY
        },
        playerHealthColor,
        BAR_LINE_WIDTH
    );

    // grey esp health bar

    gl::drawLineRGB(
        CVector2{
            fHealthBarPositionX,
            static_cast<const GLfloat>(yPosition),
        },
        CVector2{
            fHealthBarPositionX,
            vec2TargetOriginScreenPosition.y
        },
        ARR_GREY,
        BAR_LINE_WIDTH
    );

    gl::drawLineRGB(
        CVector2{
            fHealthBarPositionX,
            yPosition + (BAR_LINE_WIDTH / 2.f)
        },
        CVector2{
            fHealthBarPositionX,
            vec2TargetOriginScreenPosition.y - (BAR_LINE_WIDTH / 2.f)
        },
        ARR_BLACK,
        OUT_LINE_WIDTH
    );
}