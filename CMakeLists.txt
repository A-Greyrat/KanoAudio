cmake_minimum_required(VERSION 3.20)
project(KanoAudio)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Xclang -O3 -Xclang -Os -m64")

# need vcpkg to install these
# OpenAL(for playing audio), mpg123(decode mp3), libvorbis(decode ogg)
# optional: glfw3, glad, imgui(for graphical test)

find_package(OpenAL CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(MPG123 CONFIG REQUIRED)
find_package(Vorbis CONFIG REQUIRED)
find_package(flac CONFIG REQUIRED)

add_executable(${PROJECT_NAME} main.cpp KanoAudio/Audio.cpp KanoAudio/Decoder/PCMDecoder.cpp KanoAudio/Decoder/PCMDecoder.h KanoAudio/Decoder/WAVDecoder.cpp KanoAudio/Decoder/WAVDecoder.h KanoAudio/Decoder/MP3Decoder.cpp KanoAudio/Decoder/MP3Decoder.h KanoAudio/Decoder/OGGDecoder.cpp KanoAudio/Decoder/OGGDecoder.h KanoAudio/Decoder/FLACDecoder.cpp KanoAudio/Decoder/FLACDecoder.h)

target_link_libraries(${PROJECT_NAME} PRIVATE OpenAL::OpenAL glad::glad glfw imgui::imgui MPG123::libmpg123 MPG123::libout123 MPG123::libsyn123 Vorbis::vorbisfile FLAC::FLAC)