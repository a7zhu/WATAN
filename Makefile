CXX = g++-5
CXXFLAGS = -std=c++14 -Wall -MMD
EXEC = watan
OBJECTS = main.o Criterion.o Subject.o Player.o Board.o Controller.o TextDisplay.o Dice.o Tile.o Goal.o
DEPENDS = ${OBJECTS:.o=.d}

${EXEC}: ${OBJECTS}
	${CXX} ${CXXFLAGS} ${OBJECTS} -o ${EXEC} -lX11

-include ${DEPENDS}

.PHONY: clean

clean:
	rm ${OBJECTS} ${EXEC} ${DEPENDS}
