import snaplines;

import globals;
import CPlayer;
import utils;

import <cstdint>;

#include"../../win32api.h"

#include<gl/GL.h>
#pragma comment(lib, "opengl32.lib")

void modules::visuals::snaplines::onToggle(const CPlayer& refTarget) noexcept {
	CVector2 vec2PlayerScreenPosition = CVector2{ };

	if (!utils::math::worldToScreen(refTarget.vec3Origin, vec2PlayerScreenPosition)) {
		return;
	}

	constexpr const GLubyte arrTeamColor[4u] = { 127u, 255u, 212u, 255u };
	constexpr const GLubyte arrEnemyColor[4u] = { 123u, 104u, 238u, 255u };

	glLineWidth(visuals::snaplines::fWidth);
	glBegin(GL_LINE_LOOP);

	const GLubyte* const arrColor = (
		refTarget.uTeamID == globals::entity::pLocalPlayer->uTeamID ?
		arrTeamColor :
		arrEnemyColor
	);

	glColor4ub(
		arrColor[NULL],
		arrColor[1u],
		arrColor[2u],
		arrColor[3u]
	);

	const std::uint32_t uHalfScreenWidth = globals::screen::viewPort[VIEW_PORT_ELEMENT::VIEW_PORT_ELEMENT_WIDTH] / 2u;

	glVertex2i(
		uHalfScreenWidth,
		-20
	);
	glVertex2f(
		vec2PlayerScreenPosition.x,
		vec2PlayerScreenPosition.y
	);

	glEnd();
	return;

	/*const std::uint32_t uHalfScreenWidth = globals::screen::viewPort[VIEW_PORT_ELEMENT::VIEW_PORT_ELEMENT_WIDTH] / 2u;

	for (std::uint8_t i = globals::entity::FIRST_ENTITY_INDEX; i < *globals::match::bypPlayerInGame; ++i) {
		const CPlayer& refCurrentPlayer = *((*globals::entity::pEntityList)[i]);

		if (!refCurrentPlayer.iHealth) {
			continue;
		}

		CVector2 vec2PlayerScreenPosition = CVector2{ };

		if (!utils::math::worldToScreen(refCurrentPlayer.vec3FeetPosition, vec2PlayerScreenPosition)) {
			continue;
		}

		const GLubyte* const arrColor = (
			refCurrentPlayer.uTeamID == globals::entity::pLocalPlayer->uTeamID ?
			arrTeamColor :
			arrEnemyColor
		);

		glColor4ub(
			arrColor[NULL],
			arrColor[1u],
			arrColor[2u],
			arrColor[3u]
		);

		glVertex2i(
			uHalfScreenWidth,
			-20
		);
		glVertex2f(
			vec2PlayerScreenPosition.x,
			vec2PlayerScreenPosition.y
		);
	}

	glEnd();*/
}