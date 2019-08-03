CDEBUG=-g
CFLAGS=$(CDEBUG) -I. -D_XOPEN_SOURCE=700
SRCS_C = nts-term.c keyboard.c unicode.c
OBJS = $(SRCS_C:.c=.o)
LIBS=-lxcb

.PHONY: all
all: nts-term

nts-term: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

.PHONY: clean
clean:
	rm -f *.o nts-term
