import CTrampolineHook32;

import <sal.h>;

#include"win32api.h"

#include<exception>

#define CALCULATE_RELATIVE_OFFSET(FROM, TO) (reinterpret_cast<const std::uintptr_t>(TO) - reinterpret_cast<const std::uintptr_t>(FROM) - 5u)

namespace x86asm {
	constexpr static BYTE jmp = 0xE9u;
}

[[nodiscard]]
CTrampolineHook32::CTrampolineHook32(
	_In_ void* vpHookAddress,
	_In_ const size_t hookLength
) noexcept
	:
	m_bpHookAddress(static_cast<BYTE* const>(vpHookAddress)),
	m_HookLength(hookLength)
{ }

CTrampolineHook32::CTrampolineHook32(
	_Inout_ CTrampolineHook32&& other
) noexcept
	:
	m_bpHookAddress(other.m_bpHookAddress),
	m_HookLength(other.m_HookLength),
	m_bpGateway(other.m_bpGateway)
{
	other.m_bpGateway = nullptr;
}

CTrampolineHook32& CTrampolineHook32::operator=(
	_Inout_ CTrampolineHook32&& other
) noexcept
{
	if (this == &other) {
		return *this;
	}

	this->m_bpHookAddress = other.m_bpHookAddress;
	this->m_HookLength = other.m_HookLength;

	this->m_bpGateway = other.m_bpGateway;
	other.m_bpGateway = nullptr;

	return *this;
}

CTrampolineHook32::~CTrampolineHook32(void) noexcept {
	this->detach();
}

[[nodiscard]]
_Check_return_
_Ret_maybenull_
_Success_(return != nullptr)
const void* const CTrampolineHook32::operator()(
	_In_ const void* const vpNewFunction
) noexcept
{
	return this->attach(vpNewFunction);
}

_Success_(return == true)
bool CTrampolineHook32::operator~(void) noexcept {
	return this->detach();
}

[[nodiscard]]
_Check_return_
_Ret_maybenull_
_Success_(return != nullptr)
const void* const CTrampolineHook32::attach(
	_In_ const void* const vpNewFunction
) noexcept
{
	if (
		this->m_HookLength < 5u ||
		this->m_bpGateway
	)
	{
		return nullptr;
	}

	if (
		!(
			this->m_bpGateway = static_cast<BYTE* const>(
				VirtualAlloc(
					nullptr,
					this->m_HookLength + 5u,
					(MEM_RESERVE | MEM_COMMIT),
					PAGE_EXECUTE_READWRITE
				)
			)
		)
	)
	{
		return nullptr;
	}

	memcpy_s(
		this->m_bpGateway,
		this->m_HookLength,
		this->m_bpHookAddress,
		this->m_HookLength
	);

	this->m_bpGateway[this->m_HookLength] = x86asm::jmp;
	*reinterpret_cast<std::uintptr_t* const>(this->m_bpGateway + this->m_HookLength + 1u) = CALCULATE_RELATIVE_OFFSET(this->m_bpGateway, this->m_bpHookAddress);

	return this->jumpFromHookAddressToNewFunction(vpNewFunction) ? this->m_bpGateway : nullptr;
}

static void releaseGatewayAndSetToNullptr(BYTE*& bpGateway) noexcept {
	if (!bpGateway) {
		return;
	}

	VirtualFree(
		bpGateway,
		NULL,
		MEM_FREE
	);

	bpGateway = nullptr;
}

_Success_(return == true)
bool CTrampolineHook32::detach(void) noexcept {
	if (!this->m_bpGateway) {
		return false;
	}
	
	DWORD dwPreviousProtection = NULL;

	if (
		!VirtualProtect(
			this->m_bpHookAddress,
			this->m_HookLength,
			PAGE_EXECUTE_READWRITE,
			&dwPreviousProtection
		)
	)
	{
		releaseGatewayAndSetToNullptr(this->m_bpGateway);

		return false;
	}

	memcpy_s(
		this->m_bpHookAddress,
		this->m_HookLength,
		this->m_bpGateway,
		this->m_HookLength
	);

	DWORD dwTempProtection = NULL;
	VirtualProtect(
		this->m_bpHookAddress,
		this->m_HookLength,
		dwPreviousProtection,
		&dwTempProtection
	);

	releaseGatewayAndSetToNullptr(this->m_bpGateway);

	return true;
}

[[nodiscard]]
_Check_return_
_Success_(return == true)
bool CTrampolineHook32::jumpFromHookAddressToNewFunction(
	_In_ const void* const vpNewFunction
) noexcept
{
	DWORD dwPreviousProtection = NULL;

	if (
		!VirtualProtect(
			this->m_bpHookAddress,
			this->m_HookLength,
			PAGE_EXECUTE_READWRITE,
			&dwPreviousProtection
		)
	)
	{
		return false;
	}

	this->m_bpHookAddress[NULL] = x86asm::jmp;
	*reinterpret_cast<std::uintptr_t* const>(this->m_bpHookAddress + 1u) = CALCULATE_RELATIVE_OFFSET(this->m_bpHookAddress, vpNewFunction);

	DWORD dwTempProtection = NULL;
	VirtualProtect(
		this->m_bpHookAddress,
		this->m_HookLength,
		dwPreviousProtection,
		&dwTempProtection
	);

	return true;
}