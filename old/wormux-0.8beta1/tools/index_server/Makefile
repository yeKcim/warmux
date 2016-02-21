dynamic:
	make -f Makefile.dynamic

static:
	make -f Makefile.static

help:
	@echo "* Use 'make' to build a dynamic binary"
	@echo "* Use 'make static' to build a statically linked binary (default)"

clean:
	@rm -f *.o *~ wormux_server 2>&1 > /dev/null
