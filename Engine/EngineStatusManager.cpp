#include "pch.h"
#include "EngineStatusManager.h"

void EngineStatusManager::Update()
{

	if (TIME->TotalTime() - _timeElapsed >= 0.5f)
	{
		_frameCount = round(1.0f / TIME->DeltaTime() * 10.0f) / 10.0f;

		_timeElapsed += 0.5f;
	}
}

string EngineStatusManager::GetFrameCountString()
{
	stringstream ss;
	ss << setw(6) << fixed << setprecision(1) << _frameCount;

	return ss.str();
}

string EngineStatusManager::GetMaxFrameCountString()
{
	stringstream ss;
	ss << setw(6) << fixed << setprecision(1) << _maxFrameCount;

	return ss.str();
}

string EngineStatusManager::GetMinFrameCountString()
{
	stringstream ss;
	ss << setw(6) << setprecision(1) << _minFrameCount;

	return ss.str();
}
