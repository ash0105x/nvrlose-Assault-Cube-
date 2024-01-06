#include<Windows.h>
#include<chrono>
#include<tuple>

export module globals;

import<cstdint>;
import<array>;

export class playerent;
export class CVector3;

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
		constexpr const std::uint8_t MAX_ENTITIES = 32u;
		std::array<const playerent* const, globals::entity::MAX_ENTITIES>** ppEntityList = nullptr;
		constexpr const std::uint8_t FIRST_ENTITY_INDEX = 1u;
	}

	namespace modules {
		union _ac_client_exe {
			const HMODULE asHandle = GetModuleHandle(__TEXT("ac_client.exe"));
			std::uint8_t* const asBytePtr;
		}ac_client_exe;
	}

	namespace screen {
		std::array<std::int32_t, 4> viewPort = std::array<std::int32_t, 4>{  };
		std::array<const float, 4 * 4>* pfArrModelViewProjectionMatrix = nullptr;
		CVector3* pVec3CurrentWeaponEndTrajectory = nullptr; // 0x10A400
		float* fpFPS = nullptr;
	}

	namespace indicator {
		std::vector<std::tuple<std::string, std::uint32_t, std::int32_t, std::chrono::steady_clock::time_point>> vecEntitiesHit = std::vector<std::tuple<std::string, std::uint32_t, std::int32_t, std::chrono::steady_clock::time_point>>{ };
		std::chrono::steady_clock::time_point lastHitTimer = std::chrono::steady_clock::time_point{  };
	}

	namespace match {
		char* cstrPlayingMap = nullptr;
		std::uint8_t* bypPlayerInGame = nullptr;
	}

	namespace thread {
		DWORD dwId = NULL;
	}

	namespace dll {
		bool bUnload = false;
	}

	float fDamageMultiplicator = 1.f;
}