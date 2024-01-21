#include<Windows.h>

export module CMenu;

import<filesystem>;

import CWindow;

export class CMenu final {
public:
	[[nodiscard]] explicit CMenu(
		_In_z_ const char* const cstrClientName
	) noexcept;
public:
	~CMenu( void ) noexcept;
public:
	const bool& ok( void ) const noexcept;
public:
	static void drawMain( void ) noexcept;
public:
	static void begin( void ) noexcept;
	static void end( void ) noexcept;
public:
	static bool initialize( void ) noexcept;
public:
	static inline bool _bOpen = true;
private:
	/*
	* source:
	*	https://www.libsdl.org/release/SDL-1.2.15/docs/html/sdlwmgrabinput.html
	*/
	typedef enum class _SDL_GrabMode : std::uint32_t {
		SDL_GRAB_QUERY,
		SDL_GRAB_OFF,
		SDL_GRAB_ON
	}SDL_GrabMode;

	typedef CMenu::SDL_GrabMode(FAR __cdecl* _SDL_WM_GrabInput_t)(_In_ const CMenu::SDL_GrabMode) noexcept;

	inline static CMenu::_SDL_WM_GrabInput_t _p_SDL_WM_GrabInput = nullptr;
private:
	inline static CWindow _AssaultCubeWindow = CWindow{ nullptr };

	static LRESULT CALLBACK hk_WndProc(
		_In_ const HWND hWnd,
		_In_ const UINT msg,
		_In_ const WPARAM wParam,
		_In_ const LPARAM lParam
	) noexcept;
private:
	static inline bool _bImGuiWin32Initialized = false;
	static inline bool _bImGuiOpenGLInitialized = false;
private:
	std::filesystem::path m_filePath = std::filesystem::path{ };
	bool m_bOk = false;
};