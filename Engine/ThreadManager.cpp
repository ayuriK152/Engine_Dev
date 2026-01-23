#include "pch.h"
#include "ThreadManager.h"

ThreadManager::~ThreadManager()
{

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
						return !_jobs.empty();
					});

					if (_jobs.empty())
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
