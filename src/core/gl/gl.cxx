import gl;

#include<Windows.h>

#include<gl/GL.h>
#pragma comment(lib, "opengl32.lib")

#include<cmath>
#include<numbers>

import CVector2;
import globals;

void gl::setupOrtho( void ) noexcept {
	glGetIntegerv(GL_VIEWPORT, globals::screen::viewPort.data());

	glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(
		0.0,
		globals::screen::viewPort[VIEW_PORT_ELEMENT::VIEW_PORT_ELEMENT_WIDTH],
		0.0,
		globals::screen::viewPort[VIEW_PORT_ELEMENT::VIEW_PORT_ELEMENT_HEIGHT],
		-1.0,
		1.0
	);
}

void gl::restoreOrtho( void ) noexcept {
	glPopMatrix();
}

static void _drawLine(
	_In_ const CVector2& vec2Start,
	_In_ const CVector2& vec2End
) noexcept
{
	glBegin(GL_LINES);

	glVertex2f(
		vec2Start.x,
		vec2Start.y
	);
	glVertex2f(
		vec2End.x,
		vec2End.y
	);

	glEnd();
}

void gl::drawLineRGB(
	_In_ const CVector2& vec2Start,
	_In_ const CVector2& vec2End,
	_In_ const GLubyte (&arrColor)[3],
	_In_ const float fWidth
) noexcept
{
	glColor3ub(
		arrColor[NULL],
		arrColor[1u],
		arrColor[2u]
	);
	glLineWidth(fWidth);

	_drawLine(vec2Start, vec2End);

}

void gl::drawLineRGBA(
	_In_ const CVector2& vec2Start,
	_In_ const CVector2& vec2End,
	_In_ const GLubyte (&arrColor)[4],
	_In_ const float fWidth
) noexcept
{
	glColor4ub(
		arrColor[NULL],
		arrColor[1u],
		arrColor[2u],
		arrColor[3u]
	);
	glLineWidth(fWidth);

	_drawLine(vec2Start, vec2End);
}

void gl::drawCircle(
	_In_ const CVector2& vec2RefPosition,
	_In_ const std::uint32_t uRadius,
	_In_ const std::uint8_t uVertices,
	_In_ const GLubyte(&arrColor)[4],
	_In_ const float fWidth
) noexcept
{
	glColor4ub(
		arrColor[NULL],
		arrColor[1],
		arrColor[2],
		arrColor[3]
	);

	glLineWidth(fWidth);

	glBegin(GL_LINE_LOOP);

	for (float i = 0.f; i < uVertices; ++i) {
		const float fAngle = 2.f * std::numbers::pi_v<float> * i / static_cast<const float>(uVertices);

		glVertex2f(
			vec2RefPosition.x + uRadius * std::cos(fAngle),
			vec2RefPosition.y + uRadius * std::sin(fAngle)
		);
	}

	glEnd();
}