SRCS= draw_operation.c draw.c ground.c IMG_Operations.c map.c
OBJS= $(SRCS:.c=.o)
HDRS= draw_operation.h draw.h ground.h IMG_Operations.h
CFLAGS += -Wall $(shell pkg-config.exe --cflags libpng) \
          $(shell sdl-config --cflags) 
LDFLAGS += $(shell pkg-config.exe --libs libpng) \
           -L$(shell sdl-config --prefix)/lib -lSDL_gfx -lSDL_image \
           $(shell sdl-config --libs)

%.o: %.c Makefile .depend
	$(CC) $(CFLAGS) -c -o $@ $< 

map: $(OBJS)
	$(CC) $(CFLAGS) -o $@ \
        $(OBJS) \
        $(LDFLAGS)

.depend: Makefile
	$(CC) -MM $(CFLAGS) $(SRCS) $(HDRS) 1> .depend

clean:
	rm -f $(OBJS) map .depend
