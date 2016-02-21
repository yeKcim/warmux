dynamic:
	$(MAKE) -f Makefile.dynamic

static:
	$(MAKE) -f Makefile.static

help:
	@echo "* Use 'make' to build a dynamic binary"
	@echo "* Use 'make static' to build a statically linked binary (default)"

clean:
	@rm -f *.[od] *~ wormux_server 2>&1 > /dev/null
