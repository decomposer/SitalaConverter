cmake_minimum_required(VERSION 3.15)

add_subdirectory(JUCE)

set(CMAKE_OSX_DEPLOYMENT_TARGET "10.10")
set(CMAKE_OSX_ARCHITECTURES "arm64" "x86_64")

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
  Source/LayoutManager.h
  Source/LayoutManager.cpp
  Source/LayoutManagedComponent.h
  Source/LayoutManagedComponent.cpp
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
target_compile_definitions(${PROJECT_NAME} PUBLIC
  JUCE_MODAL_LOOPS_PERMITTED=1
  JUCE_DISPLAY_SPLASH_SCREEN=0)

set(artefacts "${CMAKE_BINARY_DIR}/${PROJECT_NAME}_artefacts")

target_include_directories(${PROJECT_NAME} PRIVATE
  ${artefacts})

configure_file(Config.h.in "${artefacts}/Config.h")

if(WIN32)
  add_custom_target(SignedBinary
    DEPENDS ${PROJECT_NAME}
    COMMAND signtool "sign"
      "/t" "http://timestamp.digicert.com"
      "/a" "$<TARGET_FILE:${PROJECT_NAME}>")
elseif(APPLE)
  set(MAC_APP_ID_IDENTITY $ENV{SITALA_CODESIGN_IDENTITY})
  set(BUNDLE_ID "de.decomposer.${PROJECT_NAME}")

  add_custom_target(SignedBinary
    DEPENDS ${PROJECT_NAME}
    COMMAND codesign
      "-s" ${MAC_APP_ID_IDENTITY}
      "--options=runtime"
      "--force"
      "$<TARGET_BUNDLE_DIR:${PROJECT_NAME}>"
    VERBATIM)

  add_custom_target(DmgPackage
    DEPENDS SignedBinary
    COMMAND
      "${CMAKE_COMMAND}" "-E" "remove_directory" "DmgContents"
    COMMAND
      "${CMAKE_COMMAND}" "-E" "make_directory" "DmgContents"
    COMMAND
      "cp" "-R" "$<TARGET_BUNDLE_DIR:${PROJECT_NAME}>" "DmgContents/"
    COMMAND
      "osascript" "-e"
      "tell application \"Finder\" to make alias file to POSIX file \"/Applications\" at POSIX file \"${CMAKE_CURRENT_BINARY_DIR}/DmgContents\""
    VERBATIM
    COMMAND
      "${CMAKE_COMMAND}" "-E" "remove" "${PROJECT_NAME}.dmg"
    COMMAND
      "hdiutil" "create" "${PROJECT_NAME}.dmg"
      "-fs" "HFS+"
      "-volname" "${PROJECT_NAME}"
      "-srcfolder" "DmgContents"
    COMMAND codesign
      "-s" ${MAC_APP_ID_IDENTITY}
      "--force"
      "${PROJECT_NAME}.dmg"
     VERBATIM)
 endif()
