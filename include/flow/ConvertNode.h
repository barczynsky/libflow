#pragma once
#include <flow/Node.h>
#include <flow/DataSource.h>

namespace flow
{
	template <typename T1, typename T2>
	class ConvertNode : public Node
	{
	public:
		ConvertNode()
		{
		}

	public:
		void bind(Node &lnode, Node &rnode)
		{
			// TODO: convert blocks from lnode into rnode
		}

	}; // class ConvertNode

} // namespace flow
