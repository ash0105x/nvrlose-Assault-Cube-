import CConsole;

#include"win32api.h"

import <iostream>;

[[nodiscard]] CConsole::CConsole(
	_In_z_ const decltype(__TEXT(' '))* const pTitle
) noexcept
{
	if (!(this->m_bConsoleAllocated = AllocConsole())) {
		return;
	}

	SetConsoleTitle(pTitle);

	freopen_s(
		&this->m_pOutFile,
		"CONOUT$",
		"w",
		stdout
	);

	this->m_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	freopen_s(
		&this->m_pInFile,
		"CONIN$",
		"r",
		stdin
	);
}

CConsole::~CConsole(void) noexcept {
	this->Release();
}

void CConsole::Release(void) noexcept {
	if (this->m_bConsoleAllocated) {
		this->m_bConsoleAllocated = !(static_cast<const bool>(FreeConsole()));
	}

	if (this->m_pOutFile) {
		fclose(this->m_pOutFile);
		this->m_pOutFile = nullptr;
	}

	if (!this->m_pInFile) {
		return;
	}

	fclose(this->m_pInFile);
	this->m_pInFile = nullptr;
}

[[nodiscard]]
_Check_return_
_Success_(return == true)
const bool& CConsole::consoleCreated(void) const noexcept {
	return this->m_bConsoleAllocated;
}

[[nodiscard]]
_Check_return_
_Success_(return == true)
bool CConsole::isReadable(void) const noexcept {
	return static_cast<const bool>(this->m_pInFile);
}

[[nodiscard]]
_Check_return_
_Success_(return == true)
bool CConsole::isWritable(void) const noexcept {
	return static_cast<const bool>(this->m_pOutFile);
}

void CConsole::resetInputBuffer(void) noexcept {
	std::cin.clear();
	std::cin.ignore();
}