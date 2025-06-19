SRCS = src/main.cpp src/Canvas.cpp src/CoordinateSpaces.cpp src/tgaimage.cpp

OBJ_NAME = build/main

INCLUDE_PATHS = -IC:/Users/colin/code/SoftwareRasterizer/SDL2/include/SDL2

LIBRARY_PATHS = -LC:/Users/colin/code/SoftwareRasterizer/SDL2/lib

COMPILER_FLAGS = -w -Wl,-subsystem,windows

LINKER_FLAGS = -lmingw32 -lSDL2main -lSDL2

TARGET = main

all : $(SRCS)
	g++ $(SRCS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)

run: all
	./$(OBJ_NAME)