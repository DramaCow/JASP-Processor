EXE ?= data

CC := gcc
CFLAGS := -Wextra -Werror -std=c99 -pedantic -g 
LIBS := -lm

SRCS := $(shell find . -name '*.c')
OBJS := $(SRCS:%=./%.o)
DEPS := $(OBJS:.o=.d)

print-%  : ; @echo $* = $($*)

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

%.c.o: %.c
	$(CC) -c $(CFLAGS) $^ $(LIBS) -o $@

clean:
	find . -type f -name '*.o' -exec rm {} +
	rm $(EXE)
