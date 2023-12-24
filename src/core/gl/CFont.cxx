import CFont;

#include<Windows.h>

#include<gl/GL.h>
#pragma comment(lib, "opengl32.lib")

#include<exception>
#include<stdio.h>

import<cassert>;
import<string>;

import globals;

constexpr const std::uint8_t MAX_GLYPHS = 96;

[[nodiscard]]
gl::CFont::CFont(
	_In_z_ const TCHAR* const tcstrName,
	_In_ const long lHeight,
	_In_ const HDC hDC
) noexcept
	:
	m_hFont(
		gl::CFont::createFontHelper(
			tcstrName,
			lHeight
		)
	)
{
	if (!this->m_hFont) {
		return;
	}

	const HFONT hOldFont = static_cast<const HFONT>(SelectObject(hDC, this->m_hFont));

	const auto restore = [&hDC, &hOldFont, this](void) noexcept -> void {
		SelectObject(hDC, hOldFont);
		DeleteObject(this->m_hFont);
	};

	if (!(this->m_iBase = glGenLists(::MAX_GLYPHS))) {
		restore();
		return;
	}

	wglUseFontBitmaps(hDC, 32, ::MAX_GLYPHS, this->m_iBase);

	TEXTMETRIC textMetric = TEXTMETRIC{ };
	GetTextMetrics(hDC, &textMetric);
	this->m_lHeight = textMetric.tmHeight / 2;

	restore();
}

gl::CFont::CFont(
	_Inout_ gl::CFont&& other
) noexcept
	:
	m_iBase(other.m_iBase),
	m_hFont(other.m_hFont),
	m_lHeight(other.m_lHeight)
{
	other.m_iBase = NULL;
	other.m_hFont = nullptr;
}

gl::CFont& gl::CFont::operator=(
	_Inout_ gl::CFont&& other
) noexcept
{
	if (this == &other) {
		return *this;
	}

	this->m_iBase = other.m_iBase;
	this->m_hFont = other.m_hFont;

	other.m_iBase = NULL;
	other.m_hFont = nullptr;

	return *this;
}

gl::CFont::~CFont( void ) noexcept {
	if (this->m_iBase) {
		DeleteObject(this->m_hFont);
		glDeleteLists(this->m_iBase, ::MAX_GLYPHS);
	}
	else if (this->m_hFont) {
		DeleteObject(this->m_hFont);
	}
}

bool gl::CFont::ok( void ) const noexcept {
	return this->m_iBase && this->m_hFont && this->m_lHeight;
}

const LONG& gl::CFont::getHeight( void ) const noexcept {
	return this->m_lHeight;
}

void gl::CFont::draw(
	_In_ const CVector2& vec2RefPosition,
	const GLubyte(&arrColor)[4u],
	_In_z_ const char* const cstrText,
	const size_t textLength
) const noexcept
{
	assert(cstrText && cstrText[NULL] != '\0' && textLength);

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
		textLength,
		GL_UNSIGNED_BYTE,
		cstrText
	);

	glPopAttrib();
}

void gl::CFont::drawCentered(
	_In_ const CVector2& vec2RefPosition,
	_In_ const float fEndX,
	_In_ const GLubyte(&arrColor)[4u],
	_In_z_ const char* const cstrText,
	const size_t textLength
) const noexcept
{
	assert(cstrText && cstrText[NULL] != '\0' && textLength);

	this->draw(
		CVector2{
			(vec2RefPosition.x + (fEndX - vec2RefPosition.x) / 2.f) - (static_cast<const float>(textLength * this->getHeight()) / 2.f),
			vec2RefPosition.y
		},
		arrColor,
		cstrText,
		textLength
	);
}

void gl::CFont::drawf(
	_In_ const CVector2& vec2RefPosition,
	const GLubyte(&arrColor)[4u],
	_Printf_format_string_ _In_z_ const char* const fmt,
	const va_list argList
) const noexcept
{
	char cstrFormatedString[256] = { };

	vsprintf_s(
		cstrFormatedString,
		fmt,
		argList
	);

	const std::size_t formatedStringLength = std::strlen(cstrFormatedString);
	assert(formatedStringLength <= 256);

	this->draw(
		vec2RefPosition,
		arrColor,
		cstrFormatedString,
		formatedStringLength
	);
}

void gl::CFont::drawCenteredf(
	_In_ const CVector2& vec2RefPosition,
	_In_ const float fEndX,
	_In_ const GLubyte(&arrColor)[4u],
	_Printf_format_string_ _In_z_ const char* const fmt,
	const va_list argList
) const noexcept
{
	char cstrFormatedString[256] = { };

	vsprintf_s(
		cstrFormatedString,
		fmt,
		argList
	);

	const std::size_t formatedStringLength = std::strlen(cstrFormatedString);
	assert(formatedStringLength <= 256);

	this->draw(
		CVector2{
			(vec2RefPosition.x + (fEndX - vec2RefPosition.x) / 2.f) - (static_cast<const float>(formatedStringLength * this->m_lHeight) / 2.f),
			vec2RefPosition.y
		},
		arrColor,
		cstrFormatedString,
		formatedStringLength
	);
}

void gl::CFont::drawf(
	_In_ const CVector2& vec2RefPosition,
	const GLubyte(&arrColor)[4u],
	_Printf_format_string_ _In_z_ const char* const fmt,
	...
) const noexcept
{
	va_list argList = nullptr;
	va_start(argList, fmt);

	this->drawf(
		vec2RefPosition,
		arrColor,
		fmt,
		argList
	);

	va_end(argList);
}

void gl::CFont::drawCenteredf(
	_In_ const CVector2& vec2RefPosition,
	_In_ const float fEndX,
	_In_ const GLubyte(&arrColor)[4u],
	_Printf_format_string_ _In_z_ const char* const fmt,
	...
) const noexcept
{
	va_list argList = nullptr;
	va_start(argList, fmt);

	this->drawCenteredf(
		vec2RefPosition,
		fEndX,
		arrColor,
		fmt,
		argList
	);

	va_end(argList);
}

HFONT gl::CFont::createFontHelper(
	_In_z_ const TCHAR* const tcstrName,
	_In_ const long lHeight
) noexcept
{
	assert(tcstrName && tcstrName[NULL] != '\0');

	return(
		CreateFont(
			-lHeight, NULL,
			NULL, NULL,
			FW_NORMAL,
			FALSE, FALSE, FALSE,
			ANSI_CHARSET,
			OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
			ANTIALIASED_QUALITY,
			(FF_DONTCARE | DEFAULT_PITCH),
			tcstrName
		)
	);
}