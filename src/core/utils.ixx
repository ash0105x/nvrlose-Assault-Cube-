export module utils;

import <sal.h>;
import <vector>;
import <cstdint>;
import <array>;

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
			const std::array<std::ptrdiff_t, t_OffsetCount>& arrOffsets
		) noexcept
		{
			BYTE* bypDMAAddress = static_cast<BYTE*>(vpStart);

			for (const std::ptrdiff_t& refOffset : arrOffsets) {
				bypDMAAddress = *reinterpret_cast<BYTE* const* const>(bypDMAAddress);
				bypDMAAddress += refOffset;
			}

			return bypDMAAddress;
		}
	}

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
		constexpr decltype(auto) error = &utils::messagebox::errorA;
#endif // UNICODE
	}
	namespace dll {
		__declspec(noreturn) void eject(
			_In_ void* const vpInstDLL,
			_In_ DWORD dwExitCode
		) noexcept;
	}
}