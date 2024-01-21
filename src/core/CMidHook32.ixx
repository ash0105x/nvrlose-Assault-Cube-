#include<Windows.h>

export module CMidHook32;

import<cstdint>;
import<cassert>;

import IHook;
import utils;

export typedef enum class _MID_HOOK_ORDER : std::uint8_t {
	MID_HOOK_ORDER_STOLEN_BYTES_FIRST,
	MID_HOOK_ORDER_STOLEN_BYTES_LAST,
	MID_HOOK_ORDER_STOLEN_BYTES_DISCARD
}MID_HOOK_ORDER;

export template<MID_HOOK_ORDER midHookOrder> class CMidHook32 final : public IHook {
public:
	[[nodiscard]] explicit CMidHook32(
		_In_ void* const vpHookAddress,
		_In_ const std::uint8_t uHookLength
	) noexcept
		:
		m_vpHookAddress(vpHookAddress),
		m_uHookLength(uHookLength)
	{ }
	CMidHook32(
		const CMidHook32&
	) = delete;
	CMidHook32(
		_Inout_ CMidHook32&& other
	) noexcept
		:
		m_vpHookAddress(other.m_vpHookAddress),
		m_uHookLength(other.m_uHookLength),
		m_byArrStolenBytes(other.m_byArrStolenBytes)
	{
		other.m_byArrStolenBytes = nullptr;
	}
public:
	virtual ~CMidHook32( void ) noexcept override {
		this->detach();
	}
public:
	CMidHook32& operator=(
		const CMidHook32&
	) = delete;
	CMidHook32& operator=(
		_Inout_ CMidHook32&& other
	) noexcept
	{
		if (this == &other) {
			return *this;
		}

		this->m_vpHookAddress = other.m_vpHookAddress;
		this->m_uHookLength = other.m_uHookLength;
		this->m_byArrStolenBytes = other.m_byArrStolenBytes;

		other.m_byArrStolenBytes = nullptr;

		return *this;
	}
public:
	[[nodiscard]]
	_Ret_maybenull_
	_Check_return_
	_Success_(return != nullptr)
	virtual const void* const attach(
		_In_ const void* const vpNewFunction
	) noexcept override
	{
		assert(
			vpNewFunction &&
			utils::memory::isExecutableRegion(vpNewFunction) &&
			!this->m_byArrStolenBytes &&
			this->m_uHookLength >= 5u &&
			this->m_vpHookAddress &&
			utils::memory::isExecutableRegion(this->m_vpHookAddress)
		);

		if (
			!(
				this->m_byArrStolenBytes = static_cast<std::uint8_t*>(
					VirtualAlloc(
						nullptr,
						this->m_uHookLength + 0xE,
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
			MID_HOOK_ORDER::MID_HOOK_ORDER_STOLEN_BYTES_FIRST == midHookOrder ?
			this->m_byArrStolenBytes :
			MID_HOOK_ORDER::MID_HOOK_ORDER_STOLEN_BYTES_LAST == midHookOrder ?
			this->m_byArrStolenBytes + 0x9 :
			// MID_HOOK_ORDER::MID_HOOK_ORDER_STOLEN_BYTES_DISCARD == midHookOrder
			this->m_byArrStolenBytes + 0xE,
			this->m_uHookLength,
			this->m_vpHookAddress,
			this->m_uHookLength
		);

		const std::uint8_t uPrepareFunctionCallOffset = (
			MID_HOOK_ORDER::MID_HOOK_ORDER_STOLEN_BYTES_FIRST == midHookOrder ?
			this->m_uHookLength :
			0x0
		);

		this->m_byArrStolenBytes[uPrepareFunctionCallOffset] = utils::x86asm::PUSHAD;
		this->m_byArrStolenBytes[uPrepareFunctionCallOffset + 0x1] = utils::x86asm::PUSHFD;

		this->m_byArrStolenBytes[uPrepareFunctionCallOffset + 0x2] = utils::x86asm::CALL;
		*reinterpret_cast<DWORD* const>(this->m_byArrStolenBytes + uPrepareFunctionCallOffset + 0x3) = utils::memory::calculateRelativeOffset(
			this->m_byArrStolenBytes + uPrepareFunctionCallOffset + 0x2,
			static_cast<const std::uint8_t* const>(vpNewFunction)
		);

		this->m_byArrStolenBytes[uPrepareFunctionCallOffset + 0x7] = utils::x86asm::POPFD;
		this->m_byArrStolenBytes[uPrepareFunctionCallOffset + 0x8] = utils::x86asm::POPAD;

		utils::memory::placeJump(
			this->m_byArrStolenBytes +
			(
				MID_HOOK_ORDER::MID_HOOK_ORDER_STOLEN_BYTES_DISCARD == midHookOrder ?
				0x9 :
				0x9 + this->m_uHookLength
			),
			static_cast<const std::uint8_t* const>(this->m_vpHookAddress) + this->m_uHookLength
		);

		return(
			utils::memory::detour32(
				static_cast<std::uint8_t* const>(this->m_vpHookAddress),
				this->m_byArrStolenBytes,
				this->m_uHookLength
			) ?
			this->m_byArrStolenBytes :
			nullptr
		);
	}

	_Success_(return == true)
	virtual bool detach( void ) noexcept override {
		assert(this->m_uHookLength >= 5u);

		if (!this->m_vpHookAddress || !this->m_byArrStolenBytes) {
			return false;
		}

		const auto invalidateStolenBytes = [this](void) noexcept {
			VirtualFree(
				this->m_byArrStolenBytes,
				NULL,
				MEM_FREE
			);

			this->m_byArrStolenBytes = nullptr;
		};

		DWORD dwPreviousProtection = NULL;
		if (
			!(
				VirtualProtect(
					this->m_vpHookAddress,
					this->m_uHookLength + 0xE,
					PAGE_EXECUTE_READWRITE,
					&dwPreviousProtection
				)
			)
		)
		{
			invalidateStolenBytes();
			return false;
		}

		memcpy_s(
			this->m_vpHookAddress,
			this->m_uHookLength,
			MID_HOOK_ORDER::MID_HOOK_ORDER_STOLEN_BYTES_FIRST == midHookOrder ?
			this->m_byArrStolenBytes :
			MID_HOOK_ORDER::MID_HOOK_ORDER_STOLEN_BYTES_LAST == midHookOrder ?
			this->m_byArrStolenBytes + 0x9 :
			this->m_byArrStolenBytes + 0xE,
			this->m_uHookLength
		);

		DWORD dwTempProtection = NULL;
		VirtualProtect(
			this->m_vpHookAddress,
			this->m_uHookLength + 0xE,
			dwPreviousProtection,
			&dwTempProtection
		);

		invalidateStolenBytes();
		return true;
	}
private:
	void* m_vpHookAddress = nullptr;
	std::uint8_t m_uHookLength = NULL;
	std::uint8_t* m_byArrStolenBytes = nullptr;
};