#include"win32api.h"

export module CRenderer;

import <sal.h>;

import CWindow;
import CTrampolineHook32;

export class CRenderer final {
public:
	[[nodiscard]] explicit CRenderer( void ) noexcept;
public:
	~CRenderer( void ) noexcept;
public:
	const bool& ok( void ) const noexcept;
private:
	static void begin( void ) noexcept;
	static void end( void ) noexcept;
private:
	typedef BOOL(__stdcall* _wglSwapBuffers_t)(_In_ const HDC) noexcept;

	inline static CRenderer::_wglSwapBuffers_t _p_wglSwapBuffers_gateway = nullptr;

	static BOOL __stdcall hk_wglSwapBuffers(
		_In_ const HDC hDC
	) noexcept;
private:
	inline static CWindow _AssaultCubeWindow = CWindow{ nullptr };

	static LRESULT CALLBACK hk_WndProc(
		_In_ const HWND hWnd,
		_In_ const UINT msg,
		_In_ const WPARAM wParam,
		_In_ const LPARAM lParam
	) noexcept;
private:
	inline static bool _bImGuiWin32Initialized = false;
	inline static bool _bImGuiOpenGLInitialized = false;
private:
	bool m_bOk = false;
	CTrampolineHook32 m_wglSwapBuffersTrampolineHook32 = CTrampolineHook32{ nullptr };
};