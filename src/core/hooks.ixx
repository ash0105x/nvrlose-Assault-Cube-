export module hooks;

import<utility>;
import<tuple>;
import<string>;

import globals;
import weapon;
import playerent;
import CTraceRay;
import aimbot;
import utils;

export class IHook;

export typedef enum : std::uint8_t {
    HOOK_INDEX_METHOD = NULL,
    HOOK_INDEX_FUNCTION = 1,
    HOOK_INDEX_JUMP_BACK_ADDRESS = 2
}HOOK_INDEX;

namespace jumpBackAddress {
    const void* vpHealthOpcode = nullptr;
    const void* vpUnknownOpcode = nullptr;
}

constexpr const float ONE_HALTH = 0.5f;

namespace functions {
    static void __declspec(naked) hkHealthDecreaseOpcode( void ) noexcept {
        __asm {
            // Setting eax to the localPlayer pointer
            mov eax, dword ptr[globals::entity::pLocalPlayer]
        }

        // ebx is the defender, but 0xF4 bytes ahead from the playerent base
        // edi is the damage
        // eax holds the damage value after certain modifications

#ifdef _DEBUG
        __asm {
            // Setting al to the localPlayer's player ID
            mov al, byte ptr[eax + 0x01E4u]

            // Comparing the defender's player ID with our's
            cmp byte ptr[ebx + 0x01E4u - 0xF4u], al
            // Jump if the defender is not the localPlayer
            jne short onEnemy

            // Will be executed if the defender is the localPlayer

            // Setting the damage taken value to zero.
            xor eax, eax
            // Continue original game code execution to prevent crashes
            jmp dword ptr[jumpBackAddress::vpHealthOpcode]
        }
#endif // _DEBUG

        __asm {
            // Setting al to the localPlayer's teamID (type std::uint8_t)
            mov al, byte ptr[eax + 0x032Cu]
            // Comparing the defender's teamID with our's
            cmp byte ptr[ebx + 0x032Cu - 0xF4u], al
            // Jump if the defender is an enemy
            jne short onEnemy

            // Will be executed if the defender is our teammate

            // Setting the damage taken value to zero.
            xor eax, eax
            // Continue original game code execution to prevent crashes
            jmp dword ptr[jumpBackAddress::vpHealthOpcode]
        }

        __asm {
        onEnemy:
            // Will be executed if the defender is an enemy

            // Convert the damage to a float and store its result in xmm0
            cvtsi2ss xmm0, edi
            // Multiply the damage with our damage multiplicator
            mulss xmm0, dword ptr[globals::fDamageMultiplicator]
            // Add 0.5 to floor
            addss xmm0, dword ptr[ONE_HALTH]
            // Convert the result of the multiplications to an int and store its result in eax
            cvttss2si eax, xmm0

            // Subtract the defenders health with eax
            sub dword ptr[ebx + 0x4u], eax
            // Continue original game code execution to prevent crashes
            jmp dword ptr[jumpBackAddress::vpHealthOpcode]
        }
    }

