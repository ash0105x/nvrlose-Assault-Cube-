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

    std::int32_t uAdjustedWidth = static_cast<const std::int32_t>(globals::entity::pLocalPlayer->vec3EyePosition.distance(refTarget.vec3EyePosition));

    if (!uAdjustedWidth) {
        return;
    }

    uAdjustedWidth = 300 / uAdjustedWidth;

    const std::int32_t xPosition = static_cast<const std::int32_t>(vec2TargetEyeScreenPosition.x) - uAdjustedWidth;
    const std::int32_t xPositionRight = xPosition + (uAdjustedWidth * 2);

    const std::int32_t yPosition = static_cast<const std::int32_t>(vec2TargetEyeScreenPosition.y) + uAdjustedWidth;

    //const float fPadding = static_cast<const float>(uAdjustedWidth) / 1.f;
    const float fPadding = static_cast<const float>(font.getHeight());

    constexpr GLubyte white[4] = { 255, 255, 255, 255 };

    const char* const& cstrRefTargetNickName = refTarget.cstrNickname;

    font.drawCentered(
        CVector2{
            static_cast<const float>(xPosition),
            static_cast<const float>(yPosition) + fPadding + 3.f
        },
        static_cast<const float>(xPositionRight),
        white,
        cstrRefTargetNickName,
        strlen(cstrRefTargetNickName)
    );

    if (refTarget.pCurrentWeapon) {
        const char* const& cstrRefTargetCurrentWeaponName = refTarget.pCurrentWeapon->pInitialWeaponData->cstrName;

        CVector2 vec2WeaponInfoPosition = CVector2{
            static_cast<const float>(xPosition),
            vec2TargetOriginScreenPosition.y - fPadding - 3.f
        };

        font.drawCentered(
            vec2WeaponInfoPosition,
            static_cast<const float>(xPositionRight),
            white,
            cstrRefTargetCurrentWeaponName,
            strlen(cstrRefTargetCurrentWeaponName)
        );

        vec2WeaponInfoPosition.y -= (font.getHeight() * 2);

        font.drawCenteredf(
            vec2WeaponInfoPosition,
            static_cast<const float>(xPositionRight),
            white,
            "%d / %d",
            *refTarget.pCurrentWeapon->upAmmo,
            refTarget.pCurrentWeapon->pInitialWeaponData->uAmmo
        );
    }

    const float fHealthBarPositionX = static_cast<const float>(xPosition) - fPadding;
    const float fHealthBarEndPositionY = vec2TargetOriginScreenPosition.y - ((vec2TargetOriginScreenPosition.y - static_cast<const float>(yPosition)) * (static_cast<const float>(refTarget.iHealth) / 100.f));

    char cstrHealthBuffer[10] = { };

    sprintf_s(
        cstrHealthBuffer,
        "%d",
        refTarget.iHealth
    );

    const size_t healthStringLength = strlen(cstrHealthBuffer);

    font.draw(
        CVector2{
            fHealthBarPositionX - (static_cast<const float>(healthStringLength * font.getHeight()) / 2.f),
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
    
    const GLubyte playerHealthColorValue = static_cast<const GLubyte>(refTarget.iHealth * 2.25f);

    glColor4ub(
        255u - playerHealthColorValue,
        playerHealthColorValue,
        NULL,
        255u
    );

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
        0.1f
    );

    // grey esp health bar

    constexpr GLubyte arrGrey[3] = {
        128u,
        128u,
        128u
    };

    gl::drawLineRGB(
        CVector2{
            fHealthBarPositionX,
            static_cast<const GLfloat>(yPosition),
        },
        CVector2{
            fHealthBarPositionX,
            vec2TargetOriginScreenPosition.y
        },
        arrGrey,
        0.1f
    );

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