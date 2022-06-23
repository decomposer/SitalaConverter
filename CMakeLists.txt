cmake_minimum_required(VERSION 3.15)

add_subdirectory(JUCE)

project(SitalaImporter)

set(CMAKE_PROJECT_VERSION "0.1")

juce_add_gui_app("${PROJECT_NAME}"
  PRODUCT_NAME "${PROJECT_NAME}"
  VERSION "0.1"
  BUILD_VERSION "${PROJECT_VERSION}"
)

juce_generate_juce_header("${PROJECT_NAME}")

target_sources("${PROJECT_NAME}" PUBLIC
  Source/AbletonDeviceGroupReader.h
  Source/AbletonDeviceGroupReader.cpp
  Source/MainComponent.h
  Source/MainComponent.cpp
  Source/SitalaKitGenerator.h
  Source/SitalaKitGenerator.cpp
  Source/main.cpp
)

target_link_libraries("${PROJECT_NAME}" PUBLIC
  juce::juce_gui_basics
  juce::juce_audio_basics
  juce::juce_audio_formats
  juce::juce_core
  juce::juce_cryptography
  juce::juce_data_structures

  juce::juce_recommended_lto_flags
  juce::juce_recommended_warning_flags
  juce::juce_recommended_config_flags
)

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)
target_compile_definitions(${PROJECT_NAME} PUBLIC JUCE_DISPLAY_SPLASH_SCREEN=0)

set(artefacts "${CMAKE_BINARY_DIR}/${PROJECT_NAME}_artefacts")

target_include_directories(${PROJECT_NAME} PRIVATE
  ${artefacts})

configure_file(Config.h.in "${artefacts}/Config.h")
