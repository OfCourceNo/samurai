CPPFLAGS= -std=c++11 -g -Wall
CPP= c++

COMMONOBJECTS= players.o
RANDOMOBJECTS= ${COMMONOBJECTS} randomPlayer.o
TARGET= randomPlayer
OBJECTS= ${RANDOMOBJECTS}

all: ${TARGET}

randomPlayer: ${RANDOMOBJECTS}
	${CPP} ${CPPFLAGS} -o $@ $^

-include ${OBJECTS:.o=.d}

%.o: %.cpp
	${CPP} -c ${CPPFLAGS} $*.cpp -o $*.o
	${CPP} -MM ${CPPFLAGS} $*.cpp > $*.d

clean:
	rm -f ${TARGET}
	rm -f *.o *.d
	rm -rf *~ */*~ \#*\#
