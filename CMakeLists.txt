CMAKE_MINIMUM_REQUIRED(VERSION 3.11)
PROJECT(Magpie)

set(SD2_INCLUDE_DIR "/usr/local/Cellar/sdl2/2.0.8/include/SDL2/")

include_directories(/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/ /usr/local/Cellar/libpng/1.6.35/include /usr/local/Cellar/glm/0.9.9.0/include ${SD2_INCLUDE_DIR} ${DIR_SRCS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -std=c++14")
AUX_SOURCE_DIRECTORY(. DIR_SRCS)
ADD_EXECUTABLE(main ${DIR_SRCS} Grid.cpp Grid.h)