    static void hkShoot( void ) noexcept {
        weapon* This = nullptr;

        __asm {
            mov dword ptr[This], esi
        }

        const playerent* const pWeaponOwner = This->pOwner;

        if (globals::entity::pLocalPlayer->uTeamID != pWeaponOwner->uTeamID) {
            *This->upAmmo = NULL;
            *This->upReservedAmmo = NULL;

            return;
        }

        const std::uint32_t& uRefInitialAmmo = This->pInitialWeaponData->uAmmo;

        *This->upAmmo = uRefInitialAmmo + 1;
        *This->upReservedAmmo = uRefInitialAmmo * 2;

        if (pWeaponOwner != globals::entity::pLocalPlayer || !modules::combat::aimbot::bSilent) {
            return;
        }

        const CVector3* pvec3ClosestEnemyPosition = nullptr;
        float fClosestEnemyDistance = std::numeric_limits<float>::max();
        float fAimbotFOV = std::numeric_limits<float>::max();

        for (std::uint8_t i = globals::entity::FIRST_ENTITY_INDEX; i < *globals::match::bypPlayerInGame; ++i) {
            const playerent& refCurrentPlayer = *(**globals::entity::ppEntityList)[i];

            if (refCurrentPlayer.iHealth <= NULL || refCurrentPlayer.uTeamID == globals::entity::pLocalPlayer->uTeamID) {
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

            if (modules::combat::aimbot::bVisible && traceResult.bCollided) {
                continue;
            }

            const CVector3* const pvec3CurrentPlayerEyePosition = &refCurrentPlayer.vec3EyePosition;

            const float fCurrentDistance = globals::entity::pLocalPlayer->vec3EyePosition.distance(*pvec3CurrentPlayerEyePosition);

            if (modules::combat::aimbot::bIgnoreFOV) {
                if (fCurrentDistance >= fClosestEnemyDistance) {
                    continue;
                }

                fClosestEnemyDistance = fCurrentDistance;
                pvec3ClosestEnemyPosition = pvec3CurrentPlayerEyePosition;

                continue;
            }

            CVector2 vec2EnemyWorldPosition = CVector2{ };
            if (!utils::math::worldToScreen(refCurrentPlayer.vec3EyePosition, vec2EnemyWorldPosition)) {
                continue;
            }

            const float fCurrentPlayerAimbotFOV = globals::screen::vec2MiddleScreen.distance(vec2EnemyWorldPosition);

            if (fCurrentPlayerAimbotFOV > modules::combat::aimbot::fFOV || fCurrentPlayerAimbotFOV > fAimbotFOV) {
                continue;
            }

            fAimbotFOV = fCurrentPlayerAimbotFOV;
            pvec3ClosestEnemyPosition = pvec3CurrentPlayerEyePosition;
        }

        if (pvec3ClosestEnemyPosition) {
            *globals::screen::pVec3CurrentWeaponEndTrajectory = *pvec3ClosestEnemyPosition;
        }
    }

    static void __cdecl resetLastHitTimer( void ) noexcept {
        globals::indicator::lastHitTimer = std::chrono::steady_clock::now();
    }

    static void __cdecl setValues(
        const char* const cstrNickname,
        const std::uint32_t uDamageGiven,
        const std::int32_t iHealthRemaining
    ) noexcept
    {
        globals::indicator::vecEntitiesHit.emplace_back(
            std::tuple<std::string, std::uint32_t, std::int32_t, std::chrono::steady_clock::time_point> {
                std::string{ cstrNickname },
                uDamageGiven,
                iHealthRemaining,
                std::chrono::steady_clock::now()
            }
        );
    }

    static void __declspec(naked) hkUnknown(
        void
        //const playerent* const pDefender, // ecx
        //const std::int32_t iDamage, // [ebp + 0x8]
        //const playerent* const pAttacker, // [ebp + 0xC]
        //const std::int32_t a4, // [ebp + 0x10]
        //const bool a5, // [ebp + 0x14]
        //const bool a6 // [ebp + 0x18]
    ) noexcept
    {
        __asm {
            push ebp
            mov ebp, esp

            mov ecx, dword ptr[ebp + 0xC] // pAttacker
            cmp dword ptr[globals::entity::pLocalPlayer], ecx // if globals::entity::pLocalPlayer == pAttacker
            je onLocalPlayer
            jmp dword ptr[jumpBackAddress::vpUnknownOpcode]
            onLocalPlayer:
            // eax is the defender (playerent*)
            mov ecx, dword ptr[eax + 0xF8] // defender->iHealth
            sub ecx, dword ptr[ebp + 0x8] // iDamage

            // we will call a function that
            // could change the value in our
            // eax register. This should not
            // happen so we preserve it
            push eax

            push ecx // iHealthRemaining
            push dword ptr[ebp + 0x8] // iDamage
            lea ecx, dword ptr[eax + 0x225] // defender->cstrNickname
            push ecx
            call setValues
            add esp, 0xC // stack clean-up

            call resetLastHitTimer

            // restore original eax value
            pop eax
            jmp dword ptr[jumpBackAddress::vpUnknownOpcode]
        }
    }
}

export namespace hooks {
	std::tuple<IHook*, const void* const, const void*&> healthDecreaseOpcode = std::tuple<IHook*, const void* const, const void*&>{ nullptr, &functions::hkHealthDecreaseOpcode, jumpBackAddress::vpHealthOpcode };
    std::pair<IHook*, const void* const> shootFunction = std::make_pair<IHook*, const void* const>(nullptr, &functions::hkShoot);
    std::tuple<IHook*, const void* const, const void*&> unknownOpcode = std::tuple<IHook*, const void* const, const void*&>{ nullptr, &functions::hkUnknown, jumpBackAddress::vpUnknownOpcode };
}