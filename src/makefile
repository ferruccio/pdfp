include ../make.inc

OBJ = pdfp.o lexer.o
HDR = pdfp.hpp tools.hpp lexer.hpp
TGT = ../bin/pdfp.a

$(TGT):	$(OBJ) $(HDR)
	mkdir -p ../bin
	ar cr $(TGT) $(OBJ)

%.o: %.cpp
	$(CPP) $(CCOPTS) -c -o $@ $^

clean:
	rm *.o $(TGT)