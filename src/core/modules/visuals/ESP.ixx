export module ESP;

import playerent;

export namespace modules {
	namespace visuals {
		namespace ESP {
			void onToggle(const playerent& refTarget) noexcept;
			inline bool bToggle = false;
		}
	}
}