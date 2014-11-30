#include <algorithm>
#include <chrono>
#include <iostream>
#include <thread>

#include <flow/flow.h>
#include <flow/DataSource.h>
// #include <flow/INode.h>
// #include <flow/ONode.h>
// #include <flow/IONode.h>
// #include <flow/ConvertNode.h>


int main()
{
	fprintf(stderr, "__cplusplus=%ld\n", __cplusplus);

	flow::DataSource<char> buffer(64*1024, 4);

	std::thread rd_thread = std::thread([&]
	{
		while (flow::sync())
		{
			auto block = buffer.getVacantBlock();
			if (block == nullptr)
				break;

			size_t count = 0;
			while (std::cin.good() && count < block->data_size())
				count += std::cin.read((char*)block->data(), block->data_size()).gcount();

			if (!std::cin.good())
				break;
			block->make_active();
		}
	});

	std::thread wr_thread = std::thread([&]
	{
		while (flow::sync())
		{
			auto block = buffer.getActiveBlock();
			if (block == nullptr)
				break;

			std::cout.write((char*)block->data(), block->data_size());

			if (!std::cout.good())
				break;
		}
	});

	flow::run();
	while (flow::sync())
		std::this_thread::sleep_for(std::chrono::milliseconds(50));

	if (wr_thread.joinable())
		wr_thread.join();
	if (rd_thread.joinable())
		rd_thread.join();
}
