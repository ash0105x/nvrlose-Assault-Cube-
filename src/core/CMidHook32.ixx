#include<Windows.h>
#include<assert.h>

export module CMidHook32;

import<cstdint>;

import IHook;
import utils;

export enum class MID_HOOK_ORDER : std::uint8_t {
	MID_HOOK_ORDER_STOLEN_BYTES_FIRST,
	MID_HOOK_ORDER_STOLEN_BYTES_LAST,
	MID_HOOK_ORDER_STOLEN_BYTES_DISCARD
};

constexpr const BYTE arrPreserveStack[2u] = {
	utils::x86asm::pushad,
	utils::x86asm::pushfd
};

constexpr const BYTE arrRestoreStack[2u] = {
	utils::x86asm::popfd,
	utils::x86asm::popad
};

constexpr const std::uint8_t SIZE_OF_JMP = 5u;
constexpr const std::uint8_t SIZE_OF_CALL = ::SIZE_OF_JMP;

export template<MID_HOOK_ORDER midHookOrder> class CMidHook32 final : public IHook {
public:
	[[nodiscard]] explicit CMidHook32(
		_In_ void* const vpHookAddress,
		_In_ const size_t hookLength
	) noexcept
		:
		m_vpHookAddress(vpHookAddress),
		m_hookLength(hookLength)
	{ }
	CMidHook32(
		const CMidHook32&
	) = delete;
	CMidHook32(
		_Inout_ CMidHook32&& other
	) noexcept
		:
		m_vpHookAddress(other.m_vpHookAddress),
		m_hookLength(other.m_hookLength),
		m_byArrStolenBytes(other.m_byArrStolenBytes)
	{
		other.m_byArrStolenBytes = nullptr;
	}
public:
	virtual ~CMidHook32(void) noexcept {
		~(*this);
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
		this->m_hookLength = other.m_hookLength;
		this->m_byArrStolenBytes = other.m_byArrStolenBytes;

		other.m_byArrStolenBytes = nullptr;

		return *this;
	}
public:
	[[nodiscard]]
	_Check_return_
	_Success_(return != nullptr)
	virtual const void* const operator()(
		_In_ const void* const vpNewFunction
	) noexcept override
	{
		assert(
			vpNewFunction &&
			!this->m_byArrStolenBytes &&
			this->m_hookLength >= 5u &&
			this->m_vpHookAddress
		);

		if (
			!(
				this->m_byArrStolenBytes = static_cast<std::uint8_t*>(
					VirtualAlloc(
						nullptr,
						this->m_hookLength + 14u,
						(MEM_RESERVE | MEM_COMMIT),
						PAGE_EXECUTE_READWRITE
					)
				)
			)
		)
		{
			return nullptr;
		}

		const auto onSuccessfullyInitialization = [this, &vpNewFunction](void) noexcept -> const void* const {
			return(
				utils::memory::detour32(
					static_cast<std::uint8_t* const>(this->m_vpHookAddress),
					this->m_byArrStolenBytes,
					this->m_hookLength
				) ?
				this->m_byArrStolenBytes :
				nullptr
			);
		};

		if constexpr (midHookOrder == MID_HOOK_ORDER::MID_HOOK_ORDER_STOLEN_BYTES_FIRST) {
			memcpy_s(
				this->m_byArrStolenBytes,
				this->m_hookLength,
				this->m_vpHookAddress,
				this->m_hookLength
			);

			memcpy_s(
				this->m_byArrStolenBytes + this->m_hookLength,
				sizeof(::arrPreserveStack),
				::arrPreserveStack,
				sizeof(::arrPreserveStack)
			);

			this->m_byArrStolenBytes[this->m_hookLength + sizeof(::arrPreserveStack)] = utils::x86asm::call;
			*reinterpret_cast<DWORD* const>(this->m_byArrStolenBytes + this->m_hookLength + sizeof(::arrPreserveStack) + 1u) = utils::memory::calculateRelativeOffset(
				this->m_byArrStolenBytes + this->m_hookLength + sizeof(::arrPreserveStack),
				static_cast<const std::uint8_t* const>(vpNewFunction)
			);

			memcpy_s(
				this->m_byArrStolenBytes + this->m_hookLength + sizeof(::arrPreserveStack) + ::SIZE_OF_CALL,
				sizeof(::arrRestoreStack),
				::arrRestoreStack,
				sizeof(::arrRestoreStack)
			);

			this->m_byArrStolenBytes[this->m_hookLength + sizeof(::arrPreserveStack) + ::SIZE_OF_CALL + sizeof(::arrRestoreStack)] = utils::x86asm::jmp;
			*reinterpret_cast<DWORD* const>(this->m_byArrStolenBytes + this->m_hookLength + sizeof(::arrPreserveStack) + ::SIZE_OF_CALL + sizeof(::arrRestoreStack) + 1u) = utils::memory::calculateRelativeOffset(
				this->m_byArrStolenBytes + this->m_hookLength + sizeof(::arrPreserveStack) + ::SIZE_OF_CALL + sizeof(::arrRestoreStack),
				static_cast<const std::uint8_t* const>(this->m_vpHookAddress) + this->m_hookLength
			);
		
			return onSuccessfullyInitialization();
		}

		memcpy_s(
			this->m_byArrStolenBytes,
			sizeof(::arrPreserveStack),
			::arrPreserveStack,
			sizeof(::arrPreserveStack)
		);

		this->m_byArrStolenBytes[sizeof(::arrPreserveStack)] = utils::x86asm::call;
		*reinterpret_cast<DWORD* const>(this->m_byArrStolenBytes + sizeof(::arrPreserveStack) + 1u) = utils::memory::calculateRelativeOffset(
			this->m_byArrStolenBytes + sizeof(::arrPreserveStack),
			static_cast<const std::uint8_t* const>(vpNewFunction)
		);

		memcpy_s(
			this->m_byArrStolenBytes + sizeof(::arrPreserveStack) + ::SIZE_OF_CALL,
			sizeof(::arrRestoreStack),
			::arrRestoreStack,
			sizeof(::arrRestoreStack)
		);

		if constexpr (midHookOrder == MID_HOOK_ORDER::MID_HOOK_ORDER_STOLEN_BYTES_LAST) {
			memcpy_s(
				this->m_byArrStolenBytes + sizeof(::arrPreserveStack) + ::SIZE_OF_CALL + sizeof(::arrRestoreStack),
				this->m_hookLength,
				this->m_vpHookAddress,
				this->m_hookLength
			);

			this->m_byArrStolenBytes[sizeof(::arrPreserveStack) + ::SIZE_OF_CALL + sizeof(::arrRestoreStack) + this->m_hookLength] = utils::x86asm::jmp;
			*reinterpret_cast<DWORD* const>(this->m_byArrStolenBytes + sizeof(::arrPreserveStack) + ::SIZE_OF_CALL + sizeof(::arrRestoreStack) + this->m_hookLength + 1u) = utils::memory::calculateRelativeOffset(
				this->m_byArrStolenBytes + sizeof(::arrPreserveStack) + ::SIZE_OF_CALL + sizeof(::arrRestoreStack) + this->m_hookLength,
				static_cast<const std::uint8_t* const>(this->m_vpHookAddress) + this->m_hookLength
			);
		
			return onSuccessfullyInitialization();
		}

		this->m_byArrStolenBytes[sizeof(::arrPreserveStack) + ::SIZE_OF_CALL + sizeof(::arrRestoreStack)] = utils::x86asm::jmp;
		*reinterpret_cast<DWORD* const>(this->m_byArrStolenBytes + sizeof(::arrPreserveStack) + ::SIZE_OF_CALL + sizeof(::arrRestoreStack) + 1u) = utils::memory::calculateRelativeOffset(
			this->m_byArrStolenBytes + sizeof(::arrPreserveStack) + ::SIZE_OF_CALL + sizeof(::arrRestoreStack),
			static_cast<const std::uint8_t* const>(this->m_vpHookAddress) + this->m_hookLength
		);

		memcpy_s(
			this->m_byArrStolenBytes + sizeof(::arrPreserveStack) + ::SIZE_OF_CALL + sizeof(::arrRestoreStack) + ::SIZE_OF_JMP,
			this->m_hookLength,
			this->m_vpHookAddress,
			this->m_hookLength
		);

		return onSuccessfullyInitialization();
	}

	_Success_(return == true)
	virtual bool operator~(void) noexcept override {
		assert(
			this->m_vpHookAddress &&
			this->m_hookLength >= 5u
		);

		if (!this->m_byArrStolenBytes) {
			return false;
		}

		const auto releaseStolenBytesAndSetToNullptr = [this](void) noexcept {
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
					this->m_hookLength + sizeof(::arrPreserveStack) + ::SIZE_OF_CALL + sizeof(::arrRestoreStack) + ::SIZE_OF_JMP,
					PAGE_EXECUTE_READWRITE,
					&dwPreviousProtection
				)
			)
		)
		{
			releaseStolenBytesAndSetToNullptr();
			return false;
		}

		memcpy_s(
			this->m_vpHookAddress,
			this->m_hookLength,
			midHookOrder == MID_HOOK_ORDER::MID_HOOK_ORDER_STOLEN_BYTES_FIRST ?
			this->m_byArrStolenBytes :
			midHookOrder == MID_HOOK_ORDER::MID_HOOK_ORDER_STOLEN_BYTES_LAST ?
			this->m_byArrStolenBytes + sizeof(::arrPreserveStack) + ::SIZE_OF_CALL + sizeof(::arrRestoreStack) :
			this->m_byArrStolenBytes + sizeof(::arrPreserveStack) + ::SIZE_OF_CALL + sizeof(::arrRestoreStack) + ::SIZE_OF_JMP,
			this->m_hookLength
		);

		DWORD dwTempProtection = NULL;
		VirtualProtect(
			this->m_vpHookAddress,
			this->m_hookLength + sizeof(::arrPreserveStack) + ::SIZE_OF_CALL + sizeof(::arrRestoreStack) + ::SIZE_OF_JMP,
			dwPreviousProtection,
			&dwTempProtection
		);

		releaseStolenBytesAndSetToNullptr();
		return true;
	}
public:
	[[nodiscard]]
	_Check_return_
	_Success_(return != nullptr)
	virtual const void* const attach(
		_In_ const void* const vpNewFunction
	) noexcept override
	{
		return (*this)(vpNewFunction);
	}

	_Success_(return == true)
	virtual bool detach(void) noexcept override {
		return ~(*this);
	}
private:
	void* m_vpHookAddress = nullptr;
	size_t m_hookLength = NULL;
	std::uint8_t* m_byArrStolenBytes = nullptr;
};