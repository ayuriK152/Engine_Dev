#pragma once

class ThreadManager
{
	DECLARE_SINGLE(ThreadManager);
public:
	~ThreadManager();

	void Init();

private:
	size_t _numThreads;

	vector<thread> _threads;
	queue<function<void()>> _jobs;

	mutex _queueMutex;
	condition_variable _condition;
};

