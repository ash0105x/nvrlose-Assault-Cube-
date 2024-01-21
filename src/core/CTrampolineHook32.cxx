import CTrampolineHook32;

#include<Windows.h>
#include<exception>

import utils;

import<cassert>;

[[nodiscard]]
CTrampolineHook32::CTrampolineHook32(
	[[maybe_unused]] _In_ std::nullptr_t
) noexcept
{ }

[[nodiscard]]
CTrampolineHook32::CTrampolineHook32(
	_In_ void* const vpHookAddress,
	_In_ const size_t hookLength
) noexcept
	:
	m_bypHookAddress(static_cast<std::uint8_t* const>(vpHookAddress)),
	m_HookLength(hookLength)
{ }

CTrampolineHook32::CTrampolineHook32(
	_Inout_ CTrampolineHook32&& other
) noexcept
	:
	m_bypHookAddress(other.m_bypHookAddress),
	m_HookLength(other.m_HookLength),
	m_bypGateway(other.m_bypGateway)
{
	other.m_bypGateway = nullptr;
}

CTrampolineHook32& CTrampolineHook32::operator=(
	_Inout_ CTrampolineHook32&& other
) noexcept
{
	if (this == &other) {
		return *this;
	}

	this->m_bypHookAddress = other.m_bypHookAddress;
	this->m_HookLength = other.m_HookLength;
	this->m_bypGateway = other.m_bypGateway;

	other.m_bypGateway = nullptr;

	return *this;
}

CTrampolineHook32::~CTrampolineHook32( void ) noexcept {
	this->detach();
}

[[nodiscard]]
_Ret_maybenull_
_Check_return_
_Ret_maybenull_
_Success_(return != nullptr)
const void* const CTrampolineHook32::attach(
	_In_ const void* const vpNewFunction
) noexcept
{
	assert(
		vpNewFunction &&
		utils::memory::isExecutableRegion(vpNewFunction) &&
		this->m_HookLength >= 5u &&
		!this->m_bypGateway &&
		this->m_bypHookAddress &&
		utils::memory::isExecutableRegion(this->m_bypHookAddress)
	);

	if (
		!(
			this->m_bypGateway = static_cast<std::uint8_t* const>(
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
		this->m_bypGateway,
		this->m_HookLength,
		this->m_bypHookAddress,
		this->m_HookLength
	);

	this->m_bypGateway[this->m_HookLength] = utils::x86asm::JMP;
	*reinterpret_cast<DWORD* const>(this->m_bypGateway + this->m_HookLength + 1u) = utils::memory::calculateRelativeOffset(
		this->m_bypGateway,
		this->m_bypHookAddress
	);

	return(
		utils::memory::detour32(
			this->m_bypHookAddress,
			static_cast<const std::uint8_t* const>(vpNewFunction),
			this->m_HookLength
		) ?
		this->m_bypGateway :
		nullptr
	);
}

_Success_(return == true)
bool CTrampolineHook32::detach( void ) noexcept {
	assert(this->m_HookLength >= 5u);

	if (!this->m_bypHookAddress || !this->m_bypGateway) {
		return false;
	}

	const auto invalidateGateway = [this]( void ) noexcept {
		VirtualFree(
			this->m_bypGateway,
			NULL,
			MEM_FREE
		);

		this->m_bypGateway = nullptr;
	};

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
		invalidateGateway();

		return false;
	}

	memcpy_s(
		this->m_bypHookAddress,
		this->m_HookLength,
		this->m_bypGateway,
		this->m_HookLength
	);

	DWORD dwTempProtection = NULL;
	VirtualProtect(
		this->m_bypHookAddress,
		this->m_HookLength,
		dwPreviousProtection,
		&dwTempProtection
	);

	invalidateGateway();

	return true;
}