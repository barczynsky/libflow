#pragma once
#include <flow/flow.h>

namespace flow
{
	class Node : public Module
	{
	private:
		bool running{ true };
		bool dead{ false };

	private:
		std::mutex runtime_mtx;
		std::condition_variable runtime_cv;

	protected:
		std::unique_ptr<Node> parent;

	public:
		Node()
		{
			flow::kill(this);
		}

		~Node()
		{
			this->kill();

			parent.release();
		}

		void notify()
		{
			std::lock_guard<std::mutex> lck(runtime_mtx);
			runtime_cv.notify_all();
		}

		void kill()
		{
			std::lock_guard<std::mutex> lck(runtime_mtx);
			dead = true;
			runtime_cv.notify_all();
		}

	protected:
		bool runOrDie()
		{
			std::unique_lock<std::mutex> lck(runtime_mtx);
			runtime_cv.wait(lck, [this]{ return running || dead; });
			return dead;
		}

		bool trueOrDie(std::function<bool()> pred)
		{
			std::unique_lock<std::mutex> lck(runtime_mtx);
			runtime_cv.wait(lck, [&, this]{ return pred() || dead; });
			return dead;
		}

	public:
		void start()
		{
			std::lock_guard<std::mutex> lck(runtime_mtx);
			running = true;
			runtime_cv.notify_all();
		}

		void stop()
		{
			std::lock_guard<std::mutex> lck(runtime_mtx);
			running = false;
		}

	public:
		void bindNode(Node& node)
		{
			node.bindParentNode(this);
			// add_child(node);
		}

		void bindParentNode(Node* node)
		{
			parent.release();
			parent.reset(node);
		}

	}; // class Node

} // namespace flow
