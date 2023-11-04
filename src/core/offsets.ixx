export module offsets;

import <cstdint>;

import <stddef.h>;

export namespace offsets {
    namespace ac_client_exe {
        // type_t* ptr = reinterpret_cast<type_t*>(reinterpret_cast<std::uintptr_t>(GetModuleHandle(__TEXT("ac_client.exe"))) + offsets::ac_client_exe::{offset});
        namespace pointer {
            constexpr const ptrdiff_t ENTITY_LIST = 0x0010F4F8u;
            constexpr const ptrdiff_t LOCAL_PLAYER = offsets::ac_client_exe::pointer::ENTITY_LIST - sizeof(void*);
            constexpr const ptrdiff_t I_CURRENT_PLAYER_IN_GAME = 0x10f500u;
            constexpr const ptrdiff_t I_GAME_TIME = 0x109EACu;
        }

        // typedef type_t({callingConvention}* func_t)({parameterList});
        // or
        // using func_t = type_t({callingConvention}*)({parameterList});
        // func_t funcPtr = reinterpret_cast<func_t>(reinterpret_cast<std::uintptr_t>(GetModuleHandle(__TEXT("ac_client.exe"))) + offsets::ac_client_exe::function::{offset});
        namespace function {
            constexpr const ptrdiff_t POPUP_MESSAGE = 0x090F0u;
            constexpr const ptrdiff_t TRACE_LINE = 0x8A310u;
            constexpr const ptrdiff_t IS_VISIBLE = 0x8ABD0u;
            constexpr const ptrdiff_t INTERSECT = 0x603C0u;
        }
    }

    // type_t* ptr = reinterpret_cast<type_t*>(reinterpret_cast<std::uintptr_t>(pPlayer) + offsets::player::{offset});
    namespace player {
        constexpr const ptrdiff_t VEC3_EYE_POSITION = 0x0004u;
        constexpr const ptrdiff_t VEC3_VELOCITY = 0x0028u;
        constexpr const ptrdiff_t VEC3_FEET_POSITION = 0x0034u;
        constexpr const ptrdiff_t VEC2_VIEW_ANGLES = 0x0040u;
        constexpr const ptrdiff_t B_ON_GROUND = 0x69u;
        constexpr const ptrdiff_t F_CAMERA_HEIGHT = 0x005Cu;
        constexpr const ptrdiff_t B_IS_NOT_SHOOTING = 0x70u;
        constexpr const ptrdiff_t I_MOVE_TYPE = 0x80u;
        constexpr const ptrdiff_t I_HEALTH = 0x00F8u;
        constexpr const ptrdiff_t I_AMMO = 0x0150u;
        constexpr const ptrdiff_t I_FIRING_SPEED = 0x0178u;
        constexpr const ptrdiff_t I_SHOTS_FIRED = 0x01A0u;
        constexpr const ptrdiff_t CSTR_NICKNAME = 0x225u;
        constexpr const ptrdiff_t U_TEAM_ID = 0x032Cu;
    }
}