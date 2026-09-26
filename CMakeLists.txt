## TODO
# Might want to download SFML-3.0.0 and make a CMakeLists.txt file for its subdirectory so that it can build the lib files
# Need to add build configurations for release & debug on x64, for both the client and server.
# Add Precompiled Header
#
## IDEA
# For the 2 different exe's, I can have folders exclusive to each for server & client only source files.
# I can then have a 3rd folder for any shared source files such as util.h



set(CMAKE_POLICY_VERSION_MINIMUM 3.29 CACHE STRING "")
cmake_minimum_required(VERSION 3.29)
project(
    NetworkingProject
    DESCRIPTION "A Networked game, created for CMP415 Networking University Module"
    HOMEPAGE_URL "https://github.com/Toddynator/CMP425-Networking-Project"
    LANGUAGES CXX C
)



## GLOBAL SETTINGS



# C / C++ Language Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
#set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
# Set the output directory for all binaries
set(OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
# Set the output directory to sort by platform (Output directory will be something like "bin/windows-amd64-debug")
# NOTE: Multi-configuration generators (such as visual studio) append a per-configuration subdirectory to the specified directory 
# https://cmake.org/cmake/help/latest/prop_tgt/LIBRARY_OUTPUT_DIRECTORY.html
# e.g. OUTPUT_DIRECTORY_RELEASE
#foreach(config DEBUG RELEASE)
#    # Sorts by the different types of binaries
#    # DLL, EXE
#    #foreach(var CMAKE_LIBRARY_OUTPUT_DIRECTORY_${config} CMAKE_RUNTIME_OUTPUT_DIRECTORY_${config})
#    # NOTE: Setting runtime output directory seems to overwrite any custom output directory set on executables. So only do this for libraries instead.
#    foreach(var CMAKE_LIBRARY_OUTPUT_DIRECTORY_${config})
#        set(${var} "${OUTPUT_DIRECTORY}/${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}-${config}")
#        string(TOLOWER "${${var}}" ${var})
#    endforeach()
#
#    # Lib's should stay in build folder, not needed for shipping / after the executable has been generated.
#    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${config} "${CMAKE_SOURCE_DIR}/out/lib/${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}-${config}")
#endforeach()
# Install directory, whatever that is
set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/install")
# Remove Unused Configurations (Use only Release & Debug)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
if (MSVC)
    add_compile_options($<$<CONFIG:Debug>:/ZI>)
endif()



## PROJECT



# Get the Server source code
file(GLOB_RECURSE SERVER_SRC CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/Server/*.cpp"
    "${CMAKE_SOURCE_DIR}/Server/*.h"
)
# Get the Client source code
file(GLOB_RECURSE CLIENT_SRC CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/Client/*.cpp"
    "${CMAKE_SOURCE_DIR}/Client/*.h"
)
file(GLOB_RECURSE SHARED_SRC CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/Shared/*.h"
    "${CMAKE_SOURCE_DIR}/Shared/*.cpp"
)
# IDE Project will use the same Folder Structure as found in the source files (If the IDE supports it)
# In VS2022 for example, this ONLY applies to CMake Targets view. But it is unlikely I will use that.
#source_group(TREE "${CMAKE_SOURCE_DIR}/SFML-ECS-Networking/SFML-ECS-Networking" PREFIX "src" FILES ${SERVER_SRC})
# Create the server exe
add_executable(${PROJECT_NAME}-Server ${SERVER_SRC} ${SHARED-SRC})
set_target_properties(${PROJECT_NAME}-Server PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIRECTORY}/${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}-Debug-Server)
set_target_properties(${PROJECT_NAME}-Server PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIRECTORY}/${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}-Release-Server)
# Create the client exe
add_executable(${PROJECT_NAME}-Client ${CLIENT_SRC} ${SHARED-SRC})
set_target_properties(${PROJECT_NAME}-Client PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIRECTORY}/${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}-Debug-Client)
set_target_properties(${PROJECT_NAME}-Client PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIRECTORY}/${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}-Release-Client)
# Include the Source Code
target_include_directories(${PROJECT_NAME}-Server PUBLIC ${CMAKE_SOURCE_DIR}/Server PUBLIC ${CMAKE_SOURCE_DIR}/Shared)
target_include_directories(${PROJECT_NAME}-Client PUBLIC ${CMAKE_SOURCE_DIR}/Client PUBLIC ${CMAKE_SOURCE_DIR}/Shared)
# Precompiled Header [TODO]
#target_precompile_headers(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/NetworkingProject/Core/pch.h")
# Copy over required files into bin folder with the executables
#add_custom_command(
#    TARGET ${PROJECT_NAME}-Server POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy_directory
#        "${CMAKE_SOURCE_DIR}/external/SFML-3.0.0/bin"
#        "$<TARGET_FILE_DIR:${PROJECT_NAME}-Server>"
#)
#add_custom_command(
#    TARGET ${PROJECT_NAME}-Client POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy_directory
#        "${CMAKE_SOURCE_DIR}/external/SFML-3.0.0/bin"
#        "$<TARGET_FILE_DIR:${PROJECT_NAME}-Client>"
#)



## EXTERNAL LIBRARIES / PROJECTS



## ENTT
target_include_directories(${PROJECT_NAME}-Server PUBLIC ${CMAKE_SOURCE_DIR}/external/Entt)
target_include_directories(${PROJECT_NAME}-Client PUBLIC ${CMAKE_SOURCE_DIR}/external/Entt)
## SFML
add_subdirectory("external/SFML-3.0.0" EXCLUDE_FROM_ALL)
target_link_libraries(${PROJECT_NAME}-Server
    PRIVATE SFML::Graphics
    PRIVATE SFML::Network
)
target_link_libraries(${PROJECT_NAME}-Client
    PRIVATE SFML::Graphics
    PRIVATE SFML::Network
)