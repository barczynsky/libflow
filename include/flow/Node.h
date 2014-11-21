#pragma once
#include <condition_variable>
// #include <functional>
#include <memory>
#include <mutex>

#include <flow/flow.h>

namespace flow
{
	class Node : public Module
	{
	private:
		bool dead{ false };
		bool running{ true };

	private:
		std::mutex runtime_mtx;
		std::condition_variable runtime_cv;

	protected:
		std::unique_ptr<Node> lnode;
		std::unique_ptr<Node> rnode;

	public:
		Node()
		{
			flow::to_kill(this);
		}

		~Node()
		{
			this->kill();

			lnode.release();
			rnode.release();
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
		void bindLNode(Node* node)
		{
			node->bindRNode(node);
			lnode.release();
			lnode.reset(node);
		}

		void bindRNode(Node* node)
		{
			node->bindLNode(node);
			rnode.release();
			rnode.reset(node);
		}

	protected:
		bool runOrDie()
		{
			std::unique_lock<std::mutex> lck(runtime_mtx);
			runtime_cv.wait(lck, [this]{ return running || dead; });
			return dead;
		}

		// bool trueOrDie(std::function<bool()> pred)
		// {
		// 	std::unique_lock<std::mutex> lck(runtime_mtx);
		// 	runtime_cv.wait(lck, [&, this]{ return pred() || dead; });
		// 	return dead;
		// }

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
			runtime_cv.notify_all();
		}

	}; // class Node

} // namespace flow
