export module signatures;

import<sal.h>;
import<regex>;

typedef unsigned char byte_t;

export template<size_t t_patternLength>
using SignatureDataEx_t = const std::pair<const byte_t(&)[t_patternLength], const char(&)[t_patternLength]>;

export typedef const std::pair<const byte_t* const, const char* const> SignatureData_t;

export typedef enum : std::uint8_t {
    SIGNATURE_DATA_INDEX_PATTERN = NULL,
    SIGNATURE_DATA_INDEX_MASK
}SIGNATURE_DATA_INDEX;

namespace patterns {
    namespace code {
        constexpr const byte_t HEALTH[6] = "\x29\x7B\x04\x8B\xC7";
    }
    namespace pointer {
        constexpr const byte_t PLAYER_IN_GAME[9] = "\xFF\x05\xCC\xCC\xCC\xCC\x8B\xC7";
        constexpr const byte_t LOCAL_PLAYER[10] = "\x39\x05\xCC\xCC\xCC\xCC\x0F\x94\xC3";
        constexpr const byte_t FPS[13] = "\xD9\x05\xCC\xCC\xCC\xCC\x53\x56\xD9\x5C\x24\x40";
        constexpr const byte_t ENTITY_LIST[19] = "\xBE\xCC\xCC\xCC\xCC\xE8\xCC\xCC\xCC\xCC\x8B\x15\xCC\xCC\xCC\xCC\x8B\x35";
        constexpr const byte_t CURRENT_PLAYING_MAP[20] = "\xBE\xCC\xCC\xCC\xCC\x68\xCC\xCC\xCC\xCC\x56\xE8\xCC\xCC\xCC\xCC\x83\xC4\x20";
        constexpr const byte_t VEC3_CURRENT_WEAPON_END_TRAJECTORY[25] = "\x68\xCC\xCC\xCC\xCC\xE8\xCC\xCC\xCC\xCC\x83\xC4\x04\xE8\xCC\xCC\xCC\xCC\xE8\xCC\xCC\xCC\xCC\xE8";
        constexpr const byte_t MODEL_VIEW_PROJECTION_MATRIX[28] = "\xBE\xCC\xCC\xCC\xCC\x8D\x7C\x24\x18\xF3\xA5\x8D\x4C\x24\x18\xE8\xCC\xCC\xCC\xCC\x68\x02\x17\x00\x00\xFF\x15";
    }
    namespace function {
        constexpr const byte_t TRACE_LINE[10] = "\x8B\x4C\x24\x08\x8B\x54\x24\x0C\x83";
        constexpr const byte_t INTERSECT[12] = "\x55\x8B\xEC\x83\xE4\xF8\xD9\xEE\x83\xEC\x20";
        constexpr const byte_t POPUP_MESSAGE[11] = "\x81\xEC\x04\x01\x00\x00\x8B\x8C\x24\x08";
        constexpr const byte_t IS_VISIBLE[12] = "\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x10\xD9\xEE";
        constexpr const byte_t UNKNOWN[16] = "\x55\x8B\xEC\x83\xE4\xF8\x81\xEC\x14\x01\x00\x00\x53\x56\x8B";
        constexpr const byte_t SHOOT[159] = "\x8B\x3D\xCC\xCC\xCC\xCC\x3B\xC7\x7C\x02\x8B\xC7\x2B\xC8\x8B\xF9\x52\x8B\xDF\xE8\xCC\xCC\xCC\xCC\x8B\x4E\x14\x33\xDB\x83\xC4\x04\x39\x19\x75\x71\x6A\x18\x68\xCC\xCC\xCC\xCC\x6A\x0B\xE8\xCC\xCC\xCC\xCC\xD9\xEE\x83\xC4\x0C\x53\x51\xD9\x1C\x24\x6A\x01\x8D\x44\x24\x1C\x8D\x4B\x18\xBF\xCC\xCC\xCC\xCC\x89\x5C\x24\x20\xC7\x44\x24\xCC\xCC\xCC\xCC\xCC\xE8\xCC\xCC\xCC\xCC\x8B\x46\x18\x81\x00\xFA\x00\x00\x00\x8B\x56\x08\x89\x9A\x84\x03\x00\x00\x89\x5E\x1C\x39\x1D\xCC\xCC\xCC\xCC\x0F\x84\x79\xFF\xFF\xFF\x8B\x46\x08\x3B\x05\xCC\xCC\xCC\xCC\x0F\x85\x6A\xFF\xFF\xFF\x8B\x4E\x14\x39\x19\xE9\x53\xFF\xFF\xFF\x8B\x4E\x08\x89\xB1\x84\x03\x00\x00";
    }
}

