#
# NHF - Németh Marcell
#

PROG	= receptkonyv
OBJ	    = memtrace.o components.o string5.o file.o controller.o
HEAD	= components.h string5.h list.h file.h controller.h
TEST	= jporta_test.txt
DATA	= recipes.dat pantry.dat ingredients.dat

JPORTA_PACK = jporta_test.cpp $(HEAD)

CXXFLAGS= -std=c++11 -Wall -Werror -g -DCPORTA -DMEMTRACE

all:	$(PROG)

gen_array3_main: $(OBJ)
	$(CXX) -o $(PROG) $(OBJ)

$(OBJ): $(HEAD)

test:	$(PROG) $(TEST)
	for i in $(TEST); do \
	  ./$(PROG) < $$i ; \
	done

clean:
	rm -f $(PROG) $(OBJ)

tar:
	tar -czf $(PROG).tgz $(SRC) $(HEAD) $(TEST) $(DATA)

zip:
	zip kesz.zip $(JPORTA_PACK)
