#	gcc -lwiringPi -lpthread -I codeFromChanYu resistanceReader.c -o resistanceReader 
resistanceReader: resistanceReader.cpp Linux_UART.cpp
	g++ -O3 resistanceReader.cpp Linux_UART.cpp -lpthread -lwiringPi -o resistanceReader
	gcc resistanceReaderController.c -o resistanceReaderController

resistanceReader.o: resistanceReader.cpp
	g++ -lwiringPi resistanceReader.cpp -c
#
#Linux_UART.o: Linux_UART.cpp
#	g++ Linux_UART.cpp -c

#Install:
#	sudo rm /bin/resistanceReaderController
#	sudo cp resistanceReaderController /bin/
	
