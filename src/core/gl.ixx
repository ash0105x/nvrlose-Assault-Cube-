#include"win32api.h"

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
	template<size_t t_lineCount> void drawLinesRGB(
		_In_ const std::array<const CVector2* const, t_lineCount>& vec2pArrRefLines,
		_In_ const GLubyte* const arrColor,
		_In_ const float fWidth
	) noexcept
	{
		glLineWidth(fWidth);
		glColor3ub(
			arrColor[NULL],
			arrColor[1u],
			arrColor[2u]
		);

		glBegin(GL_LINE_LOOP);

		for (const CVector2* const& vec2pLine : vec2pArrRefLines) {
			glVertex2f(vec2pLine->x, vec2pLine->y);
		}

		glEnd();
	}
	template<size_t t_lineCount> void drawLinesRGBA(
		_In_ const std::array<const CVector2* const, t_lineCount>& vec2pArrRefLines,
		_In_ const GLubyte* const arrColor,
		_In_ const float fWidth
	) noexcept
	{
		glLineWidth(fWidth);
		glColor4ub(
			arrColor[NULL],
			arrColor[1u],
			arrColor[2u],
			arrColor[3u]
		);

		glBegin(GL_LINE_LOOP);

		for (const CVector2* const& vec2pLine : vec2pArrRefLines) {
			glVertex2f(vec2pLine->x, vec2pLine->y);
		}

		glEnd();
	}
}