.PHONY: build
build:
	./do_cmake.sh && cd build && make

.PHONY: run
run: build
	./bin/catfs -b xsx -m /tmp/catfs -f

.PHONY: umount
umount:
	umount -f /tmp/catfs
