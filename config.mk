ifeq ($(OS),Windows_NT)
RM = del /q $(subst /,\,$(1))
#RM = del /q
RMD = rmdir /Q /S $(subst /,\,$(1))
MKD = cmd /c if not exist $1 (mkdir $1)
else
#RM = rm
RM = rm $1
RMD = rm -r $1
MKD = mkdir -p $1
endif

#CLEANDIR = $(call RMD,$1) 
#mkdir $1

ifneq ($(LLVM),)
CC  = clang
LD  = ld
else
CC  = gcc
LD  = ld
endif

