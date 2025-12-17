#include <filesystem>
#include "peztool/peztool.hpp"
#include "peztool/utils/misc.hpp"
#include "./scene.hpp"


int main(int const argc, char* const argv[])
{
    // The configuration file to load
    auto constexpr conf_filename{"conf.txt"};
    // Set working directory
    pez::setWorkingDirectoryToExecutablePath(argv[0]);
    // Create the App
    pez::App app("TimeTracker", conf_filename);
    app.addScene<TimeTracker>();
    // Spin the application until exit requested
    app.run();
    return 0;
}
