#include "include/application.hpp"
#include <iostream>

int main() {
	try {
		Application app;
		app.run();
		return 0;
	} catch (const std::exception &e) {
		std::cerr << "Fatal error: " << e.what() << std::endl;
		return -1;
	}
}
