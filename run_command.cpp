#include "run_command.hpp"

#include <array>
#include <cstdio>
#include <iostream>
#include <memory>

#include <unistd.h>

void runPortage(const std::string& package)
{
    const std::string command("emerge --nospinner --ask=n " + package);
    std::cout << "Running command: " << command << std::endl;

    std::array<char, 1024> buffer; // 1 KiB of output.
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
    if (not pipe) {
        std::cerr << "There was an error while trying to run command: " << command << std::endl;
        return;
    }

    while (std::fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        std::cout << buffer.data();
    }
}
