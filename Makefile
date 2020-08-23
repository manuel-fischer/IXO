CFLAGS =

SOURCE_DIR = "$(CURDIR)"

include config.mk

build: 
	$(MAKE) -C ./src/ build -D

clean:
	$(RM) ./obj/*
	$(RM) ./bin/*
