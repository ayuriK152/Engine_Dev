#pragma once

class BULB_API EngineStatusManager
{
	friend class BulbApplication;

private:
	EngineStatusManager() = default;
	~EngineStatusManager() = default;

	void Update();

public:
	EngineStatusManager(const EngineStatusManager& rhs) = delete;
	EngineStatusManager& operator=(const EngineStatusManager& rhs) = delete;

	static EngineStatusManager* GetInstance();
	static Bulb::ProcessResult Delete();

	void ResetValues() {
		_frameCount = 0.0f;
		_maxFrameCount = 0.0f;
		_minFrameCount = -1.0f;
		_timeElapsed = 0.0f;
		_averageFrameCount = 0.0f;
	}

	float GetFrameCount() { return _frameCount; }
	string GetFrameCountString();

	float GetMaxFrameCount() { return _maxFrameCount; }
	string GetMaxFrameCountString();

	float GetMinFrameCount() { return _minFrameCount; }
	string GetMinFrameCountString();

	float GetAverageFrameCount() { return _averageFrameCount; }
	string GetAverageFrameCountString();

private:
	static EngineStatusManager* s_instance;

	int _frameCount = 0;
	int _currentFrameCount = 0;
	float _maxFrameCount = 0.0f;
	float _minFrameCount = -1.0f;
	float _averageFrameCount = 0.0f;
	float _timeElapsed = 0.0f;
};

