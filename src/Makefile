CXXFLAGS=-c -std=c++11
SRCS=main.cpp
OBJS=$(SRCS:.cpp=.o)
DEPS=$(SRCS:.cpp=.d)
EXE=main

all: debug

-include $(DEPS)

.PHONY: debug
debug: CXXFLAGS+=-g -O3 -Wall -W
debug: build

.PHONY: release
release: CXXFLAGS+=-O0 -DNDEBUG
release: build


.PHONY: build
build: $(OBJS)
	g++ -o $(EXE) $(OBJS)


.cpp.o: 
	g++ -c -MMD -MP $(CXXFLAGS) $<

.PHONY: clean
clean:
	rm $(EXE) $(OBJS) $(DEPS)

