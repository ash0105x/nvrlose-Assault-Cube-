export module utils;

import <sal.h>;
import <vector>;

typedef unsigned long int DWORD;
static_assert(sizeof(DWORD) == 4u);

typedef unsigned char BYTE;
static_assert(sizeof(BYTE) == 1u);

export namespace utils {
	namespace memory {
		[[nodiscard]]
		_Must_inspect_result_
		void* const findDMAAddress(
			void* const vpStart,
			const std::vector<std::ptrdiff_t>& offsets
		) noexcept;

		template<const std::size_t t_OffsetCount>
		[[nodiscard]]
		_Must_inspect_result_
		void* const findDMAAddress(
			void* const vpStart,
			const std::ptrdiff_t(&arrOffsets)[t_OffsetCount]
		) noexcept
		{
			BYTE* bpDMAAddress = static_cast<BYTE*>(vpStart);

			for (const std::ptrdiff_t& refOffset : arrOffsets) {
				bpDMAAddress = *reinterpret_cast<BYTE* const* const>(bpDMAAddress);
				bpDMAAddress += refOffset;
			}

			return bpDMAAddress;
		}
	}

	/*namespace entity {
		CPlayer& fromIndex(
			_In_ const std::uint8_t uIndex
		) noexcept
		{
			return *reinterpret_cast<CPlayer* const>(globals::entity::entityList + (uIndex * sizeof(std::uintptr_t)));
		}
	}*/

	namespace messagebox {
		void errorA(
			_In_z_ const char* const cstrBody
		) noexcept;

		void errorW(
			_In_z_ const wchar_t* const wcstrBody
		) noexcept;

#ifdef UNICODE
		constexpr decltype(auto) error = &utils::messagebox::errorW;
#else
		constexpr decltype(auto) error = &utils::messagebox::errorA
#endif // UNICODE
	}
	namespace dll {
		__declspec(noreturn) void eject(
			_In_ void* const vpInstDLL,
			_In_ DWORD dwExitCode
		) noexcept;
	}
}