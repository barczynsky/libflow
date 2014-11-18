#pragma once
#include <condition_variable>
#include <deque>
#include <mutex>
#include <vector>
#include <unordered_set>
#include <flow/flow.h>
#include <flow/DataBlock.h>

namespace flow
{
	template <typename T>
	class DataSource : public Module
	{
	public:
		typedef DataBlock<T> DataType;
		typedef std::unique_ptr<DataType> DataPtr;
		typedef std::shared_ptr<DataType> DataShare;

	private:
		bool dry{ false };
		size_t value_size{ sizeof(T) };
		size_t block_size{ 0 };
		size_t block_qty{ 0 };

	private:
		std::mutex vacant_mtx;
		std::condition_variable vacant_cv;
		std::deque<DataPtr> vacant_blocks;

	private:
		std::mutex active_mtx;
		std::condition_variable active_cv;
		std::deque<DataPtr> active_blocks;

	private:
		std::mutex shared_mtx;
		std::condition_variable shared_cv;
		std::unordered_set<DataType*> shared_blocks;

	public:
		DataSource(size_t size = 64*1024, size_t qty = 2):
		block_size{ size },
		block_qty{ qty }
		{
			flow::kill(this);

			vacant_blocks.resize(block_qty);
			for (auto & block : vacant_blocks)
			{
#if __cplusplus>=201300L
				block = std::make_unique<DataType>(block_size);
#else
				block.reset(new DataType(block_size));
#endif
			}
		}

		~DataSource()
		{
			this->kill();

			std::unique_lock<std::mutex> lck(shared_mtx);
			shared_cv.wait(lck, [this]
			{
				return shared_blocks.empty();
			});
		}

		void notify()
		{
			vacant_cv.notify_all();
			active_cv.notify_all();
		}

		void kill()
		{
			dry = true;
			vacant_cv.notify_all();
			active_cv.notify_all();
		}

	public:
		size_t getBlockSize()
		{
			return block_size;
		}

		size_t getValueSize()
		{
			return value_size;
		}

	public:
		DataShare getVacantBlock()
		{
			if (dry) return nullptr;
			std::unique_lock<std::mutex> lck(vacant_mtx);
			vacant_cv.wait(lck, [this]
			{
				return !vacant_blocks.empty() || dry;
			});
			if (dry) return nullptr;

			auto data_ptr = vacant_blocks.front().release();
			vacant_blocks.pop_front();
			DataShare data_share(data_ptr, [this](DataType* ptr)
			{
				if (ptr->active()) // diff
				{
					std::lock_guard<std::mutex> lck(active_mtx);
					active_blocks.emplace_back(ptr);
					active_cv.notify_one();
				}
				else
				{
					std::lock_guard<std::mutex> lck(vacant_mtx);
					vacant_blocks.emplace_back(ptr);
					vacant_cv.notify_one();
				}
				std::lock_guard<std::mutex> shlck(shared_mtx);
				shared_blocks.erase(ptr);
			});
			data_ptr->resize(block_size); // diff

			std::lock_guard<std::mutex> shlck(shared_mtx);
			shared_blocks.insert(data_ptr);
			return data_share;
		}

		DataShare getActiveBlock()
		{
			if (dry) return nullptr;
			std::unique_lock<std::mutex> lck(active_mtx);
			active_cv.wait(lck, [this]
			{
				return !active_blocks.empty() || dry;
			});
			if (dry) return nullptr;

			auto data_ptr = active_blocks.front().release();
			active_blocks.pop_front();
			DataShare data_share(data_ptr, [this](DataType* ptr)
			{
				std::lock_guard<std::mutex> lck(vacant_mtx);
				vacant_blocks.emplace_back(ptr);
				vacant_cv.notify_one();
				ptr->make_vacant(); // diff

				std::lock_guard<std::mutex> shlck(shared_mtx);
				shared_blocks.erase(ptr);
			});

			std::lock_guard<std::mutex> shlck(shared_mtx);
			shared_blocks.insert(data_ptr);
			return data_share;
		}

	}; // class DataSource

} // namespace flow
