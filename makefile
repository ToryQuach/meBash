.PHONY: build run clean

build:
	make clean && mkdir -p build && cd build && cmake .. && cmake --build .

run:
	cd build && ./meBash

clean:
	rm -rf build