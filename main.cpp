/**
 * @file main.cpp
 * @brief Entry point for the Load Balancer simulation.
 *
 * @details Reads the configuration file path from the user or uses a default,
 * seeds the random number generator, constructs the LoadBalancer, and starts
 * the simulation by calling run(). All configuration is handled via config.txt
 * so no code edits are needed to change simulation parameters.
 *
 * @author Larissa Kim
 * @date 2/27/2026
 */

#include <iostream>
#include <cstdlib> 
#include <ctime>
#include <stdexcept>
#include "LoadBalancer.h"

/**
 * @brief Main entry point.
 * @details Accepts an optional command-line argument for the config file path.
 * If none is provided, defaults to "config.txt" in the current directory.
 * @param argc Argument count.
 * @param argv Argument vector. argv[1] (optional) is the config file path.
 * @return 0 on success, 1 on error.
 */
int main(int argc, char* argv[]) {
    srand(static_cast<unsigned int>(time(nullptr)));

    std::string configFile = "config.txt";
    if (argc >= 2) {
        configFile = argv[1];
        std::cout << "Using config file: " << configFile << "\n";
    } else {
        std::cout << "No config file specified. Using default: config.txt\n";
    }

    try {
        LoadBalancer lb(configFile);
        lb.run();
    } catch (const std::exception& e) {
        std::cerr << "\033[31m[ERROR] " << e.what() << "\033[0m\n";
        return 1;
    }

    return 0;
}