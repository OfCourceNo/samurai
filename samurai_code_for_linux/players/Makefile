CPPFLAGS= -std=c++11 -g -Wall
CPP= c++

COMMONOBJECTS= players.o
YUUSEI= ${COMMONOBJECTS} yuusei.o
MIZUMARU= ${COMMONOBJECTS} mizumaru.o
KIKUCHI= ${COMMONOBJECTS} kikuchi.o
YOKOYOKO= ${COMMONOBJECTS} yokoyoko.o
TORIYABE=  ${COMMONOBJECTS} toriyabe.o
TARGET= yuuseiPlayer mizumaruPlayer kikuchiPlayer yokoyokoPlayer toriyabePlayer
OBJECTS= ${YUUSEI} ${MIZUMARU} ${KIKUCHI} ${YOKOYOKO} ${TORIYABE}

all: ${TARGET}

yuuseiPlayer: ${YUUSEI}
	${CPP} ${CPPFLAGS} -o $@ $^

mizumaruPlayer: ${MIZUMARU}
	${CPP} ${CPPFLAGS} -pthread -o $@ $^

kikuchiPlayer: ${KIKUCHI}
	${CPP} ${CPPFLAGS} -o $@ $^

yokoyokoPlayer: ${YOKOYOKO}
	${CPP} ${CPPFLAGS} -o $@ $^

toriyabePlayer: ${TORIYABE}
	${CPP} ${CPPFLAGS} -o $@ $^

-include ${OBJECTS:.o=.d}

%.o: %.cpp
	${CPP} -c ${CPPFLAGS} $*.cpp -o $*.o
	${CPP} -MM ${CPPFLAGS} $*.cpp > $*.d

clean:
	rm -f ${TARGET}
	rm -f *.o *.d
	rm -rf *~ */*~ \#*\#
