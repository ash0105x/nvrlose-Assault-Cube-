#include<Windows.h>

#include<gl/GL.h>
#pragma comment(lib, "opengl32.lib")

#include<array>

#include<cassert>
#include<exception>
#include<stdio.h>
#include<string>

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
		_In_ const GLubyte* const arrColor,
		_In_ const float fWidth
	) noexcept;
	void drawLineRGBA(
		_In_ const CVector2& vec2Start,
		_In_ const CVector2& vec2End,
		_In_ const GLubyte* const arrColor,
		_In_ const float fWidth
	) noexcept;
	
	class CFont final {
	public:
		[[nodiscard]] explicit CFont(
			_In_z_ const char* const cstrName,
			_In_ const std::int32_t iHeight,
			_In_ const HDC hDC = wglGetCurrentDC()
		) noexcept
			:
			m_iBase(glGenLists(96)),
			m_hFont(
				CreateFontA(
					-iHeight, NULL,
					NULL, NULL,
					FW_NORMAL,
					FALSE, FALSE, FALSE,
					ANSI_CHARSET,
					OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
					PROOF_QUALITY,
					FF_DONTCARE | DEFAULT_PITCH,
					cstrName
				)
			)
		{
			const HFONT hOldFont = static_cast<const HFONT>(SelectObject(hDC, this->m_hFont));

			wglUseFontBitmapsA(hDC, 32, 96, this->m_iBase);
			
			SelectObject(hDC, hOldFont);
			DeleteObject(this->m_hFont);
		}

		void draw(
			_In_ const CVector2& vec2RefPosition,
			const GLubyte(&arrColor)[4u],
			_Printf_format_string_ _In_z_ const char* const fmt,
			...
		) const noexcept
		{
			assert(fmt);

			char cstrFormatedString[256] = { };

			va_list argList = nullptr;
			va_start(argList, fmt);
			
			vsprintf_s(
				cstrFormatedString,
				fmt,
				argList
			);

			va_end(argList);

			glColor4ub(
				arrColor[NULL],
				arrColor[1u],
				arrColor[2u],
				arrColor[3u]
			);

			glRasterPos2f(vec2RefPosition.x, vec2RefPosition.y);

			glPushAttrib(GL_LIST_BIT);

			glListBase(this->m_iBase - 32);
			glCallLists(
				std::strlen(cstrFormatedString),
				GL_UNSIGNED_BYTE,
				cstrFormatedString
			);

			glPopAttrib();
		}

		void drawCentered(
			_In_ const CVector2& vec2RefPosition,
			_In_ const float fEndX,
			_In_ const float fCharWidth,
			_In_ const GLubyte(&arrColor)[4u],
			_Printf_format_string_ _In_z_ const char* const fmt,
			...
		) noexcept
		{
			assert(fmt && fmt[NULL]);

			char cstrFormatedString[256] = { };

			va_list argList = nullptr;
			va_start(argList, fmt);

			vsprintf_s(
				cstrFormatedString,
				fmt,
				argList
			);

			va_end(argList);

			glColor4ub(
				arrColor[NULL],
				arrColor[1u],
				arrColor[2u],
				arrColor[3u]
			);

			const std::int32_t formatedStringLength = std::strlen(cstrFormatedString);

			glRasterPos2f(
				(vec2RefPosition.x + (fEndX - vec2RefPosition.x) / 2.f) - ((static_cast<const float>(formatedStringLength) * fCharWidth) / 2.f),
				vec2RefPosition.y
			);
			
			glPushAttrib(GL_LIST_BIT);

			glListBase(this->m_iBase - 32);
			glCallLists(
				formatedStringLength,
				GL_UNSIGNED_BYTE,
				cstrFormatedString
			);
			
			glPopAttrib();
		}
	private:
		const std::int32_t m_iBase = NULL;
		const HFONT m_hFont = nullptr;
	};
}