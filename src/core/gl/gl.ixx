#include<Windows.h>

#include<gl/GL.h>

export module gl;

import CVector2;

import <sal.h>;
import <cstdint>;

export namespace gl {
	void setupOrtho(void) noexcept;
	void restoreOrtho(void) noexcept;
	void drawLineRGB(
		_In_ const CVector2& vec2Start,
		_In_ const CVector2& vec2End,
		_In_ const GLubyte (&arrColor)[3],
		_In_ const float fWidth
	) noexcept;
	void drawLineRGBA(
		_In_ const CVector2& vec2Start,
		_In_ const CVector2& vec2End,
		_In_ const GLubyte (&arrColor)[4],
		_In_ const float fWidth
	) noexcept;
}