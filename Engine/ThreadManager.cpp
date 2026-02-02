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
	// 싱글 스레드 환경에서도 해당 기능이 정상작동 하는지 확인이 안된 상황임.
	// 스레드 개수에 따라 성능이 어떻게 달라지는지 확인해 볼 필요가 있기는 함.
	// 일단은 전체 스레드 중 75%만 할당하도록 설정
	_numThreads = thread::hardware_concurrency() / 4 * 3;
	_numThreads = _numThreads > 0 ? _numThreads : 1;

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
