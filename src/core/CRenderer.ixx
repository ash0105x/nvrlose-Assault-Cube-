#include<Windows.h>

export module CRenderer;

import<sal.h>;
import<cstdint>;

import CWindow;
import CTrampolineHook32;

import CVector2;
import CVector3;

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
	/*
	* source:
	*	https://www.libsdl.org/release/SDL-1.2.15/docs/html/sdlwmgrabinput.html
	*/
	typedef enum {
		SDL_GRAB_QUERY,
		SDL_GRAB_OFF,
		SDL_GRAB_ON
	}SDL_GrabMode;

	typedef CRenderer::SDL_GrabMode(__cdecl* _SDL_WM_GrabInput_t)(_In_ const CRenderer::SDL_GrabMode) noexcept;
	
	inline static CRenderer::_SDL_WM_GrabInput_t _p_SDL_WM_GrabInput = nullptr;
private:
	typedef BOOL(WINAPI* _wglSwapBuffers_t)(_In_ const HDC) noexcept;

	inline static CRenderer::_wglSwapBuffers_t _p_wglSwapBuffers_gateway = nullptr;

	static BOOL WINAPI hk_wglSwapBuffers(
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