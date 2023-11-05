export module snaplines;

import CPlayer;

export namespace modules {
	namespace visuals {
		namespace snaplines {
			void onToggle(const CPlayer& refTarget) noexcept;
			inline float fWidth = 0.1f;
			inline bool bToggle = false;
		}
	}
}