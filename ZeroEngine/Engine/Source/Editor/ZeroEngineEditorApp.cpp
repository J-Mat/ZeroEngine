#include <spdlog/spdlog.h>


int main()
{
	spdlog::info("Hello, {}!", "World");
	return 0;
}