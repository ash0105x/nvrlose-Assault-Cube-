export module hooks;

import<utility>;
import<tuple>;
import<string>;

import globals;
import weapon;
import playerent;
import CDetour32;
import CMidHook32;

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

        if (globals::entity::pLocalPlayer->uTeamID != This->pOwner->uTeamID) {
            *This->upAmmo = NULL;
            *This->upReservedAmmo = NULL;

            return;
        }

        const std::uint32_t& uRefInitialAmmo = This->pInitialWeaponData->uAmmo;

        *This->upAmmo = uRefInitialAmmo + 1;
        *This->upReservedAmmo = uRefInitialAmmo * 2;
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

            mov ecx, dword ptr[ebp + 0xC]
            cmp dword ptr[globals::entity::pLocalPlayer], ecx
            je onLocalPlayer
            jmp dword ptr[jumpBackAddress::vpUnknownOpcode]
            onLocalPlayer:
            mov ecx, dword ptr[eax + 0xF8]
            sub ecx, dword ptr[ebp + 0x8]

            push eax

            push ecx
            push dword ptr[ebp + 0x8]
            lea ecx, dword ptr[eax + 0x225]
            push ecx
            call setValues
            add esp, 0xC

            call resetLastHitTimer

            pop eax
            jmp dword ptr[jumpBackAddress::vpUnknownOpcode]
        }
    }
}

export namespace hooks {
	std::tuple<IHook*, const void* const, const void*&> healthDecreaseOpcode = std::tuple<IHook*, const void* const, const void*&>{ nullptr, &functions::hkHealthDecreaseOpcode, jumpBackAddress::vpHealthOpcode };
    std::pair<IHook*, const void* const> shootFunction = std::pair<IHook*, const void* const>{ nullptr, &functions::hkShoot };
    std::tuple<IHook*, const void* const, const void*&> unknownOpcode = std::tuple<IHook*, const void* const, const void*&>{ nullptr, &functions::hkUnknown, jumpBackAddress::vpUnknownOpcode };
}