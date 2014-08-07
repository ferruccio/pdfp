include ../make.inc

OBJ = pdfp.o parser.o tools.o
HDR = pdfp.hpp tools.hpp parser.hpp
TGT = ../bin/pdfp.a

$(TGT):	$(OBJ)
	mkdir -p ../bin
	ar cr $(TGT) $(OBJ)

%.o: %.cpp
	$(CPP) $(CCOPTS) -c -o $@ $^

clean:
	rm *.o $(TGT)