# CC refers to the compiler
CC = gcc

# OBJ_SRC refers to the files to be compiled
OBJ_SRC = main.c

# OBJ_OUT refers to the output binary
OBJ_OUT = output

# LFS refers to linker flags
LFS = -lSDL2 -lSDL2_ttf

# CFS refers to compiler flags
#CFS

all : main.c
	$(CC) $(OBJ_SRC) $(LFS) -o $(OBJ_OUT)
