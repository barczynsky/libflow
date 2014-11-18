#pragma once
#include <vector>

namespace flow
{
	template <typename T>
	class DataBlock
	{
		using size_type = typename std::vector<T>::size_type;

	private:
		std::vector<T> data_block;
		bool data_active{ false };

	public:
		DataBlock()
		{
		}

		DataBlock(size_type n): data_block(n)
		{
		}

		decltype(auto) begin()
		{
			return data_block.begin();
		}

		decltype(auto) end()
		{
			return data_block.end();
		}

		decltype(auto) size()
		{
			return data_block.size();
		}

		decltype(auto) empty()
		{
			return data_block.empty();
		}

		decltype(auto) data()
		{
			return data_block.data();
		}

		decltype(auto) resize(size_type n)
		{
			return data_block.resize(n);
		}

		decltype(auto) at(size_type n)
		{
			return data_block.at(n);
		}

	public:
		size_t value_size() const
		{
			return sizeof(T);
		}

		size_t data_size() const
		{
			return data_block.size() * sizeof(T);
		}

		void data_resize(size_type n)
		{
			data_block.resize(n / sizeof(T));
		}

	public:
		void make_active()
		{
			data_active = true;
		}

		void make_vacant()
		{
			data_active = false;
		}

		bool active() const
		{
			return data_active;
		}

	}; // class DataBlock

} // namespace flow
