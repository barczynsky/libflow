#pragma once
#include <atomic>
#include <condition_variable>
#include <future>
#include <mutex>
#include <thread>

namespace flow
{
	class Module
	{
	public:
		virtual void notify() = 0;
		virtual void kill() = 0;
	};
	void kill(Module* module);

	void run();
	bool sync();
	void abort(int signum = 9);

} // namespace flow
