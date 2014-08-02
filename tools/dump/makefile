CPP = clang++
CCOPTS = -std=c++1y -stdlib=libc++ -I ../../src
OBJ = dump.cpp
HDR = pdfp.hpp tools.hpp lexer.hpp
TGT = ../../bin/dump
LIB = ../../bin/pdfp.a

$(TGT): dump.cpp $(LIB)
	$(CPP) $(CCOPTS) dump.cpp $(LIB) -o $(TGT)

clean:
	rm $(TGT)