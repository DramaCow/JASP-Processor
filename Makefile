EXE ?= msim

CC := g++
CFLAGS := -Wextra -Werror -std=c++11 -pedantic -Ofast #-g
LIBS := -lm

SRCS := $(shell find . -name '*.cpp')
OBJS := $(SRCS:%=./%.o)
DEPS := $(OBJS:.o=.d)

print-%  : ; @echo $* = $($*)

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

%.c.o: %.c
	$(CC) -c $(CFLAGS) $^ $(LIBS) -o $@

%.cpp.o: %.cpp
	$(CC) -c $(CFLAGS) $^ $(LIBS) -o $@

clean:
	find . -type f -name '*.o' -exec rm {} +
	rm $(EXE)
