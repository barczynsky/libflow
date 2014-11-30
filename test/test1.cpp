#include <algorithm>
#include <chrono>
#include <iostream>
#include <thread>

#include <flow/flow.h>
#include <flow/INode.h>
#include <flow/ONode.h>
// #include <flow/IONode.h>
// #include <flow/ConvertNode.h>


int main()
{
	fprintf(stderr, "__cplusplus=%ld\n", __cplusplus);

	flow::INode<float> source_node(65*1024, 2);
	source_node.bindSource(std::cin);
	flow::ONode<short> sink_node(65*1024, 2);
	sink_node.bindSink(std::cout);

	// flow::IONode<char> pipe_node(64*1024, 2);
	// pipe_node.bindSource(std::cin);
	// pipe_node.bindSink(std::cout);

	// flow::DataSource<short> storage(64*1024, 2);
	// std::thread custom_thread = std::thread([&]
	// {
	// 	while (flow::sync())
	// 	{
	// 		auto block = source_node.getActiveBlock();
	// 		// fprintf(stderr, "%d\n", block->at(0));
	// 		// std::this_thread::sleep_for(std::chrono::milliseconds(50));
	// 	}
	// });

	std::thread custom_thread = std::thread([&]
	{
		while (flow::sync())
		{
			auto block1 = source_node.getActiveBlock();
			if (block1 == nullptr)
				break;
			auto block2 = sink_node.getVacantBlock();
			if (block2 == nullptr)
				break;
			std::copy(block1->begin(), block1->end(), block2->begin());
			block2->make_active();
			// std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	});

	flow::run();
	while (flow::sync())
		std::this_thread::sleep_for(std::chrono::milliseconds(50));

	if (custom_thread.joinable())
		custom_thread.join();
}
