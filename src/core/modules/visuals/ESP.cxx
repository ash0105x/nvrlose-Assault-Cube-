import ESP;

import globals;
import CPlayer;
import utils;

import <cstdint>;

#include"../../win32api.h"

#include<gl/GL.h>
#pragma comment(lib, "opengl32.lib")

void modules::visuals::ESP::onToggle(const CPlayer& refTarget) noexcept {
	CVector2 vec2TargetScreenPosition = CVector2{ };

	if (!utils::math::worldToScreen(refTarget.vec3EyePosition, vec2TargetScreenPosition)) {
		return;
	}

	constexpr const GLubyte arrTeamColor[4u] = { NULL, 255, NULL, 255 };
	constexpr const GLubyte arrEnemyColor[4u] = { 255, NULL, NULL, 255 };

	const GLubyte* const arrColor = (
		refTarget.uTeamID == globals::entity::pLocalPlayer->uTeamID ?
		arrTeamColor :
		arrEnemyColor
	);

	glLineWidth(0.1f);
	glBegin(GL_LINE_LOOP);

	glEnd();
}