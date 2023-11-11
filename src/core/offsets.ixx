export module offsets;

import<stddef.h>;

export namespace offsets {
    namespace ac_client_exe {
        // type_t* ptr = reinterpret_cast<type_t*>(reinterpret_cast<std::uintptr_t>(GetModuleHandle(__TEXT("ac_client.exe"))) + offsets::ac_client_exe::{offset});
        namespace pointer {
            constexpr const ptrdiff_t ENTITY_LIST = 0x0010F4F8u;
            constexpr const ptrdiff_t LOCAL_PLAYER = offsets::ac_client_exe::pointer::ENTITY_LIST - sizeof(void*);
            constexpr const ptrdiff_t I_CURRENT_PLAYER_IN_GAME = 0x10f500u;
            constexpr const ptrdiff_t MODEL_VIEW_PROJECTION_MATRIX = 0x00101AE8u;
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
}