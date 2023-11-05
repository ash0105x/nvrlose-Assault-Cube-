export module ESP;

import CPlayer;

export namespace modules {
	namespace visuals {
		namespace ESP {
			void onToggle(const CPlayer& refTarget) noexcept;
			inline bool bToggle = false;
		}
	}
}