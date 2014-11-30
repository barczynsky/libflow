#include <atomic>
#include <functional>
#include <future>
#include <vector>

#include <csignal>

#include <flow/flow.h>
#include <flow/Node.h>

namespace flow
{
	std::atomic_bool flowing{ false };
	std::promise<void> flow_promise;
	std::shared_future<void> flow_future{ flow_promise.get_future() };

	bool init_sync() { flow_future.get(); return flowing.load(); }
	bool fast_sync() { return flowing.load(); }
	// std::atomic<std::function<bool()>> flow_sync{ init_sync }; // TODO: not supported (ISO C++14)
	std::atomic<bool(*)()> flow_sync{ init_sync };

	std::vector<Module*> modules;
	void to_kill(Module* module)
	{
		modules.push_back(module);
	}

	void notify_all()
	{
		for (auto module : modules)
			module->notify();
	}

	void kill_all()
	{
		for (auto module : modules)
			module->kill();
		// modules.clear();
	}

	void run()
	{
		flowing.store(true);
		// flow_sync.assign(fast_sync); // TODO: not supported (g++ 4.9.1)
		flow_sync.store(fast_sync); // TODO: remove on assign() supported
		try { flow_promise.set_value(); }
		catch (std::future_error&) {}
	}

	bool sync()
	{
		return flow_sync.load()();
	}

	void flow_halt(int = SIGINT)
	{
		flowing.store(false);
		try { flow_promise.set_value(); }
		catch (std::future_error&) {}
		flow::kill_all();
	}

	void halt()
	{
		flow_halt();
	}

	void sigint(bool action)
	{
		if (action) std::signal(SIGINT, flow_halt);
		else std::signal(SIGINT, SIG_IGN);
	}

	auto flow_main()
	{
		flow::sigint();
		return 0;
	}
	auto flow_init = flow_main();

} // namespace flow
