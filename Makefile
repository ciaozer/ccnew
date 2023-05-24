CFLAGS = -O3 -static -m64
TARGET = ccehc

all: ${TARGET} clean

${TARGET}: data_structure.h build.h init.h heuristic.h generate.h parse.h wscpc.cpp	
	g++ ${CFLAGS} wscpc.cpp -o ${TARGET}

clean:
	rm -f *~

cleanup:
	rm -f ${TARGET}
	rm -f *~
