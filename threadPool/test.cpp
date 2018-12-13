#include "thread_pool.hpp"
#include <iostream>

int main(int argc, char const *argv[])
{
	newplan::ThreadPool tp(10);
	for (int index = 0; index < 1000; index++)
	{
		tp.runTask([]
		{
			std::cout << "Hello, World!" << std::endl;
		});
	}
	tp.waitWorkComplete();
	return 0;
}