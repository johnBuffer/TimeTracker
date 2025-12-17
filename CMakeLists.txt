cmake_minimum_required(VERSION 3.16)
project(TimeTracker LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(BUILD_SHARED_LIBS OFF)
set(SFML_USE_SYSTEM_DEPS OFF)

include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)

# Fetch SFML
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.x)
FetchContent_MakeAvailable(SFML)

include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)

# Automatically adds all cpp files contained in the src directory
file(GLOB_RECURSE source_files src/*.cpp)
set(SOURCES ${source_files})

function(create_default_target name)
   add_executable(${name} ${SOURCES})
   target_include_directories(${name} PRIVATE "src")
   target_link_libraries(${name} PRIVATE sfml-graphics sfml-audio)
   target_compile_features(${name} PRIVATE cxx_std_20)

   # Copy res dir to the binary directory
   add_custom_command(
       TARGET ${name}
       COMMENT "Copy Res directory"
       PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:${name}>/res
       VERBATIM)
endfunction()

create_default_target(${PROJECT_NAME}_full_version)
create_default_target(${PROJECT_NAME}_demo)
target_compile_definitions(${PROJECT_NAME}_demo PRIVATE DEMO=1)
