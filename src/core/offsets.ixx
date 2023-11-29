export module offsets;

import<stddef.h>;

export namespace offsets {
    namespace ac_client_exe {
        // type_t* ptr = reinterpret_cast<type_t*>(reinterpret_cast<std::uintptr_t>(GetModuleHandle(__TEXT("ac_client.exe"))) + offsets::ac_client_exe::{offset});
        namespace pointer {
            constexpr ptrdiff_t ENTITY_LIST = 0x0010F4F8u;
            constexpr ptrdiff_t LOCAL_PLAYER = offsets::ac_client_exe::pointer::ENTITY_LIST - sizeof(void*);
            constexpr ptrdiff_t I_CURRENT_PLAYER_IN_GAME = 0x10f500u;
            constexpr ptrdiff_t MODEL_VIEW_PROJECTION_MATRIX = 0x00101AE8u;
            constexpr ptrdiff_t VEC3_CURRENT_WEAPON_END_TRAJECTORY = 0x10A400u;
        }

        // typedef type_t({callingConvention}* func_t)({parameterList});
        // or
        // using func_t = type_t({callingConvention}*)({parameterList});
        // func_t funcPtr = reinterpret_cast<func_t>(reinterpret_cast<std::uintptr_t>(GetModuleHandle(__TEXT("ac_client.exe"))) + offsets::ac_client_exe::function::{offset});
        namespace function {
            constexpr ptrdiff_t POPUP_MESSAGE = 0x090F0u;
            constexpr ptrdiff_t TRACE_LINE = 0x8A310u;
            constexpr ptrdiff_t IS_VISIBLE = 0x8ABD0u;
            constexpr ptrdiff_t INTERSECT = 0x603C0u;
        }
    }
}