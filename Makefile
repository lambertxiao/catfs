.PHONY: build
build:
	./do_cmake.sh && cd build && make -j 6

.PHONY: run
run: build
	./bin/catfs -b xsx -m /tmp/catfs -f

.PHONY: umount
umount:
	umount -f /tmp/catfs
