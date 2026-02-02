CC = g++
SRCS = $(wildcard *.cpp)
SRCS += $(wildcard vecmath/src/*.cpp)
OBJS = $(SRCS:.cpp=.o)
PROG = a5
INCFLAGS = -Ivecmath/include

BUILD ?= debug

ifeq ($(BUILD), release)
    CFLAGS = -O3 -DNDEBUG -march=native
else
    CFLAGS = -O2 -Wall -Wextra
endif

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LINKFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -c -o $@ $(INCFLAGS)

clean:
	rm -f *.bak vecmath/src/*.o *.o core.* $(PROG)
