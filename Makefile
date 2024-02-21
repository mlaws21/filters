CXX=g++
CFLAGS+= -Wall -g -std=c++11
# CFLAGS+=-Wall -Werror -g -std=c++11
CXXFLAGS+=${CFLAGS}

all: quotient

quotient: quotient.cc
	${CXX} ${CFLAGS} -o quotient quotient.cc

run:
	make; ./quotient;
clean: 
	rm quotient