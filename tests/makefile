CPP = clang++
CCOPTS = -std=c++1y -stdlib=libc++ -I ../src
OBJ = tests.o slice_tests.o
TGT = ../../bin/tests

$(TGT): $(OBJ)
	mkdir -p ../../bin
	$(CPP) $(CCOPTS) $(OBJ) -o $(TGT)

tests.o: tests.cpp
	$(CPP) $(CCOPTS) -c tests.cpp

slice_tests.o: slice_tests.cpp
	$(CPP) $(CCOPTS) -c slice_tests.cpp

clean:
	rm *.o