target = pukoban verifier display solver_bfs
CPPFLAGS = -O2 -std=c++11 -Wall
CPPFLAGS = -O0 -std=c++11 -Wall -g3 # for debug
SRC = src
DEL = rm -f

all: $(target)

$(SRC)/%.o: $(SRC)/%.cpp $(SRC)/%.hpp
	g++ $(CPPFLAGS) $< -c -o $@

$(SRC)/%.o: $(SRC)/%.cpp
	g++ $(CPPFLAGS) $< -c -o $@

%: $(SRC)/%.cpp $(SRC)/pukoban.o
	g++ $(CPPFLAGS) $^ -o $@

pukoban: $(SRC)/game.o $(SRC)/getch.o $(SRC)/pukoban.o
	g++ $(CPPFLAGS) $^ -o $@
verifier: $(SRC)/verifier.o $(SRC)/pukoban.o
	g++ $(CPPFLAGS) $^ -o $@
clean_obj:
	$(DEL) $(SRC)/*.o
clean: clean_obj
	$(DEL) $(target)