export namespace signatures {
    namespace code {
        constexpr const SignatureDataEx_t<sizeof(patterns::code::HEALTH)> HEALTH = SignatureDataEx_t<sizeof(patterns::code::HEALTH)>{ patterns::code::HEALTH, "xxxxx" };
    }
    namespace pointer {
        constexpr const SignatureDataEx_t<sizeof(patterns::pointer::FPS)> FPS = SignatureDataEx_t<sizeof(patterns::pointer::FPS)>{ patterns::pointer::FPS, "xx????xxxxxx" };
        constexpr const SignatureDataEx_t<sizeof(patterns::pointer::LOCAL_PLAYER)> LOCAL_PLAYER = SignatureDataEx_t<sizeof(patterns::pointer::LOCAL_PLAYER)>{ patterns::pointer::LOCAL_PLAYER, "xx????xxx" };
        constexpr const SignatureDataEx_t<sizeof(patterns::pointer::CURRENT_PLAYING_MAP)> CURRENT_PLAYING_MAP = SignatureDataEx_t<sizeof(patterns::pointer::CURRENT_PLAYING_MAP)>{ patterns::pointer::CURRENT_PLAYING_MAP, "x????x????xx????xxx" };
        constexpr const SignatureDataEx_t<sizeof(patterns::pointer::ENTITY_LIST)> ENTITY_LIST = SignatureDataEx_t<sizeof(patterns::pointer::ENTITY_LIST)>{ patterns::pointer::ENTITY_LIST, "x????x????xx????xx" };
        constexpr const SignatureDataEx_t<sizeof(patterns::pointer::PLAYER_IN_GAME)> PLAYER_IN_GAME = SignatureDataEx_t<sizeof(patterns::pointer::PLAYER_IN_GAME)>{ patterns::pointer::PLAYER_IN_GAME, "xx????xx" };
        constexpr const SignatureDataEx_t<sizeof(patterns::pointer::MODEL_VIEW_PROJECTION_MATRIX)> MODEL_VIEW_PROJECTION_MATRIX = SignatureDataEx_t<sizeof(patterns::pointer::MODEL_VIEW_PROJECTION_MATRIX)>{ patterns::pointer::MODEL_VIEW_PROJECTION_MATRIX, "x????xxxxxxxxxxx????xxxxxxx" };
        constexpr const SignatureDataEx_t<sizeof(patterns::pointer::VEC3_CURRENT_WEAPON_END_TRAJECTORY)> VEC3_CURRENT_WEAPON_END_TRAJECTORY = SignatureDataEx_t<sizeof(patterns::pointer::VEC3_CURRENT_WEAPON_END_TRAJECTORY)>{ patterns::pointer::VEC3_CURRENT_WEAPON_END_TRAJECTORY, "x????x????xxxx????x????x" };
    }
    namespace function {
        constexpr const SignatureDataEx_t<sizeof(patterns::function::UNKNOWN)> UNKNOWN = SignatureDataEx_t<sizeof(patterns::function::UNKNOWN)>{ patterns::function::UNKNOWN, "xxxxxxxxxxxxxxx" };
        constexpr const SignatureDataEx_t<sizeof(patterns::function::INTERSECT)> INTERSECT = SignatureDataEx_t<sizeof(patterns::function::INTERSECT)>{ patterns::function::INTERSECT, "xxxxxxxxxxx" };
        constexpr const SignatureDataEx_t<sizeof(patterns::function::TRACE_LINE)> TRACE_LINE = SignatureDataEx_t<sizeof(patterns::function::TRACE_LINE)>{ patterns::function::TRACE_LINE, "xxxxxxxxx" };
        constexpr const SignatureDataEx_t<sizeof(patterns::function::IS_VISIBLE)> IS_VISIBLE = SignatureDataEx_t<sizeof(patterns::function::IS_VISIBLE)>{ patterns::function::IS_VISIBLE, "xxxxxxxxxxx" };
        constexpr const SignatureDataEx_t<sizeof(patterns::function::POPUP_MESSAGE)> POPUP_MESSAGE = SignatureDataEx_t<sizeof(patterns::function::POPUP_MESSAGE)>{ patterns::function::POPUP_MESSAGE,"xxxxxxxxxx" };
        constexpr const SignatureDataEx_t<sizeof(patterns::function::SHOOT)> SHOOT = SignatureDataEx_t<sizeof(patterns::function::SHOOT)>{ patterns::function::SHOOT, "xx????xxxxxxxxxxxxxx????xxxxxxxxxxxxxxx????xxx????xxxxxxxxxxxxxxxxxxxx????xxxxxxx?????x????xxxxxxxxxxxxxxxxxxxxxxx????xxxxxxxxxxx????xxxxxxxxxxxxxxxxxxxxxxxxx" };
    }
}