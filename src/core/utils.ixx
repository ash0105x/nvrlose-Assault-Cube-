export module utils;

import <sal.h>;
import <vector>;
import <cstdint>;
import <array>;

export namespace utils {
	namespace memory {
		[[nodiscard]]
		_Check_return_
		bool detour32_s(
			_In_ std::uint8_t* const bypAddress,
			_In_ const std::uint8_t* const bypNewFunction,
			_In_ const std::size_t length
		) noexcept;

		[[nodiscard]]
		_Check_return_
		bool detour32(
			_In_ std::uint8_t* const bypAddress,
			_In_ const std::uint8_t* const bypNewFunction,
			_In_ const std::size_t length
		) noexcept;

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
			std::uint8_t* bypDMAAddress = static_cast<std::uint8_t*>(vpStart);

			for (const std::ptrdiff_t& refOffset : arrOffsets) {
				bypDMAAddress = *reinterpret_cast<std::uint8_t* const* const>(bypDMAAddress);
				bypDMAAddress += refOffset;
			}

			return bypDMAAddress;
		}

		[[nodiscard]]
		std::ptrdiff_t calculateRelativeOffset(
			_In_ const std::uint8_t* const pFrom,
			_In_ const std::uint8_t* const pTo
		) noexcept;
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
			_In_ unsigned long int dwExitCode
		) noexcept;
	}

	namespace x86asm {
		constexpr std::uint8_t jmp = 0xE9u;
		constexpr std::uint8_t pushad = 0x60u;
		constexpr std::uint8_t pushfd = 0x9Cu;
		constexpr std::uint8_t call = 0xE8u;
		constexpr std::uint8_t popfd = 0x9Du;
		constexpr std::uint8_t popad = 0x61u;
	}
}