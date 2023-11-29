#include<Windows.h>

export module CWindow;

import<cstdint>;
import<type_traits>;

// Declaration of CWindow class
export class CWindow final {
public:
	// Constructor declarations
	[[nodiscard]] CWindow(
		[[maybe_unused]] _In_ const std::nullptr_t = nullptr
	) noexcept;
	[[nodiscard]] CWindow(
		_In_ const HWND hwWindow
	) noexcept;
public:
	[[nodiscard]] CWindow(
		_In_ const CWindow&
	) noexcept = default;
	[[nodiscard]] CWindow(
		CWindow&&
	) noexcept = default;
public:
	~CWindow( void ) noexcept = default;
public:
	CWindow& operator=(
		_In_ const CWindow&
	) noexcept = default;
	CWindow& operator=(
		CWindow&&
	) noexcept = default;
	CWindow& operator=(
		_In_ const HWND&
	) noexcept;
public:
	operator HWND( void ) const noexcept;
public:
	_Check_return_opt_
	_Success_(return == true)
	bool changeWndProc(
		_In_ const WNDPROC pNewWndProc
	) noexcept;

	[[nodiscard]]
	const WNDPROC& getOriginalWndProc( void ) const noexcept;
public:
	[[nodiscard]]
	_Check_return_
	_Success_(return == true)
	bool getWindowSize(
		_Out_ std::uint16_t& uWidth,
		_Out_ std::uint16_t& uHeight
	) noexcept;
public:
	[[nodiscard]]
	_Success_(return != nullptr)
	_Ret_maybenull_
	_Check_return_
	static HWND getCurrentWindow( void ) noexcept;
private:
	HWND m_hwWindow = nullptr;
	WNDPROC m_pOriginalWndProc = nullptr;

	//TCHAR* m_pOriginalWindowText = nullptr;
};