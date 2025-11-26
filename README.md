# VOXFLOWAi

VOXFLOWAi is a standalone, pre-DAW vocal processor built with JUCE. It targets Windows and macOS as a C++17 GUI application that captures live input, applies an AI-inspired safety chain, and exposes a polished feed to any DAW or streaming app.

## Project layout
- `CMakeLists.txt` – CMake entry point that fetches JUCE via `FetchContent` and builds the `VOXFLOWAi` GUI app.
- `Source/` – Application sources:
  - `Main.cpp` – JUCE application entry point and window wiring.
  - `MainComponent.*` – UI shell with gain, bypass, and safety controls.
  - `AudioEngine.*` – Real-time audio I/O with a lightweight DSP chain placeholder for AI processing.

## Build (Windows/macOS/Linux)
1. Install a C++17 toolchain and CMake 3.15+.
2. Configure and build:
   ```bash
   cmake -S . -B build
   cmake --build build --config Release
   ```
3. On Windows, run the produced `build/Release/VOXFLOWAi.exe`. On macOS/Linux, run the built `VOXFLOWAi` binary (note: Linux requires standard JUCE audio/GUI dependencies).

## Notes
- The DSP chain currently uses a high-pass filter, compressor, and waveshaper as a placeholder for the AI processor so the signal path stays stable.
- Use the **Safe mode** toggle to bypass the waveshaper when running on constrained hardware.
- If the repository previously used the `VOXFLO` name, all identifiers have been normalised to **VOXFLOWAi** for consistency.
