include ../make.inc

OBJ = tests.o slice_tests.o parser_tests.o atom_table_tests.o variant_tests.o
TGT = ../bin/tests

$(TGT): $(OBJ)
	mkdir -p ../../bin
	$(CPP) $(CCOPTS) $(OBJ) ../bin/pdfp.a -o $(TGT)

%.o: %.cpp
	$(CPP) $(CCOPTS) -I ../src -c -o $@ $^

clean:
	rm -f *.o $(TGT)