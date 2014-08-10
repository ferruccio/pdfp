include ../../make.inc

OBJ = dump.cpp
HDR = pdfp.hpp tools.hpp parser.hpp
TGT = ../../bin/dump
LIB = ../../bin/pdfp.a

$(TGT): dump.cpp $(LIB)
	$(CPP) $(CCOPTS) -I ../../src dump.cpp $(LIB) -o $(TGT)

clean:
	rm -f $(TGT)