CCOPTS = -std=c++1y -stdlib=libc++
SRC = dump.cpp pdfp.cpp lexer.cpp
HDR = pdfp.hpp tools.hpp lexer.hpp

dump:	$(SRC) $(HDR)
	clang++ $(CCOPTS) $(SRC) -o dump