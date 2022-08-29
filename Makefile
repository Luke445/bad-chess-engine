CC = g++
CFLAGS = -O3 -std=c++11

BadChess : main.o Board.o Computer.o EnhancedBoard.o Gui.o Threads.o
	$(CC) $(CFLAGS) -o ./BadChess *.o -lSDL2 -lSDL2_image
	rm *.o

Board.o : Board.cpp Board.h
	$(CC) $(CFLAGS) -c Board.cpp

Computer.o : Computer.cpp Computer.h
	$(CC) $(CFLAGS) -c Computer.cpp

EnhancedBoard.o : EnhancedBoard.cpp EnhancedBoard.h
	$(CC) $(CFLAGS) -c EnhancedBoard.cpp

Gui.o : Gui.cpp Gui.h
	$(CC) $(CFLAGS) -c Gui.cpp

main.o : main.cpp
	$(CC) $(CFLAGS) -c main.cpp

Threads.o : Threads.cpp Threads.h
	$(CC) $(CFLAGS) -c Threads.cpp -pthread

clean :
	rm BadChess *.o