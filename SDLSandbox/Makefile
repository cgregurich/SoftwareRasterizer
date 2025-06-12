# SRCS specifies files to compile as part of the project
SRCS = SDLSandbox.cpp

# OBJ_NAME specifies the name of our executable
OBJ_NAME = ../build/SDLSandbox

INCLUDE_PATHS = -IC:/Users/gregurichcoli/graphics-learning/SoftwareRasterizer/SDL2/include/SDL2

LIBRARY_PATHS = -LC:/Users/gregurichcoli/graphics-learning/SoftwareRasterizer/SDL2/lib

COMPILER_FLAGS = -w -Wl,-subsystem,windows

LINKER_FLAGS = -lmingw32 -lSDL2main -lSDL2

all : $(SRCS)
	g++ $(SRCS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)

# TODO: i don't understand makefiles and this run thing nor this PHONY thing.
# .PHONY = run
# run: $(OBJ_NAME)
# 	$(OBJ_NAME)
