export module initialWeaponData;

import <cstdint>;

// name = 0x0
// initialFirerate = 0x10a
// ammo = 0x118

export class initialWeaponData final {
public:
	const char cstrName[8]; //0x0000
private:
	char pad_0008[258]; //0x0008
public:
	std::uint16_t uFirerate; //0x010A
private:
	char pad_010C[12]; //0x010C
public:
	std::uint8_t uAmmo; //0x0118
}; //Size: 0x0119
static_assert(sizeof(initialWeaponData) == 0x11A);