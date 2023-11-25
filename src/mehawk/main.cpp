#include <mehawk/app.hpp>

auto main(int argc, char** argv) -> int
{
	auto app = App();
	return app.run(argc, argv);
}
