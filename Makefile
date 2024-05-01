all:
	mkdir -p output && gcc-12 -g -std=c99 *.c lib/*.c database/*.c ui/*.c -o output/paroolitasku -lsqlite3 `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0` `pkg-config --cflags libxml-2.0` `pkg-config --libs libxml-2.0`