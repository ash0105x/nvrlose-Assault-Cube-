import CWindow;

#include"win32api.h"

#include<exception>

#include<assert.h>
//import <assert.h>;


[[nodiscard]] CWindow::CWindow(
	[[maybe_unused]] _In_ const std::nullptr_t
) noexcept
	:
	m_hwWindow(nullptr)
{ }

[[nodiscard]] CWindow::CWindow(
	_In_ const HWND hwWindow
) noexcept
	:
	m_hwWindow(hwWindow)
{ }

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
bool CWindow::changeWndProc(
	_In_ const WNDPROC pNewWndProc
) noexcept
{
	assert(pNewWndProc);

	this->m_pOriginalWndProc = reinterpret_cast<const WNDPROC>(
		SetWindowLongPtr(
			this->m_hwWindow,
			GWLP_WNDPROC,
			reinterpret_cast<const LONG_PTR>(pNewWndProc)
		)
	);

	return static_cast<const bool>(this->m_pOriginalWndProc);
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