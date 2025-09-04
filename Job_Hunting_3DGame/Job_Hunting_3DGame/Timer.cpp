#include "Timer.h"
#include "Debug_New.h"

void Timer::Init()
{
	LARGE_INTEGER li;
	QueryPerformanceFrequency(&li);
	m_Frequency = li.QuadPart;

	QueryPerformanceCounter(&li);
	m_OldCount = li.QuadPart;
}

void Timer::Update()
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	LONGLONG nowCount = li.QuadPart;

	m_DeltaTime = static_cast<float>(nowCount - m_OldCount) / m_Frequency;
	m_OldCount = nowCount;
}

float Timer::GetDeltaTime() const
{
	return m_DeltaTime;
}