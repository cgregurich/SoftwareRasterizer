SRCS = src/main.cpp src/Canvas.cpp src/CoordinateSpaces.cpp src/tgaimage.cpp

OBJ_NAME = build/main


INCLUDE_PATHS = -IC:/Users/gregurichcoli/graphics-learning/SoftwareRasterizer/SDL2/include/SDL2
# INCLUDE_PATHS = -IC:/Users/colin/code/SoftwareRasterizer/SDL2/include/SDL2

LIBRARY_PATHS = -LC:/Users/gregurichcoli/graphics-learning/SoftwareRasterizer/SDL2/lib
# LIBRARY_PATHS = -LC:/Users/colin/code/SoftwareRasterizer/SDL2/lib

COMPILER_FLAGS = -w -Wl,-subsystem,windows

# Why the -static things? Ran into a segfault when running on work computer and evidentally this
# could be caused by a dynamically loaded DLL, almost certainly ShellHelper64.dll from Admin By Request. 
# This fixes the issue so I'm running with it.
LINKER_FLAGS = -lmingw32 -lSDL2main -lSDL2 -static-libstdc++ -static-libgcc

TARGET = main

all : $(SRCS)
	g++ $(SRCS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)

debug: 
	g++ -g -O0 $(SRCS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)_debug

run: all
	./$(OBJ_NAME)