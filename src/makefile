include ../make.inc

OBJ = pdfp.o parser.o tools.o pdf_atoms.o
HDR = pdfp.hpp tools.hpp parser.hpp pdf_atoms.hpp tools/atom_table.hpp tools/slice.hpp tools/variant.hpp
TGT = ../bin/pdfp.a

$(TGT):	$(OBJ)
	mkdir -p ../bin
	ar cr $(TGT) $(OBJ)

%.o: %.cpp
	$(CPP) $(CCOPTS) -c -o $@ $^

clean:
	rm -f *.o $(TGT)