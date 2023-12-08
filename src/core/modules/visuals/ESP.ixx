#include<Windows.h>

#include<gl/GL.h>

export module ESP;

import playerent;
import CFont;

export namespace modules {
	namespace visuals {
		namespace ESP {
			void onToggle(
				const playerent& refTarget,
				const gl::CFont& font,
				const CVector2& vec2TargetOriginScreenPosition,
				const GLubyte(&arrColor)[4]
			) noexcept;
			inline bool bToggle = true;
			inline constexpr float MIN_DISTANCE = 1.f;
			inline constexpr float MAX_DISTANCE = 300.f;
			inline float fDistance = modules::visuals::ESP::MAX_DISTANCE;
		}
	}
}