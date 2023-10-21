export module offsets;

// ptrdiff_t
import <vcruntime.h>;

export namespace offsets {
    // type_t* ptr = reinterpret_cast<type_t*>(reinterpret_cast<std::uintptr_t>(GetModuleHandle(__TEXT("ac_client.exe"))) + offsets::ac_client_exe::offset);
    namespace ac_client_exe {
        constexpr const ptrdiff_t ENTITY_LIST = 0x0010F4F8u;
        constexpr const ptrdiff_t LOCAL_PLAYER = offsets::ac_client_exe::ENTITY_LIST - 0x4u;
        constexpr const ptrdiff_t CURRENT_PLAYER_IN_GAME = 0x10f500u;
    }

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
        constexpr const ptrdiff_t CSTR_NAME = 0x225u;
    }

    namespace function {
        constexpr const uintptr_t POPUP_MESSAGE = 0x4090F0u;
    }
}