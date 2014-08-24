include ../make.inc

OBJ = pdfp.o parser.o tools.o pdf_atoms.o xref_table.o
TOOLS_HDR = tools/atom_table.hpp tools/slice.hpp tools/variant.hpp
HDR = pdfp.hpp tools.hpp parser.hpp pdf_atoms.hpp xref_table.hpp $(TOOLS_HDR)
TGT = ../bin/pdfp.a

$(TGT):	$(OBJ)
	mkdir -p ../bin
	ar cr $(TGT) $(OBJ)

%.o: %.cpp
	$(CPP) $(CCOPTS) -c -o $@ $^

clean:
	rm -f *.o $(TGT)