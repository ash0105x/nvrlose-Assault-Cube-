import aimbot;

#include"../../win32api.h"

#include <limits>

import CVector3;
import globals;
import CTraceRay;

void modules::combat::aimbot::onToggle(void) noexcept {
    float fPlayerDistanceToLocalPlayer = std::numeric_limits<const float>::max();

    CVector3 vec3Delta = CVector3{ };

    for (std::uint8_t i = 1u; i < *globals::match::ipPlayerInGame; ++i) {
        const CPlayer* const pCurrentPlayer = (*globals::entity::pEntityList)[i];

        if (
            !pCurrentPlayer ||
            pCurrentPlayer->uTeamID == globals::entity::pLocalPlayer->uTeamID ||
            !pCurrentPlayer->iHealth
        )
        {
            continue;
        }

        CTraceRay traceResult = CTraceRay{ };

        traceResult.traceLine(
            globals::entity::pLocalPlayer->vec3EyePosition,
            pCurrentPlayer->vec3EyePosition,
            globals::entity::pLocalPlayer,
            false,
            false
        );

        if (traceResult.bCollided) {
            continue;
        }

        vec3Delta = pCurrentPlayer->vec3EyePosition - globals::entity::pLocalPlayer->vec3EyePosition;

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
}