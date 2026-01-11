#ifndef _TIME_H_
#define _TIME_H_

class Time
{
	public:

		static void Initialize();
		static void Tick();
		static float GetDeltaTimeSec();

	private:	
		Time() {} // Can only be static
		static uint64_t m_PerformanceFrequency;
		static uint64_t m_CurrentTime;
		static uint64_t m_DeltaTime;
		static uint64_t m_ClockTicksPerSec;
		static float m_DeltaTimeSec;
};

#endif
