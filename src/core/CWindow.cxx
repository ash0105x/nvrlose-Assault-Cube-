import CWindow;

#include<Windows.h>
#include<exception>

import<type_traits>;
import<cassert>;

[[nodiscard]]
CWindow::CWindow(
	[[maybe_unused]] _In_ const std::nullptr_t
) noexcept
{ }

[[nodiscard]]
CWindow::CWindow(
	_In_ const HWND hwWindow
) noexcept
	:
	m_hwWindow(hwWindow)
{ }

CWindow::CWindow(
	_Inout_ CWindow&& other
) noexcept
	:
	m_hwWindow(other.m_hwWindow),
	m_pOriginalWndProc(other.m_pOriginalWndProc),
	m_cstrOriginalWindowTitle(other.m_cstrOriginalWindowTitle)
{
	other.m_pOriginalWndProc = nullptr;
	other.m_cstrOriginalWindowTitle = nullptr;
}

CWindow::~CWindow( void ) noexcept {
	this->restoreOriginalWndProc();
	this->restoreOriginalTitle();
}

CWindow& CWindow::operator=(
	CWindow&& other
) noexcept
{
	if (&other == this) {
		return *this;
	}

	this->m_hwWindow = other.m_hwWindow;
	this->m_pOriginalWndProc = other.m_pOriginalWndProc;
	this->m_cstrOriginalWindowTitle = other.m_cstrOriginalWindowTitle;

	other.m_pOriginalWndProc = nullptr;
	other.m_cstrOriginalWindowTitle = nullptr;

	return *this;
}

CWindow& CWindow::operator=(
	_In_ const HWND& hwWindow
) noexcept
{
	*this = CWindow{ hwWindow };

	return *this;
}

CWindow::operator HWND( void ) const noexcept {
	return this->m_hwWindow;
}

_Check_return_opt_
_Success_(return == true)
bool CWindow::setWndProc(
	_In_ const WNDPROC pNewWndProc
) noexcept
{
	assert(pNewWndProc);

	if (this->m_pOriginalWndProc) {
		return SetWindowLongPtr(
			this->m_hwWindow,
			GWLP_WNDPROC,
			reinterpret_cast<const LONG_PTR>(pNewWndProc)
		);
	}

	this->m_pOriginalWndProc = reinterpret_cast<const WNDPROC>(
		SetWindowLongPtr(
			this->m_hwWindow,
			GWLP_WNDPROC,
			reinterpret_cast<const LONG_PTR>(pNewWndProc)
		)
	);

	return this->m_pOriginalWndProc;
}

_Check_return_opt_
_Success_(return == true)
bool CWindow::restoreOriginalWndProc( void ) noexcept {
	if (!this->m_pOriginalWndProc) {
		return false;
	}

	const bool bSuccessfullyRestoredOriginalWndProc = SetWindowLongPtr(
		this->m_hwWindow,
		GWLP_WNDPROC,
		reinterpret_cast<const LONG_PTR>(this->m_pOriginalWndProc)
	);

	this->m_pOriginalWndProc = nullptr;

	return bSuccessfullyRestoredOriginalWndProc;
}

[[nodiscard]]
const WNDPROC& CWindow::getOriginalWndProc( void ) const noexcept {
	return this->m_pOriginalWndProc;
}

[[nodiscard]]
_Check_return_
_Success_(return == true)
bool CWindow::getWindowSize(
	_Out_ std::uint16_t& uWidth,
	_Out_ std::uint16_t& uHeight
) noexcept
{
	// Create a RECT variable to store the window rectangle.
	RECT windowRect = RECT{ };

	// Retrieve the client area rectangle of the application window
	// using the GetClientRect function. If the function call fails,
	// return false to indicate the failure.
	if (!GetClientRect(this->m_hwWindow, &windowRect)) {
		return false;
	}
	
	// Calculate the width of the window based on the retrieved rectangle
	// and store it in the width variable.
	uWidth = static_cast<const std::uint16_t>(windowRect.right - windowRect.left);

	// Calculate the height of the window based on the retrieved rectangle
	// and store it in the height variable.
	uHeight = static_cast<const std::uint16_t>(windowRect.bottom - windowRect.top);

	// Return true to indicate that the update was successful.
	return true;
}

_Check_return_opt_
_Success_(return == true)
bool CWindow::setTitleA(
	_In_z_ const char* const cstrTitle
) noexcept
{
	assert(cstrTitle && cstrTitle[NULL] != __TEXT('\0'));

	if (this->m_cstrOriginalWindowTitle) {
		return SetWindowTextA(this->m_hwWindow, cstrTitle);
	}

	const std::int32_t iOriginalWindowTitleLength = GetWindowTextLength(this->m_hwWindow);

	if (!iOriginalWindowTitleLength) {
		return false;
	}

	try {
		this->m_cstrOriginalWindowTitle = new char[iOriginalWindowTitleLength + 1];
	}
	catch (const std::bad_alloc&) {
		return false;
	}

	return(
		GetWindowTextA(this->m_hwWindow, this->m_cstrOriginalWindowTitle, iOriginalWindowTitleLength) &&
		SetWindowTextA(this->m_hwWindow, cstrTitle)
	);
}

_Check_return_opt_
_Success_(return == true)
bool CWindow::restoreOriginalTitle( void ) noexcept {
	if (!this->m_cstrOriginalWindowTitle) {
		return false;
	}

	const bool bSuccessfullyRestored = SetWindowTextA(this->m_hwWindow, this->m_cstrOriginalWindowTitle);
	delete[] this->m_cstrOriginalWindowTitle;
	this->m_cstrOriginalWindowTitle = nullptr;

	return bSuccessfullyRestored;
}

[[nodiscard]]
_Ret_maybenull_z_
const char* const& CWindow::getOriginalWindowTitleA( void ) const noexcept {
	return this->m_cstrOriginalWindowTitle;
}

[[nodiscard]]
_Success_(return != nullptr)
_Ret_maybenull_
_Check_return_
HWND CWindow::getCurrentWindow( void ) noexcept {
	struct CURRENT {
		HWND hwCurrentWindow = nullptr;
		const DWORD dwCurrentProcessId = GetCurrentProcessId();
	}current;

	EnumWindows(
		[](_In_ const HWND hwWindow, _In_ const LPARAM currentData) noexcept -> BOOL {
			DWORD dwProcessId = NULL;
			GetWindowThreadProcessId(hwWindow, &dwProcessId);

			CURRENT& refCurrentData = *reinterpret_cast<CURRENT* const>(currentData);

			if (refCurrentData.dwCurrentProcessId != dwProcessId) {
				return TRUE;
			}

			refCurrentData.hwCurrentWindow = hwWindow;
			return FALSE;
		},
		reinterpret_cast<const LPARAM>(&current)
	);

	return current.hwCurrentWindow;
}