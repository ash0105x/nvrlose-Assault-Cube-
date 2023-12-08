export module signatures;

import<sal.h>;

typedef unsigned char byte_t;

template<size_t t_patternLength> struct SignatureData final {
    const byte_t (&pattern)[t_patternLength];
    const char (&cstrMask)[t_patternLength];
};

namespace patterns {
    namespace code {
        constexpr const byte_t health[] = "\x29\x7B\x04\x8B\xC7";
    }
    namespace function {
        constexpr const byte_t shoot[] = "\x8B\x3D\xCC\xCC\xCC\xCC\x3B\xC7\x7C\x02\x8B\xC7\x2B\xC8\x8B\xF9\x52\x8B\xDF\xE8\xCC\xCC\xCC\xCC\x8B\x4E\x14\x33\xDB\x83\xC4\x04\x39\x19\x75\x71\x6A\x18\x68\xCC\xCC\xCC\xCC\x6A\x0B\xE8\xCC\xCC\xCC\xCC\xD9\xEE\x83\xC4\x0C\x53\x51\xD9\x1C\x24\x6A\x01\x8D\x44\x24\x1C\x8D\x4B\x18\xBF\xCC\xCC\xCC\xCC\x89\x5C\x24\x20\xC7\x44\x24\xCC\xCC\xCC\xCC\xCC\xE8\xCC\xCC\xCC\xCC\x8B\x46\x18\x81\x00\xFA\x00\x00\x00\x8B\x56\x08\x89\x9A\x84\x03\x00\x00\x89\x5E\x1C\x39\x1D\xCC\xCC\xCC\xCC\x0F\x84\x79\xFF\xFF\xFF\x8B\x46\x08\x3B\x05\xCC\xCC\xCC\xCC\x0F\x85\x6A\xFF\xFF\xFF\x8B\x4E\x14\x39\x19\xE9\x53\xFF\xFF\xFF\x8B\x4E\x08\x89\xB1\x84\x03\x00\x00";
        constexpr const byte_t popupMessage[] = "\x81\xEC\x04\x01\x00\x00\x8B\x8C\x24\x08";
        constexpr const byte_t traceLine[] = "\x8B\x4C\x24\x08\x8B\x54\x24\x0C\x83";
        constexpr const byte_t isVisible[] = "\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x10\xD9\xEE";
        constexpr const byte_t intersect[] = "\x55\x8B\xEC\x83\xE4\xF8\xD9\xEE\x83\xEC\x20";
    }
}

export namespace signatures {
    namespace code {
        constexpr SignatureData health{ patterns::code::health, "xxxxx" };
    }
    namespace function {
        constexpr SignatureData shoot{ patterns::function::shoot, "xx????xxxxxxxxxxxxxx????xxxxxxxxxxxxxxx????xxx????xxxxxxxxxxxxxxxxxxxx????xxxxxxx?????x????xxxxxxxxxxxxxxxxxxxxxxx????xxxxxxxxxxx????xxxxxxxxxxxxxxxxxxxxxxxxx" };
        constexpr SignatureData popupMessage{ patterns::function::popupMessage,"xxxxxxxxxx" };
        constexpr SignatureData traceLine{ patterns::function::traceLine, "xxxxxxxxx" };
        constexpr SignatureData isVisible{ patterns::function::isVisible, "xxxxxxxxxxx" };
        constexpr SignatureData intersect{ patterns::function::intersect, "xxxxxxxxxxx" };
    }
}