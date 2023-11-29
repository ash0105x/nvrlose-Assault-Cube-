import CDetour32;

#include<Windows.h>

import<cstdint>;

#include<cassert>
#include<exception>

import utils;

[[nodiscard]] CDetour32::CDetour32(
	[[maybe_unused]] _In_ std::nullptr_t
) noexcept
{ }

[[nodiscard]] CDetour32::CDetour32(
	_In_ void* const vpHookAddress,
	_In_ const size_t hookLength
) noexcept
	:
	m_bypHookAddress(static_cast<std::uint8_t*>(vpHookAddress)),
	m_HookLength(hookLength)
{ }

CDetour32::CDetour32(
	_Inout_ CDetour32&& other
) noexcept
	:
	m_bypHookAddress(other.m_bypHookAddress),
	m_HookLength(other.m_HookLength),
	m_byArrStolenBytes(other.m_byArrStolenBytes)
{
	other.m_byArrStolenBytes = nullptr;
}

CDetour32& CDetour32::operator=(
	_Inout_ CDetour32&& other
) noexcept
{
	if (this == &other) {
		return *this;
	}

	this->m_bypHookAddress = other.m_bypHookAddress;
	this->m_HookLength = other.m_HookLength;
	this->m_byArrStolenBytes = nullptr;

	other.m_byArrStolenBytes = nullptr;

	return *this;
}

CDetour32::~CDetour32(void) noexcept {
	this->detach();
}

[[nodiscard]]
_Check_return_
_Success_(return != nullptr)
const void* const CDetour32::attach(
	_In_ const void* const vpNewFunction
) noexcept
{
	assert(
		vpNewFunction &&
		utils::memory::isExecutableRegion(vpNewFunction) &&
		!this->m_byArrStolenBytes &&
		this->m_HookLength >= 5u &&
		this->m_bypHookAddress &&
		utils::memory::isExecutableRegion(this->m_bypHookAddress)
	);

	try {
		this->m_byArrStolenBytes = new std::uint8_t[this->m_HookLength];
	}
	catch (const std::bad_alloc&) {
		return nullptr;
	}

	memcpy_s(
		this->m_byArrStolenBytes,
		this->m_HookLength,
		this->m_bypHookAddress,
		this->m_HookLength
	);

	return(
		utils::memory::detour32(
			this->m_bypHookAddress,
			static_cast<const std::uint8_t* const>(vpNewFunction),
			this->m_HookLength
		) ?
		reinterpret_cast<const void* const>(this->m_bypHookAddress + this->m_HookLength) :
		nullptr
	);
}

_Success_(return == true)
bool CDetour32::detach(void) noexcept {
	assert(
		this->m_HookLength >= 5u &&
		this->m_bypHookAddress
	);

	if (!this->m_byArrStolenBytes) {
		return false;
	}

	DWORD dwPreviousProtection = NULL;
	if (
		!VirtualProtect(
			this->m_bypHookAddress,
			this->m_HookLength,
			PAGE_EXECUTE_READWRITE,
			&dwPreviousProtection
		)
	)
	{
		delete[] this->m_byArrStolenBytes;
		this->m_byArrStolenBytes = nullptr;

		return false;
	}

	memcpy_s(
		this->m_bypHookAddress,
		this->m_HookLength,
		this->m_byArrStolenBytes,
		this->m_HookLength
	);

	DWORD dwTempProtection = NULL;
	VirtualProtect(
		this->m_bypHookAddress,
		this->m_HookLength,
		dwPreviousProtection,
		&dwTempProtection
	);

	delete[] this->m_byArrStolenBytes;
	this->m_byArrStolenBytes = nullptr;

	return true;
}