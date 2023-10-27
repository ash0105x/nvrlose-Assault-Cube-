import utils;

//import <assert.h>;
//import <iostream>;

#include"win32api.h"

#include<iostream>
#include<assert.h>

import globals;

[[nodiscard]]
_Must_inspect_result_
void* const utils::memory::findDMAAddress(
	void* const vpStart,
	const std::vector<std::ptrdiff_t>& offsets
) noexcept
{
	BYTE* bpDMAddress = static_cast<BYTE*>(vpStart);

	for (const std::ptrdiff_t& refOffset : offsets) {
		bpDMAddress = *reinterpret_cast<BYTE* const* const>(bpDMAddress);
		bpDMAddress += refOffset;
	}

	return bpDMAddress;
}

void utils::messagebox::errorA(
	_In_z_ const char* const cstrBody
) noexcept
{
	assert(cstrBody && cstrBody[NULL] != '\0');

	MessageBoxA(
		nullptr,
		"Error!",
		cstrBody,
		(MB_OK | MB_ICONERROR)
	);
}

void utils::messagebox::errorW(
	_In_z_ const wchar_t* const wcstrBody
) noexcept
{
	assert(wcstrBody && wcstrBody[NULL] != L'\0');

	MessageBoxW(
		nullptr,
		L"Error!",
		wcstrBody,
		(MB_OK | MB_ICONERROR)
	);
}

__declspec(noreturn) void utils::dll::eject(
	_In_ void* const vpInstDLL,
	_In_ DWORD dwExitCode
) noexcept
{
	(*globals::function::pointer::pPopupMessage)("Exitting...");

	FreeLibraryAndExitThread(static_cast<const HMODULE>(vpInstDLL), dwExitCode);
}