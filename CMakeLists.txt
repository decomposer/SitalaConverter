cmake_minimum_required(VERSION 3.15)

add_subdirectory(JUCE)

project(SitalaImporter)

set(CMAKE_PROJECT_VERSION "0.1")

juce_add_console_app("${PROJECT_NAME}"
  PRODUCT_NAME "${PROJECT_NAME}"
  VERSION "0.1"
  BUILD_VERSION "${PROJECT_VERSION}"
)

juce_generate_juce_header("${PROJECT_NAME}")

target_sources("${PROJECT_NAME}" PUBLIC
  Source/ADGReader.h
  Source/ADGReader.cpp
  Source/SitalaKitGenerator.h
  Source/SitalaKitGenerator.cpp
  Source/Main.cpp
)

target_link_libraries("${PROJECT_NAME}" PUBLIC
  juce::juce_audio_basics
  juce::juce_audio_devices
  juce::juce_audio_formats
  juce::juce_audio_plugin_client
  juce::juce_audio_processors
  juce::juce_audio_utils
  juce::juce_core
  juce::juce_cryptography
  juce::juce_data_structures
  juce::juce_events
  juce::juce_graphics
  juce::juce_gui_basics
  juce::juce_gui_extra
  juce::juce_dsp

  juce::juce_recommended_lto_flags
  juce::juce_recommended_warning_flags
  juce::juce_recommended_config_flags
)

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)

set(artefacts "${CMAKE_BINARY_DIR}/${PROJECT_NAME}_artefacts")

target_include_directories(${PROJECT_NAME} PRIVATE
  ${artefacts})

configure_file(Config.h.in "${artefacts}/Config.h")
