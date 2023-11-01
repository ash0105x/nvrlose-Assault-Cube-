import aimbot;

#include"../win32api.h"

#include <limits>

import CVector3;
import globals;
import CTraceRay;


import offsets;

void modules::aimbot::onToggle(void) noexcept {
    float fPlayerDistanceToLocalPlayer = std::numeric_limits<const float>::max();

    static const CVector3& vec3RefLocalPlayerEyePosition = globals::entity::pLocalPlayer->vec3EyePosition;

    CVector3 vec3Delta = CVector3{ };

    for (std::uint8_t i = 1u; i < *globals::match::ipPlayerInGame; ++i) {
        const CPlayer& refCurrentPlayer = *((*globals::entity::pEntityList)[i]);

        if (!refCurrentPlayer.uHealth) {
            continue;
        }

        CTraceRay traceResult = CTraceRay{ };

        traceResult.traceLine(
            vec3RefLocalPlayerEyePosition,
            refCurrentPlayer.vec3EyePosition,
            globals::entity::pLocalPlayer,
            false,
            false
        );

        if (traceResult.bCollided) {
            continue;
        }

        /*if (
            !refCurrentPlayer.uHealth ||
            !refCurrentPlayer.isVisibleTo(vec3RefLocalPlayerEyePosition)
        )
        {
            continue;
        }*/

        vec3Delta = refCurrentPlayer.vec3EyePosition - vec3RefLocalPlayerEyePosition;

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
            asinf(vec3Delta.z / fPlayerDistanceToLocalPlayer) * CVector3::fDegreesRadiansConversionValue
        };
    }
}