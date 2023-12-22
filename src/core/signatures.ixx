export module signatures;

import<sal.h>;
import<regex>;

typedef unsigned char byte_t;

export template<size_t t_patternLength>
using SignatureData_t = const std::pair<const byte_t(&)[t_patternLength], const char(&)[t_patternLength]>;

export typedef enum : std::uint8_t {
    SIGNATURE_DATA_INDEX_PATTERN = NULL,
    SIGNATURE_DATA_INDEX_MASK
}SIGNATURE_DATA_INDEX;

namespace patterns {
    namespace code {
        constexpr const byte_t health[6] = "\x29\x7B\x04\x8B\xC7";
    }
    namespace function {
        constexpr const byte_t shoot[159] = "\x8B\x3D\xCC\xCC\xCC\xCC\x3B\xC7\x7C\x02\x8B\xC7\x2B\xC8\x8B\xF9\x52\x8B\xDF\xE8\xCC\xCC\xCC\xCC\x8B\x4E\x14\x33\xDB\x83\xC4\x04\x39\x19\x75\x71\x6A\x18\x68\xCC\xCC\xCC\xCC\x6A\x0B\xE8\xCC\xCC\xCC\xCC\xD9\xEE\x83\xC4\x0C\x53\x51\xD9\x1C\x24\x6A\x01\x8D\x44\x24\x1C\x8D\x4B\x18\xBF\xCC\xCC\xCC\xCC\x89\x5C\x24\x20\xC7\x44\x24\xCC\xCC\xCC\xCC\xCC\xE8\xCC\xCC\xCC\xCC\x8B\x46\x18\x81\x00\xFA\x00\x00\x00\x8B\x56\x08\x89\x9A\x84\x03\x00\x00\x89\x5E\x1C\x39\x1D\xCC\xCC\xCC\xCC\x0F\x84\x79\xFF\xFF\xFF\x8B\x46\x08\x3B\x05\xCC\xCC\xCC\xCC\x0F\x85\x6A\xFF\xFF\xFF\x8B\x4E\x14\x39\x19\xE9\x53\xFF\xFF\xFF\x8B\x4E\x08\x89\xB1\x84\x03\x00\x00";
        constexpr const byte_t popupMessage[11] = "\x81\xEC\x04\x01\x00\x00\x8B\x8C\x24\x08";
        constexpr const byte_t traceLine[10] = "\x8B\x4C\x24\x08\x8B\x54\x24\x0C\x83";
        constexpr const byte_t isVisible[12] = "\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x10\xD9\xEE";
        constexpr const byte_t intersect[12] = "\x55\x8B\xEC\x83\xE4\xF8\xD9\xEE\x83\xEC\x20";
        constexpr const byte_t unknown[16] = "\x55\x8B\xEC\x83\xE4\xF8\x81\xEC\x14\x01\x00\x00\x53\x56\x8B";
    }
}

export namespace signatures {
    namespace code {
        constexpr const SignatureData_t<sizeof(patterns::code::health)> health = SignatureData_t<sizeof(patterns::code::health)>{ patterns::code::health, "xxxxx" };
    }
    namespace function {
        constexpr const SignatureData_t<sizeof(patterns::function::shoot)> shoot = SignatureData_t<sizeof(patterns::function::shoot)>{ patterns::function::shoot, "xx????xxxxxxxxxxxxxx????xxxxxxxxxxxxxxx????xxx????xxxxxxxxxxxxxxxxxxxx????xxxxxxx?????x????xxxxxxxxxxxxxxxxxxxxxxx????xxxxxxxxxxx????xxxxxxxxxxxxxxxxxxxxxxxxx" };
        constexpr const SignatureData_t<sizeof(patterns::function::popupMessage)> popupMessage = SignatureData_t<sizeof(patterns::function::popupMessage)>{ patterns::function::popupMessage,"xxxxxxxxxx" };
        constexpr const SignatureData_t<sizeof(patterns::function::traceLine)> traceLine = SignatureData_t<sizeof(patterns::function::traceLine)>{ patterns::function::traceLine, "xxxxxxxxx" };
        constexpr const SignatureData_t<sizeof(patterns::function::isVisible)> isVisible = SignatureData_t<sizeof(patterns::function::isVisible)>{ patterns::function::isVisible, "xxxxxxxxxxx" };
        constexpr const SignatureData_t<sizeof(patterns::function::intersect)> intersect = SignatureData_t<sizeof(patterns::function::intersect)>{ patterns::function::intersect, "xxxxxxxxxxx" };
        constexpr const SignatureData_t<sizeof(patterns::function::unknown)> unknown = SignatureData_t<sizeof(patterns::function::unknown)>{ patterns::function::unknown, "xxxxxxxxxxxxxxx" };
    }
}