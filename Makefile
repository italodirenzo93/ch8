setup:
	meson setup builddir

build:
	meson compile -C builddir

run:
	./builddir/ch8
