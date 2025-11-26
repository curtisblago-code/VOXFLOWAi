cmake_minimum_required(VERSION 3.15)

project(VOXFLOWAi VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(FetchContent)

set(JUCE_ENABLE_MODULE_SOURCE_GROUPS ON CACHE BOOL "Enable JUCE source groups" FORCE)
set(JUCE_BUILD_EXTRAS OFF CACHE BOOL "Disable JUCE extras" FORCE)
set(JUCE_BUILD_EXAMPLES OFF CACHE BOOL "Disable JUCE examples" FORCE)

FetchContent_Declare(
  juce
  GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
  GIT_TAG 7.0.12
)

FetchContent_MakeAvailable(juce)

juce_add_gui_app(
  VOXFLOWAi
  PRODUCT_NAME "VOXFLOWAi"
  COMPANY_NAME "VOXFLOWAi"
  COMPANY_COPYRIGHT "Unlicensed"
)

juce_generate_juce_header(VOXFLOWAi)

set(VOXFLOWAI_SOURCES
  Source/Main.cpp
  Source/MainComponent.cpp
  Source/AudioEngine.cpp
)

set(VOXFLOWAI_HEADERS
  Source/MainComponent.h
  Source/AudioEngine.h
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${VOXFLOWAI_SOURCES} ${VOXFLOWAI_HEADERS})

target_sources(
  VOXFLOWAi
  PRIVATE ${VOXFLOWAI_SOURCES} ${VOXFLOWAI_HEADERS}
)

target_compile_definitions(
  VOXFLOWAi
  PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_REPORT_APP_USAGE=0
    JUCE_DISABLE_JUCE_VERSION_PRINTING=1
)

target_link_libraries(
  VOXFLOWAi
  PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_processors
    juce::juce_audio_basics
    juce::juce_gui_extra
    juce::juce_dsp
)

if (MSVC)
  target_compile_options(VOXFLOWAi PRIVATE /W4 /permissive-)
else()
  target_compile_options(VOXFLOWAi PRIVATE -Wall -Wextra -Wpedantic)
endif()
