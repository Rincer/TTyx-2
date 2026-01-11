#include "stdafx.h"
#include "Time.h"

uint64_t Time::m_PerformanceFrequency;
uint64_t Time::m_CurrentTime;
uint64_t Time::m_DeltaTime;
uint64_t Time::m_ClockTicksPerSec;
float Time::m_DeltaTimeSec;

//----------------------------------------------------------------------------------------------
void Time::Initialize()
{
	QueryPerformanceFrequency((LARGE_INTEGER*)&m_PerformanceFrequency);
	QueryPerformanceCounter((LARGE_INTEGER*)&m_CurrentTime);
}

//----------------------------------------------------------------------------------------------
void Time::Tick()
{
	uint64_t NewTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&NewTime);
	m_DeltaTime = NewTime - m_CurrentTime;
	m_CurrentTime = NewTime;
	// handle wrap around
	if(m_DeltaTime < 0)
	{
		m_DeltaTime = m_PerformanceFrequency / 60; // 60 fps default
	}
	m_DeltaTimeSec = (float)((double)m_DeltaTime / (double)m_PerformanceFrequency);
}

//----------------------------------------------------------------------------------------------
float Time::GetDeltaTimeSec()
{
	return m_DeltaTimeSec;
}
