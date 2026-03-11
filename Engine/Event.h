#pragma once

template<typename... Args>
class Event
{
	using Callback = function<void(Args...)>;
public:
	void operator+=(Callback callback) {
		_callbacks.push_back(callback);
	}

	void Execute(Args... args) {
		if (_callbacks.size() == 0) return;

		for (auto& callback : _callbacks) {
			callback(args...);
		}
	}

	void Clear() { _callbacks.clear(); }

private:
	vector<Callback> _callbacks;
};
