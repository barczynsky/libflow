#include <algorithm>
#include <chrono>
#include <iostream>
#include <thread>

#include <flow/flow.h>
#include <flow/INode.h>
#include <flow/ONode.h>
// #include <flow/ConvertNode.h>


int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	fprintf(stderr, "__cplusplus=%ld\n", __cplusplus);

	flow::INode<float> data_input(64*1024, 2);
	data_input.bindSource(std::cin);
	flow::ONode<short> data_output(64*1024, 4);
	data_output.bindSink(std::cout);
	// flow::DataSource<short> data_output(64*1024, 2);

	std::thread custom_thread = std::thread([&]
	{
		while (flow::sync())
		{
			auto block1 = data_input.getActiveBlock();
			if (block1 == nullptr)
				break;
			auto block2 = data_output.getVacantBlock();
			if (block2 == nullptr)
				break;
			std::copy(block1->begin(), block1->end(), block2->begin());
			block2->make_active();
		}
	});

	// std::thread custom_thread = std::thread([&]
	// {
	// 	while (flow::sync())
	// 	{
	// 		auto block = data_input.getActiveBlock();
	// 		// fprintf(stderr, "%d\n", block->at(0));
	// 		// std::this_thread::sleep_for(std::chrono::milliseconds(50));
	// 	}
	// });

	flow::run();
	while (flow::sync())
		std::this_thread::sleep_for(std::chrono::milliseconds(50));

	if (custom_thread.joinable())
		custom_thread.join();
}
