#pragma once
#include <Windows.h>

class Timer
{
private:
	LONGLONG m_Frequency = 0.0f;
	LONGLONG m_OldCount = 0.0f;
	float m_DeltaTime = 0.0f;
public:
	void Init();
	void Update();
	float GetDeltaTime() const;
};

