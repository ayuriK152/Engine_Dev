#include "pch.h"
#include "EngineStatusManager.h"

void EngineStatusManager::Update()
{
	_frameCount += 1;
	if (TIME->TotalTime() - _timeElapsed >= 1.0f)
	{
		//_frameCount = round(1.0f / TIME->DeltaTime() * 10.0f) * 0.1f;
		_currentFrameCount = _frameCount;

		if (_timeElapsed > 0.0f)
		{
			_averageFrameCount = (_averageFrameCount * _timeElapsed + _frameCount) / (_timeElapsed + 1.0f);
		}
		else
		{
			_averageFrameCount = _frameCount;
		}

		_timeElapsed += 1.0f;
		_frameCount = 0;
	}
}

string EngineStatusManager::GetFrameCountString()
{
	return to_string(_currentFrameCount);
}

string EngineStatusManager::GetMaxFrameCountString()
{
	stringstream ss;
	ss << fixed << setprecision(1) << _maxFrameCount;

	return ss.str();
}

string EngineStatusManager::GetMinFrameCountString()
{
	stringstream ss;
	ss << fixed << setprecision(1) << _minFrameCount;

	return ss.str();
}

string EngineStatusManager::GetAverageFrameCountString()
{
	stringstream ss;
	float value = round(_averageFrameCount * 10.0f) * 0.1f;
	ss << fixed << setprecision(1) << value;

	return ss.str();
}
