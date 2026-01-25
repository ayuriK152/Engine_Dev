#include "pch.h"
#include "ThreadManager.h"

ThreadManager::~ThreadManager()
{
	{
		unique_lock<mutex> lock(_queueMutex);
		_stop = true;
	}
	_condition.notify_all();

	for (thread& t : _threads)
	{
		if (t.joinable())
			t.join();
	}
}

void ThreadManager::Init()
{
	_numThreads = thread::hardware_concurrency();

	for (int i = 0; i < _numThreads; ++i) {
		_threads.emplace_back([this] {
			while (true) {
				function<void()> job;
				{
					unique_lock<mutex> lock(_queueMutex);
					_condition.wait(lock, [this] {
						return _stop || !_jobs.empty();
					});

					if (_stop && _jobs.empty())
						return;

					job = move(this->_jobs.front());
					_jobs.pop();
				}

				if (job)
					job();
			}
		});
	}
}
