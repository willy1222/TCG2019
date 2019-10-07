CPLUSPLUS = g++ -std=c++11 -O2 -Wall
DEL = rm -f
all:
	$(CPLUSPLUS) src/*.cpp -c
	$(CPLUSPLUS) game.o getch.o pukoban.o -o pukoban
	$(CPLUSPLUS) verifier.o pukoban.o -o verifier
	$(DEL) -f *.o
pukoban:
	$(CPLUSPLUS) src/*.cpp -c
	$(CPLUSPLUS) game.o getch.o pukoban.o -o pukoban
	$(DEL) -f *.o
verifier:
	$(CPLUSPLUS) src/*.cpp -c
	$(CPLUSPLUS) verifier.o pukoban.o -o verifier	
	$(DEL) -f *.o
clean:
	$(DEL) -f pukoban verifier

