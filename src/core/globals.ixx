#include"win32api.h"

export module globals;

import <cstdint>;
import <array>;

import CPlayer;
import offsets;

export namespace globals {
	namespace function {
		namespace definition {
			typedef void(__cdecl* _popupMessage_t)(_In_z_ _Printf_format_string_ const char* const, ...) noexcept;
		}
		namespace pointer {
			globals::function::definition::_popupMessage_t pPopupMessage = nullptr;
		}
	}

	namespace entity {
		CPlayer* pLocalPlayer = nullptr;
		constexpr const std::uint8_t MAX_ENTITIES = 32u;
		const std::array<const CPlayer* const, globals::entity::MAX_ENTITIES>* pEntityList = nullptr;
	}

	namespace match {
		const std::uint32_t* ipPlayerInGame = nullptr;
	}
}