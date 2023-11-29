#include<Windows.h>

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
			typedef void(__cdecl* _popupMessage_t)(_In_z_ _Printf_format_string_ const char* const, ...) noexcept;
		}
		namespace pointer {
			globals::function::definition::_popupMessage_t pPopupMessage = nullptr;
		}
	}

	namespace entity {
		playerent* pLocalPlayer = nullptr;
		constexpr std::uint8_t MAX_ENTITIES = 32u;
		const std::array<const playerent* const, globals::entity::MAX_ENTITIES>* pEntityList = nullptr;
		constexpr std::uint8_t FIRST_ENTITY_INDEX = 1u;
	}

	namespace screen {
		std::int32_t viewPort[4u];
		const float* pModelViewProjectionMatrix = nullptr;
		const CVector3* vec3CurrentWeaponEndTrajectory = nullptr;
	}

	namespace modules {
		std::uint8_t* const ac_client_exe = reinterpret_cast<std::uint8_t* const>(GetModuleHandle(__TEXT("ac_client.exe")));
	}

	namespace thread {
		DWORD dwId = NULL;
	}

	namespace match {
		const std::uint8_t* bypPlayerInGame = nullptr;
	}
}