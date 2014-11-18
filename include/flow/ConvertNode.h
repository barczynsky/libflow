#pragma once
#include <flow/DataSource.h>
#include <flow/Node.h>

namespace flow
{
	template <typename T>
	class ConvertNode : public Node
	{
	private:
		DataSource<T> data_supply;

	public:
		ConvertNode()
		{
		}

	}; // class ConvertNode

} // namespace flow
