#include<Windows.h>

#include<gl/GL.h>

export module CFont;

import CVector2;

import<cstdint>;

export namespace gl {
	class CFont final {
	public:
		[[nodiscard]] explicit CFont(
			_In_z_ const TCHAR* const tcstrName,
			_In_ const long lHeight,
			_In_ const HDC hDC = wglGetCurrentDC()
		) noexcept;
		CFont(
			_Inout_ CFont&&
		) noexcept;
	public:
		CFont& operator=(
			_Inout_ CFont&&
		) noexcept;
	public:
		~CFont(void) noexcept;
	public:
		bool ok(void) const noexcept;
	public:
		const LONG& getHeight(void) const noexcept;
	public:
		void draw(
			_In_ const CVector2& vec2RefPosition,
			const GLubyte(&arrColor)[4u],
			_In_z_ const char* const cstrText,
			const size_t textLength
		) const noexcept;
		
		void drawCentered(
			_In_ const CVector2& vec2RefPosition,
			_In_ const float fEndX,
			_In_ const GLubyte(&arrColor)[4u],
			_In_z_ const char* const cstrText,
			const size_t textLength
		) const noexcept;
	public:
		void drawf(
			_In_ const CVector2& vec2RefPosition,
			const GLubyte(&arrColor)[4u],
			_Printf_format_string_ _In_z_ const char* const fmt,
			const va_list argList
		) const noexcept;

		void drawCenteredf(
			_In_ const CVector2& vec2RefPosition,
			_In_ const float fEndX,
			_In_ const GLubyte(&arrColor)[4u],
			_Printf_format_string_ _In_z_ const char* const fmt,
			const va_list argList
		) const noexcept;
	public:
		void drawf(
			_In_ const CVector2& vec2RefPosition,
			const GLubyte(&arrColor)[4u],
			_Printf_format_string_ _In_z_ const char* const fmt,
			...
		) const noexcept;

		void drawCenteredf(
			_In_ const CVector2& vec2RefPosition,
			_In_ const float fEndX,
			_In_ const GLubyte(&arrColor)[4u],
			_Printf_format_string_ _In_z_ const char* const fmt,
			...
		) const noexcept;
	private:
		static HFONT createFontHelper(
			_In_z_ const TCHAR* const tcstrName,
			_In_ const long lHeight
		) noexcept;
	private:
		GLuint m_iBase = NULL;
		HFONT m_hFont = nullptr;
		LONG m_lHeight = NULL;
	};
}