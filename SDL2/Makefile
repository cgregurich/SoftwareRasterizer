# SRCS specifies which files to compile as part of the project
SRCS = 01_hello_SDL/01_hello_SDL.cpp

# OBJ_NAME specifies the name of our executable
OBJ_NAME = ../build/01_hello_SDL

INCLUDE_PATHS = -IC:/Users/gregurichcoli/graphics-learning/SoftwareRasterizer/SDL2/include/SDL2

LIBRARY_PATHS = -LC:/Users/gregurichcoli/graphics-learning/SoftwareRasterizer/SDL2/lib

COMPILER_FLAGS = -w -Wl,-subsystem,windows

LINKER_FLAGS = -lmingw32 -lSDL2main -lSDL2

# This is the target that compiles our executable
all : $(SRCS)
	g++ $(SRCS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)
