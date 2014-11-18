#include <csignal>
// #include <cstdlib>
#include <deque>
#include <functional>
#include <flow/flow.h>
#include <flow/Node.h>

namespace flow
{
	std::mutex flow_mtx;
	std::atomic<bool> flowing{ false };

	std::promise<bool> flow_promise;
	std::shared_future<bool> flow_future{ flow_promise.get_future() };

	void sigint(bool action) { action ? signal(SIGINT, &flow::abort) : signal(SIGINT, SIG_IGN); }
	bool init_sync() { flow::sigint(true); return flow_future.get() && flowing.load(); }
	bool fast_sync() { return flowing.load(); };
	std::function<bool()> flow_sync{ init_sync };

	std::deque<Module*> modules;
	void kill(Module* module)
	{
		modules.push_back(module);
	}

	void run()
	{
		std::lock_guard<std::mutex> lck(flow_mtx);
		if (flowing.load()) return;
		flowing.store(true);
		flow_sync = fast_sync;
		// flow_sync.assign(fast_sync);
		flow_promise.set_value(true);
	}

	bool sync()
	{
		return flow_sync();
	}

	void abort(int)
	{
		if (flowing.load())
			flowing.store(false);
		else
			flow_promise.set_value(false);
		for (auto module : modules)
			module->kill();
	}

} // namespace flow
