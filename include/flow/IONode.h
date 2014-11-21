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
	class IONode : public Node
	{
	private:
		std::mutex source_mtx;
		std::thread source_thread;
		std::unique_ptr<std::istream> source_stream;

	private:
		std::mutex sink_mtx;
		std::thread sink_thread;
		std::unique_ptr<std::ostream> sink_stream;

	protected:
		DataSource<T> data_supply;

	public:
		IONode(size_t size = 64*1024, size_t qty = 2):
		data_supply(size, qty)
		{
		}

		~IONode()
		{
			this->kill();

			if (source_thread.joinable())
				source_thread.join();
			if (sink_thread.joinable())
				sink_thread.join();

			source_stream.release();
			sink_stream.release();
		}

	public:
		size_t getValueSize()
		{
			return data_supply.getValueSize();
		}

		size_t getBlockSize()
		{
			return data_supply.getBlockSize();
		}

		decltype(auto) getVacantBlock()
		{
			return data_supply.getVacantBlock();
		}

		decltype(auto) getActiveBlock()
		{
			return data_supply.getActiveBlock();
		}

	public:
		void bindSource(std::istream& source)
		{
			std::lock_guard<std::mutex> lck(source_mtx);
			source_stream.release();
			source_stream.reset(&source);
			source_stream->tie(nullptr);

			source_thread = std::thread([this]
			{
				while (flow::sync())
				{
					if (runOrDie())
						break;

					auto block = data_supply.getVacantBlock();
					if (block == nullptr)
						break;

					auto data = (char*)block->data();
					size_t data_size = block->data_size();
					// read()
					std::lock_guard<std::mutex> lck(source_mtx);
					size_t count = 0;
					while (source_stream->good() && count < data_size)
						count += source_stream->read(data + count, data_size - count).gcount();

					block->data_resize(count);
					if (count > 0)
						block->make_active();

					if (!source_stream->good())
						break;
				}
			});
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

	}; // class IONode

} // namespace flow
