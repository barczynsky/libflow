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
	class INode : public Node
	{
	private:
		std::mutex source_mtx;
		std::thread source_thread;
		std::unique_ptr<std::istream> source_stream;

	protected:
		DataSource<T> data_supply;

	public:
		INode(size_t size = 64*1024, size_t qty = 2):
		data_supply(size, qty)
		{
		}

		~INode()
		{
			this->kill();

			if (source_thread.joinable())
				source_thread.join();
			source_stream.release();
		}

	public:
		decltype(auto) getActiveBlock()
		{
			return data_supply.getActiveBlock();
		}

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

	}; // class INode

} // namespace flow
