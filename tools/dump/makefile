include ../../make.inc

OBJ = dump.cpp
HDR = pdfp.hpp tools.hpp lexer.hpp
TGT = ../../bin/dump
LIB = ../../bin/pdfp.a

$(TGT): dump.cpp $(LIB)
	$(CPP) $(CCOPTS) -I ../../src dump.cpp $(LIB) -o $(TGT)

clean:
	rm $(TGT)