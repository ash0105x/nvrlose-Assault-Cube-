export module snaplines;

import playerent;

export namespace modules {
	namespace visuals {
		namespace snaplines {
			void onToggle(const playerent& refTarget) noexcept;
			inline float fWidth = 0.1f;
			inline bool bToggle = false;
		}
	}
}