#pragma once
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

#include <flow/Node.h>
#include <flow/DataSource.h>

namespace flow
{
	template <typename T>
	class ONode : public Node
	{
	private:
		std::mutex sink_mtx;
		std::thread sink_thread;
		std::unique_ptr<std::ostream> sink_stream;

	protected:
		DataSource<T> data_supply;

	public:
		ONode(size_t size = 64*1024, size_t qty = 2):
		data_supply(size, qty)
		{
		}

		~ONode()
		{
			this->kill();

			if (sink_thread.joinable())
				sink_thread.join();
			sink_stream.release();
		}

	public:
		decltype(auto) getVacantBlock()
		{
			return data_supply.getVacantBlock();
		}

		void bindSink(std::ostream& sink)
		{
			std::lock_guard<std::mutex> lck(sink_mtx);
			sink_stream.release();
			sink_stream.reset(&sink);
			sink_stream->tie(nullptr);

			sink_thread = std::thread([this]
			{
				while (flow::sync())
				{
					if (runOrDie())
						break;

					auto block = data_supply.getActiveBlock();
					if (block == nullptr)
						break;

					auto data = (char*)block->data();
					size_t data_size = block->data_size();
					// write()
					std::lock_guard<std::mutex> lck(sink_mtx);
					sink_stream->write(data, data_size).flush();

					if (!sink_stream->good())
						break;
				}
			});
		}

	}; // class ONode

} // namespace flow
