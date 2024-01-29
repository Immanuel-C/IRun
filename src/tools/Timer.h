#pragma once

#include <chrono>

namespace IRun {
	namespace Tools {
		using Nanosecond = std::chrono::duration<double, std::ratio<1, 1000000000>>;
		using Microseconds = std::chrono::duration<double, std::ratio<1, 1000000>>;
		using Milliseconds = std::chrono::duration<double, std::ratio<1, 1000>>;
		using Seconds = std::chrono::duration<double>;
		using Minutes = std::chrono::duration<double, std::ratio<60>>;
		using Hours = std::chrono::duration<double, std::ratio<3600>>;
		using Days = std::chrono::duration<double, std::ratio<86400>>;
		using Weeks = std::chrono::duration<double, std::ratio<604800>>;
		using Months = std::chrono::duration<double, std::ratio<2629746>>;
		using Years = std::chrono::duration<double, std::ratio<31556952>>;

		template <class Period>
		class Timer {
		public:
			Timer() = default;
			/// <summary>
			/// Starts the timer.
			/// </summary>
			void Start() { m_startTime = std::chrono::high_resolution_clock::now(); }
			/// <summary>
			/// Stops the timer and returns the amount of time passed since starting the timer.
			/// </summary>
			/// <returns>time passed since the timer</returns>
			double Stop() {
				Period dur = (std::chrono::high_resolution_clock::now() - m_startTime);
				return dur.count();
			}
		private:
			std::chrono::high_resolution_clock::time_point m_startTime;
		};
	}
}
