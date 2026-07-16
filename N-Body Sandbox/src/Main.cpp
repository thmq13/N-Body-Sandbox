#include <cstdlib>
#include <exception>
#include <iostream>

#include <NBodySandboxApplication.hpp>

int main() {
    std::cout << "\x1B[2J\x1B[H";
    std::cout << "We are but dust in the void waiting for the inevitable collapse.\n";

    try {
        NBodySandboxApplication application;
        application.setUpAndGenerateScene();
        application.executeMasterLoop();
    }
    catch (const std::exception& error) {
        std::cerr << "[Fatal Error Intercepted] Exception detail: " << error.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}