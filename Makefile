# parallel --jobs
MAKEFLAGS += -j16

CFLAGS =

include config.mk

DUMMY := $(shell $(call MKD,obj))
DUMMY := $(shell $(call MKD,bin))
DUMMY := $(shell $(call MKD,lib))

SRC_FILES = $(wildcard src/*.c)
OBJ_FILES = $(foreach sfile,$(SRC_FILES), \
				$(subst src/,obj/,$(subst .c,.o,$(sfile))))

CFLAGS = -O3
INCLUDES = -Iinclude

.PHONY: build-lib
build-lib: lib/libIXO.a
#	$(MAKE) -C ./src/ build

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< $(INCLUDES) -o $@

lib/libIXO.a: $(OBJ_FILES)
	ar rvs $@ $(OBJ_FILES)

bin/test_IXO: tst/test_IXO.c lib/libIXO.a
	$(CC) $(CFLAGS) tst/test_IXO.c lib/libIXO.a $(INCLUDES) -o $@

.PHONY: build-test test
build-test: bin/test_IXO	

test: build-test
	./bin/test_IXO


.PHONY: clean
clean:
	$(call RMD,obj)
	$(call RMD,bin)
	$(call RMD,lib)

