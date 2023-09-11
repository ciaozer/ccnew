CFLAGS = -O3 -static -m64
TARGET = ccehc

all: ${TARGET} clean

${TARGET}: src/data_structure.h src/build.h src/init.h src/heuristic.h src/generate.h src/parse.h src/preprocess.h src/wscpc.cpp	
	g++ ${CFLAGS} src/wscpc.cpp -o ${TARGET}

clean:
	rm -f *~

cleanup:
	rm -f ${TARGET}
	rm -f *~

#g++ -g  src/wscpc.cpp src/init.h src/generate.h src/heuristic.h src/data_structure.h src/build.h src/parse.h src/preprocess.h -o debug