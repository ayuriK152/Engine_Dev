#pragma once

class GameTimer
{
	DECLARE_SINGLE(GameTimer);
public:
	bool Initialize();

	float TotalTime()const; // in seconds
	float DeltaTime()const; // in seconds

	void Reset(); // Call before message loop.
	void Start(); // Call when unpaused.
	void Stop();  // Call when paused.
	void Tick();  // Call every frame.

private:
	double _secondsPerCount;
	double _deltaTime;

	__int64 _baseTime;
	__int64 _pausedTime;
	__int64 _stopTime;
	__int64 _prevTime;
	__int64 _currTime;

	bool _stopped;
};