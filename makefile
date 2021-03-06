CC=gcc
CFLAGS=-Wall -std=c99 -O2 -DHAVE_OPENGL -I/usr/local/include
LDFLAGS=-lSDL2 -lSDL2_ttf -lSDL2_gfx -lSDL2_mixer -lm
BUILD_FILES=Project03_01

Project03_01: Project03_01.c GameInitialization.c GameFunctions.c
	$(CC) $^ -o Project03_01 $(CFLAGS) $(LDFLAGS)

.PHONY: clean
clean:
	rm -f *.o $(BUILD_FILES)

.PHONY: gdb
gdb:
	$(CC) Project03_01.c GameInitialization.c GameFunctions.c -o Project03_01 $(CFLAGS) $(LDFLAGS) -g
