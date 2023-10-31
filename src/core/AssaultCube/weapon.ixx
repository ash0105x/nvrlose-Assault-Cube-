export module weapon;

import <cstdint>;

export class CPlayer;

// Created with ReClass.NET 1.2 by KN4CK3R

export import gun;

export class weapon {
public:
	gun* const vpVTable = nullptr; //0x0000
	std::uint8_t uUniqueID = 0u; //0x0004
private:
	const char pad_0005[3u] = { }; //0x0005
public:
	CPlayer* pOwner = nullptr; //0x0008
	char* cstrName = nullptr; //0x000C
	std::uint32_t* const upReservedAmmo = nullptr; //0x0010
	std::uint32_t* const upAmmo = nullptr; //0x0014
	std::uint32_t* const upFirerate = nullptr; //0x0018
	std::uint32_t uShotsFired = 0u; //0x001C
	std::uint32_t uReloadTime = 0u; //0x0020
}; //Size: 0x0024
static_assert(sizeof(weapon) == 0x24u);