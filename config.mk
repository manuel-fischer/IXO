ifeq ($(OS),Windows_NT)
RM = del
else
RM = rm
endif

ifneq ($(LLVM),)
CC  = clang
LD  = ld
else
CC  = gcc
LD  = ld
endif

