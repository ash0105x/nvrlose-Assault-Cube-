export module offsets;

import<stddef.h>;

export namespace offsets {
    namespace ac_client_exe {
        // type_t* ptr = reinterpret_cast<type_t*>(reinterpret_cast<std::uint8_t*>(GetModuleHandle(__TEXT("ac_client.exe"))) + offsets::ac_client_exe::{offset});
        namespace pointer {
            constexpr const ptrdiff_t ENTITY_LIST = 0x0010F4F8u;
            constexpr const ptrdiff_t LOCAL_PLAYER = offsets::ac_client_exe::pointer::ENTITY_LIST - sizeof(void*);
            constexpr const ptrdiff_t I_CURRENT_PLAYER_IN_GAME = 0x10f500u;
            constexpr const ptrdiff_t MODEL_VIEW_PROJECTION_MATRIX = 0x00101AE8u;
            constexpr const ptrdiff_t VEC3_CURRENT_WEAPON_END_TRAJECTORY = 0x10A400u;
        }
    }
}