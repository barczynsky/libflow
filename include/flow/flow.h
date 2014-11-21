#pragma once

namespace flow
{
	class Module {
	public:
		virtual void notify() = 0;
		virtual void kill() = 0;
	};
	void to_kill(Module* module);
	void notify_all();
	void kill_all();

	void run();
	bool sync();
	void halt();

	void sigint(bool = true);

} // namespace flow
