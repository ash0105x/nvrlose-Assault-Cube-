export module globals;

import <cstdint>;
import <array>;

import CPlayer;
import offsets;

#include"win32api.h"

export namespace globals {
	namespace modules {
		std::uintptr_t ac_client_exe = reinterpret_cast<const std::uintptr_t>(GetModuleHandle(__TEXT("ac_client.exe")));
	}

	namespace entity {
		CPlayer* pLocalPlayer = nullptr;
		std::array<CPlayer* const, 32u>* pEntityList = nullptr;
	}

	namespace match {
		const std::int32_t* const ipPlayerInGame = reinterpret_cast<const std::int32_t* const>(globals::modules::ac_client_exe + offsets::ac_client_exe::CURRENT_PLAYER_IN_GAME);
	}
}