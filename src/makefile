include ../make.inc

OBJ = pdfp.o lexer.o tools.o
HDR = pdfp.hpp tools.hpp lexer.hpp
TGT = ../bin/pdfp.a

$(TGT):	$(OBJ)
	mkdir -p ../bin
	ar cr $(TGT) $(OBJ)

%.o: %.cpp
	$(CPP) $(CCOPTS) -c -o $@ $^

clean:
	rm *.o $(TGT)