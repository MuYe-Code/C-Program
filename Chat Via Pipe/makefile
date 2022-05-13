SRCS:=$(wildcard *.c)
OUTS:=$(patsubst %.c, %, $(SRCS))
CC:=gcc

.PHONY: all clean rebuild

all: $(OUTS)

%: %.c
	$(CC) $^ -o $@ -I ./

clean:
	$(RM) $(OUTS)

rebuild: clean all
