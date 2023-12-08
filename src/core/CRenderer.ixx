#include<Windows.h>

export module CRenderer;

import<sal.h>;
import<cstdint>;

import CWindow;
import CTrampolineHook32;
import CMenu;

import CVector2;
import CVector3;

export class CRenderer final {
public:
	[[nodiscard]] explicit CRenderer( 
		_In_z_ const char* const cstrName
	) noexcept;
public:
	~CRenderer( void ) noexcept;
public:
	const bool& ok( void ) const noexcept;
private:
	typedef BOOL(FAR WINAPI* _wglSwapBuffers_t)(_In_ const HDC) noexcept;

	inline static CRenderer::_wglSwapBuffers_t _p_wglSwapBuffers_gateway = nullptr;

	static BOOL WINAPI hk_wglSwapBuffers(
		_In_ const HDC hDC
	) noexcept;
public:
	static inline bool _bCanSafelyRender = false;
private:
	bool m_bOk = false;
	CMenu m_menu;
	CTrampolineHook32 m_wglSwapBuffersTrampolineHook32 = CTrampolineHook32{ nullptr };
};