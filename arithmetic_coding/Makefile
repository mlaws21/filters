CXX=g++
CFLAGS+= -Wall -g -std=c++11
# CFLAGS+=-Wall -Werror -g -std=c++11
CXXFLAGS+=${CFLAGS}

all: acode

acode: acode.cc
	${CXX} ${CFLAGS} -o acode acode.cc

run:
	make; ./acode;
clean: 
	rm acode