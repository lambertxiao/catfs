.PHONY: build
build:
	./do_cmake.sh && cd build && make

.PHONY: mount
mount:
	./bin/catfs -f /tmp/catfs

.PHONY: umount
umount:
	sudo umount -f /tmp/catfs
