#	gcc -lwiringPi -lpthread -I codeFromChanYu resistanceReader.c -o resistanceReader 
resistanceReader: resistanceReader.o Linux_UART.o
	g++ -O3 resistanceReader.cpp Linux_UART.cpp -lpthread -lwiringPi -o resistanceReader
	gcc resistanceReaderController.c -o resistanceReaderController
	sudo rm /bin/resistanceReaderController
	sudo cp resistanceReaderController /bin/

resistanceReader.o: resistanceReader.cpp
	g++ -lwiringPi resistanceReader.cpp -c

Linux_UART.o: Linux_UART.cpp
	g++ Linux_UART.cpp -c
