CDEBUG=-g
CFLAGS=$(CDEBUG) -I. -Ix -D_XOPEN_SOURCE=700
SRCS_C = nts-term.c x/keyboard.c unicode.c x/font.c x/common.c
OBJS = $(SRCS_C:.c=.o)
LIBS=-lxcb

.PHONY: all
all: nts-term

nts-term: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

.PHONY: clean
clean:
	rm -f *.o nts-term
