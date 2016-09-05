CXXFLAGS=-c -std=c++14
SRCS=main.cpp
OBJS=$(SRCS:.cpp=.o)
EXE=main

all: debug

.PHONY: debug
debug: CXXFLAGS+=-g -O3 -Wall -W
debug: build

.PHONY: release
release: CXXFLAGS+=-O0 -DNDEBUG
release: build


.PHONY: build
build: $(OBJS)
	clang++ -o $(EXE) $(OBJS)


.cpp.o: 
	clang++ -c -std=c++11 $(CXXFLAGS) $<

.PHONY: clean
clean:
	rm $(EXE) $(OBJS)

