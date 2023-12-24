#include<Windows.h>

export module CWindow;

import<cstdint>;
import<type_traits>;

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
	CWindow(
		_Inout_ CWindow&&
	) noexcept;
public:
	~CWindow( void ) noexcept;
public:
	CWindow& operator=(
		CWindow&&
	) noexcept;
	CWindow& operator=(
		_In_ const HWND&
	) noexcept;
public:
	operator HWND( void ) const noexcept;
public:
	_Check_return_opt_
	_Success_(return == true)
	bool setWndProc(
		_In_ const WNDPROC pNewWndProc
	) noexcept;

	_Check_return_opt_
	_Success_(return == true)
	bool restoreOriginalWndProc( void ) noexcept;

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
	_Check_return_opt_
	_Success_(return == true)
	bool setTitle(
		_In_z_ const TCHAR* const tcstrTitle
	) noexcept;

	_Check_return_opt_
	_Success_(return == true)
	bool restoreOriginalTitle( void ) noexcept;

	[[nodiscard]]
	const TCHAR* const& getOriginalWindowTitle( void ) const noexcept;
public:
	[[nodiscard]]
	_Success_(return != nullptr)
	_Ret_maybenull_
	_Check_return_
	static HWND getCurrentWindow( void ) noexcept;
private:
	HWND m_hwWindow = nullptr;
	WNDPROC m_pOriginalWndProc = nullptr;
	TCHAR* m_tcstrOriginalWindowTitle = nullptr;
};