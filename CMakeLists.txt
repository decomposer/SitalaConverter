cmake_minimum_required(VERSION 3.15)

add_subdirectory(JUCE)

project(SitalaImporter)

set(CMAKE_PROJECT_VERSION "0.1")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
find_package(Qt6 COMPONENTS Widgets REQUIRED)

add_executable(${PROJECT_NAME} MACOSX_BUNDLE
  Source/AbletonDeviceGroupReader.h
  Source/AbletonDeviceGroupReader.cpp
  Source/SitalaKitGenerator.h
  Source/SitalaKitGenerator.cpp
  Source/main.cpp
)

target_compile_definitions(${PROJECT_NAME} PRIVATE JUCE_STANDALONE_APPLICATION=1)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  target_compile_definitions(${PROJECT_NAME} PRIVATE _CONSOLE=1)
endif()

# Call the internal JUCE CMake function since we're using JUCE as a library,
# not creating an executable

_juce_initialise_target(${PROJECT_NAME}
  PRODUCT_NAME "${PROJECT_NAME}"
  VERSION "0.1"
  BUILD_VERSION "${PROJECT_VERSION}"
)

# Reset these back to CMake defaults (instead of JUCE's scheme)

set_target_properties(${PROJECT_NAME} PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

juce_generate_juce_header("${PROJECT_NAME}")

target_link_libraries("${PROJECT_NAME}" PUBLIC
  juce::juce_audio_basics
  juce::juce_audio_formats
  juce::juce_core
  juce::juce_cryptography
  juce::juce_data_structures

  juce::juce_recommended_lto_flags
  juce::juce_recommended_warning_flags
  juce::juce_recommended_config_flags

  Qt6::Widgets
)

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR})
configure_file(Config.h.in Config.h)
