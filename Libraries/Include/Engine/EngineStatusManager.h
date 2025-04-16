#pragma once

class EngineStatusManager
{
	DECLARE_SINGLE(EngineStatusManager)
public:
	~EngineStatusManager() = default;

	void Update();

public:
	void ResetValues() {
		_frameCount = 0.0f;
		_maxFrameCount = 0.0f;
		_minFrameCount = -1.0f;
		_timeElapsed = 0.0f;
	}

	float GetFrameCount() { return _frameCount; }
	string GetFrameCountString();

	float GetMaxFrameCount() { return _maxFrameCount; }
	string GetMaxFrameCountString();

	float GetMinFrameCount() { return _minFrameCount; }
	string GetMinFrameCountString();

private:
	float _frameCount = 0.0f;
	float _maxFrameCount = 0.0f;
	float _minFrameCount = -1.0f;
	float _timeElapsed = 0.0f;
};

