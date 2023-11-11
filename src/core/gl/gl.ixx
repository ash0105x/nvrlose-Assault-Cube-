#include<Windows.h>

#include<gl/GL.h>
#pragma comment(lib, "opengl32.lib")

#include<array>

export module gl;

import CVector2;

import <sal.h>;
import <cstdint>;

export namespace gl {
	void setupOrtho(void) noexcept;
	void drawLineRGB(
		_In_ const CVector2& vec2Start,
		_In_ const CVector2& vec2End,
		_In_ const GLubyte* const arrColor,
		_In_ const float fWidth
	) noexcept;
	void drawLineRGBA(
		_In_ const CVector2& vec2Start,
		_In_ const CVector2& vec2End,
		_In_ const GLubyte* const arrColor,
		_In_ const float fWidth
	) noexcept;
	
	class Font final {
	public:
		[[nodiscard]] explicit Font(const std::uint32_t uHeight, const HDC hDC = nullptr) noexcept;

	private:
		
	};
}