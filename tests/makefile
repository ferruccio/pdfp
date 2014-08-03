CPP = clang++
CCOPTS = -std=c++1y -stdlib=libc++ -I ../src
OBJ = tests.o slice_tests.o
TGT = ../bin/tests

$(TGT): $(OBJ)
	mkdir -p ../../bin
	$(CPP) $(CCOPTS) $(OBJ) -o $(TGT)

%.o: %.cpp
	$(CPP) $(CCOPTS) -c -o $@ $^

clean:
	rm *.o