#include<Windows.h>
#include<chrono>
#include<tuple>

export module globals;

import<cstdint>;
import<array>;

import playerent;
import offsets;

export typedef enum : std::uint8_t {
	VIEW_PORT_ELEMENT_X = NULL,
	VIEW_PORT_ELEMENT_Y,
	VIEW_PORT_ELEMENT_WIDTH,
	VIEW_PORT_ELEMENT_HEIGHT = 3u
}VIEW_PORT_ELEMENT;

export namespace globals {
	namespace function {
		namespace definition {
			typedef void(FAR __cdecl* _popupMessage_t)(_In_z_ _Printf_format_string_ const char* const, ...) noexcept;
		}

		globals::function::definition::_popupMessage_t pPopupMessage = nullptr;
	}

	namespace entity {
		playerent* pLocalPlayer = nullptr;
		constexpr std::uint8_t MAX_ENTITIES = 32u;
		const std::array<const playerent* const, globals::entity::MAX_ENTITIES>* pEntityList = nullptr;
		constexpr std::uint8_t FIRST_ENTITY_INDEX = 1u;
	}

	namespace modules {
		std::uint8_t* const ac_client_exe = reinterpret_cast<std::uint8_t* const>(GetModuleHandle(__TEXT("ac_client.exe")));
	}

	namespace screen {
		std::int32_t viewPort[4u];
		const float* const pModelViewProjectionMatrix = reinterpret_cast<const float* const>(globals::modules::ac_client_exe + offsets::ac_client_exe::pointer::MODEL_VIEW_PROJECTION_MATRIX);
		CVector3* const pvec3CurrentWeaponEndTrajectory = reinterpret_cast<CVector3* const>(globals::modules::ac_client_exe + offsets::ac_client_exe::pointer::VEC3_CURRENT_WEAPON_END_TRAJECTORY);
	}

	namespace match {
		const std::uint8_t* const bypPlayerInGame = reinterpret_cast<const std::uint8_t* const>(globals::modules::ac_client_exe + offsets::ac_client_exe::pointer::I_CURRENT_PLAYER_IN_GAME);
	}

	namespace thread {
		DWORD dwId = NULL;
	}

	std::vector<std::tuple<CVector3, CVector3, std::chrono::steady_clock::time_point>> vecLocalPlayerShotPositions = std::vector<std::tuple<CVector3, CVector3, std::chrono::steady_clock::time_point>>{  };
}