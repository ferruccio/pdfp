CPP = clang++
CCOPTS = -std=c++1y -stdlib=libc++
OBJ = pdfp.o lexer.o
HDR = pdfp.hpp tools.hpp lexer.hpp

pdfp.a:	$(OBJ)
	ar cr pdfp.a $(OBJ)
	mkdir -p ../bin
	cp pdfp.a ../bin

%.o: %.cpp
	$(CPP) $(CCOPTS) -c -o $@ $^

clean:
	rm *.o *.a