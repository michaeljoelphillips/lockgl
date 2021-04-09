.PHONY: install test debug tidy lint

CC = clang
LD_FLAGS = -lX11 -lGL -lcrypt -lGLEW -lpthread

TESTS = src/auth.test.c
HEADERS = src/auth.h src/display.h src/display/window.h
SRC = src/main.c src/auth.c src/display/x11.c src/display/window_screenshot.c

build/lockgl: ${SRC}
	${CC} ${LD_FLAGS} $^ -o $@

build/lockgl-debug: ${SRC}
	${CC} ${LD_FLAGS} -ggdb $^ -o $@

build/lockgl-test: ${SRC} ${TESTS}
	${CC} ${LD_FLAGS} -lcmocka -ggdb ${TESTS} -o $@ \
	-Wl,--wrap=getspnam \
	-Wl,--wrap=endspent \
	-Wl,--wrap=getuid \
	-Wl,--wrap=setuid \
	-Wl,--wrap=getgid \
	-Wl,--wrap=setgid

install: build/lockgl
	sudo install -D -o root -g root -m 4755 build/lockgl /usr/bin/lockgl
	sudo install -D -o root -g root -m 444 shaders/blur.fragment.glsl /usr/share/lockgl/blur.fragment.glsl
	sudo install -D -o root -g root -m 444 shaders/passthrough.vertex.glsl /usr/share/lockgl/passthrough.vertex.glsl

test: build/lockgl-test
	$^

debug: build/lockgl-debug
	gdb $^

tidy: ${SRC} ${HEADERS}
	clang-tidy --fix --fix-errors $^

lint: ${SRC} ${HEADERS}
	clang-format -i $^
