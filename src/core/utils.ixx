#include<Windows.h>
#include<TlHelp32.h>

export module utils;

import<sal.h>;
import<cstdint>;
import<array>;
import<tchar.h>;
import<cassert>;

import CVector2;
import CVector3;

import signatures;

export namespace utils {
	namespace memory {
		[[nodiscard]]
		_Must_inspect_result_
		_Ret_maybenull_
		_Success_(return != nullptr)
		std::uint8_t* const findSignature(
			_In_ const HMODULE hModule,
			_In_reads_(patternLength) const SignatureData_t& signatureData,
			_In_ const std::size_t patternLength
		) noexcept;

		template<std::size_t t_patternLength>
		[[nodiscard]]
		_Must_inspect_result_
		_Ret_maybenull_
		_Success_(return != nullptr)
		std::uint8_t* const findSignature(
			_In_ const HMODULE hModule,
			_In_ const SignatureDataEx_t<t_patternLength>& signatureData
		) noexcept
		{
			return utils::memory::findSignature(hModule, signatureData, t_patternLength - 1);
		}

		[[nodiscard]]
		_Check_return_
		_Success_(return == true)
		bool isExecutableRegion(
			_In_opt_ const void* const vpMemoryRegion
		) noexcept;

		[[nodiscard]]
		_Check_return_
		_Success_(return == true)
		bool detour32(
			_In_ std::uint8_t* const bypAddress,
			_In_ const std::uint8_t* const bypNewFunction,
			_In_ const std::size_t length
		) noexcept;

		template<const std::size_t t_OffsetCount>
		[[nodiscard]]
		_Must_inspect_result_
		void* const findDMAAddress(
			void* const vpStart,
			const std::array<const std::ptrdiff_t, t_OffsetCount>& arrOffsets
		) noexcept
		{
			assert(vpStart);

			std::uint8_t* bypDMAAddress = static_cast<std::uint8_t* const>(vpStart);

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

	namespace process {
		_Check_return_opt_
		_Success_(return == true)
		bool enumerate(
			_In_ bool(* const& pRefEnumFunction)(_In_ const PROCESSENTRY32& refProcessInfo32, _In_opt_ void* const vpExtraParameter) noexcept,
			_In_opt_ void* const vpExtraParameter
		) noexcept;

		[[nodiscard]]
		_Check_return_
		_Success_(return == true)
		bool isRunning(
			_In_z_ const TCHAR* const tcstrProcessName
		) noexcept;

		[[nodiscard]]
		_Check_return_
		_Ret_maybenull_z_
		_Ret_z_
		_Success_(return != nullptr)
		const TCHAR* name(
			_In_ const DWORD dwId
		) noexcept;
	}

	namespace math {
		bool worldToScreen(
			_In_ const CVector3& vec3RefWorld,
			_Out_ CVector2& vecRef2Screen
		) noexcept;
	}

	namespace x86asm {
		constexpr const std::uint8_t jmp = 0xE9u;
		constexpr const std::uint8_t call = 0xE8u;

		constexpr const std::uint8_t pushad = 0x60u;
		constexpr const std::uint8_t pushfd = 0x9Cu;

		constexpr const std::uint8_t popad = 0x61u;
		constexpr const std::uint8_t popfd = 0x9Du;
	}
}